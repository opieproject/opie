
#include "pop3wrapper.h"
#include "mailtypes.h"
#include <libetpan/mailpop3.h>

POP3wrapper::POP3wrapper( POP3account *a )
{
    account = a;
    m_pop3 = NULL;
}

POP3wrapper::~POP3wrapper()
{
    logout();
}

void POP3wrapper::pop3_progress( size_t current, size_t maximum )
{
    qDebug( "POP3: %i of %i", current, maximum );
}

void POP3wrapper::listMessages( QList<RecMail> &target )
{
    int err = MAILPOP3_NO_ERROR;
    char *header;
    size_t length;
    carray *messages;
    
    login();
    mailpop3_list( m_pop3, &messages );

    for ( int i = carray_count( messages ); i > 0; i-- ) {
        mailpop3_msg_info *info = (mailpop3_msg_info *) carray_get( messages, i - 1 );

        err = mailpop3_header( m_pop3, info->index, &header, &length );
        if ( err != MAILPOP3_NO_ERROR ) {
            qDebug( "POP3: error retrieving header msgid: %i", info->index );
            logout();
            return;
        }
        RecMail *mail = parseHeader( header );
        mail->setNumber( info->index );
        target.append( mail );
    }

    logout();
}

RecMail *POP3wrapper::parseHeader( const char *h )
{
    RecMail *mail = new RecMail();
    QString header( h );

    //TODO: parse header - maybe something like this is already implemented in libetpan?
    mail->setSubject( "Blah blubb" );

    return mail;
}

void POP3wrapper::login()
{
    if ( m_pop3 != NULL ) logout();

    const char *server, *user, *pass;
    uint16_t port;
    int err = MAILPOP3_NO_ERROR;

    server = account->getServer().latin1();
    port = account->getPort().toUInt();
    user = account->getUser().latin1();
    pass = account->getPassword().latin1();

    m_pop3 = mailpop3_new( 200, &pop3_progress );

    // connect
    err = mailpop3_socket_connect( m_pop3, (char *) server, port );
    if ( err != MAILPOP3_NO_ERROR ) {
        qDebug( "pop3: error connecting to %s\n reason: %s", server, 
                m_pop3->response );
        mailpop3_free( m_pop3 );
        m_pop3 = NULL;
        return;
    }
    qDebug( "POP3: connected!" );

    // login
    // TODO: decide if apop or plain login should be used
    err = mailpop3_login( m_pop3, (char *) user, (char *) pass );
    if ( err != MAILPOP3_NO_ERROR ) {
        qDebug( "pop3: error logging in: %s", m_pop3->response );
        logout();
        return;
    }

    qDebug( "POP3: logged in!" );
}

void POP3wrapper::logout()
{
    int err = MAILPOP3_NO_ERROR;
    if ( m_pop3 == NULL ) return;
    err = mailpop3_quit( m_pop3 );
    mailpop3_free( m_pop3 );
    m_pop3 = NULL;
}


