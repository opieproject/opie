/*
                             This file is part of the Opie Project
                             Copyright (C) 2009 The Opie Team <opie-devel@handhelds.org>
              =.
            .=l.
           .>+-=
 _;:,     .>    :=|.         This program is free software; you can
.> <`_,   >  .   <=          redistribute it and/or  modify it under
:`=1 )Y*s>-.--   :           the terms of the GNU Library General Public
.="- .-=="i,     .._         License as published by the Free Software
 - .   .-<_>     .<>         Foundation; either version 2 of the License,
     ._= =}       :          or (at your option) any later version.
    .%`+i>       _;_.
    .i_,=:_.      -<s.       This program is distributed in the hope that
     +  .  -:.       =       it will be useful,  but WITHOUT ANY WARRANTY;
    : ..    .:,     . . .    without even the implied warranty of
    =_        +     =;=|`    MERCHANTABILITY or FITNESS FOR A
  _.=:.       :    :=>`:     PARTICULAR PURPOSE. See the GNU
..}^=.=       =       ;      Library General Public License for more
++=   -.     .`     .:       details.
 :     =  ...= . :.=-
 -.   .:....=;==+<;          You should have received a copy of the GNU
  -_. . .   )=.  =           Library General Public License along with
    --        :-=`           this library; see the file COPYING.LIB.
                             If not, write to the Free Software Foundation,
                             Inc., 59 Temple Place - Suite 330,
                             Boston, MA 02111-1307, USA.
*/

#include "mainwindow.h"

#include "editor.h"

using namespace Opie;
using namespace Opie::Datebook;

Editor::Editor( MainWindow* win, QWidget*)
    : m_win( win )
{
}

Editor::~Editor() {

}

DescriptionManager Editor::descriptions()const {
    return m_win->descriptionManager();
}

LocationManager Editor::locations()const {
    return m_win->locationManager();
}

void Editor::setDescriptions( const DescriptionManager& dsc) {
    m_win->setDescriptionManager( dsc );
}

void Editor::setLocations( const LocationManager& loc) {
    m_win->setLocationManager( loc );
}

bool Editor::isAP()const {
    return m_win->viewAP();
}

bool Editor::weekStartOnMonday()const {
    return m_win->viewStartMonday();
}

QString Editor::defaultLocation()const {
    return m_win->defaultLocation();
}

QArray<int> Editor::defaultCategories()const {
    return m_win->defaultCategories();
}

bool Editor::alarmPreset()const {
    return m_win->alarmPreset();
}

int Editor::alarmPresetTime()const {
    return m_win->alarmPresetTime();
}

int Editor::alarmPresetUnits()const {
    return m_win->alarmPresetUnits();
}
