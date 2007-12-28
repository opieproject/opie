/**
 * \file multiauthcommon.h
 * \brief Objects and functions for Opie multiauth framework
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

#ifndef MULTIAUTHCOMMON_H
#define MULTIAUTHCOMMON_H


/* OwnerDialog stuff */
#include <qpe/global.h>
#include <qpe/contact.h>
#include <qdialog.h>

class QVBoxLayout; 
class QHBoxLayout; 
class QGridLayout; 
class QLabel;
class QPushButton;

#include "ownerInfoDialog.h"

namespace Opie {
namespace Security {

enum ownerInfoWhenToShow {
    oimNever,
    oimFirst,
    oimAuthFail
};

/// QDialog simply showing the owner information
class SecOwnerDlg : public QDialog
{
    Q_OBJECT
public:
    SecOwnerDlg( QWidget *parent, const char * name, bool modal, bool fullscreen);

    QLabel* TextLabel1;
    QLabel* pxIcon;
    QLabel* lbHomePhoneLabel;
    QLabel* lbEmail;
    QLabel* lbNameLabel;
    QLabel* lbHomeMobile;
    QLabel* lbHomeAddressLabel;
    QLabel* lbWorkAddress;
    QLabel* lbEmailLabel;
    QLabel* lbWorkAddressLabel;
    QLabel* lbWorkPhoneLabel;
    QLabel* lbHomeAddress;
    QLabel* lbWorkMobile;
    QLabel* lbHomePhone;
    QLabel* lbWorkMobileLabel;
    QLabel* lbName;
    QLabel* lbWorkPhone;
    QLabel* lbHomeMobileLabel;
    QLabel* lbMessage;
    QPushButton* pbOK;

protected:
    QVBoxLayout* OwnerInfoDialogLayout;
    QHBoxLayout* Layout5;
    QGridLayout* Layout8;

private:
    struct Private;
    Private *d;
};

namespace Internal {
int runPlugins();
void showOwnerInfo();
}

}
}

#endif // MULTIAUTHCOMMON_H
