/*
                             This file is part of the Opie Project

                             Copyright (C) 2003 Michael Lauer <mickey@tm.informatik.uni-frankfurt.de>
                             Inspired by the KDE globalsettings which are
                             Copyright (C) 2000 David Faure <faure@kde.org>
              =.
            .=l.
           .>+-=
 _;:,     .>    :=|.         This program is free software; you can
.> <`_,   >  .   <=          redistribute it and/or  modify it under
:`=1 )Y*s>-.--   :           the terms of the GNU Library General Public
.="- .-=="i,     .._         License as published by the Free Software
 - .   .-<_>     .<>         Foundation; either version 2 of the License,
     ._= =}       :          or (at your option) any later version.
    .%`+i>       _;_.
    .i_,=:_.      -<s.       This program is distributed in the hope that
     +  .  -:.       =       it will be useful,  but WITHOUT ANY WARRANTY;
    : ..    .:,     . . .    without even the implied warranty of
    =_        +     =;=|`    MERCHANTABILITY or FITNESS FOR A
  _.=:.       :    :=>`:     PARTICULAR PURPOSE. See the GNU
..}^=.=       =       ;      Library General Public License for more
++=   -.     .`     .:       details.
 :     =  ...= . :.=-
 -.   .:....=;==+<;          You should have received a copy of the GNU
  -_. . .   )=.  =           Library General Public License along with
    --        :-=`           this library; see the file COPYING.LIB.
                             If not, write to the Free Software Foundation,
                             Inc., 59 Temple Place - Suite 330,
                             Boston, MA 02111-1307, USA.
*/

/* OPIE */

#include <opie2/oglobalsettings.h>
#include <opie2/oconfig.h>
#include <opie2/oglobal.h>

/* QT */

#include <qdir.h>
#include <qpixmap.h>
#include <qfontinfo.h>

/* UNIX */

#include <stdlib.h>

QString* OGlobalSettings::s_desktopPath = 0;
QString* OGlobalSettings::s_autostartPath = 0;
QString* OGlobalSettings::s_trashPath = 0;
QString* OGlobalSettings::s_documentPath = 0;
QFont *OGlobalSettings::_generalFont = 0;
QFont *OGlobalSettings::_fixedFont = 0;
QFont *OGlobalSettings::_toolBarFont = 0;
QFont *OGlobalSettings::_menuFont = 0;
QFont *OGlobalSettings::_windowTitleFont = 0;
QFont *OGlobalSettings::_taskbarFont = 0;

QColor *OGlobalSettings::OpieGray = 0;
QColor *OGlobalSettings::OpieHighlight = 0;
QColor *OGlobalSettings::OpieAlternate = 0;
    
OGlobalSettings::OMouseSettings *OGlobalSettings::s_mouseSettings = 0;

//FIXME: Add manipulators to the accessors

int OGlobalSettings::dndEventDelay()
{
    OConfig *c = OGlobal::config();
    OConfigGroupSaver cgs( c, "General" );
    return c->readNumEntry("DndDelay", 2);
}

bool OGlobalSettings::singleClick()
{
    OConfig *c = OGlobal::config();
    OConfigGroupSaver cgs( c, "OPIE" );
    return c->readBoolEntry("SingleClick", OPIE_DEFAULT_SINGLECLICK);
}

bool OGlobalSettings::insertTearOffHandle()
{
    OConfig *c = OGlobal::config();
    OConfigGroupSaver cgs( c, "OPIE" );
    return c->readBoolEntry("InsertTearOffHandle", OPIE_DEFAULT_INSERTTEAROFFHANDLES);
}

bool OGlobalSettings::changeCursorOverIcon()
{
    OConfig *c = OGlobal::config();
    OConfigGroupSaver cgs( c, "OPIE" );
    return c->readBoolEntry("ChangeCursor", OPIE_DEFAULT_CHANGECURSOR);
}

