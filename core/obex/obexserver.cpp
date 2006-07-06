/*
               =.            This file is part of the OPIE Project
             .=l.            Copyright (c)  2002 Maximilian Reiss <max.reiss@gmx.de>
           .>+-=
 _;:,     .>    :=|.         This library is free software; you can
.> <,   >  .   <=           redistribute it and/or  modify it under
:=1 )Y*s>-.--   :            the terms of the GNU Library General Public
.="- .-=="i,     .._         License as published by the Free Software
 - .   .-<_>     .<>         Foundation; version 2 of the License.
     ._= =}       :
    .%+i>       _;_.
    .i_,=:_.      -<s.       This library is distributed in the hope that
     +  .  -:.       =       it will be useful,  but WITHOUT ANY WARRANTY;
    : ..    .:,     . . .    without even the implied warranty of
    =_        +     =;=|     MERCHANTABILITY or FITNESS FOR A
  _.=:.       :    :=>:      PARTICULAR PURPOSE. See the GNU
..}^=.=       =       ;      Library General Public License for more
++=   -.     .     .:        details.
 :     =  ...= . :.=-
 -.   .:....=;==+<;          You should have received a copy of the GNU
  -_. . .   )=.  =           Library General Public License along with
    --        :-=            this library; see the file COPYING.LIB.
                             If not, write to the Free Software Foundation,
                             Inc., 59 Temple Place - Suite 330,
                             Boston, MA 02111-1307, USA.

*/
/*
 * The OBEX server class implementation
 * Based on OBEX server from GPE (thanks, guys)
 */

#include "obexserver.h"
#include <unistd.h>
#include <opie2/odebug.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <qapplication.h>
#include <opie2/oprocctrl.h>
#include <qstring.h>
#include <qfile.h>

using namespace Opie::Core;
using namespace Opie::Core::Internal;
using namespace OpieObex;

ObexServer::ObexServer() : 
    OProcess(tr("ObexServer"), 0, "ObexServer")
{
    m_obex = NULL;
}

ObexServer::~ObexServer()
{
    stop();
}

/**
 * Function handles the file received
 * @param name the file name
 * @param data the file data
 * @param data_len the data length
 * @return 0 on success -1 on error
 */
static int file_received(uint8_t* name, const uint8_t* data, size_t data_len)
{
    QString path("/tmp/");
    path += (char*)name;
    QFile out(path);
    int err = 0;
    
    if (!out.open(IO_Raw | IO_ReadWrite | IO_Truncate)) {
        printf("File %s open error %d\n", (const char*)path, errno);
        err = -1;
        goto out;
    }
    if (out.writeBlock((const char*)data, data_len) < 0) {
        printf("File %s write error %d\n", (const char*)path, errno);
        err = -1;
        goto out;
    }
out:
    out.close();
    if (err == 0) {
        printf("Wrote %s (%d bytes)\n", (const char*)path, data_len);
        fflush(stdout);
    }
    return err;
}

/**
 * Function handles the situation when the PUT request has been done
 * @param handle OBEX connection handle
 * @param object OBEX object itself
 */
static int put_done(obex_t* handle, obex_object_t* object)
{
    obex_headerdata_t hv; //Received file header
    uint8_t hi; //Type of the request
    uint32_t hlen; //File (file name) length
    int err = 0;

    const uint8_t *body = NULL;
    int body_len = 0;
    uint8_t* name = NULL;

    while (OBEX_ObjectGetNextHeader (handle, object, &hi, &hv, &hlen)) {
        switch(hi) {
        case OBEX_HDR_BODY:
            body = hv.bs;
            body_len = hlen;
            break;
        
        case OBEX_HDR_NAME:
            name = new uint8_t[(hlen / 2) + 1];
            OBEX_UnicodeToChar(name, hv.bs, hlen);
            break;
	  
        default:
            break;
        }
    }

    if (body)
        err = file_received(name, body, body_len);
  
    if (name)
        delete[] name;
    return err;
}

