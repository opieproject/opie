#include <qwindowsystem_qws.h>
#include <qapplication.h>

#include "oqwsserver.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h> 

#include <syslog.h>
#include <stdio.h>

#define APPNAME "op-qws"

void toSyslog(QtMsgType type, const char *msg)
{
    int level = LOG_INFO;
    switch (type) {
        case QtDebugMsg:
            level = LOG_DEBUG;
            break;
        case QtWarningMsg:
            level = LOG_WARNING;
            break;
        case QtFatalMsg:
            level = LOG_ERR;
            break;
    }
    syslog (LOG_DAEMON | level, msg);
}

int daemon_init(void)
{
    pid_t pid;

    if ((pid = fork()) < 0)
        return(-1);
    else if (pid != 0)
        exit(0);

    setsid();

    chdir("/");	

    umask(0);

    fclose(stdout);
    fclose(stderr);
    fclose(stdin);

    return(0);
}

int main( int argc, char ** argv )
{
    while (argc > 1) {
        if (strcmp(argv[--argc], "-d") == 0) {
            // daemonize
            openlog(APPNAME, 0, LOG_DAEMON);
            qInstallMsgHandler(toSyslog);
            if (daemon_init() != 0) {
                fprintf(stderr, "%s: Error: Unable to daemonize\n", APPNAME);
                return 1;
	    }
	}
    }

    OQWSServer a(argc, argv, QApplication::GuiServer);
    return a.exec();
}
