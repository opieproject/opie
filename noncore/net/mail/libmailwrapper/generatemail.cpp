#include "generatemail.h"
#include "mailwrapper.h"

#include <libetpan/libetpan.h>

#include <qt.h>

const char* Generatemail::USER_AGENT="OpieMail v0.6";

Generatemail::Generatemail()
{
}

Generatemail::~Generatemail()
{
}

void Generatemail::addRcpts( clist *list, mailimf_address_list *addr_list ) {
    clistiter *it, *it2;

    for ( it = clist_begin( addr_list->ad_list ); it; it = it->next ) {
        mailimf_address *addr;
        addr = (mailimf_address *) it->data;

        if ( addr->ad_type == MAILIMF_ADDRESS_MAILBOX ) {
            esmtp_address_list_add( list, addr->ad_data.ad_mailbox->mb_addr_spec, 0, NULL );
        } else if ( addr->ad_type == MAILIMF_ADDRESS_GROUP ) {
            clist *l = addr->ad_data.ad_group->grp_mb_list->mb_list;
            for ( it2 = clist_begin( l ); it2; it2 = it2->next ) {
                mailimf_mailbox *mbox;
                mbox = (mailimf_mailbox *) it2->data;
                esmtp_address_list_add( list, mbox->mb_addr_spec, 0, NULL );
            }
        }
    }
}

char *Generatemail::getFrom( mailimf_field *ffrom) {
    char *from = NULL;
    if ( ffrom && (ffrom->fld_type == MAILIMF_FIELD_FROM)
            && ffrom->fld_data.fld_from->frm_mb_list && ffrom->fld_data.fld_from->frm_mb_list->mb_list ) {
        clist *cl = ffrom->fld_data.fld_from->frm_mb_list->mb_list;
        clistiter *it;
        for ( it = clist_begin( cl ); it; it = it->next ) {
            mailimf_mailbox *mb = (mailimf_mailbox *) it->data;
            from = strdup( mb->mb_addr_spec );
        }
    }

    return from;
}

char *Generatemail::getFrom( mailmime *mail ) {
    /* no need to delete - its just a pointer to structure content */
    mailimf_field *ffrom = 0;
    ffrom = getField( mail->mm_data.mm_message.mm_fields, MAILIMF_FIELD_FROM );
    return getFrom(ffrom);
}

mailimf_field *Generatemail::getField( mailimf_fields *fields, int type ) {
    mailimf_field *field;
    clistiter *it;

    it = clist_begin( fields->fld_list );
    while ( it ) {
        field = (mailimf_field *) it->data;
        if ( field->fld_type == type ) {
            return field;
        }
        it = it->next;
    }

    return NULL;
}

mailimf_address_list *Generatemail::parseAddresses(const QString&addr ) {
    mailimf_address_list *addresses;

    if ( addr.isEmpty() )
        return NULL;

    addresses = mailimf_address_list_new_empty();

    bool literal_open = false;
    unsigned int startpos = 0;
    QStringList list;
    QString s;
    unsigned int i = 0;
    for (; i < addr.length();++i) {
        switch (addr[i]) {
        case '\"':
            literal_open = !literal_open;
            break;
        case ',':
            if (!literal_open) {
                s = addr.mid(startpos,i-startpos);
                if (!s.isEmpty()) {
                    list.append(s);
                    qDebug("Appended %s",s.latin1());
                }
                // !!!! this is a MUST BE!
                startpos = ++i;
            }
            break;
        default:
            break;
        }
    }
    s = addr.mid(startpos,i-startpos);
    if (!s.isEmpty()) {
        list.append(s);
        qDebug("Appended %s",s.latin1());
    }
    QStringList::Iterator it;
    for ( it = list.begin(); it != list.end(); it++ ) {
        int err = mailimf_address_list_add_parse( addresses, (char*)(*it).latin1() );
        if ( err != MAILIMF_NO_ERROR ) {
            qDebug( "Error parsing" );
            qDebug( *it );
        } else {
            qDebug( "Parse success! %s",(*it).latin1());
        }
    }
    return addresses;
}