/**
 * Function handles OBEX request
 * @param handle OBEX connection handle
 * @param object OBEX object itself
 * @param mode 
 * @param event event code
 * @param cmd OBEX command itself
 */
static void handle_request (obex_t* handle, obex_object_t* object, 
    int event, int cmd)
{
    (void)event;
    switch(cmd)	{
    case OBEX_CMD_SETPATH:
        OBEX_ObjectSetRsp (object, OBEX_RSP_CONTINUE, OBEX_RSP_SUCCESS);
        break;
    case OBEX_CMD_PUT:
        if (put_done (handle, object) < 0)
            OBEX_ObjectSetRsp (object, OBEX_RSP_INTERNAL_SERVER_ERROR, 
                OBEX_RSP_INTERNAL_SERVER_ERROR);
        else
            OBEX_ObjectSetRsp (object, OBEX_RSP_CONTINUE, OBEX_RSP_SUCCESS);
        break;
    case OBEX_CMD_CONNECT:
        OBEX_ObjectSetRsp (object, OBEX_RSP_SUCCESS, OBEX_RSP_SUCCESS);
        break;
    case OBEX_CMD_DISCONNECT:
        OBEX_ObjectSetRsp (object, OBEX_RSP_SUCCESS, OBEX_RSP_SUCCESS);
        break;
    default:
        printf("Denied %02x request\n", cmd);
        fflush(stdout);
        OBEX_ObjectSetRsp (object, OBEX_RSP_NOT_IMPLEMENTED, 
            OBEX_RSP_NOT_IMPLEMENTED);
        break;
    }
}


/**
 * Function handles OBEX event when a client is connected to the server
 * @param handle OBEX connection handle
 * @param object OBEX object itself
 * @param mode 
 * @param event event code
 * @param obex_cmd OBEX command itself
 * @param obex_rsp OBEX responce
 */
static void obex_conn_event (obex_t *handle, obex_object_t *object, 
    int mode, int event, int obex_cmd, int obex_rsp)
{
    (void)mode;
    (void)obex_rsp;
    
    switch(event) {
    case OBEX_EV_REQHINT:
        switch(obex_cmd) {
        case OBEX_CMD_PUT:
        case OBEX_CMD_CONNECT:
        case OBEX_CMD_DISCONNECT:
            OBEX_ObjectSetRsp (object, OBEX_RSP_CONTINUE, OBEX_RSP_SUCCESS);
            break;
        default:
            OBEX_ObjectSetRsp (object, OBEX_RSP_NOT_IMPLEMENTED, 
                OBEX_RSP_NOT_IMPLEMENTED);
            break;
        }
        break;

        case OBEX_EV_REQ:
            /* Comes when a server-request has been received. */
            handle_request (handle, object, event, obex_cmd);
            break;

        case OBEX_EV_LINKERR:
            break;
    }
}

/**
 * Function handles OBEX event
 * @param handle OBEX connection handle
 * @param object OBEX object itself
 * @param mode 
 * @param event event code
 * @param obex_cmd OBEX command itself
 * @param obex_rsp OBEX responce
 */
static void obex_event (obex_t* handle, obex_object_t* object, int mode, 
    int event, int obex_cmd, int obex_rsp)
{

    obex_t *obex; //OBEX connection handle
    
    switch (event) {
    case OBEX_EV_ACCEPTHINT:
        obex = OBEX_ServerAccept (handle, obex_conn_event, NULL);
        break;
        
    default:
        obex_conn_event(handle, object, mode, event, obex_cmd, obex_rsp);
    }
}

/**
 * Function registers OBEX push service on a specified channel
 * Based on The same function from GPE.
 * @param session SDP session
 * @param chan channel to listen
 * @name name to show
 */
