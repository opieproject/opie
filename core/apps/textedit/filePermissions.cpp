/****************************************************************************
** copyright 2002 ljp ljp@llornkcor.com
** Created: Sat Feb 23 19:44:40 2002 L.J. Potter
**
** This file may be distributed and/or modified under the terms of the
** GNU General Public License version 2 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
****************************************************************************/
#include "filePermissions.h"

#include <qfile.h>
#include <qfileinfo.h>

#include <qcheckbox.h>
#include <qlabel.h>
#include <qlineedit.h>
#include <qlayout.h>
#include <qvariant.h>
#include <qtooltip.h>
#include <qmessagebox.h>

#include <unistd.h>
#include <sys/stat.h>
#include <stdlib.h> 
#include <sys/types.h>
#include <pwd.h>
#include <grp.h>

filePermissions::filePermissions( QWidget* parent,  const char* name, bool modal, WFlags fl, const QString &fileName )
    : QDialog( parent, name, modal, fl )
{
    if ( !name )
  setName( tr("File Permissions") );
//    qDebug("FilePermissions "+fileName);
    resize( 236, 210 ); 
    setMaximumSize( QSize( 236, 210 ) );
    setCaption( tr( "Set File Permissions" ) );

    TextLabel1 = new QLabel( this, "TextLabel1" );
    TextLabel1->setGeometry( QRect( 25, 5, 175, 20 ) ); 
    TextLabel1->setText( tr( "Set file permissions for:" ) );

    LineEdit1 = new QLineEdit( this, "LineEdit1" );
    LineEdit1->setGeometry( QRect( 10, 25, 218, 22 ) );
    LineEdit1->setReadOnly(true);

    TextLabel4 = new QLabel( this, "TextLabel4" );
    TextLabel4->setGeometry( QRect( 5, 85, 50, 15 ) ); 
    TextLabel4->setText( tr( "owner" ) );

    TextLabel4_2 = new QLabel( this, "TextLabel4_2" );
    TextLabel4_2->setGeometry( QRect( 5, 105, 50, 15 ) ); 
    TextLabel4_2->setText( tr( "group" ) );

    TextLabel4_3 = new QLabel( this, "TextLabel4_3" );
    TextLabel4_3->setGeometry( QRect( 5, 125, 50, 15 ) ); 
    TextLabel4_3->setText( tr( "others" ) );

    CheckBox1 = new QCheckBox( this, "CheckBox1" );
    CheckBox1->setGeometry( QRect( 75, 85, 20, 16 ) ); 
    connect(CheckBox1, SIGNAL(released()),this,SLOT(ownReadCheck()));
    
    CheckBox1_2 = new QCheckBox( this, "CheckBox1_2" );
    CheckBox1_2->setGeometry( QRect( 135, 85, 20, 16 ) ); 
    connect(CheckBox1_2, SIGNAL(released()),this,SLOT(ownWriteCheck()));

    CheckBox1_3 = new QCheckBox( this, "CheckBox1_3" );
    CheckBox1_3->setGeometry( QRect( 195, 85, 20, 16 ) ); 
    connect(CheckBox1_3, SIGNAL(released()),this,SLOT(ownExeCheck()));

    CheckBox1_4 = new QCheckBox( this, "CheckBox1_4" );
    CheckBox1_4->setGeometry( QRect( 75, 105, 20, 16 ) ); 
    connect(CheckBox1_4, SIGNAL(released()),this,SLOT(grpReadCheck()));

    CheckBox1_5 = new QCheckBox( this, "CheckBox1_5" );
    CheckBox1_5->setGeometry( QRect( 135, 105, 20, 16 ) ); 
    connect(CheckBox1_5, SIGNAL(released()),this,SLOT(grpWriteCheck()));

    CheckBox1_6 = new QCheckBox( this, "CheckBox1_6" );
    CheckBox1_6->setGeometry( QRect( 195, 105, 20, 16 ) ); 
    connect(CheckBox1_6, SIGNAL(released()),this,SLOT(grpExeCheck()));

    CheckBox1_7 = new QCheckBox( this, "CheckBox1_7" );
    CheckBox1_7->setGeometry( QRect( 75, 125, 16, 16 ) ); 
    connect(CheckBox1_7, SIGNAL(released()),this,SLOT(wrldReadCheck()));

    CheckBox1_8 = new QCheckBox( this, "CheckBox1_8" );
    CheckBox1_8->setGeometry( QRect( 135, 125, 20, 16 ) );
    connect(CheckBox1_8, SIGNAL(released()),this,SLOT(wrldWriteCheck()));

    CheckBox1_8_2 = new QCheckBox( this, "CheckBox1_8_2" );
    CheckBox1_8_2->setGeometry( QRect( 195, 125, 20, 16 ) ); 
    connect(CheckBox1_8_2, SIGNAL(released()),this,SLOT(wrldExeCheck()));

    GroupLineEdit = new QLineEdit( this, "GroupLineEdit" );
    GroupLineEdit->setGeometry( QRect( 125, 155, 106, 22 ) ); 

    OwnerLineEdit = new QLineEdit( this, "OwnerLineEdit" );
    OwnerLineEdit->setGeometry( QRect( 10, 155, 106, 22 ) ); 

    TextLabel5 = new QLabel( this, "TextLabel5" );
    TextLabel5->setGeometry( QRect( 45, 180, 40, 16 ) ); 
    TextLabel5->setText( tr( "Owner" ) );

    TextLabel5_2 = new QLabel( this, "TextLabel5_2" );
    TextLabel5_2->setGeometry( QRect( 155, 180, 40, 16 ) ); 
    TextLabel5_2->setText( tr( "Group" ) );

    ModeLine = new QLineEdit( this, "TextLabelMode" );
    ModeLine->setGeometry( QRect( 10, 60, 40, 15 ) ); 

    TextLabel3_2 = new QLabel( this, "TextLabel3_2" );
    TextLabel3_2->setGeometry( QRect( 60, 55, 50, 20 ) ); 
    TextLabel3_2->setText( tr( "read" ) );
    TextLabel3_2->setAlignment( int( QLabel::AlignBottom | QLabel::AlignHCenter ) );

    TextLabel3_2_2 = new QLabel( this, "TextLabel3_2_2" );
    TextLabel3_2_2->setGeometry( QRect( 120, 55, 50, 20 ) ); 
    TextLabel3_2_2->setText( tr( "write" ) );
    TextLabel3_2_2->setAlignment( int( QLabel::AlignBottom | QLabel::AlignHCenter ) );

    TextLabel3 = new QLabel( this, "TextLabel3" );
    TextLabel3->setGeometry( QRect( 180, 55, 50, 20 ) ); 
    TextLabel3->setText( tr( "execute" ) );
    TextLabel3->setAlignment( int( QLabel::AlignBottom | QLabel::AlignHCenter ) );

    struct stat buf;
    mode_t mode;
    file = fileName;
    QFileInfo fi(file);

    LineEdit1->setText( file);
    OwnerLineEdit->setText( fi.owner());
    GroupLineEdit->setText( fi.group());

    if( fi.permission( QFileInfo::ReadUser)) { CheckBox1->setChecked(true); }
    if( fi.permission( QFileInfo::WriteUser)) { CheckBox1_2->setChecked(true); }
    if( fi.permission( QFileInfo::ExeUser)) { CheckBox1_3->setChecked(true); }

    if( fi.permission( QFileInfo::ReadGroup)) { CheckBox1_4->setChecked(true); }
    if( fi.permission( QFileInfo::WriteGroup)) { CheckBox1_5->setChecked(true); }
    if( fi.permission( QFileInfo::ExeGroup)) { CheckBox1_6->setChecked(true); }

    if( fi.permission( QFileInfo::ReadOther)) { CheckBox1_7->setChecked(true); }
    if( fi.permission( QFileInfo::WriteOther)) { CheckBox1_8->setChecked(true); }
    if( fi.permission( QFileInfo::ExeOther)) { CheckBox1_8_2->setChecked(true); }
    
    stat(file.latin1(), &buf);
    mode = buf.st_mode;
    modeStr.sprintf("%#o", buf.st_mode & ~(S_IFMT) );
    ModeLine->setText(modeStr);
    bool ok;
    i_mode = modeStr.toInt(&ok,10);
    
}

