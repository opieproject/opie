#include <stdlib.h>
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
    //qDebug( "POP3: %i of %i", current, maximum );
}

RecBody POP3wrapper::fetchBody( const RecMail &mail )
{
    int err = MAILPOP3_NO_ERROR;
    char *message;
    size_t length = 0;

    login();
    if ( !m_pop3 ) return RecBody();
    
    err = mailpop3_retr( m_pop3, mail.getNumber(), &message, &length );
    if ( err != MAILPOP3_NO_ERROR ) {
        qDebug( "POP3: error retrieving body with index %i", mail.getNumber() );
        return RecBody();
    }
    
    return parseBody( message );
}

RecBody POP3wrapper::parseBody( const char *message )
{
    int err = MAILIMF_NO_ERROR;
    /* these vars are used recurcive! set it to 0!!!!!!!!!!!!!!!!! */
    size_t curTok = 0;
    mailimf_message *result = 0;
    RecBody body;
    
    err = mailimf_message_parse( (char *) message, strlen( message ), &curTok, &result );
    if ( err != MAILIMF_NO_ERROR ) {
        if (result) mailimf_message_free(result);
        return body;
    }
    
    if ( result && result->msg_body && result->msg_body->bd_text ) {
        qDebug( "POP3: bodytext found" );
        // when curTok isn't set to 0 this line will fault! 'cause upper line faults!
        body.setBodytext( QString( result->msg_body->bd_text ) );
    }
    if (result) mailimf_message_free(result);
    return body;
}

void POP3wrapper::listMessages(const QString &, QList<RecMail> &target )
{
    int err = MAILPOP3_NO_ERROR;
    char * header = 0;
    /* these vars are used recurcive! set it to 0!!!!!!!!!!!!!!!!! */
    size_t length = 0;
    carray * messages = 0;

    login();
    if (!m_pop3) return;
    mailpop3_list( m_pop3, &messages );

    for (unsigned int i = 0; i < carray_count(messages);++i) {
        mailpop3_msg_info *info;
        err = mailpop3_get_msg_info(m_pop3,i+1,&info);
        err = mailpop3_header( m_pop3, info->msg_index, &header, &length );
        if ( err != MAILPOP3_NO_ERROR ) {
            qDebug( "POP3: error retrieving header msgid: %i", info->msg_index );
            free(header);
            return;
        }
        RecMail *mail = parseHeader( header );
        mail->setNumber( info->msg_index );
        mail->setWrapper(this);
        mail->setMsgsize(info->msg_size);
        target.append( mail );
        free(header);
    }
}

RecMail *POP3wrapper::parseHeader( const char *header )
{
    int err = MAILIMF_NO_ERROR;
    size_t curTok = 0;
    RecMail *mail = new RecMail();
    mailimf_fields *fields;
    mailimf_references * refs;
    mailimf_keywords*keys;
    QString status;
    QString value;
    QBitArray mFlags(7);

    err = mailimf_fields_parse( (char *) header, strlen( header ), &curTok, &fields );
    for ( clistiter *current = clist_begin( fields->fld_list ); current != NULL; current = current->next ) {
        mailimf_field *field = (mailimf_field *) current->data;
        switch ( field->fld_type ) {
            case MAILIMF_FIELD_FROM:
                mail->setFrom( parseMailboxList( field->fld_data.fld_from->frm_mb_list ) );
                break;
            case MAILIMF_FIELD_TO:
                mail->setTo( parseAddressList( field->fld_data.fld_to->to_addr_list ) );
                break;
            case MAILIMF_FIELD_CC:
                mail->setCC( parseAddressList( field->fld_data.fld_cc->cc_addr_list ) );
                break;
            case MAILIMF_FIELD_BCC:
                mail->setBcc( parseAddressList( field->fld_data.fld_bcc->bcc_addr_list ) );
                break;
            case MAILIMF_FIELD_SUBJECT:
                mail->setSubject( QString( field->fld_data.fld_subject->sbj_value ) );
                break;
            case MAILIMF_FIELD_ORIG_DATE:
                mail->setDate( parseDateTime( field->fld_data.fld_orig_date->dt_date_time ) );
                break;
            case MAILIMF_FIELD_MESSAGE_ID:
                mail->setMsgid(QString(field->fld_data.fld_message_id->mid_value));
                break;
            case MAILIMF_FIELD_REFERENCES:
                refs = field->fld_data.fld_references;
                if (refs && refs->mid_list && clist_count(refs->mid_list)) {
                    char * text = (char*)refs->mid_list->first->data;
                    mail->setReplyto(QString(text));
                }
                break;
            case MAILIMF_FIELD_KEYWORDS:
                keys = field->fld_data.fld_keywords;
                for (clistcell*cur = clist_begin(keys->kw_list);cur!=0;cur=clist_next(cur)) {
                    qDebug("Keyword: %s",(char*)cur->data);
                }
                break;
            case MAILIMF_FIELD_OPTIONAL_FIELD:
                status = field->fld_data.fld_optional_field->fld_name;
                value = field->fld_data.fld_optional_field->fld_value;
                if (status.lower()=="status") {
                    if (value.lower()=="ro") {
                        mFlags.setBit(FLAG_SEEN);
                    }
                } else if (status.lower()=="x-status") {
                    qDebug("X-Status: %s",value.latin1());
                    if (value.lower()=="a") {
                        
                        mFlags.setBit(FLAG_ANSWERED);
                    }
                } else {
//                    qDebug("Optionales feld: %s -> %s)",field->fld_data.fld_optional_field->fld_name,
//                        field->fld_data.fld_optional_field->fld_value);
                }
                break;
            default:
                qDebug("Non parsed field");
                break;
        }
    }
    if (fields) mailimf_fields_free(fields);
    mail->setFlags(mFlags);
    return mail;
}

