
#include "portable.h"

#if defined( KDE2_PORT )
#include <kapp.h>
#include <kconfig.h>
#include <kstddirs.h>
#include <kpacmanwidget.h>
#include <kpacmanwidget.moc>
#elif defined( QPE_PORT )
#include <qpe/qpeapplication.h>
#include "config.h"
#include "kpacmanwidget.h"
#endif

#include <qmessagebox.h>

#include "bitfont.h"
#include "score.h"
#include "referee.h"
#include "status.h"

KpacmanWidget::KpacmanWidget( QWidget *parent, const char *name)
        : QWidget( parent, name )
{
    bitfont = NULL;
    fontName = "";

    scheme = mode = -1;
    confScheme();

    score = new Score(this, name, scheme, mode, bitfont);
    referee = new Referee( this, name, scheme, mode, bitfont);
    status = new Status(this, name, scheme, mode);

#ifndef QWS
    setFixedSize(referee->width(), bitfont->height()*3 + referee->height() + status->height());
#else
    setBackgroundColor( black );
#endif
}

KpacmanWidget::~KpacmanWidget()
{
}

void KpacmanWidget::confMisc(bool defGroup)
{
    APP_CONFIG_BEGIN( cfg );
    //KStandardDirs *dirs = KGlobal::dirs();
    QString findPath;

    if (defGroup || cfg->hasKey("Font")) {
        fontName = cfg->readEntry("Font");

        if (fontName.left(1) != "/" && fontName.left(1) != "~")
            fontName.insert(0, "fonts/");
        if (fontName.right(1) == "/")
            fontName.append("font.xbm");

        //findPath = dirs->findResource("appdata", fontName);
        findPath = FIND_APP_DATA( fontName );
        if (!findPath.isEmpty())
            fontName = findPath;

        bitfontFirstChar = cfg->readNumEntry("FontFirstChar", 0x0e);
        bitfontLastChar = cfg->readNumEntry("FontLastChar", 0x5f);
    }
    APP_CONFIG_END( cfg );
}

void KpacmanWidget::confScheme()
{
    APP_CONFIG_BEGIN( cfg );
    QString lastFontName = fontName;
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

    if (lastFontName != fontName) {

        if (bitfont != 0)
            delete bitfont;

        bitfont = new Bitfont(fontName, bitfontFirstChar, bitfontLastChar);
        if (bitfont->width() == 0 || bitfont->height() == 0) {
            QString msg = i18n("The bitfont could not be contructed.\n\n"
                               "The file '@FONTNAME@' does not exist,\n"
                               "or is of an unknown format.");
            msg.replace(QRegExp("@FONTNAME@"), fontName);
            // QMessageBox::critical(this, i18n("Initialization Error"), msg);
            printf("%s\n", msg.data());
        }
    }

    RESTORE_CONFIG_GROUP( cfg, oldgroup );
    APP_CONFIG_END( cfg );
}

void KpacmanWidget::setScheme(int Scheme, int Mode)
{
    mode = Mode;
    scheme = Scheme;

    confScheme();

    score->setScheme(Scheme, Mode, bitfont);
    referee->setScheme(Scheme, Mode, bitfont);
    status->setScheme(Scheme, Mode);

#ifndef QWS
    setFixedSize(referee->width(),
                 bitfont->height()*3 + referee->height() + status->height());
#endif

    score->repaint(FALSE);
    referee->repaint(FALSE);
    status->repaint(FALSE);
}

void KpacmanWidget::resizeEvent( QResizeEvent * )
{
    referee->setGeometry(0, bitfont->height()*3, referee->width(), referee->height());
    referee->setBackgroundColor(BLACK);

    status->setGeometry(0, bitfont->height()*3+referee->height(), referee->width(),
                        status->height());
    status->setBackgroundColor(BLACK);

    score->setGeometry(0, 0, referee->width(), bitfont->height()*3+referee->height()+status->height());
    score->setBackgroundColor(BLACK);
}
