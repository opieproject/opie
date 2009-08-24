#include "ProcessInvoker.h"

#ifndef PIPE_BUF // uClibc or similar
#include <linux/limits.h>
#endif

static ProcessInvoker* g_this;
/* ------------------------------------------------------------------------ */
/*	static functions														*/
/* ------------------------------------------------------------------------ */

static Sigfunc* setSignalHandler(int signo, Sigfunc* handler)
{
	struct sigaction act,oact;

	act.sa_handler = handler;
	::sigemptyset(&act.sa_mask);
	act.sa_flags = 0;
#ifdef	SA_RESTART
	act.sa_flags |= SA_RESTART;
#endif
	if(::sigaction(signo, &act, &oact) < 0){
		return(NULL);
	}
	return(oact.sa_handler);
}

static void childHandler(int /*signo*/)
{
	pid_t pid;
	int status;
	while((pid = ::waitpid(-1, &status, WNOHANG)) > 0){
		if(pid == g_this->m_child){
			g_this->notifyFinish(status);
		}
	}
}

/* ------------------------------------------------------------------------ */
/*	ProcessInvoker Class : parent process									*/
/* ------------------------------------------------------------------------ */
ProcessInvoker::ProcessInvoker()
{
	g_this = this;
	m_isRunning = false;
	m_child = 0;
	m_defChildHandler = SIG_DFL;
	m_pTimer = new QTimer(this);
	m_stdfd[0] = m_stdfd[1] = -1;
	m_errfd[0] = m_errfd[1] = -1;
	connect(m_pTimer, SIGNAL(timeout()),
		this, SLOT(readOutputs()));
}

ProcessInvoker::~ProcessInvoker()
{
	qDebug("ProcessInvoker::~ProcessInvoker()");
}

bool ProcessInvoker::openPipe()
{
	if(m_stdfd[0] >= 0) closePipe(m_stdfd, 0);
	if(m_stdfd[1] >= 0) closePipe(m_stdfd, 1);
	if(::pipe(m_stdfd) < 0){
		return(false);
	}
	if(m_errfd[0] >= 0) closePipe(m_errfd, 0);
	if(m_errfd[1] >= 0) closePipe(m_errfd, 1);
	if(::pipe(m_errfd) < 0){
		closePipe(m_stdfd);
		return(false);
	}
	m_maxfdp1 = m_stdfd[0];
	if(m_errfd[0] > m_maxfdp1){
		m_maxfdp1 = m_errfd[0];
	}
	m_maxfdp1++;
	return(true);
}

void ProcessInvoker::closePipe(int fd[], int n)
{
	if(fd == NULL){
		closePipe(m_stdfd, n);
		closePipe(m_errfd, n);
	} else {
		if(n != 1 && fd[0] >= 0){
			::close(fd[0]);
			fd[0] = -1;
		}
		if(n != 0 && fd[1] >= 0){
			::close(fd[1]);
			fd[1] = -1;
		}
	}
}

void ProcessInvoker::setRunning(int pid)
{
	m_child = pid;
	m_isRunning = true;
}

bool ProcessInvoker::run(const QString& args)
{
	//setArguments(KHUtil::parseArgs(args));
	setArguments(StringParser::split(' ', args));
	return(run());
}

bool ProcessInvoker::run()
{
	if(m_isRunning){
		return(false);
	}
	m_isRunning = true;
	if(m_arguments.isEmpty()){
		m_isRunning = false;
		return(false);
	}
	if(m_arguments[0][0] != '/'){
		m_isRunning = false;
		return(false);
	}

	for(QStringList::Iterator it=m_arguments.begin();
			it!=m_arguments.end(); ++it){
		qDebug("arguments[%s]", (*it).ascii());
	}

	/* open pipe */
	if(openPipe() == false){
		m_isRunning = false;
		return(false);
	}

	/* signal handler reset */
	m_defChildHandler = setSignalHandler(SIGCHLD, SIG_DFL);

	m_child = ::fork();
	if(m_child < 0){
		/* fork error */
		closePipe();
		setSignalHandler(SIGCHLD, m_defChildHandler);
		m_isRunning = false;
		return(false);
	} else if(m_child == 0){
		/* child process */
		qDebug("child process[%d]", ::getpid());
		m_child = ::getpid();
		//setSignalHandler(SIGCHLD, SIG_DFL);
		workerProc();
		/* no return */
	}
	/* close pipe(write) */
	closePipe(NULL, 1);
#if 0
	m_pTimer = new QTimer(this);
	connect(m_pTimer, SIGNAL(timeout()),
		this, SLOT(readOutputs()));
#endif
	m_pTimer->start(500);
	{
		emit start(m_child, m_arguments);
		QCopEnvelope e(SC_CHANNEL, "start(int,QStringList)");
		e << m_child << m_arguments;
		if(m_isNotify){
			int idx = m_arguments[0].findRev('/');
			notifyStatus(m_arguments[0].mid(idx+1), m_child);
		}
	}
	int status;
	if(::waitpid(-1, &status, WNOHANG) > 0){
		qDebug("finish");
		notifyFinish(status);
	} else {
		/* signal handler set */
		setSignalHandler(SIGCHLD, childHandler);
	}
	return(true);
}

void ProcessInvoker::terminate()
{
	if(m_isRunning && m_child > 0){
		terminate(m_child);
	}
}

void ProcessInvoker::terminate(pid_t pid)
{
	::kill(pid, SIGTERM);
}

void ProcessInvoker::kill()
{
	if(m_isRunning && m_child > 0){
		kill(m_child);
	}
}

void ProcessInvoker::kill(pid_t pid)
{
	::kill(pid, SIGKILL);
}

