/****************************************************************************
** Created: Sun Jan 27 11:03:24 2002 fileDialog.cpp
copyright 2002 by L.J. Potter ljp@llornkcor.com

** This file may be distributed and/or modified under the terms of the
** GNU General Public License version 2 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
****************************************************************************/
#include "fontDialog.h"

#include <qpe/fontdatabase.h>
#include <qpe/config.h>

#include <qstringlist.h>
#include <qfontinfo.h>
#include <qvaluelist.h>
#include <qpe/qpeapplication.h>

#include <qgroupbox.h>
#include <qlabel.h>
#include <qlistbox.h>
#include <qmultilineedit.h>
#include <qpushbutton.h>
#include <qlayout.h>
#include <qvariant.h>
#include <qtooltip.h>
#include <qwhatsthis.h>

FontDialog::FontDialog( QWidget * parent,  const char* name , bool modal, WFlags fl )
        :QDialog /*QWidget*/( parent, name , modal, fl )
{
    if ( !name )
        setName( tr("FontDialog") );
    setCaption( tr( "Font Dialog" ) );
    FontTextLabel4 = new QLabel( this, "TextLabel4" );
    FontTextLabel4->setGeometry( QRect(  145, 100, 30, 19  /*192, 0, 43, 19*/ ) );
    FontTextLabel4->setText( tr( "Size" ) );

    FamilyGroup = new QGroupBox( this, "FamilyGroup" );
    FamilyGroup->setGeometry( QRect(  5, 5, 115, 135  /*0, 6, 185, 131 */) );
    FamilyGroup->setTitle( tr( "Font" ) );

    familyListBox = new QListBox( FamilyGroup, "familyListBox" );
    familyListBox->setGeometry( QRect( 5, 20, 100, 105  /* 6, 18, 170, 105 */) );

    GroupBox2 = new QGroupBox( this, "GroupBox2" );
    GroupBox2->setGeometry( QRect(   120, 5, 100, 90/* 0, 138, 170, 90*/ ) );
    GroupBox2->setTitle( tr( "Font Style" ) );

    styleListBox = new QListBox( GroupBox2, "styleListBox" );
    styleListBox->setGeometry( QRect( 5, 20, 90, 60 /*10, 20, 150, 61*/ ) );

    sizeListBox = new QListBox( this, "sizeListBox" );
    sizeListBox->setGeometry( QRect(  180, 100, 40, 60 /* 192, 24, 40, 108 */) );

    MultiLineEdit1 = new QMultiLineEdit( this, "MultiLineEdit1" );
    MultiLineEdit1->setGeometry( QRect(   5, 165, 215, 45 /* 6, 234, 222, 55*/ ) );
    MultiLineEdit1->setText( tr( "The Quick Brown Fox Jumps Over The Lazy Dog" ) );
    MultiLineEdit1->setWordWrap(QMultiLineEdit::WidgetWidth);

    connect( familyListBox,SIGNAL( highlighted( const QString &)),SLOT( familyListBoxSlot(const QString &) ));
    connect( styleListBox,SIGNAL(  highlighted( const QString &)),SLOT( styleListBoxSlot(const QString &) ));
    connect( sizeListBox,SIGNAL(  highlighted( const QString &)),SLOT( sizeListBoxSlot(const QString &) ));

    populateLists();
//     MultiLineEdit1
//      connect();

}

FontDialog::~FontDialog()
{
}

void FontDialog::familyListBoxSlot(const QString & text)
{
  int styleInt=styleListBox->currentItem();
  int sizeInt=sizeListBox->currentItem();

  styleListBox->clear();
  sizeListBox->clear();
  family= text;
//  qDebug(family);
  QStringList styles = fdb.styles( family ); // string list of styles of our current font family
  styleListBox->insertStringList( styles);
  QString dstyle;// = "\t" + style + " (";
  QValueList<int> smoothies = fdb.smoothSizes( family, styleListBox->text(0) );
  for ( QValueList<int>::Iterator points = smoothies.begin(); points != smoothies.end(); ++points ) {
      dstyle = QString::number( *points );
      sizeListBox ->insertItem( dstyle.left( dstyle.length() - 1 ));
  }
  if(styleInt == -1 || styleInt > styleListBox->count() )
      styleListBox->setCurrentItem(0);
  else
      styleListBox->setCurrentItem(styleInt);

  if(sizeInt == -1 || sizeInt > sizeListBox->count())
      sizeListBox->setCurrentItem(0);
  else
      sizeListBox->setCurrentItem(sizeInt);

  changeText();
}

void FontDialog::styleListBoxSlot(const QString &text)
{
    changeText();
}

