/***************************************************************************
  portable.h  -  various bits that ease porting kpacman to other platforms.
  Currently KDE2 and Qtopia ports exist.
                             -------------------
    begin                : Mon Mar 18 12:35:24 EET 2002
    copyright            : (C) 2002 by Catalin Climov
    email                : catalin@climov.com
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef PORTABLE_H
#define PORTABLE_H

#ifdef QWS
#define QPE_PORT
#else
#define KDE2_PORT
#define KDE_PORT
#endif

#if defined( KDE2_PORT )

#define APP kapp

#define APP_CONFIG_BEGIN( cfgname ) KConfig cfgname = kapp->config()
#define APP_CONFIG_END( cfgname ) cfgname->sync()
#define SAVE_CONFIG_GROUP( cfgname, groupname ) QString groupname = configname->group()
#define RESTORE_CONFIG_GROUP( cfgname, groupname ) configname->setGroup( groupname )

#define FIND_APP_DATA( dataname ) KGlobal::dirs()->findResource( "appdata", dataname )

#elif defined( QPE_PORT )

#define i18n( x ) x
#define KTMainWindow QMainWindow
#define KMenuBar QMenuBar
#define KAccel QAccel
#define APP qApp

#define APP_CONFIG_BEGIN( cfgname ) Config* cfgname = new Config("kpacman"); cfgname->setGroup("Default");
#define APP_CONFIG_END( cfgname ) delete cfgname
#define SAVE_CONFIG_GROUP( cfgname, groupname )
#define RESTORE_CONFIG_GROUP( cfgname, groupname ) cfgname->setGroup("Default")

#define FIND_APP_DATA( dataname ) (QPEApplication::qpeDir()+"share/kpacman/"+dataname)

#else

#error "Err, I don't know what platform to compile for (KDE2 or Qtopia)"

#endif

#define APP_QUIT() APP->quit()

#endif // PORTABLE_H