QString POP3wrapper::parseDateTime( mailimf_date_time *date )
{
    char tmp[23];
    
    snprintf( tmp, 23,  "%02i.%02i.%04i %02i:%02i:%02i %+05i", 
        date->dt_day, date->dt_month, date->dt_year, date->dt_hour, date->dt_min, date->dt_sec, date->dt_zone );
    
    return QString( tmp );
}

QString POP3wrapper::parseAddressList( mailimf_address_list *list )
{
    QString result( "" );

    bool first = true;
    for ( clistiter *current = clist_begin( list->ad_list ); current != NULL; current = current->next ) {
        mailimf_address *addr = (mailimf_address *) current->data;
        
        if ( !first ) {
            result.append( "," );
        } else {
            first = false;
        }

        switch ( addr->ad_type ) {
            case MAILIMF_ADDRESS_MAILBOX:
                result.append( parseMailbox( addr->ad_data.ad_mailbox ) );
                break;
            case MAILIMF_ADDRESS_GROUP:
                result.append( parseGroup( addr->ad_data.ad_group ) );
                break;
            default:
                qDebug( "POP3: unkown mailimf address type" );
                break;
        }
    }
    
    return result;
}

QString POP3wrapper::parseGroup( mailimf_group *group )
{
    QString result( "" );

    result.append( group->grp_display_name );
    result.append( ": " );

    if ( group->grp_mb_list != NULL ) {
        result.append( parseMailboxList( group->grp_mb_list ) );
    }

    result.append( ";" );
    
    return result;
}

QString POP3wrapper::parseMailbox( mailimf_mailbox *box )
{
    QString result( "" );

    if ( box->mb_display_name == NULL ) {
        result.append( box->mb_addr_spec );
    } else {
        result.append( box->mb_display_name );
        result.append( " <" );
        result.append( box->mb_addr_spec );
        result.append( ">" );
    }
        
    return result;
}

QString POP3wrapper::parseMailboxList( mailimf_mailbox_list *list )
{
    QString result( "" );

    bool first = true;
    for ( clistiter *current = clist_begin( list->mb_list ); current != NULL; current = current->next ) {
        mailimf_mailbox *box = (mailimf_mailbox *) current->data;

        if ( !first ) {
            result.append( "," );
        } else {
            first = false;
        }
       
        result.append( parseMailbox( box ) );
    }
    
    return result;
}

void POP3wrapper::login()
{
    /* we'll hold the line */
    if ( m_pop3 != NULL ) return;

    const char *server, *user, *pass;
    uint16_t port;
    int err = MAILPOP3_NO_ERROR;

    server = account->getServer().latin1();
    port = account->getPort().toUInt();
    user = account->getUser().latin1();
    pass = account->getPassword().latin1();

    m_pop3 = mailpop3_new( 200, &pop3_progress );

    // connect
    if (account->getSSL()) {
        err = mailpop3_ssl_connect( m_pop3, (char*)server, port );
    } else {
        err = mailpop3_socket_connect( m_pop3, (char*)server, port );
    }

    if ( err != MAILPOP3_NO_ERROR ) {
        qDebug( "pop3: error connecting to %s\n reason: %s", server,
                m_pop3->pop3_response );
        mailpop3_free( m_pop3 );
        m_pop3 = NULL;
        return;
    }
    qDebug( "POP3: connected!" );

    // login
    // TODO: decide if apop or plain login should be used
    err = mailpop3_login( m_pop3, (char *) user, (char *) pass );
    if ( err != MAILPOP3_NO_ERROR ) {
        qDebug( "pop3: error logging in: %s", m_pop3->pop3_response );
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


QList<Folder>* POP3wrapper::listFolders()
{
    /* TODO: integrate MH directories 
       but not vor version 0.1 ;)
    */
    QList<Folder> * folders = new QList<Folder>();
    folders->setAutoDelete( false );
    Folder*inb=new Folder("INBOX");
    folders->append(inb);
    return folders;
}

QString POP3wrapper::fetchPart(const RecMail&,const RecPart&)
{
    return "";
}

void POP3wrapper::deleteMail(const RecMail&mail)
{
    login();
    if (!m_pop3) return;
    int err = mailpop3_dele(m_pop3,mail.getNumber());
    if (err != MAILPOP3_NO_ERROR) {
        qDebug("error deleting mail");
    }
}

void POP3wrapper::answeredMail(const RecMail&)
{
}
