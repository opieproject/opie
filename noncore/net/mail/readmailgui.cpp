#include "readmailgui.h"
#include "defines.h"
#include <qlayout.h>

ReadMailGui::ReadMailGui( QWidget* parent, const char* name, bool modal, WFlags fl )
: QDialog( parent, name, modal, fl ) {


QVBoxLayout * layout = new QVBoxLayout( this );
QMainWindow * m_mainWindow = new QMainWindow( this );
m_toolBar = new QToolBar( m_mainWindow );
m_mainWindow->addToolBar( m_toolBar );

m_reply = new QAction( tr( "Compose new mail" ), ICON_SENDQUEUED, 0, 0, m_mainWindow );
m_reply->addTo( m_toolBar );

m_textWindow = new QTextBrowser( this );
m_textWindow->setResizePolicy( QScrollView::AutoOneFit );

layout->addWidget( m_mainWindow );
layout->addWidget( m_textWindow );
}

ReadMailGui::~ReadMailGui() {
}

void ReadMailGui::passMessage( const QString & message ) {
  m_textWindow->setText( message );
}