bool OGlobalSettings::visualActivate()
{
    OConfig *c = OGlobal::config();
    OConfigGroupSaver cgs( c, "OPIE" );
    return c->readBoolEntry("VisualActivate", OPIE_DEFAULT_VISUAL_ACTIVATE);
}

unsigned int OGlobalSettings::visualActivateSpeed()
{
    OConfig *c = OGlobal::config();
    OConfigGroupSaver cgs( c, "OPIE" );
    return
        c->readNumEntry(
            "VisualActivateSpeed",
            OPIE_DEFAULT_VISUAL_ACTIVATE_SPEED
        );
}

int OGlobalSettings::autoSelectDelay()
{
    OConfig *c = OGlobal::config();
    OConfigGroupSaver cgs( c, "OPIE" );
    return c->readNumEntry("AutoSelectDelay", OPIE_DEFAULT_AUTOSELECTDELAY);
}

OGlobalSettings::Completion OGlobalSettings::completionMode()
{
    int completion;
    OConfig *c = OGlobal::config();
    OConfigGroupSaver cgs( c, "General" );
    completion = c->readNumEntry("completionMode", -1);
    if ((completion < (int) CompletionNone) ||
        (completion > (int) CompletionPopupAuto))
      {
        completion = (int) CompletionPopup; // Default
      }
  return (Completion) completion;
}


bool OGlobalSettings::showContextMenusOnPress ()
{
    OConfig *c = OGlobal::config();
    OConfigGroupSaver cgs (c, "ContextMenus");

    return cgs.config()->readBoolEntry("ShowOnPress", true);
}


int OGlobalSettings::contextMenuKey ()
{
    OConfig *c = OGlobal::config();
    OConfigGroupSaver cgs (c, "Shortcuts");

    //OShortcut cut (cgs.config()->readEntry ("PopupMenuContext", "Menu"));
    //return cut.keyCodeQt();

    return 0; // FIXME
}


OGlobalSettings::Debug OGlobalSettings::debugMode()
{
    OConfig *c = OGlobal::config();
    OConfigGroupSaver cgs( c, "General" );
    int debug = c->readNumEntry( "debugMode", -1 );
    if ( (debug < (int) DebugNone) || (debug > (int) DebugSocket) )
    {
        debug = (int) DebugStdErr; // Default
    }
    return (Debug) debug;
}


QString OGlobalSettings::debugOutput()
{
    OConfig *c = OGlobal::config();
    OConfigGroupSaver cgs( c, "General" );
    QString deflt = QString::null;
    switch( debugMode() )
    {
        case DebugNone: break; // no additional information needed
        case DebugFiles: deflt = "/var/log/opiedebug.log"; break; // file to save output in
        case DebugMsgBox: break; // no additional information needed
        case DebugStdErr: break; // no additional information needed
        case DebugSysLog: break; // no additional information needed
        case DebugSocket: deflt = "127.0.0.1:8913"; break; // address to send packets to
    }

    return c->readEntry( "debugOutput"+ QString::number(debugMode()), deflt );
}


QColor OGlobalSettings::toolBarHighlightColor()
{
    initColors();
    OConfig *c = OGlobal::config();
    OConfigGroupSaver cgs( c, QString::fromLatin1("Toolbar style") );
    return c->readColorEntry("HighlightColor", OpieHighlight);
}

QColor OGlobalSettings::inactiveTitleColor()
{
    if (!OpieGray) OpieGray = new QColor(220, 220, 220);
    OConfig *c = OGlobal::config();
    OConfigGroupSaver cgs( c, QString::fromLatin1("WM") );
    return c->readColorEntry( "inactiveBackground", OpieGray );
}

QColor OGlobalSettings::inactiveTextColor()
{
    OConfig *c = OGlobal::config();
    OConfigGroupSaver cgs( c, QString::fromLatin1("WM") );
    return c->readColorEntry( "inactiveForeground", &Qt::darkGray );
}

