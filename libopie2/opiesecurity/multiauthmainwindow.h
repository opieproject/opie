/**
 * \file multiauthmainwindow.h
 * \brief Defines the Opie multiauth main window.
 * 
 * This implementation was derived from the today plugins implementation.
 * \author Clément Séveillac (clement . seveillac (at) via . ecp . fr)
 */
/*
               =.            This file is part of the Opie Project
             .=l.            Copyright (C) 2004 Opie Developer Team <opie-devel@handhelds.org>
           .>+-=
 _;:,     .>    :=|.         This library is free software; you can 
.> <`_,   >  .   <=          redistribute it and/or  modify it under
:`=1 )Y*s>-.--   :           the terms of the GNU Library General Public
.="- .-=="i,     .._         License as published by the Free Software
 - .   .-<_>     .<>         Foundation; either version 2 of the License,
     ._= =}       :          or (at your option) any later version.
    .%`+i>       _;_.        
    .i_,=:_.      -<s.       This library is distributed in the hope that  
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

#ifndef MULTIAUTHMAINWINDOW_H
#define MULTIAUTHMAINWINDOW_H

#include <qdialog.h> 
#include <qlayout.h>
#include <qpushbutton.h>
#include <qlabel.h>

/// Multiauth main window
/**
 * This QDialog window displays some information and an exit button,
 * and completely hides the desktop, preventing user interactions
 * with it.
 */
class MultiauthMainWindow : public QDialog {
    Q_OBJECT

    public:
        MultiauthMainWindow();
        ~MultiauthMainWindow();
        bool isAlreadyDone();

    private:
        QVBoxLayout * layout;
        QLabel * title, * message, * message2;
        QPushButton * proceedButton, * quit;
        /// whether to show explanatory screens before and after the authentication plugins
        bool explanScreens;
        /// allow to bypass authnentication via 'exit' buttons on both explan. screens
        bool allowBypass;
        /// true when the authentication has been done successfully
        bool alreadyDone;

        private slots:
            void proceed();
};

#endif // MULTIAUTHMAINWINDOW_H

