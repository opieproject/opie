#include "editwindow.h"

using namespace Opie;
using namespace Opie::Notes;

editWindowWidget::editWindowWidget( QWidget* parent,  const char* name, bool modal, WFlags fl ) : QDialog( parent, name, modal, fl )
{
    setCaption( tr( "Information:" ) );
    QGridLayout *gridLayout = new QGridLayout(this, 1, 1, 5, 5);
    editArea = new QMultiLineEdit(this, "editArea");
    gridLayout->addWidget(editArea, 0, 0);
    editArea->setWordWrap(QMultiLineEdit::WidgetWidth);

    showMaximized();
}

editWindowWidget::~editWindowWidget()
{
}

void editWindowWidget::readMemo( const OPimMemo &memo )
{
    editArea->setText( memo.text() );
}

void editWindowWidget::writeMemo( OPimMemo &memo )
{
    memo.setText( editArea->text() );
}

