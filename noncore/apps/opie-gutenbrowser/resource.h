/***************************************************************************
                          resource.h  -  description
                             -------------------
    begin                : Mon Jul 24 22:33:12 MDT 2000
    copyright            : (C) 2000 -2004 by llornkcor
    email                : ljp@llornkcor.com
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
#ifndef RESOURCE_H
#define RESOURCE_H

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

//#define NO_DEBUG
///////////////////////////////////////////////////////////////////
// File-menu entries
#define ID_FILE_NEW                 10020
#define ID_FILE_OPEN                10030

#define ID_FILE_SAVE                10050
#define ID_FILE_SAVE_AS             10060
#define ID_FILE_CLOSE               10070

#define ID_FILE_PRINT               10080

#define ID_FILE_QUIT                10100


///////////////////////////////////////////////////////////////////
// Edit-menu entries
#define ID_EDIT_UNDO                11010
#define ID_EDIT_REDO                11020
#define ID_EDIT_COPY                11030
#define ID_EDIT_CUT                 11040
#define ID_EDIT_PASTE               11050
#define ID_EDIT_SELECT_ALL          11060


///////////////////////////////////////////////////////////////////
// View-menu entries                    
#define ID_VIEW_TOOLBAR             12010
#define ID_VIEW_STATUSBAR           12020

///////////////////////////////////////////////////////////////////
// Help-menu entries
#define ID_HELP_ABOUT               1002


///////////////////////////////////////////////////////////////////
// General application values
#define VERSION                     "0.8.0"
#define METAL     13030

#if defined(_WS_X11_)
#define IDS_APP_ABOUT               "linuxgutenbrowser\n " VERSION  "\n(w) 2001 -2004 by llornkcor"
#endif
#if defined(_WS_WIN_)
#define IDS_APP_ABOUT               "winuxgutenbrowser\n " VERSION  "\n(w) 2001 -2004 by llornkcor"
#endif
//#define IDS_STATUS_DEFAULT          "resistence is futile, prepare to be assimilated..."
#define IDS_STATUS_DEFAULT          ">>>=====>    gutenbrowser    <=====<<<"

#endif // RESOURCE_H
