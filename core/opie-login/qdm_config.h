//-- -*- c++ -*-

#ifndef _QDM_CONFIG_H
#define _QDM_CONFIG_H

/*
 * Config file for QDM
 */


/** define this to enable the qdm login dialog for qpe */
#define QT_QWS_LOGIN

/** define this to let qdm use Linux-PAM */
//define QT_QWS_LOGIN_USEPAM

/** this is the list of users shown in the input-list */
#define QDM_SHOWNUSERS static char *Shown_Users[] = { "guest", "root", "lisa", 0 };

/** which command to execute when going to sleep mode */
#define QDM_CMD_SLEEP		"/sbin/shutdow","-z","now"

/** which command to execute when shutting down */
#define QDM_CMD_SHUTDOWN	"/sbin/shutdow","-z","now"

/** Welcome string on dialog */
#define QDM_WELCOME_STRING	"Welcome to\nmLinux [iPAQ]"


#endif //-- _QDM_CONFIG_H
