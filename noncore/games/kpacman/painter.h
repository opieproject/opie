#ifndef PAINTER_H
#define PAINTER_H

#include "portable.h"

#if defined( KDE2_PORT )
#include <kapp.h>
#include <klocale.h>
#endif

#include <qpixmap.h>
#include <qbitmap.h>
#include <qlabel.h>
#include <qcolor.h>
#include <qlist.h>
#include <qstrlist.h>
#include <qregexp.h>

#include "board.h"
#include "bitfont.h"
#include "colors.h"

enum PixMap { PacmanPix, DyingPix, MonsterPix, EyesPix, FruitPix,
              PointPix, EnergizerPix, WallPix, PrisonPix,
              FruitScorePix, MonsterScorePix };
enum DrawWidget { Widget, RoomPix, BackPix };

class Painter
{
public:
    Painter (Board *, QWidget *parent=0, int scheme=-1, int mode=-1,Bitfont *font=0);
    QPixmap levelPix() { return roomPix; }

    void setScheme(int scheme=-1, int mode=-1, Bitfont *font=0);
    void setLevel(int level=0);

    QRect rect(int pos, PixMap pix, uint i = 0);
    QRect rect(int pos, QString str, int align = QLabel::AlignCenter );
    QRect rect(QRect r1, QRect r2);

    void draw(QPoint point, DrawWidget where, QPixmap pix);
    void draw(QRect rect, DrawWidget where, QPixmap pix);
    void draw(int pos, DrawWidget where, PixMap pix, uint i = 0);
    QPixmap draw(int pos, DrawWidget where, QString str,
                 QColor fg, QColor bg = QColor(), int align = QLabel::AlignCenter);
    QRect draw(int col, int row, DrawWidget where, QString str,
               QColor fg, QColor bg = QColor(), int align = QLabel::AlignCenter);

    void drawBrick(int pos);
    void drawPrison(int pos);
    void drawPoint(int pos);

    void erase(int pos, PixMap pix, uint i = 0);

    int maxPixmaps(PixMap pix);

protected:
    QString decodeHexOctString(QString str);

    void fillScoreString(QStrList &, QArray<int> &);
    void fillArray(QArray<int> &, QString, int);
    void fillStrList(QStrList &, QString, int);
    void fillPixmapName(QStrList &);

    void confScheme();
    void confLevels(bool defGroup=TRUE);
    void confMisc(bool defGroup=TRUE);
    void confScoring(bool defGroup=TRUE);

    void initPixmaps();
    void initRoomPixmap();
    void initbackPixmaps();

private:
    QWidget *w;
    Board *board;
    Bitfont *bitfont;

    int BlockWidth;
    int BlockHeight;

    QArray<int> fruitScore;
    QStrList fruitScoreString;
    QArray<int> monsterScore;
    QStrList monsterScoreString;

    QString pixmapDirectory;

    QStrList pointPixmapName;
    QStrList wallPixmapName;
    QStrList prisonPixmapName;
    QStrList energizerPixmapName;
    QStrList fruitPixmapName;
    QStrList pacmanPixmapName;
    QStrList dyingPixmapName;
    QStrList eyesPixmapName;
    QStrList monsterPixmapName;
    QStrList fruitScorePixmapName;
    QStrList monsterScorePixmapName;

    QString lastPointPixmapName;
    QString lastWallPixmapName;
    QString lastPrisonPixmapName;
    QString lastEnergizerPixmapName;
    QString lastFruitPixmapName;
    QString lastPacmanPixmapName;
    QString lastDyingPixmapName;
    QString lastEyesPixmapName;
    QString lastMonsterPixmapName;
    QString lastFruitScorePixmapName;
    QString lastMonsterScorePixmapName;

    QList<QPixmap> *loadPixmap(QWidget *parent, QString pixmapName,
                               QList<QPixmap> *pixmaps=0);
    QList<QPixmap> *textPixmap(QStrList &, QList<QPixmap> *pixmaps=0,
                               QColor fg = BLACK, QColor bg = QColor());
    QList<QPixmap> *textPixmap(QString str, QList<QPixmap> *pixmaps=0,
                               QColor fg = BLACK, QColor bg = QColor());

    QPoint point(int pos);
    int checkRange(int value, int max, int min=0);

    QList<QPixmap> *wallPix;
    QList<QPixmap> *prisonPix;
    QList<QPixmap> *pointPix;
    QList<QPixmap> *energizerPix;
    QList<QPixmap> *fruitPix;
    QList<QPixmap> *pacmanPix;
    QList<QPixmap> *dyingPix;
    QList<QPixmap> *eyesPix;
    QList<QPixmap> *monsterPix;
    QList<QPixmap> *fruitScorePix;
    QList<QPixmap> *monsterScorePix;

    QPixmap roomPix;
    QPixmap backPix;

    bool plainColor;
    QColor backgroundColor;

    int maxLevel;
    int level;

    int scheme;
    int mode;
};

#endif // PAINTER_H
