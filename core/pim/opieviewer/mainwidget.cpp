
#include <qlayout.h>

#include "mainlistview.h"
#include "maintextview.h"
#include "mainwidget.h"

using namespace Viewer;

MainWidget::MainWidget( QWidget* parent,  const char* name )
    : QWidget( parent, name ) {
    qWarning("GUI");
    m_layout = new QVBoxLayout( this, 0, -1, "Hija" );

    m_textView = new RichTextView( this );
    m_mainView = new MainListView( this );

    m_layout->addWidget(m_mainView,  100 );
    m_layout->addWidget(m_textView,  0 );
}
MainWidget::~MainWidget() {

}
RichTextView* MainWidget::textView() const {
    return m_textView;
}
MainListView* MainWidget::listView() const {
    return m_mainView;
}


