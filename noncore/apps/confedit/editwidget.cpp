#include "editwidget.h"

#include <qlabel.h>
#include <qlineedit.h>
#include <qpushbutton.h>
#include <qlayout.h>
#include <qvariant.h>
#include <qtooltip.h>
#include <qwhatsthis.h>
#include <qtimer.h>
#include <qpopupmenu.h>
#include <qaction.h>
#include "listviewitemconfigentry.h"

EditWidget::EditWidget( QWidget* parent,  const char* name, WFlags fl )
    : QWidget( parent, name, fl )
{

    EditWidgetLayout = new QGridLayout( this );
    EditWidgetLayout->setSpacing( 0 );
    EditWidgetLayout->setMargin( 0 );
//    setSizePolicy( QSizePolicy( QSizePolicy::Minimum, QSizePolicy::Maximum));//, sizePolicy().hasHeightForWidth() ) );

    TextLabelFileName = new QLabel( this, "TextLabelFileName" );
    TextLabelFileName->setText( tr( "File Name:" ) );
    EditWidgetLayout->addWidget( TextLabelFileName, 0, 0 );
    TextFileName = new QLabel( this, "TextFileName" );
    EditWidgetLayout->addWidget( TextFileName, 0, 1 );

    TextLabelGroup = new QLabel( this, "TextLabelGroup" );
    TextLabelGroup->setText( tr( "Group:" ) );
    EditWidgetLayout->addWidget( TextLabelGroup, 1, 0 );
    LineEditGroup = new QLineEdit( this, "LineEditGroup" );
    EditWidgetLayout->addWidget( LineEditGroup, 1, 1 );

    TextLabelKey = new QLabel( this, "TextLabelKey" );
    TextLabelKey->setText( tr( "Key:" ) );
    EditWidgetLayout->addWidget( TextLabelKey, 2, 0 );
    LineEditKey = new QLineEdit( this, "LineEditKey" );
    EditWidgetLayout->addWidget( LineEditKey, 2, 1 );

    TextLabelValue = new QLabel( this, "TextLabelValue" );
    TextLabelValue->setText( tr( "Value:" ) );
    EditWidgetLayout->addWidget( TextLabelValue, 3, 0 );
    LineEditValue = new QLineEdit( this, "LineEditValue" );
    EditWidgetLayout->addWidget( LineEditValue, 3, 1 );

  //  layoutType(Nothing);
}


EditWidget::~EditWidget()
{
}


void EditWidget::layoutType(int t)
{
	switch (t)
 	{
	 	case Entry:
  	  LineEditGroup->setEnabled(false);
    	TextLabelKey->show();
	    LineEditKey->show();
  	  TextLabelValue->show();
    	LineEditValue->show();
     	TextLabelGroup->show();
    	LineEditGroup->show();
     	TextLabelFileName->show();
      TextFileName->show();
	    break;
  	case Group:
    	LineEditGroup->setEnabled(true);
	    TextLabelKey->hide();
  	  LineEditKey->hide();
    	TextLabelValue->hide();
	    LineEditValue->hide();
     	TextLabelGroup->show();
    	LineEditGroup->show();
     	TextLabelFileName->show();
      TextFileName->show();
  	  break;
   	default:
	    TextLabelKey->hide();
  	  LineEditKey->hide();
    	TextLabelValue->hide();
	    LineEditValue->hide();
     	TextLabelGroup->hide();
    	LineEditGroup->hide();
     	TextLabelFileName->hide();
      TextFileName->hide();
  }
//  update();
// updateGeometry();
//  adjustSize();
}

