#include "LnkWrapper.h"

LnkWrapper::LnkWrapper(const QStringList& params)
{
	if(params[0][0] == '/'){
		qDebug("create DocLnk instance");
		m_pLnk = new DocLnkWrapper(params);
	} else if(params[0] == "@exec"){
		qDebug("create ExecLnk instance");
		m_pLnk = new ExecLnk(params);
	} else if(params[0] == "@qcop"){
		qDebug("create QCopLnk instance");
		m_pLnk = new QCopLnk(params);
	} else if(params[0] == "@text"){
		qDebug("create TextLnk instance");
		m_pLnk = new TextLnk(params);
	} else if(params[0] == "@menu"){
		qDebug("create MenuLnk instance");
		m_pLnk = new MenuLnk(params);
	} else {
		qDebug("create AppLnk instance");
		m_pLnk = new AppLnkWrapper(params);
	}
}

LnkWrapper::~LnkWrapper()
{
	if(m_pLnk){
		delete m_pLnk;
	}
}

