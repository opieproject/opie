
#include "portable.h"

#if defined( KDE2_PORT )
#include <kapp.h>
#include <kconfig.h>
#include <kstddirs.h>
#elif defined( QPE_PORT )
#include <qpe/qpeapplication.h>
#include "config.h"
#endif

#include <qcolor.h>
#include <qpainter.h>
#include <qpixmap.h>
#include <qbitmap.h>
#include <qrect.h>
#include <qstring.h>

#include <qmessagebox.h>
#include <qfileinfo.h>

#include "painter.h"
#include "board.h"

Painter::Painter( Board *b, QWidget *parent, int Scheme, int Mode, Bitfont *font)
{
    w = parent;
    board = b;

    pointPix = NULL;
    wallPix = NULL;
    prisonPix = NULL;
    energizerPix = NULL;
    fruitPix = NULL;
    pacmanPix = NULL;
    dyingPix = NULL;
    eyesPix = NULL;
    monsterPix = NULL;
    fruitScorePix = NULL;
    monsterScorePix = NULL;

    lastPointPixmapName = "";
    lastWallPixmapName = "";
    lastPrisonPixmapName = "";
    lastEnergizerPixmapName = "";
    lastFruitPixmapName = "";
    lastPacmanPixmapName = "";
    lastDyingPixmapName = "";
    lastEyesPixmapName = "";
    lastMonsterPixmapName = "";
    lastFruitScorePixmapName = "";
    lastMonsterScorePixmapName = "";

    bitfont = font;

    scheme = Scheme;
    mode = Mode;
    level = 0;

    confScheme();
}

QList<QPixmap> *Painter::loadPixmap(QWidget*, QString pixmapName,
                                    QList<QPixmap> *pixmaps)
{
    if (pixmaps == NULL) {
        pixmaps = new QList<QPixmap>;
        pixmaps->setAutoDelete(TRUE);
    }

    if (!pixmaps->isEmpty())
        pixmaps->clear();

    QPixmap PIXMAP(pixmapName);
    if (PIXMAP.isNull() || PIXMAP.mask() == NULL) {
        QString msg = "The pixmap could not be contructed.\n\n"
                           "The file '@PIXMAPNAME@' does not exist,\n"
                           "or is of an unknown format.";
        msg.replace(QRegExp("@PIXMAPNAME@"), pixmapName);
        // QMessageBox::critical(parent, tr("Initialization Error"), msg);
        printf("%s\n", msg.data());
        return 0;
    }

    int height = PIXMAP.height();
    int width = (height == 0) ? 0 : PIXMAP.width()/(PIXMAP.width()/height);

    QBitmap BITMAP;
    QBitmap MASK;

    BITMAP = *PIXMAP.mask();
    MASK.resize(width, height);

    for (int x = 0; x < PIXMAP.width()/width; x++) {
        QPixmap *pixmap = new QPixmap(width, height);
        pixmaps->append(pixmap);
        bitBlt(pixmap, 0, 0, &PIXMAP, x*width, 0, width, height, QPixmap::CopyROP, TRUE);
        bitBlt(&MASK, 0, 0, &BITMAP, x*width, 0, width, height, QPixmap::CopyROP, TRUE);
        pixmap->setMask(MASK);
    }

    return pixmaps;
}

QList<QPixmap> *Painter::textPixmap(QStrList &str, QList<QPixmap> *pixmaps,
                                    QColor fg, QColor bg)
{
    if (pixmaps == NULL) {
        pixmaps = new QList<QPixmap>;
        pixmaps->setAutoDelete(TRUE);
    }

    if (!pixmaps->isEmpty())
        pixmaps->clear();

    for (uint s = 0; s < str.count(); s++) {
        QPixmap *pixmap = new QPixmap(bitfont->text(str.at(s), fg, bg));
        pixmaps->append(pixmap);
    }

    return pixmaps;
}

QList<QPixmap> *Painter::textPixmap(QString str, QList<QPixmap> *pixmaps,
                                    QColor fg, QColor bg)
{
    if (pixmaps == NULL) {
        pixmaps = new QList<QPixmap>;
        pixmaps->setAutoDelete(TRUE);
    }

    if (!pixmaps->isEmpty())
        pixmaps->clear();

    QPixmap *pixmap = new QPixmap(bitfont->text(str, fg, bg));
    pixmaps->append(pixmap);

    return pixmaps;
}

