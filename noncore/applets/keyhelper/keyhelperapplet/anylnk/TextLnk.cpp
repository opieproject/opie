#include "TextLnk.h"

void TextLnk::execute()
{
	QClipboard* cb = QApplication::clipboard();
	parseText();
	cb->setText(m_params[1]);
	QWSServer::sendKeyEvent('V'-'@',Qt::Key_V, Qt::ControlButton,
		true, false);
	QWSServer::sendKeyEvent('V'-'@',Qt::Key_V, Qt::ControlButton,
		false, false);
}

void TextLnk::parse(QString& str)
{
	replace(str, "\\\\", "\\");
	replace(str, "\\n", "\n");
	replace(str, "\\r", "\r");
	replace(str, "\\t", "\t");
}

void TextLnk::replace(QString& str, const QString& s1, const QString& s2)
{
	int index = 0;
	int idx;
	int len = s1.length();
	idx = str.find(s1, index);
	for(;;){
		idx = str.find(s1, index);
		if(idx < 0) break;
		str.replace(idx, len, s2);
		index = idx;
	}
}
