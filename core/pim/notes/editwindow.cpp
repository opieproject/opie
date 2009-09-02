#include "editwindow.h"

using namespace Opie;
using namespace Opie::Notes;

editWindowWidget::editWindowWidget( QWidget* parent,  const char* name, bool modal, WFlags fl ) : QDialog( parent, name, modal, fl )
{
    setCaption( tr( "Information:" ) );
    QGridLayout *gridLayout = new QGridLayout(this, 1, 1, 5, 5);
    m_editArea = new QMultiLineEdit(this, "m_editArea");
    gridLayout->addWidget(m_editArea, 0, 0);
    m_editArea->setWordWrap(QMultiLineEdit::WidgetWidth);

    showMaximized();
}

editWindowWidget::~editWindowWidget()
{
}

void editWindowWidget::readMemo( const OPimMemo &memo )
{
    m_editArea->setText( memo.text() );
}

void editWindowWidget::writeMemo( OPimMemo &memo )
{
    memo.setText( m_editArea->text() );
}