/* Return the point of the upperleft pixel of the block representing that position
 * on the board.
 */
QPoint Painter::point(int pos)
{
    return QPoint((board->x(pos)-1)*BlockWidth, (board->y(pos)-1)*BlockHeight);
}


QRect Painter::rect(int pos, PixMap pix, uint i)
{
    if (pos == OUT)
        return QRect();

    QPixmap *PIXMAP = NULL;
    switch (pix) {
        case PacmanPix       : PIXMAP = pacmanPix->
                                        at(checkRange(i, pacmanPix->count()-1));
                               break;
        case DyingPix        : PIXMAP = dyingPix->
                                        at(checkRange(i, dyingPix->count()-1));
                               break;
        case MonsterPix      : PIXMAP = monsterPix->
                                        at(checkRange(i, monsterPix->count()-1));
                               break;
        case EyesPix         : PIXMAP = eyesPix->
                                        at(checkRange(i, eyesPix->count()-1));
                               break;
        case FruitPix        : PIXMAP = fruitPix->
                                        at(checkRange(i, fruitPix->count()-1));
                               break;
        case PointPix        : PIXMAP = pointPix->
                                        at(checkRange(i, pointPix->count()-1));
                               break;
        case EnergizerPix    : PIXMAP = energizerPix->
                                        at(checkRange(i, energizerPix->count()-1));
                               break;
        case FruitScorePix   : PIXMAP = fruitScorePix->
                                        at(checkRange(i, fruitScorePix->count()-1));
                               break;
        case MonsterScorePix : PIXMAP = monsterScorePix->
                                        at(checkRange(i,monsterScorePix->count()-1));
                               break;
        default              : PIXMAP = wallPix->
                                        at(checkRange(i, wallPix->count()-1));
    }
    if (PIXMAP == NULL)
        return QRect();

    QRect rect = PIXMAP->rect();
    QPoint point = this->point(pos);
    rect.moveCenter(QPoint(point.x()-1, point.y()-1));
  
    return rect;
}

QRect Painter::rect(int pos, QString str, int align)
{
    if (pos == OUT)                     // return an empty rect if the position
        return QRect();                 // is invalid
    QPoint point = this->point(pos);
    QRect rect = bitfont->rect(str);

    rect.moveCenter(QPoint(point.x()-1, point.y()-1));

    int dx = 0;
    int dy = 0;

    if (align & QLabel::AlignLeft || align & QLabel::AlignRight) {
        dx = (str.length()-1) * (bitfont->width()/2);
        if (align & QLabel::AlignRight)
            dx *= -1;
    }

    if (align & QLabel::AlignTop || align & QLabel::AlignBottom) {
        dy = bitfont->height()/2;
        if (align & QLabel::AlignBottom)
            dy *= -1;
    }

    if (dx != 0 || dy != 0)
        rect.moveBy(dx, dy);

    return rect;
}

QRect Painter::rect(QRect r1, QRect r2)
{
    QRect rect;
    rect.setLeft(r1.left() < r2.left() ? r1.left() : r2.left());
    rect.setTop(r1.top() < r2.top() ? r1.top() : r2.top());
    rect.setRight(r1.right() > r2.right() ? r1.right() : r2.right());
    rect.setBottom(r1.bottom() > r2.bottom() ? r1.bottom() : r2.bottom());

    return rect;
}

void Painter::erase(int pos, PixMap pix, uint i)
{
    if (pos == OUT)
        return;
    QRect rect = this->rect(pos, pix, i);
    bitBlt(&roomPix, rect.x(), rect.y(), &backPix,
           rect.x(), rect.y(), rect.width(), rect.height());
}

int Painter::maxPixmaps(PixMap pix)
{
    switch (pix) {
        case WallPix         : return (int) wallPix->count();
        case PrisonPix       : return (int) prisonPix->count();
        case PointPix        : return (int) pointPix->count();
        case EnergizerPix    : return (int) energizerPix->count();
        case FruitPix        : return (int) fruitPix->count();
        case PacmanPix       : return (int) pacmanPix->count();
        case DyingPix        : return (int) dyingPix->count();
        case EyesPix         : return (int) eyesPix->count();
        case MonsterPix      : return (int) monsterPix->count();
        case FruitScorePix   : return (int) fruitScorePix->count();
        case MonsterScorePix : return (int) monsterScorePix->count();
        default              : return 0;
    }
}
        