void FontDialog::sizeListBoxSlot(const QString & text)
{
    changeText();
}

void FontDialog::populateLists()
{
//  QFont defaultFont=MultiLineEdit1->font();
//          QFont defaultFont=Lview->font();
//          QFontInfo fontInfo(defaultFont);
  Config config("TextEdit");
          config.setGroup("Font");
          QString familyStr = config.readEntry("Family", "fixed");
          QString styleStr = config.readEntry("Style", "Regular");
          QString sizeStr = config.readEntry("Size", "10");
          QString charSetStr = config.readEntry("CharSet", "iso10646-1" );
          bool ok;
          int i_size=sizeStr.toInt(&ok,10);
          selectedFont = fdb.font(familyStr,styleStr,i_size,charSetStr);
//          defaultFont.setItalic(TRUE);
  families = fdb.families();
  for ( QStringList::Iterator f = families.begin(); f != families.end();++f ) {
      QString family = *f;
//      if(family == defaultFont.family())
//          qDebug(family);
      familyListBox->insertItem( family);

      if( familyListBox->text(0) == family) {
          QStringList styles = fdb.styles( family ); // string list of styles of our current font family
          styleListBox->insertStringList( styles);
          for ( QStringList::Iterator s = styles.begin(); s != styles.end();++s ) { // for each font style
             style = *s;
              QString dstyle;// = "\t" + style + " (";
              if(styleListBox->text(0) == style) {
                  QValueList<int> smoothies = fdb.smoothSizes( family, style );
                  for ( QValueList<int>::Iterator points = smoothies.begin(); points != smoothies.end(); ++points ) {
                      dstyle = QString::number( *points ) + " ";

                      sizeListBox ->insertItem( dstyle.left( dstyle.length() - 1 ));
                  }
                  dstyle = dstyle.left( dstyle.length() - 1 ) + ")";
              }
          } // styles
      }
  }
  for(int i=0;i < familyListBox->count();i++) {
      if( familyListBox->text(i) == familyStr)
          familyListBox->setSelected( i, TRUE);
  }

  for(int i=0;i < styleListBox->count();i++) {
      if( styleListBox->text(i) == styleStr)
          styleListBox->setSelected( i, TRUE);
  }

  for(int i=0;i < sizeListBox->count();i++) {
      if( sizeListBox->text(i) == sizeStr)
          sizeListBox->setSelected( i, TRUE);
  }
  changeText();
}

void FontDialog::clearListBoxes() {
    familyListBox->clear();
    sizeListBox->clear();
    styleListBox->clear();
}

void FontDialog::changeText()
{
    if( familyListBox->currentItem() == -1)
        family= familyListBox->text(0);
    else {
        family = familyListBox->currentText();
    }
//          qDebug("Font family is "+family);
    if( styleListBox->currentItem() == -1)
        style=styleListBox->text(0);
    else {
        style = styleListBox->currentText();
    }
//          qDebug("font style is "+style);
    if( sizeListBox->currentItem() == -1 )
        size= sizeListBox->text(0);
    else {
     size = sizeListBox->currentText();
    }

//       qDebug("Font size is "+size);
    bool ok;
    int i_size= size.toInt(&ok,10);
    QStringList charSetList= fdb.charSets(family);
//      QStringList styles = fdb.styles( family ); // string list of styles of our current font family
    QString charSet;
          for ( QStringList::Iterator s = charSetList.begin(); s != charSetList.end();++s ) { // for each font style
            charSet = *s;
//            qDebug(charSet);
          }
    selectedFont = fdb.font(family,style,i_size,charSet);
    QFontInfo fontInfo( selectedFont);
//      if(fontInfo.italic() ) qDebug("italic");
    selectedFont.setWeight(fontInfo.weight() );
//      qDebug("Style are "+style+" %d ",fontInfo.weight());
  Config cfg("TextEdit");
    cfg.setGroup("Font");
    cfg.writeEntry("Family",family);
    cfg.writeEntry("Style",style);
    cfg.writeEntry("Size",size);
    cfg.writeEntry("CharSet",charSet);

    if(style.find("Italic",0,TRUE) != -1) {
          selectedFont = fdb.font(family,"Regular",i_size,charSet);
          selectedFont.setItalic(TRUE); //ya right
          cfg.writeEntry("Italic","TRUE");
//            qDebug("Style is "+styleListBox->currentText());
      } else
          cfg.writeEntry("Italic","FALSE");
  changedFonts=TRUE;
    MultiLineEdit1->setFont( selectedFont);
    MultiLineEdit1->update();
}