/*  
 *  Destroys the object and frees any allocated resources
 */
filePermissions::~filePermissions()
{
}


// might this be better as a callback routine???
void filePermissions::ownReadCheck() {
    if(CheckBox1->isChecked()) { i_mode +=400; }
    else i_mode -=400;
    modeStr.sprintf("0%d",i_mode);
    ModeLine->setText( modeStr);
// 0400
}

void filePermissions::ownWriteCheck() {
    if(CheckBox1_2->isChecked()) { i_mode +=200; }
    else i_mode -=200;
    modeStr.sprintf("0%d",i_mode);
    ModeLine->setText(modeStr);
// 0200
}

void filePermissions::ownExeCheck() {
    if(CheckBox1_3->isChecked()) { i_mode +=100; }
    else i_mode -=100;
    modeStr.sprintf("0%d",i_mode);
    ModeLine->setText(modeStr);
// 0100
}

void filePermissions::grpReadCheck() {
    if(CheckBox1_4->isChecked()) { i_mode +=40; }
    else i_mode -=40;
    modeStr.sprintf("0%d",i_mode);
    ModeLine->setText(modeStr);
// 0040
}

void filePermissions::grpWriteCheck() {
    if(CheckBox1_5->isChecked()) { i_mode +=20; }
    else i_mode -=20;
    modeStr.sprintf("0%d",i_mode);
    ModeLine->setText(modeStr);
// 0020
}