void Painter::draw(QPoint point, DrawWidget where, QPixmap pix)
{
    switch (where) {
        case Widget   : bitBlt(w, point.x(), point.y(), &pix);
                        break;
        case RoomPix  : bitBlt(&roomPix, point.x(), point.y(), &pix);
                        break;
        case BackPix  : bitBlt(&backPix, point.x(), point.y(), &pix);
                        break;
    }   
}

void Painter::draw(QRect rect, DrawWidget where, QPixmap pix)
{
    draw(QPoint(rect.x(), rect.y()), where, pix);
}

void Painter::draw(int pos, DrawWidget where, PixMap pix, uint i)
{
    QPixmap *PIXMAP = NULL;
    switch (pix) {
        case PacmanPix       : PIXMAP = pacmanPix->
                                        at(checkRange(i, pacmanPix->count()-1));
                               break;
        case DyingPix        : PIXMAP = dyingPix->
                                        at(checkRange(i, dyingPix->count()-1));
                               break;
        case MonsterPix      : PIXMAP = monsterPix->
                                        at(checkRange(i, monsterPix->count()-1));
                               break;
        case EyesPix         : PIXMAP = eyesPix->
                                        at(checkRange(i, eyesPix->count()-1));
                               break;
        case FruitPix        : PIXMAP = fruitPix->
                                        at(checkRange(i, fruitPix->count()-1));
                               break;
        case EnergizerPix    : PIXMAP = energizerPix->
                                        at(checkRange(i, energizerPix->count()-1));
                               break;
        case FruitScorePix   : PIXMAP = fruitScorePix->
                                        at(checkRange(i, fruitScorePix->count()-1));
                               break;
        case MonsterScorePix : PIXMAP = monsterScorePix->
                                        at(checkRange(i,monsterScorePix->count()-1));
                               break;
        default              : ;
    }

    if (PIXMAP == NULL)
        return;

    QRect rect = PIXMAP->rect();
    QPoint point = this->point(pos);
    rect.moveCenter(QPoint(point.x()-1, point.y()-1));

    switch (where) {
        case Widget   : bitBlt(w, rect.x(), rect.y(), PIXMAP); 
                        break;
        case RoomPix  : bitBlt(&roomPix, rect.x(), rect.y(), PIXMAP);
                        break;
        case BackPix  : bitBlt(&backPix, rect.x(), rect.y(), PIXMAP);
                        break;
    }   
}

QPixmap Painter::draw(int pos, DrawWidget where,
                      QString str, QColor fg, QColor bg, int align)
{
    QPixmap TEXT = bitfont->text(str, fg, bg);

    QRect rect = this->rect(pos, str, align);
    QPixmap SAVE = QPixmap(rect.width(), rect.height());

    switch (where) {
        case Widget  : bitBlt(&SAVE, 0, 0, w, rect.x(), rect.y());
                       bitBlt(w, rect.x(), rect.y(), &TEXT);
                       break;
        case RoomPix : bitBlt(&SAVE, 0, 0, &roomPix, rect.x(), rect.y());
                       bitBlt(&roomPix, rect.x(), rect.y(), &TEXT);
                       break;
        case BackPix : bitBlt(&SAVE, 0, 0, &backPix, rect.x(), rect.y());
                       bitBlt(&backPix, rect.x(), rect.y(), &TEXT);
                       break;
    }

    return SAVE;
}

QRect Painter::draw(int col, int row, DrawWidget where,
                    QString str, QColor fg, QColor bg, int align)
{
    QPixmap TEXT = bitfont->text(str, fg, bg);

    QRect rect = this->rect(row*BoardWidth+col, str, align);
    draw(rect, where, TEXT);
     
    return rect;
}

