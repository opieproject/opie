#ifndef _PROCESS_INVOKER_H_
#define _PROCESS_INVOKER_H_

#include <qobject.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/time.h>
#include <signal.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>

#include <qtimer.h>
#include <qdatastream.h>
#include <qcstring.h>
#include <qpe/qcopenvelope_qws.h>
#include <qpe/global.h>
//#include "KHUtil.h"
#include "StringParser.h"

typedef void Sigfunc(int);

#define	SC_CHANNEL	"QPE/ShellCommander"

/* Sigleton Object */
class ProcessInvoker : public QObject
{
	Q_OBJECT
public:
	static ProcessInvoker& getInstance()
	{
		static ProcessInvoker instance;
		return(instance);
	}

	bool run();
	bool run(const QString& args);
	void terminate();
	void terminate(pid_t pid);
	void kill();
	void kill(pid_t pid);
	void setCommand(const QString& command){
		m_arguments.clear();
		addArgument(command);
	}
	void setArguments(const QStringList& arglist){
		m_arguments = arglist;
	}
	void setArguments(const QString& arguments){
		//setArguments(KHUtil::parseArgs(arguments));
		setArguments(StringParser::split(' ', arguments));
	}
	void addArgument(const QString& argument){
		m_arguments.append(argument);
	}
	void addArguments(const QString& arguments){
		QStringList arglist;
		//arglist = KHUtil::parseArgs(arguments);
		arglist = StringParser::split(' ', arguments);
		addArguments(arglist);
	}
	void addArguments(const QStringList& arglist){
		for(QStringList::ConstIterator it=arglist.begin();
				it!=arglist.end(); ++it){
			addArgument(*it);
		}
	}
	//const QStringList parseArgs(const QString& arguments);
	void setRunning(int pid);
	void setNotify(bool enable=true){
		m_isNotify = enable;
	}

	bool isRunning(){
		return(m_isRunning);
	}
	void notifyFinish(int status, bool success=true);

	pid_t m_child;

	friend class Dummy; /* for compile warning */
signals:
	void start(int, QStringList);
	void finish(QString,int);
private:
	ProcessInvoker();
	ProcessInvoker(const ProcessInvoker&);
	ProcessInvoker& operator=(const ProcessInvoker&);
	~ProcessInvoker();

	class Dummy{}; /* for compile warning */

	QTimer* m_pTimer;
	QStringList m_arguments;

	bool m_isRunning;
	bool m_isNotify;

	Sigfunc* m_defChildHandler;

	int m_stdfd[2];
	int m_errfd[2];
	int m_maxfdp1;

	bool openPipe();
	void closePipe(int fd[] = NULL, int n = 2);
	void notifyStatus(const QString& result, int code);

	void workerProc();
private slots:
	void readOutputs();
};

#endif /* _PROCESS_INVOKER_H_ */