void filePermissions::grpExeCheck() {
    if(CheckBox1_6->isChecked()) { i_mode +=10; }
    else i_mode -=10;
    modeStr.sprintf("0%d",i_mode);
    ModeLine->setText(modeStr);
// 0010
}

void filePermissions::wrldReadCheck() {
    if(CheckBox1_7->isChecked()) { i_mode +=4; }
    else i_mode -=4;
    modeStr.sprintf("0%d",i_mode);
    ModeLine->setText(modeStr);
// 0004
}

void filePermissions::wrldWriteCheck() {
    if(CheckBox1_8->isChecked()) { i_mode +=2; }
    else i_mode -=2;
    modeStr.sprintf("0%d",i_mode);
    ModeLine->setText(modeStr);
// 0002
}

void filePermissions::wrldExeCheck() {
    if(CheckBox1_8_2->isChecked()) { i_mode +=1; }
    else i_mode -=1;
    modeStr.sprintf("0%d",i_mode);
    ModeLine->setText(modeStr);
// 0001
}

void filePermissions::accept() {

    QFileInfo fi(file);
    struct passwd *pwd=0;
    struct group *grp=0;
    pwd = getpwnam(OwnerLineEdit->text().latin1() );
    if(pwd == NULL) {
        perror("getpwnam");
        QMessageBox::warning(this,tr("Warning"),tr("Error- no user"));
        return;
    } else {
        grp = getgrnam(GroupLineEdit->text().latin1());
        if(grp==NULL) {
        perror("getgrnam");
        QMessageBox::warning(this,tr("Warning"),tr("Error- no  group"));
        return;
        }
        if( chown( file.latin1(),  pwd->pw_uid, grp->gr_gid) <0) {
        perror("chown");
        QMessageBox::warning(this,tr("Warning"),tr("Error setting ownership or group"));
        return;
        }
        bool ok;
        uint moder = modeStr.toUInt(&ok,8);
        if( chmod( file.latin1(), moder) < 0) {
            perror("chmod");
            QMessageBox::warning(this,tr("Warning"),tr("Error setting mode"));
            return;
        }
    }
    close();
}
