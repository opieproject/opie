#include <qwidgetstack.h>
#include <qlayout.h>

#include "ofileselectormain.h"

OFileSelectorMain::OFileSelectorMain( QWidget* parent )
    : QWidget( parent ),   m_tool(0)
{
    m_lay = 0;
    init();
}
OFileSelectorMain::~OFileSelectorMain() {

}
void OFileSelectorMain::setToolbar( QWidget* tool ) {
    delete m_tool;
    m_tool = tool;
    add();
}
void OFileSelectorMain::setWidget( QWidget* wid ) {
    static int i = 0;
    m_stack->addWidget( wid, i );
    m_stack->raiseWidget( i );
    i++;
}
void OFileSelectorMain::add() {
    qWarning("adding items ");
    if (m_tool )
        m_lay->addWidget( m_tool, 0, 0 );

}
void OFileSelectorMain::init() {
    delete m_lay;

    m_lay = new QGridLayout( this, 2, 1 );
    m_lay->setRowStretch( 0,  1 );
    m_lay->setRowStretch( 1,  500 );

    m_stack = new QWidgetStack( this );
    m_lay->addWidget( m_stack, 1, 0 );
}
