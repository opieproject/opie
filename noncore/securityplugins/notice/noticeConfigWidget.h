/**
 * \file noticeConfigWidget.h
 * \brief Notice plugin configuration widget
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

#ifndef NOTICECONFIGWIDGET_H
#define NOTICECONFIGWIDGET_H

#include <qpushbutton.h>
#include <qmultilineedit.h>
#include <qpe/config.h>

#include <opie2/multiauthconfigwidget.h>

static char defaultNoticeText [] = "<h2>NOTICE TO USERS</h2>\n"
        "<p>This is a private computer system and is the property of "
        "the company XXX / Mr or Ms X. It is for authorized "
        "use only. Users have no expectation of privacy.</p>\n"
        "<p><strong>Unauthorized or improper use of this system may result in "
        "disciplinary action and civil and criminal penalties. <em>By continuing to use "
        "this system you indicate your awareness of and consent to these "
        "terms. LOG OFF IMMEDIATELY if you do not agree to them.</em></strong></p>";
        
class NoticeConfigWidget : public MultiauthConfigWidget {

    Q_OBJECT

public:
    NoticeConfigWidget(QWidget* parent, const char* name);
    virtual ~NoticeConfigWidget();
    virtual void writeConfig();
private:
    QMultiLineEdit * noticeMLE;
    QPushButton * resetNoticeButton;
private slots:
    void resetNotice();
private:
    Config * m_config;
    QString getNoticeText();
    void setNoticeText(QString noticeText);
};

#endif // NOTICECONFIGWIDGET_H
