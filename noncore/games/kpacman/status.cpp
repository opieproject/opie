
#include "portable.h"

#if defined( KDE2_PORT )
#include <kapp.h>
#include <klocale.h>
#include <kstddirs.h>
#include <status.h>
#include <status.moc>
#elif defined( QPE_PORT )
#include <qpe/qpeapplication.h>
#include "config.h"
#include "status.h"
#endif

#include <qpixmap.h>
#include <qbitmap.h>
#include <qstring.h>
#include <qmsgbox.h>
#include <qfileinfo.h>

Status::Status( QWidget *parent, const char *name, int Scheme, int Mode ) :
    QWidget( parent, name )
{
    actualLifes = 0;
    actualLevel = 0;

    lifesPix = NULL;
    levelPix = NULL;

    scheme = Scheme;
    mode = Mode;
    level = 0;

    confScheme();
}

QList<QPixmap> *Status::loadPixmap(QWidget *parent, QString pixmapName,
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
        QString msg = i18n("The pixmap could not be contructed.\n\n"
                           "The file '@PIXMAPNAME@' does not exist,\n"
                           "or is of an unknown format.");
        msg.replace(QRegExp("@PIXMAPNAME@"), pixmapName);
        QMessageBox::information(parent, i18n("Initialization Error"),
                                 (const char *) msg);
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
        bitBlt(pixmap, 0, 0, &PIXMAP, x*width, 0, width, height, CopyROP, TRUE);
        bitBlt(&MASK, 0, 0, &BITMAP, x*width, 0, width, height, CopyROP, TRUE);
        pixmap->setMask(MASK);
    }

    return pixmaps;
}

void Status::paintEvent( QPaintEvent *)
{
    for (int x = 0; x < actualLifes && !lifesPix->isEmpty(); x++)
        bitBlt(this, lifesPix->at(0)->width()+(lifesPix->at(0)->width()*x), 
               (height()-lifesPix->at(0)->height())/2,
               lifesPix->at(0), 0, 0,
               lifesPix->at(0)->width(), lifesPix->at(0)->height());

    for (int x = 0; x < actualLevel && !levelPix->isEmpty(); x++) {
        erase((width()-levelPix->at(x)->width()*2)-(levelPix->at(x)->width()*levelPos[x]),
              (height()-levelPix->at(x)->height())/2,
              levelPix->at(x)->width(), levelPix->at(x)->height());
        bitBlt(this, (width()-levelPix->at(x)->width()*2)-(levelPix->at(x)->width()*levelPos[x]),
               (height()-levelPix->at(x)->height())/2,
               levelPix->at(x), 0, 0,
               levelPix->at(x)->width(), levelPix->at(x)->height());
    }
}

void Status::initPixmaps()
{
    if (lastLifesPixmapName != lifesPixmapName.at(level)) {
        lifesPix = loadPixmap(this, lifesPixmapName.at(level), lifesPix);
        lastLifesPixmapName = lifesPixmapName.at(level);
    }
    if (lastLevelPixmapName != levelPixmapName.at(level)) {
        levelPix = loadPixmap(this, levelPixmapName.at(level), levelPix);
        lastLevelPixmapName = levelPixmapName.at(level);
    }
}

QString Status::decodeHexOctString(QString s)
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

void Status::fillArray(QArray<int> &array, QString values, int max)
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

void Status::fillStrList(QStrList &list, QString values, int max)
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

void Status::fillPixmapName(QStrList &pixmapName)
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

void Status::confLevels(bool defGroup)
{
    APP_CONFIG_BEGIN( cfg );
    if (defGroup || cfg->hasKey("Levels"))
        maxLevel = cfg->readNumEntry("Levels", 13);
    APP_CONFIG_END( cfg );
}

void Status::confMisc(bool defGroup)
{
    APP_CONFIG_BEGIN( cfg );
    if (defGroup || cfg->hasKey("LevelPosition"))
        fillArray(levelPos, cfg->readEntry("LevelPosition", "0,1,2,3,,4,,5,,6,,7"), maxLevel);

    if (defGroup || cfg->hasKey("PixmapDirectory")) {
        pixmapDirectory = cfg->readEntry("PixmapDirectory");

        if (pixmapDirectory.left(1) != "/" && pixmapDirectory.left(1) != "~")
            pixmapDirectory.insert(0, "pics/");
        if (pixmapDirectory.right(1) != "/")
            pixmapDirectory.append("/");
    }

    if (defGroup || cfg->hasKey("LifesPixmapName"))
        fillStrList(lifesPixmapName,
                    cfg->readEntry("LifesPixmapName", "lifes.xpm"), maxLevel+1);
    if (defGroup || cfg->hasKey("LevelPixmapName"))
        fillStrList(levelPixmapName,
                    cfg->readEntry("LevelPixmapName", "level.xpm"), maxLevel+1);
    APP_CONFIG_END( cfg );
}

void Status::confScheme()
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

    if (mode != -1) {
        newgroup.sprintf("Mode %d", mode);
        cfg->setGroup(newgroup);

        confMisc(FALSE);
    }

    if (scheme != -1) {
        newgroup.sprintf("Scheme %d", scheme);
        cfg->setGroup(newgroup);

        confMisc(FALSE);
    }

    fillPixmapName(lifesPixmapName);
    fillPixmapName(levelPixmapName);

    initPixmaps();

    setFixedHeight(minHeight());

    RESTORE_CONFIG_GROUP( cfg, oldgroup );
    APP_CONFIG_END( cfg );
}

void Status::setScheme(int Scheme, int Mode)
{
    mode = Mode;
    scheme = Scheme;

    confScheme();

    repaint();
}

int Status::minHeight()
{
    if (lifesPix->isEmpty() && levelPix->isEmpty())
        return 0;
    if (levelPix->isEmpty())
        return lifesPix->at(0)->height();
    if (lifesPix->isEmpty())
        return levelPix->at(0)->height();
    return (lifesPix->at(0)->height() > levelPix->at(0)->height()) ?
            lifesPix->at(0)->height() : levelPix->at(0)->height();
}

int Status::minWidth()
{
    if (lifesPix->isEmpty() && levelPix->isEmpty())
        return 0;
    if (levelPix->isEmpty())
        return lifesPix->at(0)->width();
    if (lifesPix->isEmpty())
        return levelPix->at(0)->width();
    return (lifesPix->at(0)->width() > levelPix->at(0)->width()) ?
            lifesPix->at(0)->width() : levelPix->at(0)->width();
}

void Status::setLifes(int lifes)
{
    actualLifes = lifes;
    repaint();
}

void Status::setLevel(int Level)
{
    level = Level;

    initPixmaps();

    actualLevel = (level > (int) levelPix->count()) ? (int) levelPix->count() : level;
    repaint();
}
