#ifndef BITFONT_H
#define BITFONT_H

#include <qstring.h>
#include <qbitmap.h>
#include <qpixmap.h>
#include <qrect.h>

#include "colors.h"

class Bitfont
{
public:
    Bitfont(QString fontname, uchar firstChar, uchar lastChar);

    QPixmap text(QString str, QColor fg = BLACK, QColor bg = QColor());
    QRect rect(QString str);
    int width();
    int height();
    uchar firstChar();
    uchar lastChar();
private:
    QBitmap font;
    int fontWidth;
    int fontHeight;
    uchar fontFirstChar;
    uchar fontLastChar;
};

#endif // BITFONT_H

