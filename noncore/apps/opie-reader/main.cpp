#include <qpe/qpeapplication.h>
//#include <qmainwindow.h>
#include "QTReaderApp.h"





/*
class myapp : public QPEApplication
{
 public slots:
    void receive( const QCString& msg, const QByteArray& data )
    {

        QDataStream stream( data, IO_ReadOnly );
        if ( msg == "someMessage(int,int,int)" ) {
            int a,b,c;
            stream >> a >> b >> c;
            ...
        } else if ( msg == "otherMessage(QString)" ) {
            ...
        }

    }
}
*/

int main( int argc, char ** argv )
{
    QPEApplication a( argc, argv );

    QTReaderApp m;
    a.showMainDocumentWidget( &m );

    return a.exec();
}

