
#include "pop3wrapper.h"
#include "mailtypes.h"
#include <libetpan/mailpop3.h>

POP3wrapper::POP3wrapper( POP3account *a )
{
    account = a;
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
    login();
    //TODO: list messages
    logout();
}

void POP3wrapper::login()
{
    logout();
    const char *server, *user, *pass;
    uint16_t port;
    int err = MAILPOP3_NO_ERROR;

    server = account->getServer().latin1();
    port = account->getPort().toUInt();
    user = account->getUser().latin1();
    pass = account->getPassword().latin1();

    m_pop3 = mailpop3_new( 20, &pop3_progress );

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
    if ( !m_pop3 ) return;
    err = mailpop3_quit( m_pop3 );
    mailpop3_free( m_pop3 );
    m_pop3 = NULL;
}


