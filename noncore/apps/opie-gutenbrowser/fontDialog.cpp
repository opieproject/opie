/****************************************************************************
** Created: Sun Jan 27 11:03:24 2002
copyright 2002 by L.J. Potter ljp@llornkcor.com
    copyright            : (C) 2000 -2004 by llornkcor
    email                : ljp@llornkcor.com
****************************************************************************/
#include "fontDialog.h"
#include "optionsDialog.h"

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
#include <qcombobox.h>

//#define BUGGY_SHARP_ZAURUS 0

static const int nfontsizes = 9;
static const int fontsize[nfontsizes] = {8,9,10,11,12,13,14,18,24};


FontDialog::FontDialog( QWidget * parent,  const char* name /*, bool modal, WFlags fl */)
        :/* QDialog*/ QWidget( parent, name /*, modal, fl */)
{
    if ( !name )
        setName( "FontDialog" );
    setCaption( tr( "Font Dialog" ) );

    QGridLayout *layout = new QGridLayout( this );
    layout->setSpacing(2);
    layout->setMargin(2);

    familyListBox = new QListBox( this, "familyListBox" );
    layout->addMultiCellWidget( familyListBox, 0, 2, 0, 0);

    QBoxLayout * hbox = new QHBoxLayout(this);
    QBoxLayout * vbox = new QVBoxLayout(this);

    styleListBox = new QListBox( this, "styleListBox" );
    vbox->addWidget( styleListBox, 0);

    FontTextLabel4 = new QLabel( this, "TextLabel4" );
    FontTextLabel4->setText( tr( "Size" ) );
    hbox->addWidget( FontTextLabel4,0);
    
    sizeComboBox = new QComboBox( FALSE, this, "SizeCombo");
//    sizeComboBox->setMaximumWidth(60);
    hbox->addWidget( sizeComboBox, 0);

    vbox->addLayout(hbox,0);
    layout->addLayout( vbox,0,3);
    
    MultiLineEdit1 = new QMultiLineEdit( this, "MultiLineEdit1" );
    MultiLineEdit1->setText( tr( "The Quick Brown Fox Jumps Over The Lazy Dog" ) );
    MultiLineEdit1->setWordWrap( QMultiLineEdit::WidgetWidth);

    layout->addMultiCellWidget( MultiLineEdit1, 4, 4, 0, 3);

    connect(familyListBox,SIGNAL(highlighted(const QString &)),SLOT(familyListBoxSlot(const QString &)));
    connect(styleListBox,SIGNAL(highlighted(const QString &)),SLOT(styleListBoxSlot(const QString &)));
    connect(sizeComboBox,SIGNAL(activated(const QString &)),SLOT(sizeComboBoxSlot(const QString &)));

    populateLists();

}

FontDialog::~FontDialog()
{
}

void FontDialog::familyListBoxSlot(const QString & text)
{
  int styleInt = styleListBox->currentItem();
  int sizeInt = sizeComboBox->currentText().toInt();

    sizeComboBox->clear();
    styleListBox->clear();
//  clearListBoxes();
  family = text;
//  qDebug(family);
  QStringList styles = fdb.styles( family ); // string list of styles of our current font family
  styleListBox->insertStringList( styles);
  QString dstyle;// = "\t" + style + " (";

#ifdef BUGGY_SHARP_ZAURUS   

QValueList<int> smoothies = fdb.smoothSizes( family, styleListBox->text(0) );
   for ( QValueList<int>::Iterator points = smoothies.begin(); points != smoothies.end(); ++points ) {
       dstyle = QString::number( *points );
       qDebug(dstyle);
      sizeComboBox->insertItem(  dstyle.left( dstyle.length() - 1 ));
  }
#else

   for (int i=0; i<nfontsizes; i++) {
       sizeComboBox->insertItem( QString::number(fontsize[i]));
       if(fontsize[i] == sizeInt) {
           sizeComboBox->setCurrentItem(i);
       }
   }
#endif

  if(styleInt == -1 || styleInt > styleListBox->count() )
      styleListBox->setCurrentItem(0);
  else
      styleListBox->setCurrentItem(styleInt);

  changeText();
}

void FontDialog::styleListBoxSlot(const QString &text)
{
    changeText();
}

void FontDialog::sizeComboBoxSlot(const QString & text)
{
    changeText();
}

void FontDialog::populateLists()
{
//  QFont defaultFont=MultiLineEdit1->font();
//          QFont defaultFont=Lview->font();
//          QFontInfo fontInfo(defaultFont);
          Config config("Gutenbrowser");
          config.setGroup("Font");
          QString familyStr = config.readEntry("Family", "fixed");
          QString styleStr = config.readEntry("Style", "Regular");
          QString sizeStr = config.readEntry("Size", "10");
          QString charSetStr = config.readEntry("CharSet", "iso10646-1" );
          bool ok;
          int i_size = sizeStr.toInt(&ok,10);
          selectedFont = fdb.font(familyStr,styleStr,i_size,charSetStr);
//          defaultFont.setItalic(TRUE);
         families = fdb.families();

  for ( QStringList::Iterator f = families.begin(); f != families.end();++f ) {
      QString family = *f;
//      if(family == defaultFont.family())
//          qDebug(family);
      familyListBox->insertItem( family);

      if( familyListBox->text(0) == family) {
          QStringList styles = fdb.styles( family );
// string list of styles of our current font family
          styleListBox->insertStringList( styles);

          for ( QStringList::Iterator s = styles.begin(); s != styles.end();++s ) { // for each font style
             style = *s;
              QString dstyle;// = "\t" + style + " (";
              if(styleListBox->text(0) == style) {

                  QValueList<int> smoothies = fdb.smoothSizes( family, style );
                  for ( QValueList<int>::Iterator points = smoothies.begin(); points != smoothies.end(); ++points ) {
                      dstyle = QString::number( *points ) + " ";
                      sizeComboBox ->insertItem( dstyle.left( dstyle.length() - 1 ));
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

   for (int i=0; i<sizeComboBox->count(); i++) {

#ifdef BUGGY_SHARP_ZAURUS
       if(sizeComboBox->text(i) == sizeStr)
#else
       if(fontsize[i] == i_size) 
#endif
           sizeComboBox->setCurrentItem(i);
   }
  changeText();
}

void FontDialog::clearListBoxes() {
    familyListBox->clear();
    sizeComboBox->clear();
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

    if( sizeComboBox->currentItem() == -1 )
        size = sizeComboBox->text(0);
    else {
     size = sizeComboBox->currentText();
    }

//       qDebug("Font size is "+size);
    bool ok;
    int i_size = size.toInt(&ok,10);
    QStringList charSetList = fdb.charSets(family);
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
    Config cfg("Gutenbrowser");
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

    MultiLineEdit1->setFont( selectedFont);
    MultiLineEdit1->update();
    changedFonts=TRUE;
}