sdp_session_t* ObexServer::addOpushSvc(uint8_t chan, const char* name)
{
    sdp_list_t *svclass_id, *pfseq, *apseq, *root;
    uuid_t root_uuid, opush_uuid, l2cap_uuid, rfcomm_uuid, obex_uuid;
    sdp_profile_desc_t profile[1];
    sdp_list_t *aproto, *proto[3];
    sdp_record_t record;
    sdp_data_t *channel;
    uint8_t formats[] = { 0x01, 0x02, 0x03, 0x04, 0x05, 0x06 };
    //uint8_t formats[] = { 0xff };
    void *dtds[sizeof(formats)], *values[sizeof(formats)];
    unsigned int i;
    uint8_t dtd = SDP_UINT8;
    sdp_data_t *sflist;
    int err = 0;
    sdp_session_t* lsession = 0;
    
    memset((void *)&record, 0, sizeof(sdp_record_t));
    record.handle = 0xffffffff;
    sdp_uuid16_create(&root_uuid, PUBLIC_BROWSE_GROUP);
    root = sdp_list_append(0, &root_uuid);
    sdp_set_browse_groups(&record, root);
    
    sdp_uuid16_create(&opush_uuid, OBEX_OBJPUSH_SVCLASS_ID);
    svclass_id = sdp_list_append(0, &opush_uuid);
    sdp_set_service_classes(&record, svclass_id);
    
    sdp_uuid16_create(&profile[0].uuid, OBEX_OBJPUSH_PROFILE_ID);
    profile[0].version = 0x0100;
    pfseq = sdp_list_append(0, profile);
    sdp_set_profile_descs(&record, pfseq);
    
    sdp_uuid16_create(&l2cap_uuid, L2CAP_UUID);
    proto[0] = sdp_list_append(0, &l2cap_uuid);
    apseq = sdp_list_append(0, proto[0]);
    
    sdp_uuid16_create(&rfcomm_uuid, RFCOMM_UUID);
    proto[1] = sdp_list_append(0, &rfcomm_uuid);
    channel = sdp_data_alloc(SDP_UINT8, &chan);
    proto[1] = sdp_list_append(proto[1], channel);
    apseq = sdp_list_append(apseq, proto[1]);
    
    sdp_uuid16_create(&obex_uuid, OBEX_UUID);
    proto[2] = sdp_list_append(0, &obex_uuid);
    apseq = sdp_list_append(apseq, proto[2]);
    
    aproto = sdp_list_append(0, apseq);
    sdp_set_access_protos(&record, aproto);
    
    for (i = 0; i < sizeof(formats); i++) 
    {
        dtds[i] = &dtd;
        values[i] = &formats[i];
    }
    sflist = sdp_seq_alloc(dtds, values, sizeof(formats));
    sdp_attr_add(&record, SDP_ATTR_SUPPORTED_FORMATS_LIST, sflist);
    
    sdp_set_info_attr(&record, name, 0, 0);
    
    // connect to the local SDP server, register the service record, and
    // disconnect
    lsession = sdp_connect(BDADDR_ANY, BDADDR_LOCAL, SDP_RETRY_IF_BUSY);
    if (lsession == NULL)
        goto errout;
    err = sdp_record_register(lsession, &record, 0);
    if (err) {
        sdp_close(lsession);
        lsession = NULL;
    }
errout:
    sdp_data_free(channel);
    sdp_list_free(proto[0], 0);
    sdp_list_free(proto[1], 0);
    sdp_list_free(proto[2], 0);
    sdp_list_free(apseq, 0);
    sdp_list_free(aproto, 0);
    
    return lsession;
}

int ObexServer::initObex(void)
{
    int channel = 10; //Channel on which we do listen
    if (m_obex)
        return 0;
    m_obex = ::OBEX_Init(OBEX_TRANS_BLUETOOTH, obex_event, 0);
    if (!m_obex) {
        printf("OBEX initialization error %d\n", errno);
        return -1;
    }
    ::BtOBEX_ServerRegister(m_obex, NULL, channel);
    m_session = addOpushSvc(channel, "OBEX push service");
    if (!m_session) {
        printf("OBEX registration error %d\n", errno);
        ::OBEX_Cleanup(m_obex);
        m_obex = NULL;
        return -1;
    }    
    return 0;
}

