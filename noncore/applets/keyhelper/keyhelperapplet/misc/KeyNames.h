#ifndef _KEY_NAMES_H_
#define _KEY_NAMES_H_

#include <qstring.h>
#include <qmap.h>
#include <qnamespace.h>

class KeyNames
{
public:
	static const QString& getName(int code);
	static int getCode(const QString& s);

	static void clearName(){
		namemap.clear();
	}
	static void setCode(const QString& s, int code){
		if(codemap.contains(s) == false){
			codemap.insert(s, code);
		}
	}
	static void clearCode(){
		codemap.clear();
	}
	static void reset(){
		clearCode();
	}
private:
	static QMap<QString, int> codemap;
	static QMap<int, QString> namemap;
	static QString tmpname;

	static void setCodeMap();
	static void setNameMap();
};

#endif /* _KEY_NAMES_H_ */