void Painter::initPixmaps()
{
    if (lastPointPixmapName != pointPixmapName.at(level)) {
        pointPix = loadPixmap(w, pointPixmapName.at(level), pointPix);
        lastPointPixmapName = pointPixmapName.at(level);
    }
    if (lastPrisonPixmapName != prisonPixmapName.at(level)) {
        prisonPix = loadPixmap(w, prisonPixmapName.at(level), prisonPix);
        lastPrisonPixmapName = prisonPixmapName.at(level);
    }
    if (lastEnergizerPixmapName != energizerPixmapName.at(level)) {
        energizerPix = loadPixmap(w, energizerPixmapName.at(level), energizerPix);
        lastEnergizerPixmapName = energizerPixmapName.at(level);
    }
    if (lastFruitPixmapName != fruitPixmapName.at(level)) {
        fruitPix = loadPixmap(w, fruitPixmapName.at(level), fruitPix);
        lastFruitPixmapName = fruitPixmapName.at(level);
    }
    if (lastPacmanPixmapName != pacmanPixmapName.at(level)) {
        pacmanPix = loadPixmap(w, pacmanPixmapName.at(level), pacmanPix);
        lastPacmanPixmapName = pacmanPixmapName.at(level);
    }
    if (lastDyingPixmapName != dyingPixmapName.at(level)) {
        dyingPix = loadPixmap(w, dyingPixmapName.at(level), dyingPix);
        lastDyingPixmapName = dyingPixmapName.at(level);
    }
    if (lastEyesPixmapName != eyesPixmapName.at(level)) {
        eyesPix = loadPixmap(w, eyesPixmapName.at(level), eyesPix);
        lastEyesPixmapName = eyesPixmapName.at(level);
    }
    if (lastMonsterPixmapName != monsterPixmapName.at(level)) {
        monsterPix = loadPixmap(w, monsterPixmapName.at(level), monsterPix);
        lastMonsterPixmapName = monsterPixmapName.at(level);
    }

    if (lastFruitScorePixmapName != fruitScorePixmapName.at(level) ||
        (const char *) *fruitScorePixmapName.at(level) == '\0') {
        if ((const char *) *fruitScorePixmapName.at(level) == '\0') {
            fruitScorePix = textPixmap(fruitScoreString, fruitScorePix, PINK);
        } else {
            fruitScorePix = loadPixmap(w, fruitScorePixmapName.at(level),
                                       fruitScorePix);
            lastFruitScorePixmapName = fruitScorePixmapName.at(level);
        }
    }

    if (lastMonsterScorePixmapName != monsterScorePixmapName.at(level) ||
        (const char *) *monsterScorePixmapName.at(level) == '\0') {
        if ((const char *) *monsterScorePixmapName.at(level) == '\0') {
            monsterScorePix = textPixmap(monsterScoreString, monsterScorePix, CYAN);
        } else {
            monsterScorePix = loadPixmap(w, monsterScorePixmapName.at(level),
                                         monsterScorePix);
            lastMonsterScorePixmapName = monsterScorePixmapName.at(level);
        }
    }

    if (lastWallPixmapName != wallPixmapName.at(level)) {
        wallPix = loadPixmap(w, wallPixmapName.at(level), wallPix);
        if (wallPix->isEmpty()) {
            BlockWidth = 0;
            BlockHeight = 0;
        } else {
            BlockWidth = wallPix->at(0)->width();
            BlockHeight = wallPix->at(0)->height();
        }
        lastWallPixmapName = wallPixmapName.at(level);
    }
}

void Painter::initbackPixmaps()
{
    backgroundColor = BLACK;

    backPix.resize((BoardWidth-3)*BlockWidth, (BoardHeight-3)*BlockHeight );
    backPix.fill(backgroundColor);
}

void Painter::initRoomPixmap()
{
    roomPix.resize((BoardWidth-3)*BlockWidth, (BoardHeight-3)*BlockHeight );
    bitBlt(&roomPix,0,0, &backPix);

    for (unsigned int x = 0; x < board->size(); x++) {
        if (board->isBrick(x))
            drawBrick(x);
        if (board->isPrison(x) || board->isGate(x))
            drawPrison(x);
        if (board->isPoint(x))
            drawPoint(x);
    }
}