QColor OGlobalSettings::activeTitleColor()
{
    initColors();
    OConfig *c = OGlobal::config();
    OConfigGroupSaver cgs( c, QString::fromLatin1("WM") );
    return c->readColorEntry( "activeBackground", OpieHighlight);
}

QColor OGlobalSettings::activeTextColor()
{
    OConfig *c = OGlobal::config();
    OConfigGroupSaver cgs( c, QString::fromLatin1("WM") );
    return c->readColorEntry( "activeForeground", &Qt::white );
}

int OGlobalSettings::contrast()
{
    OConfig *c = OGlobal::config();
    OConfigGroupSaver cgs( c, QString::fromLatin1("OPIE") );
    return c->readNumEntry( "contrast", 7 );
}

// following functions should work in OPIE - how to sync with appearance changes?

QColor OGlobalSettings::baseColor()
{
    OConfig *c = OGlobal::config();
    OConfigGroupSaver cgs( c, QString::fromLatin1("Appearance") );
    return c->readColorEntry( "Base", &Qt::white );
}

QColor OGlobalSettings::textColor()
{
    OConfig *c = OGlobal::config();
    OConfigGroupSaver cgs( c, QString::fromLatin1("Appearance") );
    return c->readColorEntry( "Text", &Qt::black );
}

QColor OGlobalSettings::highlightedTextColor()
{
    OConfig *c = OGlobal::config();
    OConfigGroupSaver cgs( c, QString::fromLatin1("Appearance") );
    return c->readColorEntry( "HighlightedText", &Qt::white );
}

QColor OGlobalSettings::highlightColor()
{
    initColors();
    OConfig *c = OGlobal::config();
    OConfigGroupSaver cgs( c, QString::fromLatin1("Appearance") );
    return c->readColorEntry( "Highlight", OpieHighlight );
}

QColor OGlobalSettings::alternateBackgroundColor()
{
    initColors();
    OConfig *c = OGlobal::config();
    OConfigGroupSaver cgs( c, QString::fromLatin1("Appearance") );
    *OpieAlternate = calculateAlternateBackgroundColor( baseColor() );
    return c->readColorEntry( "alternateBackground", OpieAlternate );
}

QColor OGlobalSettings::calculateAlternateBackgroundColor(const QColor& base)
{
    if (base == Qt::white)
        return QColor(238,246,255);
    else
    {
        int h, s, v;
        base.hsv( &h, &s, &v );
        if (v > 128)
            return base.dark(106);
        else if (base != Qt::black)
            return base.light(110);

        return QColor(32,32,32);
    }
}

QColor OGlobalSettings::linkColor()
{
    initColors();
    OConfig *c = OGlobal::config();
    OConfigGroupSaver cgs( c, QString::fromLatin1("Appearance") );
    return c->readColorEntry( "linkColor", OpieGray );
}

QColor OGlobalSettings::visitedLinkColor()
{
    OConfig *c = OGlobal::config();
    OConfigGroupSaver cgs( c, QString::fromLatin1("Appearance") );
    return c->readColorEntry( "visitedLinkColor", &Qt::magenta );
}

// FIXME: font stuff currently uses a different format in OPIE, so the
// functions below are not yet applicable. The whole font stuff for OPIE
// has to be revised anyway

QFont OGlobalSettings::generalFont()
{
    if (_generalFont)
        return *_generalFont;

    _generalFont = new QFont("helvetica", 10);
    _generalFont->setPixelSize(10);
    _generalFont->setStyleHint(QFont::SansSerif);

    OConfig *c = OGlobal::config();
    OConfigGroupSaver cgs( c, QString::fromLatin1("Appearance") );
    *_generalFont = c->readFontEntry("font", _generalFont);

    return *_generalFont;
}

