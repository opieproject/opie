#ifndef _KEY_NAMES_H_
#define _KEY_NAMES_H_

#include <qstring.h>
#include <qmap.h>
#include <qnamespace.h>

class KeyNames
{
public:
	static const QString& getName(int code){
		if(namemap.isEmpty()) setNameMap();
		if(namemap.contains(code)){
			return(namemap[code]);
		} else {
			return(QString::null);
		}
	}
	static void clearName(){
		namemap.clear();
	}
	static int getCode(const QString& s){
		if(codemap.isEmpty()) setCodeMap();
		if(codemap.contains(s)){
			return(codemap[s]);
		} else {
			return(0);
		}
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

	static void setCodeMap();
	static void setNameMap();
};

#endif /* _KEY_NAMES_H_ */