void Painter::drawBrick(int pos)
{
    int border = 0;
    if (board->isBrick(board->move(pos, N ))) border |= (1 << 0);
    if (board->isBrick(board->move(pos, NE))) border |= (1 << 1);
    if (board->isBrick(board->move(pos, E ))) border |= (1 << 2);
    if (board->isBrick(board->move(pos, SE))) border |= (1 << 3);
    if (board->isBrick(board->move(pos, S ))) border |= (1 << 4);
    if (board->isBrick(board->move(pos, SW))) border |= (1 << 5);
    if (board->isBrick(board->move(pos, W ))) border |= (1 << 6);
    if (board->isBrick(board->move(pos, NW))) border |= (1 << 7);

    if (board->isOut(board->move(pos, N ))) border |= (1 << 8);
    if (board->isOut(board->move(pos, NE))) border |= (1 << 9);
    if (board->isOut(board->move(pos, E ))) border |= (1 << 10);
    if (board->isOut(board->move(pos, SE))) border |= (1 << 11);
    if (board->isOut(board->move(pos, S ))) border |= (1 << 12);
    if (board->isOut(board->move(pos, SW))) border |= (1 << 13);
    if (board->isOut(board->move(pos, W ))) border |= (1 << 14);
    if (board->isOut(board->move(pos, NW))) border |= (1 << 15);

    switch (border & 0xFF)  {
        case  31 : border =  0; break;
        case 159 : border =  0; break;
        case  63 : border =  0; break;
        case 191 : border =  0; break;
        case 241 : border =  1; break;
        case 249 : border =  1; break;
        case 243 : border =  1; break;
        case 251 : border =  1; break;
        case 124 : border =  2; break;
        case 252 : border =  2; break;
        case 126 : border =  2; break;
        case 199 : border =  3; break;
        case 231 : border =  3; break;
        case 207 : border =  3; break;
        case  28 : if ((border >> 8) > 0)
                      border = 24;
                   else
                      border =  4;
                   break;
        case 112 : if ((border >> 8) > 0)
                      border = 27;
                   else
                      border =  5;
                   break;
        case   7 : if ((border >> 8) > 0)
                      border = 25;
                   else
                      border =  6;
                   break;
        case 193 : if ((border >> 8) > 0)
                      border = 26;
                   else
                      border =  7;
                   break;
        case 247 : if ((border & (1 << 11)) > 0)
                      border = 23;
                   else
                      border =  8;
                   break;
        case 119 : if ((border & (1 << 15)) > 0)
                      border =  8;
                   if ((border & (1 << 11)) > 0)
                      border = 11;
                   break;
        case 223 : if ((border & (1 << 13)) > 0)
                      border = 20;
                   else
                      border =  9;
                   break;
        case 221 : if ((border & (1 << 13)) > 0)
                      border = 10;
                   if ((border & (1 << 9)) > 0)
                      border = 9;
                   break;
        case 253 : if ((border & (1 << 9)) > 0)
                      border = 21;
                   else
                      border = 10;
                   break;
        case 127 : if ((border & (1 << 15)) > 0)
                      border = 22;
                   else
                      border = 11;
                   break;
        case  30 : border = 12; break;
        case 240 : border = 13; break;
        case  15 : border = 14; break;
        case 225 : border = 15; break;
        case 135 : border = 16; break;
        case 195 : border = 17; break;
        case  60 : border = 18; break;
        case 120 : border = 19; break;
        case 255 : border = 28; break;
        default  : border = -1;
    }
    if (border != -1 &&  border < (int) wallPix->count()) {
        QRect rect = this->rect(pos, WallPix);
        bitBlt(&roomPix, rect.x(), rect.y(), wallPix->at((uint) border));
    }
}

