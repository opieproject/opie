#include "keyedit.h"
#include <qlineedit.h>

KeyEdit::KeyEdit(QWidget* parent, const char* name) :
	QLineEdit(parent, name)
{
}

KeyEdit::~KeyEdit()
{
}

void KeyEdit::focusInEvent(QFocusEvent *event)
{
	setEchoMode(Normal);
}

void KeyEdit::focusOutEvent(QFocusEvent *event)
{
	setEchoMode(Password);
}
