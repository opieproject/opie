/*
 *  GPLv2
 */

#ifndef THUMB_NAIL_TOOL_H
#define THUMB_NAIL_TOOL_H
class QString;
class QPixmap;
class QImage;

struct ThumbNailTool {
    static QPixmap scaleImage( QImage&, int width, int height );
/* get one isInvalid() if non found */
    static QPixmap getThumb( const QString&, int width, int height );
/* put one */
    static void putThumb( const QString&, const QPixmap&, int width, int heigh );
};

#endif