void Painter::drawPrison(int pos)
{
    int border = 0;
    if (board->isPrison(board->move(pos, N ))) border |= (1 << 0);
    if (board->isPrison(board->move(pos, NE))) border |= (1 << 1);
    if (board->isPrison(board->move(pos, E ))) border |= (1 << 2);
    if (board->isPrison(board->move(pos, SE))) border |= (1 << 3);
    if (board->isPrison(board->move(pos, S ))) border |= (1 << 4);
    if (board->isPrison(board->move(pos, SW))) border |= (1 << 5);
    if (board->isPrison(board->move(pos, W ))) border |= (1 << 6);
    if (board->isPrison(board->move(pos, NW))) border |= (1 << 7);

    if (board->isGate(board->move(pos, N ))) border |= (1 << 8);
    if (board->isGate(board->move(pos, NE))) border |= (1 << 9);
    if (board->isGate(board->move(pos, E ))) border |= (1 << 10);
    if (board->isGate(board->move(pos, SE))) border |= (1 << 11);
    if (board->isGate(board->move(pos, S ))) border |= (1 << 12);
    if (board->isGate(board->move(pos, SW))) border |= (1 << 13);
    if (board->isGate(board->move(pos, W ))) border |= (1 << 14);
    if (board->isGate(board->move(pos, NW))) border |= (1 << 15);

    switch (border & 0xFF)  {
        case  31 : border =  0; break;
        case 159 : border =  0; break;
        case  63 : border =  0; break;
        case 241 : border =  1; break;
        case 249 : border =  1; break;
        case 243 : border =  1; break;
        case 124 : border =  2; break;
        case 252 : border =  2; break;
        case 126 : border =  2; break;
        case 199 : border =  3; break;
        case 231 : border =  3; break;
        case 207 : border =  3; break;
        case  28 : if ((border >> 8) != 0)
                      border = 12;
                   else
                      border =  4;
                   break;
        case 112 : if ((border >> 8) != 0)
                      border = 13;
                   else
                      border =  5;
                   break;
        case   7 : if ((border >> 8) != 0)
                      border = 14;
                   else
                      border =  6;
                   break;
        case 193 : if ((border >> 8) != 0)
                      border = 15;
                   else
                      border =  7;
                   break;
        case 247 : border =  8; break;
        case 223 : border =  9; break;
        case 253 : border = 10; break;
        case 127 : border = 11; break;
        default  : border = -1;
    }
    if (board->isGate(pos)) {
        if (board->isGate(board->move(pos, N)))
            border = 17;
        else
            border = 16;
    }

    if (border != -1 &&  border < (int) prisonPix->count()) {
        QRect rect = this->rect(pos, PrisonPix);
        bitBlt(&roomPix, rect.x(), rect.y(), prisonPix->at((uint) border));
    }
}

void Painter::drawPoint(int pos)
{
    if (!pointPix->isEmpty()) {
        QRect rect = this->rect(pos, PointPix);
        bitBlt(&roomPix, rect.x(), rect.y(), pointPix->at(0));
    }
}

QString Painter::decodeHexOctString(QString s)
{
    QString value;
    QString valids;
    int pos, xpos = 0, opos = 0;
    int v, len, leadin;
    const char *ptr;
    uchar c;

    while (((xpos = s.find(QRegExp("\\\\x[0-9a-fA-F]+"), xpos)) != -1) ||
           ((opos = s.find(QRegExp("\\\\[0-7]+"), opos)) != -1)) {
        if (xpos != -1) {
            valids = "0123456789abcdef";
            leadin = 2;
            pos = xpos;
        } else {
            valids = "01234567";
            leadin = 1;
            pos = opos;
        }

        c = '\0';
        len = 0;
        value = s.mid(pos+leadin, 3);
        ptr = (const char *) value;

        while (*ptr != '\0' && (v = valids.find(*ptr++, 0, FALSE)) != -1) {
            c = (c * valids.length()) + v;
            len++;
        }

        value.fill(c, 1);
        s.replace(pos, len+leadin, value);
    }

    return s;
}

void Painter::fillScoreString(QStrList &list, QArray<int> &values)
{
    if( !list.isEmpty())
        list.clear();

    QString s;
        
    for (uint i = 0; i < values.size(); i++) {

        if (values[i] < 10 || values[i] > 10000)
            s = "?";
        else if (values[i] == 1600)
            s = "\x1a\x1b";
        else if (values[i] < 100) {
            s = "\x0e";
            s.insert(0, char (values[i] / 10 + 0x10));
        } else if (values[i] < 1000) {
            s = "\x0f";
            s.insert(0, char (values[i] / 100 + 0x10));
        } else {
            s = "\x0f\x10";
            s.insert(0, char (values[i] / 1000 + 0x10));
        }

        list.append(s.data());
    }
}

void Painter::fillArray(QArray<int> &array, QString values, int max)
{
    array.resize(max);
    int last = 0;
    bool ok;
    QString value;

    for (uint i = 0; i < array.size(); i++) {
        if (values.find(',') < 0 &&  values.length() > 0) {
            value = values;
            values = "";
        }
        if (values.find(',') >= 0) {
            value = values.left(values.find(','));
            values.remove(0,values.find(',')+1);
        }
        array[i] = value.toInt(&ok);
        if (ok)
            last = array[i];
        else
            array[i] = last;
    }
}

