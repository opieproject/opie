#ifndef MAILWRAPPER_H
#define MAILWRAPPER_H

#include <qpe/applnk.h>

#include <qbitarray.h>
#include <qdatetime.h>

#include "settings.h"

#include <opie2/osmartpointer.h>

class Attachment
{
public:
    Attachment( DocLnk lnk );
    virtual ~Attachment(){}
    const QString getFileName()const{ return doc.file(); }
    const QString getName()const{ return doc.name(); }
    const QString getMimeType()const{ return doc.type(); }
    const QPixmap getPixmap()const{ return doc.pixmap(); }
    const int getSize()const { return size; }
    DocLnk  getDocLnk() { return doc; }

protected:
    DocLnk doc;
    int size;

};

class Mail:public Opie::Core::ORefCount
{
public:
    Mail();
    /* Possible that this destructor must not be declared virtual
     * 'cause it seems that it will never have some child classes.
     * in this case this object will not get a virtual table -> memory and
     * speed will be a little bit better?
     */
    virtual ~Mail(){}
    void addAttachment( Attachment *att ) { attList.append( att ); }
    const QList<Attachment>& getAttachments()const { return attList; }
    void removeAttachment( Attachment *att ) { attList.remove( att ); }
    const QString&getName()const { return name; }
    void setName( QString s ) { name = s; }
    const QString&getMail()const{ return mail; }
    void setMail( const QString&s ) { mail = s; }
    const QString&getTo()const{ return to; }
    void setTo( const QString&s ) { to = s; }
    const QString&getCC()const{ return cc; }
    void setCC( const QString&s ) { cc = s; }
    const QString&getBCC()const { return bcc; }
    void setBCC( const QString&s ) { bcc = s; }
    const QString&getMessage()const { return message; }
    void setMessage( const QString&s ) { message = s; }
    const QString&getSubject()const { return subject; }
    void setSubject( const QString&s ) { subject = s; }
    const QString&getReply()const{ return reply; }
    void setReply( const QString&a ) { reply = a; }
    void setInreply(const QStringList&list){m_in_reply_to = list;}
    const QStringList&Inreply()const{return m_in_reply_to;}

private:
    QList<Attachment> attList;
    QString name, mail, to, cc, bcc, reply, subject, message;
    QStringList m_in_reply_to;
};

class Folder:public Opie::Core::ORefCount
{
public:
    Folder( const QString&init_name,const QString&sep );
    virtual ~Folder();
    const QString&getDisplayName()const { return nameDisplay; }
    const QString&getName()const { return name; }
    const QString&getPrefix()const{return prefix; }
    virtual bool may_select()const{return true;}
    virtual bool no_inferior()const{return true;}
    const QString&Separator()const;

protected:
    QString nameDisplay, name, separator,prefix;
};

typedef Opie::Core::OSmartPointer<Folder> FolderP;

class MHFolder : public Folder
{
public:
    MHFolder(const QString&disp_name,const QString&mbox);
    virtual ~MHFolder();
};

class IMAPFolder : public Folder
{
    public:
        IMAPFolder(const QString&name, const QString&sep, bool select=true,bool noinf=false,const QString&prefix="" );
        virtual ~IMAPFolder();
        virtual bool may_select()const{return m_MaySelect;}
        virtual bool no_inferior()const{return m_NoInferior;}
    private:
        static QString decodeFolderName( const QString &name );
        bool m_MaySelect,m_NoInferior;
};

#endif
