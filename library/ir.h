#ifndef IR_H
#define IR_H

#include <qstring.h>
#include <qobject.h>
class DocLnk;
class QCopChannel;

class Ir : public QObject
{
    Q_OBJECT
public:
    static bool supported();
    
    Ir( QObject *parent = 0, const char *name = 0);
    
    void send( const QString &filename, const QString &description, const QString &mimetype = QString::null );
    void send( const DocLnk &doc, const QString &description );

signals:
    void done( Ir * );
    
private slots:
    void obexMessage( const QCString &msg, const QByteArray &data);
private:
    QString filename;
    QCopChannel *ch;
};


#endif