bool ObexServer::start(RunMode runmode, Communication comm)
{
    if ( runs )
    {
        return false;  // cannot start a process that is already running
        // or if no executable has been assigned
    }
    run_mode = runmode;
    status = 0;

    if ( !setupCommunication( comm ) )
        qWarning( "Could not setup Communication!" );

    // We do this in the parent because if we do it in the child process
    // gdb gets confused when the application runs from gdb.
    uid_t uid = getuid();
    gid_t gid = getgid();
#ifdef HAVE_INITGROUPS

    struct passwd *pw = getpwuid( uid );
#endif

    int fd[ 2 ];
    if ( 0 > pipe( fd ) )
    {
        fd[ 0 ] = fd[ 1 ] = 0; // Pipe failed.. continue
    }

    runs = true;

    QApplication::flushX();

    // WABA: Note that we use fork() and not vfork() because
    // vfork() has unclear semantics and is not standardized.
    pid_ = fork();

    if ( 0 == pid_ )
    {
        if ( fd[ 0 ] )
            close( fd[ 0 ] );
        if ( !runPrivileged() )
        {
            setgid( gid );
#if defined( HAVE_INITGROUPS)

            if ( pw )
                initgroups( pw->pw_name, pw->pw_gid );
#endif

            setuid( uid );
        }
        // The child process
        if ( !commSetupDoneC() )
            qWarning( "Could not finish comm setup in child!" );

        setupEnvironment();

        // Matthias
        if ( run_mode == DontCare )
            setpgid( 0, 0 );
        // restore default SIGPIPE handler (Harri)
        struct sigaction act;
        sigemptyset( &( act.sa_mask ) );
        sigaddset( &( act.sa_mask ), SIGPIPE );
        act.sa_handler = SIG_DFL;
        act.sa_flags = 0;
        sigaction( SIGPIPE, &act, 0L );

        // We set the close on exec flag.
        // Closing of fd[1] indicates that the execvp succeeded!
        if ( fd[ 1 ] )
            fcntl( fd[ 1 ], F_SETFD, FD_CLOEXEC );

        if (initObex() == 0) {
            do {
                int result; //Connection result
                if ( fd[ 1 ] ) {
                    ::close(fd[1]);
                    fd[1] = 0;
                }
                if ((result = OBEX_HandleInput(m_obex, 60)) < 0) {
                    if (errno != ECONNRESET) {
                        printf("OBEX_HandleInput error %d\n", errno);
                        fflush(stdout);
                        _exit(-1);
                    }
                    else
                        _exit(0);
                }
            } while(1);
        }
        char resultByte = 1;
        if ( fd[ 1 ] )
            write( fd[ 1 ], &resultByte, 1 );
        _exit( -1 );
    }
    else if ( -1 == pid_ )
    {
        // forking failed

        runs = false;
        return false;
    }
    else
    {
        if ( fd[ 1 ] )
            close( fd[ 1 ] );
        // the parent continues here

        // Discard any data for stdin that might still be there
        input_data = 0;

        // Check whether client could be started.
        if ( fd[ 0 ] )
            for ( ;; )
            {
                char resultByte;
                int n = ::read( fd[ 0 ], &resultByte, 1 );
                if ( n == 1 )
                {
                    // Error
                    runs = false;
                    close( fd[ 0 ] );
                    pid_ = 0;
                    return false;
                }
                if ( n == -1 )
                {
                    if ( ( errno == ECHILD ) || ( errno == EINTR ) )
                        continue; // Ignore
                }
                break; // success
            }
        if ( fd[ 0 ] )
            close( fd[ 0 ] );

        if ( !commSetupDoneP() )   // finish communication socket setup for the parent
            qWarning( "Could not finish comm setup in parent!" );

        if ( run_mode == Block )
        {
            commClose();

            // The SIGCHLD handler of the process controller will catch
            // the exit and set the status
            while ( runs )
            {
                OProcessController::theOProcessController->
                slotDoHousekeeping( 0 );
            }
            runs = FALSE;
            emit processExited( this );
        }
    }
    return true;
}

/*
 * Stop forwarding process
 */
int ObexServer::stop()
{
    kill(SIGTERM);
    return 0;
}

//eof