#if 0
const QStringList ProcessInvoker::parseArgs(const QString& arguments)
{
	QString str;
	QStringList args;
	char quote = 0;
	char c;
	for(unsigned int i=0; i<arguments.length(); i++){
		c = arguments[i];
		switch(c){
		case '\"':
			if(quote == 0){
				quote = c;
			} else if(quote == '\"'){
				if(str.length() > 0){
					args.append(str);
				}
				str = "";
				quote = 0;
			} else {
				str += c;
			}
			break;
		case '\'':
			if(quote == 0){
				quote = c;
			} else if(quote == '\''){
				if(str.length() > 0){
					args.append(str);
				}
				str = "";
				quote = 0;
			} else {
				str += c;
			}
			break;
		case ' ':
			if(quote == 0){
				if(str.length() > 0){
					args.append(str);
					str = "";
				}
			} else {
				str += c;
			}
			break;
		default:
			str += c;
			break;
		}
	}
	if(str.length() > 0){
		args.append(str);
	}
	return(args);
}
#endif

void ProcessInvoker::readOutputs()
{
	struct timeval tmval;
	tmval.tv_sec = 0;
	tmval.tv_usec = 0;
	fd_set rset;

	QByteArray stdBuf, errBuf, resBuf;
	QDataStream stdStream(stdBuf, IO_WriteOnly);
	QDataStream errStream(errBuf, IO_WriteOnly);

	int iRet;
	bool running;
	char buf[PIPE_BUF+1];
	while(true){
		running = false;
		FD_ZERO(&rset);
		if(m_stdfd[0] >= 0){
			FD_SET(m_stdfd[0], &rset);
			running = true;
		}
		if(m_errfd[0] >= 0){
			FD_SET(m_errfd[0], &rset);
			running = true;
		}
		if(running == false){
			m_pTimer->stop();
			//delete m_pTimer;
			break;
		}

		if((iRet = ::select(m_maxfdp1, &rset, NULL, NULL, &tmval)) <= 0){
			qDebug("select[%d]", iRet);
			break;
		}

		if(m_stdfd[0] >= 0 && FD_ISSET(m_stdfd[0], &rset)){
			int n = ::read(m_stdfd[0], buf, PIPE_BUF);
			if(n > 0){
				stdStream.writeRawBytes(buf, n);
			} else {
				qDebug("stdout close");
				closePipe(m_stdfd, 0);
			}
		}
		if(m_errfd[0] >= 0 && FD_ISSET(m_errfd[0], &rset)){
			int n = ::read(m_errfd[0], buf, PIPE_BUF);
			if(n > 0){
				errStream.writeRawBytes(buf, n);
			} else {
				qDebug("stderr close");
				closePipe(m_errfd, 0);
			}
		}
	}

	if(stdBuf.size() > 0){
		QCopEnvelope e(SC_CHANNEL, "stdout(int,QByteArray)");
		e << m_child << stdBuf;
	}
	if(errBuf.size() > 0){
		QCopEnvelope e(SC_CHANNEL, "stderr(int,QByteArray)");
		e << m_child << errBuf;
	}
	if(running == false){
		QCopEnvelope e(SC_CHANNEL, "close(int)");
		e << m_child;
	}
}

#if 0
void ProcessInvoker::waitFinish()
{
	int status;
	if(::waitpid(m_child, &status, 0) > 0){
		notifyFinish(status);
	} else {
		notifyFinish(0, false);
	}
}
#endif

void ProcessInvoker::notifyFinish(int status, bool success)
{
	bool stopped = false;
	QString result;
	int code;
	if(success){
		if(WIFEXITED(status)){
			code = WEXITSTATUS(status);
			if(code == 127){
				result = "error";
			} else {
				result = "exit";
			}
		} else if(WIFSIGNALED(status)){
			result = "terminated";
			code = WTERMSIG(status);
		} else if(WIFSTOPPED(status)){
			result = "stopped";
			code = WSTOPSIG(status);
			stopped = true;
		} else {
			/* これは無いはず? */
			result = "error";
			code = -2;
			qWarning("ProcessInvoker: unknown status");
		}
	} else {
		result = "error";
		code = -1;
		qWarning("ProcessInvoker: wait error");
	}
	emit finish(result, code);
	QCopEnvelope e(SC_CHANNEL, "finish(int,QString,int)");
	e << m_child << result << code;
	if(m_isNotify){
		notifyStatus(result, code);
		setNotify(false);
	}
	if(stopped == false){
		setSignalHandler(SIGCHLD, m_defChildHandler);
		m_isRunning = false;
	}
}

void ProcessInvoker::notifyStatus(const QString& result, int code)
{
	QString message = QString::number(code);
	message.append(":");
	message.append(result);
	Global::statusMessage(message);
}

/* ------------------------------------------------------------------------ */
/*	ProcessInvoker Class : child process									*/
/* ------------------------------------------------------------------------ */
void ProcessInvoker::workerProc()
{
	closePipe(m_stdfd, 0);
	closePipe(m_errfd, 0);
	if(m_stdfd[1] != STDOUT_FILENO){
		::dup2(m_stdfd[1], STDOUT_FILENO);
		closePipe(m_stdfd, 1);
	}
	if(m_errfd[1] != STDERR_FILENO){
		::dup2(m_errfd[1], STDERR_FILENO);
		closePipe(m_errfd, 1);
	}

	QCString* arglist = new QCString[m_arguments.count()+1];
	const char** argv = new const char*[m_arguments.count()+1];
	unsigned int i;
	for(i=0; i<m_arguments.count(); i++){
		//arglist[i] = m_arguments[i].local8Bit();
		arglist[i] = m_arguments[i];
		argv[i] = arglist[i];
	}
	argv[i] = 0;
	::execv(argv[0], (char*const*)argv);
	delete[] arglist;
	delete[] argv;
	::_exit(127);
}
