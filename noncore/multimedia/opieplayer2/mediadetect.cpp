#include <qstring.h>
#include "mediadetect.h"


MediaDetect::MediaDetect() {
}

MediaDetect::~MediaDetect() {
}

char MediaDetect::videoOrAudio( const QString& fileName ) {
    if( fileName.right(4) == ".avi" ||
        fileName.right(4) == ".mpg" ||
        fileName.right(4) == ".asf" ||
        fileName.right(4) == ".mov" ||
        fileName.right(5) == ".mpeg"  ) {
        return 'v';
    } else if ( fileName.right(4) == ".avi" ||
                fileName.right(4) == ".mp3" ||
                fileName.right(4) == ".ogg" ||
                fileName.right(4) == ".wav" ) {
        return 'a';
    } else {
         return 'f';
    }
}

bool MediaDetect::isStreaming( const QString& fileName ) {
    // ugly
    if( fileName.left(4) == "http" ) {
        return true;
    } else if (fileName.left(3) == "ftp" ) {
        return true;
    } else {
        return false;
    }
}

