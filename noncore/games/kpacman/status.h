#ifndef STATUS_H
#define STATUS_H

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "portable.h"

#if defined( KDE2_PORT )
#include <kapp.h>
#include <kconfig.h>
#endif

#include <qwidget.h>
#include <qpixmap.h>
#include <qstring.h>
#include <qarray.h>
#include <qlist.h>
#include <qstrlist.h>
#include <qregexp.h>

class Status : public QWidget
{
    Q_OBJECT
public:
    Status(QWidget *parent=0, const char *name=0, int scheme=-1, int mode=-1);
    ~Status() {};

public slots:
    void setScheme(int scheme, int mode);
    void setLevel(int level);
    void setLifes(int lifes);

protected:
    void  paintEvent(QPaintEvent *);
    int minHeight();
    int minWidth();

    QString decodeHexOctString(QString str);

    void fillArray(QArray<int> &, QString, int);
    void fillStrList(QStrList &, QString, int);
    void fillPixmapName(QStrList &);

    void confScheme();
    void confLevels(bool defGroup=TRUE);
    void confMisc(bool defGroup=TRUE);

    void initPixmaps();

private:
    QArray<int> levelPos;
    int actualLifes;
    int actualLevel;

    QString pixmapDirectory;

    QStrList lifesPixmapName;
    QStrList levelPixmapName;

    QString lastLifesPixmapName;
    QString lastLevelPixmapName;

    QList<QPixmap> *loadPixmap(QWidget *parent, QString pixmapName,
                               QList<QPixmap> *pixmaps=0);

    QList<QPixmap> *lifesPix;
    QList<QPixmap> *levelPix;

    int maxLevel;
    int level;

    int scheme;
    int mode;
};

#endif // STATUS_H
