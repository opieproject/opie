#include <qwidget.h>
#include <qcheckbox.h>
#include <qlabel.h>
#include <qsignalmapper.h>
#include <qpushbutton.h>
#include <qpe/resource.h>

#include "exampleboardimpl.h"

ExampleBoard::ExampleBoard(QWidget* par, WFlags fl )
	: QHBox(par, "name", fl )
{
	QCheckBox *box1 = new QCheckBox(tr("Alt"),this);
	connect(box1,SIGNAL(toggled(bool)),
			this,SLOT(slotAlt(bool)));
	m_alt = box1;
	box1 = new QCheckBox(tr("Shift"),this );
	connect(box1,SIGNAL(toggled(bool)),
			this,SLOT(slotShift(bool)));
	m_shi = box1;
	box1 = new QCheckBox(tr("Ctrl","Control Shortcut on keyboard"),this );
	connect(box1,SIGNAL(toggled(bool)),
			this,SLOT(slotCtrl(bool)));
	m_ctrl = box1;
			
	QSignalMapper *map = new QSignalMapper(this);
	QPushButton *btn = new QPushButton("a",this);
	map->setMapping(btn,0);
	connect(btn,SIGNAL(clicked()),map,SLOT(map()));
	
	btn = new QPushButton("b",this);
	map->setMapping(btn,1);
	connect(btn,SIGNAL(clicked()),map,SLOT(map()));
	
	btn = new QPushButton("c",this);
	map->setMapping(btn,2);
	connect(btn,SIGNAL(clicked()),map,SLOT(map()));
	
	connect(map,SIGNAL(mapped(int)),
			this,SLOT(slotKey(int)));
	resetState();
}

ExampleBoard::~ExampleBoard(){
}

void ExampleBoard::resetState(){
	m_state = 0;
	m_shi->setChecked(false);
	m_ctrl->setChecked(false);
	m_alt->setChecked(false);
}

void ExampleBoard::slotKey(int _ke){	
	int ke = _ke + 0x61; // 0 + 65 = 0x41 == A
	if(m_state & ShiftButton )	
		ke -= 0x20;
	
	/*
	 *  Send the key
	 * ke is the unicode
	 * _ke + 0x41 is the keycode
	 *  m_state Normally the state 
	 * down/up
	 * auto repeat
	 */
	emit key(ke, _ke +0x41,m_state,true,false);
	emit key(ke, _ke + 0x41,m_state,false,false);
}

void ExampleBoard::slotShift(bool b){
	if(b)
		m_state |= ShiftButton;
	else
		m_state &= ~ShiftButton;	
}

void ExampleBoard::slotAlt(bool b){
	if(b)
		m_state |= AltButton;
	else
		m_state &=  ~AltButton;
}

void ExampleBoard::slotCtrl(bool b){
	if(b)
		m_state |= ControlButton;
	else
		m_state &= ~ControlButton;
}



ExampleboardImpl::ExampleboardImpl()
    : m_pickboard(0), m_icn(0)
{
}

ExampleboardImpl::~ExampleboardImpl()
{
    delete m_pickboard;
    delete m_icn;
}

QWidget *ExampleboardImpl::inputMethod( QWidget *parent, Qt::WFlags f )
{
    if ( !m_pickboard )
	m_pickboard = new ExampleBoard( parent, f );
    return m_pickboard;
}

void ExampleboardImpl::resetState()
{
    if ( m_pickboard )
	m_pickboard->resetState();
}

QPixmap *ExampleboardImpl::icon()
{
    if ( !m_icn )
	m_icn = new QPixmap(Resource::loadPixmap("Tux"));
    return m_icn;
}

QString ExampleboardImpl::name()
{
    return QObject::tr("Example Input");
}

void ExampleboardImpl::onKeyPress( QObject *receiver, const char *slot )
{
    if ( m_pickboard )
	QObject::connect( m_pickboard, SIGNAL(key(ushort,ushort,ushort,bool,bool)), receiver, slot );
}

#ifndef QT_NO_COMPONENT
QRESULT ExampleboardImpl::queryInterface( const QUuid &uuid, QUnknownInterface **iface )
{
    *iface = 0;
    if ( uuid == IID_QUnknown )
	*iface = this;
    else if ( uuid == IID_InputMethod )
	*iface = this;
    else
	return QS_FALSE;

    if ( *iface )
	(*iface)->addRef();
    return QS_OK;
}

Q_EXPORT_INTERFACE()
{
    Q_CREATE_INSTANCE( ExampleboardImpl )
}
#endif

