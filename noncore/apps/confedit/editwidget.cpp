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
    EditWidgetLayout->setSpacing( 3 );
    EditWidgetLayout->setMargin( 3 );

    QLabel *TextLabelFileName = new QLabel( this, "TextLabelFileName" );
    TextLabelFileName->setText( tr( "File Name:" ) );
    EditWidgetLayout->addWidget( TextLabelFileName, 0, 0 );
    TextFileName = new QLabel( this, "TextFileName" );
    EditWidgetLayout->addWidget( TextFileName, 0, 1 );

    QLabel *TextLabelGroup = new QLabel( this, "TextLabelGroup" );
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

}


EditWidget::~EditWidget()
{
}


void EditWidget::isKey(bool h)
{
	if (h)
 	{
    LineEditGroup->setEnabled(false);
    TextLabelKey->show();
    LineEditKey->show();
    TextLabelValue->show();
    LineEditValue->show();
  }else{
    LineEditGroup->setEnabled(true);
    TextLabelKey->hide();
    LineEditKey->hide();
    TextLabelValue->hide();
    LineEditValue->hide();
  }
  update();
}

