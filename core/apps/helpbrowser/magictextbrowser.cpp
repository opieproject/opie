#include <qfile.h>
#include <qdragobject.h>

/* need to get Global::helpPath() */
#define QTOPIA_INTERNAL_LANGLIST

#include <qtopia/global.h>
#include <qtopia/mimetype.h>
#include <qtopia/applnk.h>

#include "magictextbrowser.h"



MagicTextBrowser::MagicTextBrowser(QWidget* parent) :
    QTextBrowser(parent){
}

void MagicTextBrowser::setSource( const QString& source )  {
    QTextBrowser::setSource(source);
    if ( magicQpe(source,"applications") || magicQpe(source,"games") || magicQpe(source,"settings") || magicQpe(source, "1Pim") ) // No tr
        return;
    if ( magicOpe(source, "applets") || magicOpe(source, "input") )
        return;
    // Just those are magic (for now). Could do CGI here,
    // or in Qtopia's mime source factory.
}

bool MagicTextBrowser::magicQpe(const QString& source, const QString& name) {
    if ( name+".html" == source || "help/"+name+".html" == source) {
        QString fn = mimeSourceFactory()->makeAbsolute( source, context() );
        const QMimeSource* m = mimeSourceFactory()->data( fn, context() );
        if ( m ) {
            QString txt;
            if ( QTextDrag::decode(m,txt) ) {
                QRegExp re("<qtopia-"+name+">.*</qtopia-"+name+">");
                int start,len;
                if ( (start=re.match(txt,0,&len))>=0 ) {
                    QString generated = generateQpe(name);
                    txt.replace(start,len,generated);
                    setText(txt);
                    return true;
                }
            }
        }
    }
    return false;
}
bool MagicTextBrowser::magicOpe( const QString& source, const QString& name ) {
    if ( name+".html" != source && "help/"+name+".html" != source) return false;

    QString fn = mimeSourceFactory()->makeAbsolute( source, context() );
    const QMimeSource* m = mimeSourceFactory()->data(fn, context() );
    if (!m) return false;

    QString txt;
    if ( !QTextDrag::decode(m, txt ) ) return false;

    QRegExp re("<opie-"+name+">.*</opie-"+name+">");
    int start,len;
    if ( (start=re.match(txt,0,&len))>=0 ) {
        QString generated = generateOpe(name);
        txt.replace(start,len,generated);
        setText(txt);
        return true;
    }
    return false;
}
QString MagicTextBrowser::generateOpe(const QString& name)const {
    if ( name == QString::fromLatin1("applets") ) {
        return QString::fromLatin1("<h3>No Applets found</h3>");
    }else if ( name == QString::fromLatin1("input") ) {
        return QString::fromLatin1("<h3>No input methods available</h3>");
    }else
         return QString::null;
}

QString MagicTextBrowser::generateQpe(const QString& name) const {
    QString dir = MimeType::appsFolderName()+"/"+name[0].upper()+name.mid(1);
    AppLnkSet lnkset(dir);
    AppLnk* lnk;
    QString r;
    for (QListIterator<AppLnk> it(lnkset.children()); (lnk=it.current()); ++it) {
        QString name = lnk->name();
        QString icon = lnk->icon();
        QString helpFile = lnk->exec()+".html";
        QStringList helpPath = Global::helpPath();
        bool helpExists = FALSE;
        for (QStringList::ConstIterator it=helpPath.begin(); it!=helpPath.end() && !helpExists; ++it)
            helpExists = QFile::exists( *it + "/" + helpFile );

        if ( helpExists ) {
            r += "<h3><a href="+helpFile+"><img src="+icon+">"+name+"</a></h3>\n";
        }
    }
    return r;
}