QFont OGlobalSettings::fixedFont()
{
    if (_fixedFont)
        return *_fixedFont;

    _fixedFont = new QFont("courier", 12);
    _fixedFont->setPixelSize(12);
    _fixedFont->setStyleHint(QFont::TypeWriter);

    OConfig *c = OGlobal::config();
    OConfigGroupSaver cgs( c, QString::fromLatin1("General") );
    *_fixedFont = c->readFontEntry("fixed", _fixedFont);

    return *_fixedFont;
}

QFont OGlobalSettings::toolBarFont()
{
    if(_toolBarFont)
        return *_toolBarFont;

    _toolBarFont = new QFont("helvetica", 10);
    _toolBarFont->setPixelSize(10);
    _toolBarFont->setStyleHint(QFont::SansSerif);

    OConfig *c = OGlobal::config();
    OConfigGroupSaver cgs( c, QString::fromLatin1("General") );
    *_toolBarFont = c->readFontEntry("toolBarFont", _toolBarFont);

    return *_toolBarFont;
}

QFont OGlobalSettings::menuFont()
{
    if(_menuFont)
        return *_menuFont;

    _menuFont = new QFont("helvetica", 12);
    _menuFont->setPixelSize(12);
    _menuFont->setStyleHint(QFont::SansSerif);

    OConfig *c = OGlobal::config();
    OConfigGroupSaver cgs( c, QString::fromLatin1("General") );
    *_menuFont = c->readFontEntry("menuFont", _menuFont);

    return *_menuFont;
}

QFont OGlobalSettings::windowTitleFont()
{
    if(_windowTitleFont)
        return *_windowTitleFont;

    _windowTitleFont = new QFont("helvetica", 12, QFont::Bold);
    _windowTitleFont->setPixelSize(12);
    _windowTitleFont->setStyleHint(QFont::SansSerif);

    OConfig *c = OGlobal::config();
    OConfigGroupSaver cgs( c, QString::fromLatin1("WM") );
    *_windowTitleFont = c->readFontEntry("activeFont", _windowTitleFont); // inconsistency

    return *_windowTitleFont;
}

QFont OGlobalSettings::taskbarFont()
{
    if(_taskbarFont)
        return *_taskbarFont;

    _taskbarFont = new QFont("helvetica", 8);
    _taskbarFont->setPixelSize(8);
    _taskbarFont->setStyleHint(QFont::SansSerif);

    OConfig *c = OGlobal::config();
    OConfigGroupSaver cgs( c, QString::fromLatin1("General") );
    *_taskbarFont = c->readFontEntry("taskbarFont", _taskbarFont);

    return *_taskbarFont;
}

// FIXME: the whole path stuff has to be revised for OPIE

