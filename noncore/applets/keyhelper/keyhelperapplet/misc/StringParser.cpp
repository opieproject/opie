#include "StringParser.h"

#include <qregexp.h>

QStringList StringParser::split(const QChar& sep, const QString& str,
	bool allowEmptyEntries)
{
	QString line = str + sep;
	QString quote;
	QRegExp rxend;
	QRegExp rxdbl;
	int pos=0, len, idx=0;
	QStringList list;
	while(idx < (int)line.length()-1){
		if(!quote.isEmpty()){
			QString s;
			while((pos = rxend.match(line, idx, &len)) != -1){
				s += line.mid(idx, len+pos-idx-1);
				idx = pos+len-1;
				if(len % 2 == 0){
					s.replace(rxdbl, quote);
					list.append(s.left(s.length()-1));
					idx++;
					break;
				}
			}
			quote = "";
		} else if(line[idx] == '\"'){
			rxend.setPattern(QString("\"+") + sep);
			rxdbl.setPattern("\"\"");
			quote = "\"";
			idx++;
		} else if(line[idx] == '\''){
			rxend.setPattern(QString("\'+") + sep);
			rxdbl.setPattern("\'\'");
			quote = "\'";
			idx++;
		} else if(!allowEmptyEntries && line[idx] == sep){
			idx++;
		} else {
			pos = line.find(sep, idx);
			if(pos != -1){
				const QString& s = line.mid(idx, pos-idx);
				list.append(s);
				idx = pos+1;
			}
		}
		if(pos == -1) break;
	}
	return list;
}

QString StringParser::join(const QChar& sep, const QStringList& list)
{
	QString str;
	QString s;
	QStringList tmp;
	QRegExp quote("\"");
	for(QStringList::ConstIterator it=list.begin();
			it!=list.end(); ++it){
		s = *it;
		if(s.find(sep) != -1
			|| s[0] == '\"'
			|| s[0] == '\''){
			s.replace(quote, "\"\"");
			tmp.append("\"" + s + "\"");
		} else {
			tmp.append(s);
		}
	}
	return tmp.join(sep);
}
