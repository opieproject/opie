#include <unistd.h>

#include <qdir.h>
#include <qstring.h>
#include <qfile.h>

#include "io_layer.h"
#include "receive_layer.h"

ReceiveLayer::ReceiveLayer( IOLayer* lay, const QString& startDir )
    : QObject(), m_curDir( startDir ), m_layer(lay )
{

}
ReceiveLayer::~ReceiveLayer() {

}
IOLayer* ReceiveLayer::layer() {
    return m_layer;
}
QString ReceiveLayer::currentDir()const{
    if (m_curDir.isEmpty() )
        return QDir::currentDirPath();
    return m_curDir;
}
void ReceiveLayer::changeDir( const QString& str) {
    ::chdir( str.latin1() );
}
void ReceiveLayer::receive( const QString& dir, Mode, Features ) {
    receive( dir );
}
void ReceiveLayer::cancel() {

}