void Painter::fillStrList(QStrList &list, QString values, int max)
{
    if (!list.isEmpty())
        list.clear();

    QString last = "";
    QString value;

    for (uint i = 0; i < (uint) max; i++) {
        if (values.find(',') < 0 &&  values.length() > 0) {
            value = values;
            values = "";
        }
        if (values.find(',') >= 0) {
            value = values.left(values.find(','));
            values.remove(0,values.find(',')+1);
        }
        if (!value.isEmpty())
            last = decodeHexOctString(value);
        list.append(last);
    }
}

void Painter::fillPixmapName(QStrList &pixmapName)
{
    QStrList list = pixmapName;

    if (!pixmapName.isEmpty())
        pixmapName.clear();

    QString pixmap;

    QFileInfo fileInfo;

    for (uint i = 0; i < list.count(); i++) {
        pixmap = list.at(i);

        if (pixmap.left(1) != "/" && pixmap.left(1) != "~")
            pixmap = FIND_APP_DATA( pixmapDirectory+pixmap );

        fileInfo.setFile(pixmap);
        if (!fileInfo.isReadable() || !fileInfo.isFile())
            pixmap = "";

        pixmapName.append(pixmap);
    }
}

void Painter::confLevels(bool defGroup)
{
    APP_CONFIG_BEGIN( cfg );
    if (defGroup || cfg->hasKey("Levels"))
        maxLevel = cfg->readNumEntry("Levels", 13);
    APP_CONFIG_END( cfg );
}

void Painter::confMisc(bool defGroup)
{
    APP_CONFIG_BEGIN( cfg );
    if (defGroup || cfg->hasKey("PixmapDirectory")) {
        pixmapDirectory = cfg->readEntry("PixmapDirectory");

        if (pixmapDirectory.left(1) != "/" && pixmapDirectory.left(1) != "~")
              pixmapDirectory.insert(0, "pics/");
        if (pixmapDirectory.right(1) != "/")
              pixmapDirectory.append("/");
    }

    if (defGroup || cfg->hasKey("PointPixmapName"))
        fillStrList(pointPixmapName,
                    cfg->readEntry("PointPixmapName", "point.xpm"), maxLevel+1);
    if (defGroup || cfg->hasKey("WallPixmapName"))
        fillStrList(wallPixmapName,
                    cfg->readEntry("WallPixmapName", "wall.xpm"), maxLevel+1);
    if (defGroup || cfg->hasKey("PrisonPixmapName"))
        fillStrList(prisonPixmapName,
                    cfg->readEntry("PrisonPixmapName", "prison.xpm"), maxLevel+1);
    if (defGroup || cfg->hasKey("EnergizerPixmapName"))
        fillStrList(energizerPixmapName,
                    cfg->readEntry("EnergizerPixmapName", "switch.xpm"),maxLevel+1);
    if (defGroup || cfg->hasKey("FruitPixmapName"))
        fillStrList(fruitPixmapName,
                    cfg->readEntry("FruitPixmapName", "fruit.xpm"), maxLevel+1);
    if (defGroup || cfg->hasKey("PacmanPixmapName"))
        fillStrList(pacmanPixmapName,
                    cfg->readEntry("PacmanPixmapName", "pacman.xpm"), maxLevel+1);
    if (defGroup || cfg->hasKey("DyingPixmapName"))
        fillStrList(dyingPixmapName,
                    cfg->readEntry("DyingPixmapName", "dying.xpm"), maxLevel+1);
    if (defGroup || cfg->hasKey("EyesPixmapName"))
        fillStrList(eyesPixmapName,
                    cfg->readEntry("EyesPixmapName", "eyes.xpm"), maxLevel+1);
    if (defGroup || cfg->hasKey("MonsterPixmapName"))
        fillStrList(monsterPixmapName,
                    cfg->readEntry("MonsterPixmapName", "monster.xpm"), maxLevel+1);

    if (defGroup || cfg->hasKey("FruitScorePixmapName"))
        fillStrList(fruitScorePixmapName,
                    cfg->readEntry("FruitScorePixmapName"), maxLevel+1);
    if (defGroup || cfg->hasKey("MonsterScorePixmapName"))
        fillStrList(monsterScorePixmapName,
                    cfg->readEntry("MonsterScorePixmapName"), maxLevel+1);
    APP_CONFIG_END( cfg );
}

