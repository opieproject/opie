#include "KHUtil.h"
#include <qwindowsystem_qws.h>

int KHUtil::hex2int(const QString& hexstr, bool* ok)
{
	int val;
	bool success;
	if(hexstr.find("0x") == 0){
		val = hexstr.mid(2).toInt(&success, 16);
	} else {
		val = hexstr.toInt(&success, 16);
	}
	if(!success){
		val = 0;
	}
	if(ok){
		*ok = success;
	}
	return(val);
}

const QStringList KHUtil::parseArgs(const QString& arguments)
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

const QString KHUtil::currentApp()
{
	QString app;
	const QList<QWSWindow>& list = qwsServer->clientWindows();
	QWSWindow* w;
	for(QListIterator<QWSWindow> it(list); (w=it.current()); ++it){
		if(w->isVisible() && w->client()->identity() != QString::null){
			app = w->client()->identity();
			break;
		}
	}
	return app;
}
