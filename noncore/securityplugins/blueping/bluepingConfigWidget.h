/**
 * \file bluepingConfigWidget.h
 * \brief "BluePing" plugin configuration widget
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

#ifndef BLUEPINGCONFIGWIDGET_H
#define BLUEPINGCONFIGWIDGET_H
#include <qlineedit.h>
#include <qpushbutton.h>
#include <qpe/config.h>

#include <opie2/multiauthconfigwidget.h>

class BluepingConfigWidget : public Opie::Security::MultiauthConfigWidget {

    Q_OBJECT

public:
    BluepingConfigWidget(QWidget* parent, const char* name);
    virtual ~BluepingConfigWidget();
    virtual void writeConfig();
private:
    QLineEdit *editMAC;
    QPushButton *setMAC;
    Config *m_config;
private slots:
    void changeMAC();

};

#endif // BLUEPINGCONFIGWIDGET_H
