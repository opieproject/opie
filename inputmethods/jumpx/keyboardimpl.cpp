/**************************************************************************************94x78**
**
** This file may be distributed and/or modified under the terms of the
** GNU General Public License version 2 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
*********************************************************************************************/
#include <qapplication.h>
#include <qpixmap.h>
#include "keyboard.h"
#include "keyboardimpl.h"

/* XPM */
static const char * const icon_xpm[] = {
"26 13 2 1",
" 	c None",
".	c #000000",
" ... ... ... ... ... ...  ",
".   .   .   .   .   .   . ",
".   .   .   .   .   .   . ",
".   .   .   .   .   .   . ",
" ....... ... ... .......  ",
".       .   .   .       . ",
".       .   .   .       . ",
".       .   .   .       . ",
" ....... ... ... .......  ",
".   .   .   .   .   .   . ",
".   .   .   .   .   .   . ",
".   .   .   .   .   .   . ",
" ... ... ... ... ... ...  "};


KeyboardImpl::KeyboardImpl()
    : input(0), icn(0), ref(0)
{
}

KeyboardImpl::~KeyboardImpl()
{
    delete input;
    delete icn;
}

QWidget *KeyboardImpl::inputMethod( QWidget *parent, Qt::WFlags f )
{
    if ( !input )
	input = new JumpX::Keyboard( parent, "Keyboard", f );
    return input;
}

void KeyboardImpl::resetState()
{
    if ( input )
	input->resetState();
}

QPixmap *KeyboardImpl::icon()
{
    if ( !icn )
	icn = new QPixmap( (const char **)icon_xpm );
    return icn;
}

QString KeyboardImpl::name()
{
    return qApp->translate( "InputMethods", "JumpX" );
}

void KeyboardImpl::onKeyPress( QObject *receiver, const char *slot )
{
    if ( input )
	QObject::connect( input, SIGNAL(key(ushort,ushort,ushort,bool,bool)), receiver, slot );
}

#ifndef QT_NO_COMPONENT
QRESULT KeyboardImpl::queryInterface( const QUuid &uuid, QUnknownInterface **iface )
{
    *iface = 0;
    if ( uuid == IID_QUnknown )
	*iface = this;
    else if ( uuid == IID_InputMethod )
	*iface = this;

    if ( *iface )
	(*iface)->addRef();
    return QS_OK;
}

Q_EXPORT_INTERFACE()
{
    Q_CREATE_INSTANCE( KeyboardImpl )
}
#endif