mailmime *Generatemail::buildFilePart(const QString&filename,const QString&mimetype,const QString&TextContent ) {
    mailmime * filePart = 0;
    mailmime_fields * fields = 0;
    mailmime_content * content = 0;
    mailmime_parameter * param = 0;
    char*name = 0;
    char*file = 0;
    int err;

    int pos = filename.findRev( '/' );

    if (filename.length()>0) {
        QString tmp = filename.right( filename.length() - ( pos + 1 ) );
        name = strdup( tmp.latin1() );        // just filename
        file = strdup( filename.latin1() );   // full name with path
    }

    int disptype = MAILMIME_DISPOSITION_TYPE_ATTACHMENT;
    int mechanism = MAILMIME_MECHANISM_BASE64;

    if ( mimetype.startsWith( "text/" ) ) {
        param = mailmime_parameter_new( strdup( "charset" ),
                                        strdup( "iso-8859-1" ) );
        mechanism = MAILMIME_MECHANISM_QUOTED_PRINTABLE;
    }

    fields = mailmime_fields_new_filename(
                 disptype, name,
                 mechanism );
    content = mailmime_content_new_with_str( (char*)mimetype.latin1() );
    if (content!=0 && fields != 0) {
        if (param) {
            clist_append(content->ct_parameters,param);
            param = 0;
        }
        if (filename.length()>0) {
            QFileInfo f(filename);
            param = mailmime_parameter_new(strdup("name"),strdup(f.fileName().latin1()));
            clist_append(content->ct_parameters,param);
            param = 0;
        }
        filePart = mailmime_new_empty( content, fields );
    }
    if (filePart) {
        if (filename.length()>0) {
            err = mailmime_set_body_file( filePart, file );
        } else {
            err = mailmime_set_body_text(filePart,strdup(TextContent.data()),TextContent.length());
        }
        if (err != MAILIMF_NO_ERROR) {
            qDebug("Error setting body with file %s",file);
            mailmime_free( filePart );
            filePart = 0;
        }
    }

    if (!filePart) {
        if ( param != NULL ) {
            mailmime_parameter_free( param );
        }
        if (content) {
            mailmime_content_free( content );
        }
        if (fields) {
            mailmime_fields_free( fields );
        } else {
            if (name) {
                free( name );
            }
            if (file) {
                free( file );
            }
        }
    }
    return filePart;        // Success :)

}

void Generatemail::addFileParts( mailmime *message,const QList<Attachment>&files ) {
    const Attachment *it;
    unsigned int count = files.count();
    qDebug("List contains %i values",count);
    for ( unsigned int i = 0; i < count; ++i ) {
        qDebug( "Adding file" );
        mailmime *filePart;
        int err;
        it = ((QList<Attachment>)files).at(i);

        filePart = buildFilePart( it->getFileName(), it->getMimeType(),"" );
        if ( filePart == NULL ) {
            qDebug( "addFileParts: error adding file:" );
            qDebug( it->getFileName() );
            continue;
        }
        err = mailmime_smart_add_part( message, filePart );
        if ( err != MAILIMF_NO_ERROR ) {
            mailmime_free( filePart );
            qDebug("error smart add");
        }
    }
}

mailmime *Generatemail::buildTxtPart(const QString&str ) {
    mailmime *txtPart;
    mailmime_fields *fields;
    mailmime_content *content;
    mailmime_parameter *param;
    int err;

    param = mailmime_parameter_new( strdup( "charset" ),
                                    strdup( "iso-8859-1" ) );
    if ( param == NULL )
        goto err_free;

    content = mailmime_content_new_with_str( "text/plain" );
    if ( content == NULL )
        goto err_free_param;

    err = clist_append( content->ct_parameters, param );
    if ( err != MAILIMF_NO_ERROR )
        goto err_free_content;

    fields = mailmime_fields_new_encoding(MAILMIME_MECHANISM_8BIT);
    if ( fields == NULL )
        goto err_free_content;

    txtPart = mailmime_new_empty( content, fields );
    if ( txtPart == NULL )
        goto err_free_fields;

    err = mailmime_set_body_text( txtPart, (char*)str.data(), str.length() );
    if ( err != MAILIMF_NO_ERROR )
        goto err_free_txtPart;

    return txtPart;     // Success :)

err_free_txtPart:
    mailmime_free( txtPart );
err_free_fields:
    mailmime_fields_free( fields );
err_free_content:
    mailmime_content_free( content );
err_free_param:
    mailmime_parameter_free( param );
err_free:
    qDebug( "buildTxtPart - error" );

    return NULL;        // Error :(
}

mailimf_mailbox *Generatemail::newMailbox(const QString&name, const QString&mail ) {
    return mailimf_mailbox_new( strdup( name.latin1() ),
                                strdup( mail.latin1() ) );
}

