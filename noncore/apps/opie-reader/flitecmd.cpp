#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>
#include "flitecmd.h"

void CFliteCmd::output(const QString& txt)
{
  pid_t pid;
  if ((pid = fork()) <0)
    {
      qDebug("No memory");
    }
  else if (pid == 0)
    {
      execlp("flite", "flite", "-t", (const char*)txt, NULL);
      _exit(127);
      qDebug("Can't find flite");
    }
  else
    {
      int status;
      while (waitpid(pid, &status, 0) < 0)
	{
	  if (errno != EINTR)
	    {
	      qDebug("Error from flite");
	      break;
	    }
	}
      qDebug("flite status:%d", status);
    }
}

QString CFliteCmd::about()
{
  return QString("FliteCmd output codec (c) Tim Wentford\n");
}

extern "C"
{
  COutput* newcodec() { return new CFliteCmd; }
}
