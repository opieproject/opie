#include "QCopLnk.h"
#include "StringParser.h"

void QCopLnk::execute()
{
	parseText();
	//QStringList argList = KHUtil::parseArgs(m_params[1]);
	QStringList argList = StringParser::split(' ', m_params[1]);
	if(argList.count() < 2){
		return;
	}
	QStringList paramList =
		QStringList::split(QRegExp("[(),]"), argList[1]);
	if(argList.count() < paramList.count()+1){
		return;
	}
	paramList.remove(paramList.begin());
	if(paramList.count() == 0){
		/* send qcop message */
		QCopEnvelope env(argList[0].latin1(), argList[1].latin1());
	} else {
		QCopEnvelope* e = NULL;
		QStringList::Iterator it=paramList.end();
		for(unsigned int index = 2; index<argList.count(); index++){
			if(it == paramList.end()){
				if(argList.count() - index < paramList.count()){
					break;
				}
				/* initialize */
				it = paramList.begin();
				e = new QCopEnvelope(
					argList[0].latin1(), argList[1].latin1());
			}
			QString arg = argList[index];
			if(*it == "QString"){
				*e << arg;
			} else if(*it == "int"){
				*e << arg.toInt();
			} else if(*it == "bool"){
				QString s = arg.lower();
				int on;
				if(s == "true"){
					on = TRUE;
				} else if(s == "false"){
					on = FALSE;
				} else {
					on = s.toInt();
				}
				*e << on;
			}
			++it;
			if(it == paramList.end()){
				/* send qcop message */
				delete e;
				if(argList.count() - index >= paramList.count()){
					e = new QCopEnvelope(
						argList[0].latin1(), argList[1].latin1());
					it = paramList.begin();
				}
			}
		}
	}
}