void OGlobalSettings::initStatic() // should be called initPaths(). Don't put anything else here.
{
    if ( s_desktopPath != 0 )
        return;

    s_desktopPath = new QString();
    s_autostartPath = new QString();
    s_trashPath = new QString();
    s_documentPath = new QString();

    OConfig *config = OGlobal::config();
    //bool dollarExpansion = config->isDollarExpansion();
    //config->setDollarExpansion(true);
    OConfigGroupSaver cgs( config, "Paths" );

    // Desktop Path
    *s_desktopPath = QDir::homeDirPath() + "/" + "Desktop" + "/";
    *s_desktopPath = config->readEntry( "Desktop", *s_desktopPath);
    if ( (*s_desktopPath)[0] != '/' )
      s_desktopPath->prepend( QDir::homeDirPath() + "/" );
    *s_desktopPath = QDir::cleanDirPath( *s_desktopPath );
    if ( s_desktopPath->right(1) != "/")
        *s_desktopPath += "/";

    // Trash Path
    *s_trashPath = *s_desktopPath + QObject::tr("Trash") + "/";
    *s_trashPath = config->readEntry( "Trash" , *s_trashPath);
    if ( (*s_trashPath)[0] != '/' )
      s_trashPath->prepend( QDir::homeDirPath() + "/" );
    *s_trashPath = QDir::cleanDirPath( *s_trashPath );
    if ( s_trashPath->right(1) != "/")
        *s_trashPath += "/";
    // We need to save it in any case, in case the language changes later on,
    if ( !config->hasKey( "Trash" ) )
    {
      //config->writePathEntry( "Trash", *s_trashPath, true, true );
      config->writeEntry( "Trash", *s_trashPath );
      //config->sync();
    }

/*    // Autostart Path
    *s_autostartPath = OGlobal::dirs()->localkdedir() + "Autostart" + "/";
    *s_autostartPath = config->readEntry( "Autostart" , *s_autostartPath);
    if ( (*s_autostartPath)[0] != '/' )
      s_autostartPath->prepend( QDir::homeDirPath() + "/" );
    *s_autostartPath = QDir::cleanDirPath( *s_autostartPath );
    if ( s_autostartPath->right(1) != "/")
        *s_autostartPath += "/";
*/
    // Document Path
    *s_documentPath = QString::null;
    *s_documentPath = config->readEntry( "Documents" , *s_documentPath);
    if ( (*s_documentPath)[0] != '/' )
      s_documentPath->prepend( QDir::homeDirPath() + "/" );
    *s_documentPath = QDir::cleanDirPath( *s_documentPath );
    if ( s_documentPath->right(1) != "/")
        *s_documentPath += "/";

    //config->setDollarExpansion(dollarExpansion);

    // Make sure this app gets the notifications about those paths
    //if (kapp)
        //kapp->addKipcEventMask(KIPC::SettingsChanged);
}

void OGlobalSettings::initColors()
{
    if ( not OpieHighlight ) OpieHighlight = new QColor( 156, 118, 32 );
    if ( not OpieAlternate ) OpieAlternate = new QColor( 238, 246, 255 );
    if ( not OpieGray ) OpieGray = new QColor( 220, 210, 215 );
}

void OGlobalSettings::rereadFontSettings()
{
    delete _generalFont;
    _generalFont = 0L;
    delete _fixedFont;
    _fixedFont = 0L;
    delete _menuFont;
    _menuFont = 0L;
    delete _toolBarFont;
    _toolBarFont = 0L;
    delete _windowTitleFont;
    _windowTitleFont = 0L;
    delete _taskbarFont;
    _taskbarFont = 0L;
}

void OGlobalSettings::rereadPathSettings()
{
    qDebug( "OGlobalSettings::rereadPathSettings" );
    delete s_autostartPath;
    s_autostartPath = 0L;
    delete s_trashPath;
    s_trashPath = 0L;
    delete s_desktopPath;
    s_desktopPath = 0L;
    delete s_documentPath;
    s_documentPath = 0L;
}

OGlobalSettings::OMouseSettings & OGlobalSettings::mouseSettings()
{
    if ( ! s_mouseSettings )
    {
        s_mouseSettings = new OMouseSettings;
        OMouseSettings & s = *s_mouseSettings; // for convenience

        OConfigGroupSaver cgs( OGlobal::config(), "Mouse" );
        QString setting = OGlobal::config()->readEntry("MouseButtonMapping");
        if (setting == "RightHanded")
            s.handed = OMouseSettings::RightHanded;
        else if (setting == "LeftHanded")
            s.handed = OMouseSettings::LeftHanded;
        else
        {

            // FIXME: Implement for Opie / Qt Embedded

        }
    }
    return *s_mouseSettings;
}

void OGlobalSettings::rereadMouseSettings()
{
    delete s_mouseSettings;
    s_mouseSettings = 0L;
}

// FIXME: This won't be necessary, or will it? :-D

bool OGlobalSettings::isMultiHead()
{
    QCString multiHead = getenv("OPIE_MULTIHEAD");
    if (!multiHead.isEmpty()) {
        return (multiHead.lower() == "true");
    }
    return false;
}