mailimf_fields *Generatemail::createImfFields(const Opie::osmart_pointer<Mail>&mail )
{
    mailimf_fields *fields = NULL;
    mailimf_field *xmailer = NULL;
    mailimf_mailbox *sender=0,*fromBox=0;
    mailimf_mailbox_list *from=0;
    mailimf_address_list *to=0, *cc=0, *bcc=0, *reply=0;
    clist*in_reply_to = 0;
    char *subject = strdup( mail->getSubject().latin1() );
    int err;
    int res = 1;

    sender = newMailbox( mail->getName(), mail->getMail() );
    if ( sender == NULL ) {
        res = 0;
    }

    if (res) {
        fromBox = newMailbox( mail->getName(), mail->getMail() );
    }
    if ( fromBox == NULL ) {
        res = 0;
    }

    if (res) {
        from = mailimf_mailbox_list_new_empty();
    }
    if ( from == NULL ) {
        res = 0;
    }

    if (res && from) {
        err = mailimf_mailbox_list_add( from, fromBox );
        if ( err != MAILIMF_NO_ERROR ) {
            res = 0;
        }
    }

    if (res) to = parseAddresses( mail->getTo() );
    if (res) cc = parseAddresses( mail->getCC() );
    if (res) bcc = parseAddresses( mail->getBCC() );
    if (res) reply = parseAddresses( mail->getReply() );

    if (res && mail->Inreply().count()>0) {
        in_reply_to = clist_new();
        char*c_reply;
        unsigned int nsize = 0;
        for (QStringList::ConstIterator it=mail->Inreply().begin();
             it != mail->Inreply().end();++it) {
            if ((*it).isEmpty())
                continue;
            QString h((*it));
            while (h.length()>0 && h[0]=='<') {
                h.remove(0,1);
            }
            while (h.length()>0 && h[h.length()-1]=='>') {
                h.remove(h.length()-1,1);
            }
            if (h.isEmpty()) continue;
            nsize = strlen(h.latin1());
            /* yes! must be malloc! */
            c_reply = (char*)malloc( (nsize+1)*sizeof(char));
            memset(c_reply,0,nsize+1);
            memcpy(c_reply,h.latin1(),nsize);
            clist_append(in_reply_to,c_reply);
            qDebug("In reply to: %s",c_reply);
        }
    }

    if (res) {
        fields = mailimf_fields_new_with_data( from, sender, reply, to, cc, bcc,
                                           in_reply_to, NULL, subject );
        if ( fields == NULL ) {
            qDebug("Error creating mailimf fields");
            res = 0;
        }
    }
    if (res) xmailer = mailimf_field_new_custom( strdup( "User-Agent" ),
                                        strdup( USER_AGENT ) );
    if ( xmailer == NULL ) {
        res = 0;
    } else {
        err = mailimf_fields_add( fields, xmailer );
        if ( err != MAILIMF_NO_ERROR ) {
            res = 0;
        }
    }
    if (!res ) {
        if (xmailer) {
            mailimf_field_free( xmailer );
            xmailer = NULL;
        }
        if (fields) {
            mailimf_fields_free( fields );
            fields = NULL;
        } else {
            if (reply)
                mailimf_address_list_free( reply );
            if (bcc)
                mailimf_address_list_free( bcc );
            if (cc)
                mailimf_address_list_free( cc );
            if (to)
                mailimf_address_list_free( to );
            if (fromBox) {
                mailimf_mailbox_free( fromBox );
            } else if (from) {
                mailimf_mailbox_list_free( from );
            }
            if (sender) {
                mailimf_mailbox_free( sender );
            }
            if (subject) {
                free( subject );
            }
        }
    }
    return fields;
}

mailmime *Generatemail::createMimeMail(const Opie::osmart_pointer<Mail> &mail ) {
    mailmime *message, *txtPart;
    mailimf_fields *fields;
    int err;

    fields = createImfFields( mail );
    if ( fields == NULL )
        goto err_free;

    message = mailmime_new_message_data( NULL );
    if ( message == NULL )
        goto err_free_fields;

    mailmime_set_imf_fields( message, fields );

    txtPart = buildTxtPart( mail->getMessage() );

    if ( txtPart == NULL )
        goto err_free_message;

    err = mailmime_smart_add_part( message, txtPart );
    if ( err != MAILIMF_NO_ERROR )
        goto err_free_txtPart;

    addFileParts( message, mail->getAttachments() );

    return message;         // Success :)

err_free_txtPart:
    mailmime_free( txtPart );
err_free_message:
    mailmime_free( message );
err_free_fields:
    mailimf_fields_free( fields );
err_free:
    qDebug( "createMimeMail: error" );

    return NULL;            // Error :(
}

clist *Generatemail::createRcptList( mailimf_fields *fields ) {
    clist *rcptList;
    mailimf_field *field;

    rcptList = esmtp_address_list_new();

    field = getField( fields, MAILIMF_FIELD_TO );
    if ( field && (field->fld_type == MAILIMF_FIELD_TO)
            && field->fld_data.fld_to->to_addr_list ) {
        addRcpts( rcptList, field->fld_data.fld_to->to_addr_list );
    }

    field = getField( fields, MAILIMF_FIELD_CC );
    if ( field && (field->fld_type == MAILIMF_FIELD_CC)
            && field->fld_data.fld_cc->cc_addr_list ) {
        addRcpts( rcptList, field->fld_data.fld_cc->cc_addr_list );
    }

    field = getField( fields, MAILIMF_FIELD_BCC );
    if ( field && (field->fld_type == MAILIMF_FIELD_BCC)
            && field->fld_data.fld_bcc->bcc_addr_list ) {
        addRcpts( rcptList, field->fld_data.fld_bcc->bcc_addr_list );
    }

    return rcptList;
}