void Painter::confScoring(bool defGroup)
{
    APP_CONFIG_BEGIN( cfg );
    if (defGroup || cfg->hasKey("FruitScore"))
        fillArray(fruitScore,
                  cfg->readEntry("FruitScore",
                                  "100,300,500,,700,,1000,,2000,,3000,,5000"),
                                  maxLevel+1);
    if (defGroup || cfg->hasKey("MonsterScore"))
        fillArray(monsterScore,
                  cfg->readEntry("MonsterScore", "200,400,800,1600"), 4);

    if (defGroup || cfg->hasKey("FruitScoreString"))
        fillStrList(fruitScoreString,
                    cfg->readEntry("FruitScoreString"), maxLevel+1);
    if (defGroup || cfg->hasKey("MonsterScoreString"))
        fillStrList(monsterScoreString,
                    cfg->readEntry("MonsterScoreString"), 4);
    APP_CONFIG_END( cfg );
}

void Painter::confScheme()
{
    APP_CONFIG_BEGIN( cfg );
    SAVE_CONFIG_GROUP( cfg, oldgroup );
    QString newgroup;

    // if not set, read mode and scheme from the configfile
    if (mode == -1 && scheme == -1) {
        scheme = cfg->readNumEntry("Scheme", -1);
        mode = cfg->readNumEntry("Mode", -1);

        // if mode is not set in the defGroup-group, lookup the scheme group
        if (scheme != -1 || mode == -1) {
            newgroup.sprintf("Scheme %d", scheme);
            cfg->setGroup(newgroup);

            mode = cfg->readNumEntry("Mode", -1);
            RESTORE_CONFIG_GROUP( cfg, oldgroup );
        }
    }

    confLevels();

    if (mode != -1) {
        newgroup.sprintf("Mode %d", mode);
            cfg->setGroup(newgroup);

      confLevels(FALSE);
    }

    if (scheme != -1) {
        newgroup.sprintf("Scheme %d", scheme);
        cfg->setGroup(newgroup);

        confLevels(FALSE);
    }

    RESTORE_CONFIG_GROUP( cfg, oldgroup );

    confMisc();
    confScoring();

    if (mode != -1) {
        newgroup.sprintf("Mode %d", mode);
        cfg->setGroup(newgroup);

        confMisc(FALSE);
        confScoring(FALSE);
    }

    if (scheme != -1) {
        newgroup.sprintf("Scheme %d", scheme);
        cfg->setGroup(newgroup);

        confMisc(FALSE);
        confScoring(FALSE);
    }

    if ((const char *) *fruitScoreString.at(0) == '\0')
        fillScoreString(fruitScoreString, fruitScore);
    if ((const char *) *monsterScoreString.at(0) == '\0')
        fillScoreString(monsterScoreString, monsterScore);

    fillPixmapName(pointPixmapName);
    fillPixmapName(wallPixmapName);
    fillPixmapName(prisonPixmapName);
    fillPixmapName(energizerPixmapName);
    fillPixmapName(fruitPixmapName);
    fillPixmapName(pacmanPixmapName);
    fillPixmapName(dyingPixmapName);
    fillPixmapName(eyesPixmapName);
    fillPixmapName(monsterPixmapName);
    fillPixmapName(fruitScorePixmapName);
    fillPixmapName(monsterScorePixmapName);

    initPixmaps();
    initbackPixmaps();
    initRoomPixmap();

    RESTORE_CONFIG_GROUP( cfg, oldgroup );
    APP_CONFIG_END( cfg );
}

void Painter::setScheme(int Scheme, int Mode, Bitfont *font)
{
    bitfont = font;

    mode = Mode;
    scheme = Scheme;

    confScheme();
}

void Painter::setLevel(int Level)
{
    level = Level;

    initPixmaps();
    initbackPixmaps();
    initRoomPixmap();
}

int Painter::checkRange(int value, int max, int min)
{
    if (value < min) {
        printf("Painter::checkRange (value = %d, max = %d, min = %d)\n",
               value, max, min);
        return min;
    } else if (value > max) {
        printf("Painter::checkRange (value = %d, max = %d, min = %d)\n",
               value, max, min);
        return max;
    } else
        return value;
}
