#ifndef __OPIE_LOGINAPPLICATION_H__
#define __OPIE_LOGINAPPLICATION_H__

#include <qstringlist.h>

#include <qpe/qpeapplication.h>

#ifdef USEPAM
struct pam_message;
struct pam_response;
#endif

class LoginApplication : public QPEApplication {
public:
	LoginApplication ( int &argc, char **argv );

	static bool checkPassword ( const char *user, const char *password );

	static const char *loginAs ( );
	static void setLoginAs ( const char *user );

	static bool changeIdentity ( );
	static bool login ( );

	static QStringList allUsers ( );

	void quitToConsole ( );

private:
	static const char *s_username;

#ifdef USEPAM
	static int pam_helper ( int num_msg, const struct pam_message **msg, struct pam_response **resp, void * );
	static const char *s_pam_password;
#endif
};

extern LoginApplication *lApp;

#endif
