#include <qstring.h>
#include "mediadetect.h"


MediaDetect::MediaDetect() {
}

MediaDetect::~MediaDetect() {
}

char MediaDetect::videoOrAudio( const QString& fileName ) {
    if( (fileName.lower()).right(4) == ".avi" ||
        (fileName.lower()).right(4) == ".mpg" ||
        (fileName.lower()).right(4) == ".asf" ||
        (fileName.lower()).right(4) == ".mov" ||
        (fileName.lower()).right(5) == ".mpeg"  ) {
        qDebug("Video out taken");
        return 'v';
    } else if ( (fileName.lower()).right(4) == "·mp1" ||
                (fileName.lower()).right(4) == ".mp3" ||
                (fileName.lower()).right(4) == ".ogg" ||
                (fileName.lower()).right(4) == ".wav" ) {
        qDebug("AUDIO out taken");
        return 'a';
    } else {
         return 'f';
    }
}

bool MediaDetect::isStreaming( const QString& fileName ) {
    // ugly
    if( (fileName.lower()).left(4) == "http" ) {
        return true;
    } else if ( (fileName.lower()).left(3) == "ftp" ) {
        return true;
    } else {
        return false;
    }
}

