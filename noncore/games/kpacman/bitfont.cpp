#include "bitfont.h"

Bitfont::Bitfont(QString fontname, uchar firstChar, uchar lastChar)
{
    if (!fontname.isEmpty())
        font.load(fontname);
    if (font.width() == font.height()) {
        fontWidth = fontHeight = font.width() / 16;
        fontFirstChar = 1;
        fontLastChar = 255;
    } else {
        fontWidth = font.width()/(lastChar-firstChar+1);
        fontHeight = font.height();
        fontFirstChar = firstChar;
        fontLastChar = lastChar;
    }
}

QRect Bitfont::rect(QString str)
{
    return QRect(0, 0, str.length()*fontWidth, fontHeight);
}

QPixmap Bitfont::text(QString str, QColor fg, QColor bg)
{
    QPixmap FG(str.length()*fontWidth, fontHeight);
    QBitmap MASK(str.length()*fontWidth, fontHeight, TRUE);

    const uchar *s = (const uchar *) str.data();
    for (uint i = 0; i < str.length(); i++) {
        if (font.width() == font.height())
            bitBlt(&MASK, i*fontWidth, 0, &font,
                   (*s%16)*fontWidth, (*s/16)*fontWidth, fontWidth, fontHeight);
        else
            if (*s >= fontFirstChar && *s <= fontLastChar)
                bitBlt(&MASK, i*fontWidth, 0, &font,
                       (*s-fontFirstChar)*fontWidth, 0, fontWidth, fontHeight);         
        s++;
    }

    FG.fill(fg);
    FG.setMask(MASK);

    if (bg.isValid()) {
        QPixmap BG(str.length()*fontWidth, fontHeight);
        BG.fill(bg);
        bitBlt(&BG, 0, 0, &FG);
        return BG;
    } else
        return FG;
}

uchar Bitfont::firstChar()
{
    return fontFirstChar;
}

uchar Bitfont::lastChar()
{
    return fontLastChar;
}

int Bitfont::width()
{
    return fontWidth;
}

int Bitfont::height()
{
    return fontHeight;
}
