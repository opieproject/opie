#include "TaskSwitcher.h"

TaskSwitcher::TaskSwitcher(const QString& kind) : m_kind(kind)
{
	qDebug("TaskSwitcher::TaskSwitcher()");
}

TaskSwitcher::~TaskSwitcher()
{
	qDebug("TaskSwitcher::~TaskSwitcher()");
}

bool TaskSwitcher::onKeyPress(int /*keycode*/)
{
	if(m_applist.isEmpty()){
		/* get list */
		const QList<QWSWindow>& list = qwsServer->clientWindows();
		QWSWindow* w;
		for(QListIterator<QWSWindow> it(list); (w=it.current()); ++it){
			if(w->isVisible()){
				QString app = w->client()->identity();
				qDebug("applist[%s]", app.latin1());
				if(app != NULL && m_applist.contains(app) == false){
					m_applist.append(app);
				}
			}
		}
		m_appit = m_applist.begin();
	}
	if(m_applist.count() > 1){
		/* switch next */
		next();
		if(*m_appit == "launcher"){
			next();
		}
		Global::execute(*m_appit);
	} else if(m_applist.count() == 1
		&& *m_appit != "launcher"){
		Global::execute(*m_appit);
	} else {
		qDebug("no applications");
	}
	return(true);
}

bool TaskSwitcher::onModRelease(int /*keycode*/)
{
	m_applist.clear();
	return(false);
}

void TaskSwitcher::next()
{
	++m_appit;
	if(m_appit == m_applist.end()){
		m_appit = m_applist.begin();
	}	
}
