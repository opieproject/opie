#ifndef __OPIE_LOGINWINDOW_IMPL_H__
#define __OPIE_LOGINWINDOW_IMPL_H__

#include "loginwindow.h"

class InputMethods;

class LoginWindowImpl : public LoginWindow {
	Q_OBJECT
	
public:
	LoginWindowImpl ( );
	virtual ~LoginWindowImpl ( );	

protected slots:
	void restart ( );
	void quit ( );
	void showIM ( );
	void suspend ( );
	void backlight ( );
	void login ( );
	void toggleEchoMode ( bool );

protected:
	virtual void keyPressEvent ( QKeyEvent *e );

	QStringList getAllUsers ( );
	bool changeIdentity ( const char *user );

private:
	InputMethods *m_input;
};

#endif
