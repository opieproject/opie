/**
 * \file ownerInfoConfigWidget.h
 * \brief Owner info configuration widget
 * \author Paul Eggleton (bluelightning@bluelightning.org)
 */
/*
               =.            This file is part of the Opie Project
             .=l.            Copyright (C) 2007 Opie Developer Team <opie-devel@lists.sourceforge.net>
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

#ifndef OWNERINFOCONFIGWIDGET_H
#define OWNERINFOCONFIGWIDGET_H

#include <qpushbutton.h>
#include <qcheckbox.h>
#include <qmultilineedit.h>
#include <qcombobox.h>

#include <qpe/config.h>

#include <opie2/multiauthconfigwidget.h>

class OwnerInfoConfigWidget : public Opie::Security::MultiauthConfigWidget {

    Q_OBJECT

public:
    OwnerInfoConfigWidget(QWidget* parent, const char* name);
    virtual ~OwnerInfoConfigWidget();
    virtual void writeConfig();
private:
    QMultiLineEdit *mleMessage;
    QPushButton *pbEditOwnerInfo;
    QCheckBox *ckShowHomePhone;
    QCheckBox *ckShowHomeMobile;
    QCheckBox *ckShowWorkPhone;
    QCheckBox *ckShowWorkMobile;
    QCheckBox *ckShowHomeAddress;
    QCheckBox *ckShowWorkAddress;
    QCheckBox *ckShowEmail;
    QComboBox *cbWhen;
private slots:
    void editOwnerInfo();
private:
    Config * m_config;
    QString getNoticeText();
    void readConfig();
};

#endif // OWNERINFOCONFIGWIDGET_H

