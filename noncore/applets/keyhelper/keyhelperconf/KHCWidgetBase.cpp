/****************************************************************************
** Form implementation generated from reading ui file 'KHCWidgetBase.ui'
**
** Created: Mon Feb 28 09:58:22 2005
**      by:  The User Interface Compiler (uic)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/
#include "KHCWidgetBase.h"

#include <qcheckbox.h>
#include <qlabel.h>
#include <qlineedit.h>
#include <qmultilineedit.h>
#include <qpushbutton.h>
#include <qlayout.h>
#include <qvariant.h>
#include <qtooltip.h>
#include <qwhatsthis.h>

/* 
 *  Constructs a KHCWidgetBase which is a child of 'parent', with the 
 *  name 'name' and widget flags set to 'f' 
 */
KHCWidgetBase::KHCWidgetBase( QWidget* parent,  const char* name, WFlags fl )
    : QWidget( parent, name, fl )
{
    if ( !name )
	setName( "KHCWidgetBase" );
    resize( 431, 388 ); 
    setCaption( tr( "KeyHelperConf" ) );

    btnCtrl = new QPushButton( this, "btnCtrl" );
    btnCtrl->setGeometry( QRect( 280, 150, 121, 31 ) ); 
    btnCtrl->setText( tr( "Disable" ) );

    btnGen = new QPushButton( this, "btnGen" );
    btnGen->setGeometry( QRect( 10, 150, 110, 31 ) ); 
    btnGen->setText( tr( "Generate" ) );

    btnCopy = new QPushButton( this, "btnCopy" );
    btnCopy->setGeometry( QRect( 140, 150, 121, 31 ) ); 
    btnCopy->setText( tr( "Copy" ) );

    lblOrgK = new QLabel( this, "lblOrgK" );
    lblOrgK->setGeometry( QRect( 20, 50, 50, 20 ) ); 
    lblOrgK->setText( tr( "K" ) );

    lblMapK = new QLabel( this, "lblMapK" );
    lblMapK->setGeometry( QRect( 20, 120, 20, 20 ) ); 
    lblMapK->setText( tr( "K" ) );

    lblMapKeycode = new QLabel( this, "lblMapKeycode" );
    lblMapKeycode->setGeometry( QRect( 40, 120, 80, 21 ) ); 
    lblMapKeycode->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)7, (QSizePolicy::SizeType)1, lblMapKeycode->sizePolicy().hasHeightForWidth() ) );
    lblMapKeycode->setFrameShape( QLabel::Panel );
    lblMapKeycode->setFrameShadow( QLabel::Sunken );
    lblMapKeycode->setText( tr( "keycode" ) );

    txtMapKey = new QLineEdit( this, "txtMapKey" );
    txtMapKey->setGeometry( QRect( 200, 80, 200, 22 ) ); 

    txtOrgKey = new QLineEdit( this, "txtOrgKey" );
    txtOrgKey->setGeometry( QRect( 190, 10, 200, 22 ) ); 

    lblOriginal = new QLabel( this, "lblOriginal" );
    lblOriginal->setGeometry( QRect( 10, 10, 120, 31 ) ); 
    lblOriginal->setFrameShape( QLabel::Box );
    lblOriginal->setFrameShadow( QLabel::Raised );
    lblOriginal->setText( tr( "Original Key" ) );

    lblMapping = new QLabel( this, "lblMapping" );
    lblMapping->setGeometry( QRect( 10, 80, 120, 21 ) ); 
    lblMapping->setFrameShape( QLabel::Box );
    lblMapping->setFrameShadow( QLabel::Raised );
    lblMapping->setText( tr( "Mapping Key" ) );

    mleDefine = new QMultiLineEdit( this, "mleDefine" );
    mleDefine->setGeometry( QRect( 10, 200, 391, 110 ) ); 
    mleDefine->setReadOnly( TRUE );

    lblOrgKeycode = new QLabel( this, "lblOrgKeycode" );
    lblOrgKeycode->setGeometry( QRect( 50, 50, 70, 21 ) ); 
    lblOrgKeycode->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)7, (QSizePolicy::SizeType)1, lblOrgKeycode->sizePolicy().hasHeightForWidth() ) );
    lblOrgKeycode->setFrameShape( QLabel::Panel );
    lblOrgKeycode->setFrameShadow( QLabel::Sunken );
    lblOrgKeycode->setText( tr( "keycode" ) );

    lblOrgU = new QLabel( this, "lblOrgU" );
    lblOrgU->setGeometry( QRect( 130, 50, 20, 21 ) ); 
    lblOrgU->setText( tr( "U" ) );

    lblOrgUnicode = new QLabel( this, "lblOrgUnicode" );
    lblOrgUnicode->setGeometry( QRect( 150, 50, 80, 21 ) ); 
    lblOrgUnicode->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)7, (QSizePolicy::SizeType)1, lblOrgUnicode->sizePolicy().hasHeightForWidth() ) );
    lblOrgUnicode->setFrameShape( QLabel::Panel );
    lblOrgUnicode->setFrameShadow( QLabel::Sunken );
    lblOrgUnicode->setText( tr( "unicode" ) );

    lblMapU = new QLabel( this, "lblMapU" );
    lblMapU->setGeometry( QRect( 130, 120, 20, 21 ) ); 
    lblMapU->setText( tr( "U" ) );

    lblMapUnicode = new QLabel( this, "lblMapUnicode" );
    lblMapUnicode->setGeometry( QRect( 150, 120, 80, 21 ) ); 
    lblMapUnicode->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)7, (QSizePolicy::SizeType)1, lblMapUnicode->sizePolicy().hasHeightForWidth() ) );
    lblMapUnicode->setFrameShape( QLabel::Panel );
    lblMapUnicode->setFrameShadow( QLabel::Sunken );
    lblMapUnicode->setText( tr( "unicode" ) );

    chkMapShift = new QCheckBox( this, "chkMapShift" );
    chkMapShift->setGeometry( QRect( 260, 120, 33, 19 ) ); 
    chkMapShift->setText( tr( "S" ) );
    chkMapShift->setAutoResize( TRUE );

    chkMapCtrl = new QCheckBox( this, "chkMapCtrl" );
    chkMapCtrl->setGeometry( QRect( 310, 120, 34, 19 ) ); 
    chkMapCtrl->setText( tr( "C" ) );
    chkMapCtrl->setAutoResize( TRUE );

    chkMapAlt = new QCheckBox( this, "chkMapAlt" );
    chkMapAlt->setGeometry( QRect( 360, 120, 34, 19 ) ); 
    chkMapAlt->setText( tr( "A" ) );
    chkMapAlt->setAutoResize( TRUE );

    chkOrgShift = new QCheckBox( this, "chkOrgShift" );
    chkOrgShift->setGeometry( QRect( 250, 50, 33, 19 ) ); 
    chkOrgShift->setText( tr( "S" ) );
    chkOrgShift->setAutoResize( TRUE );

    chkOrgCtrl = new QCheckBox( this, "chkOrgCtrl" );
    chkOrgCtrl->setGeometry( QRect( 300, 50, 34, 19 ) ); 
    chkOrgCtrl->setText( tr( "C" ) );
    chkOrgCtrl->setAutoResize( TRUE );

    chkOrgAlt = new QCheckBox( this, "chkOrgAlt" );
    chkOrgAlt->setGeometry( QRect( 350, 50, 34, 19 ) ); 
    chkOrgAlt->setText( tr( "A" ) );
    chkOrgAlt->setAutoResize( TRUE );

    // tab order
    setTabOrder( txtOrgKey, chkOrgShift );
    setTabOrder( chkOrgShift, chkOrgCtrl );
    setTabOrder( chkOrgCtrl, chkOrgAlt );
    setTabOrder( chkOrgAlt, txtMapKey );
    setTabOrder( txtMapKey, chkMapShift );
    setTabOrder( chkMapShift, chkMapCtrl );
    setTabOrder( chkMapCtrl, chkMapAlt );
    setTabOrder( chkMapAlt, btnGen );
    setTabOrder( btnGen, btnCopy );
    setTabOrder( btnCopy, btnCtrl );
    setTabOrder( btnCtrl, mleDefine );
}

/*  
 *  Destroys the object and frees any allocated resources
 */
KHCWidgetBase::~KHCWidgetBase()
{
    // no need to delete child widgets, Qt does it all for us
}

