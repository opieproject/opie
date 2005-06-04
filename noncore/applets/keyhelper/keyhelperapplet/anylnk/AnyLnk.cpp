#include "AnyLnk.h"
#include "KHUtil.h"

void AnyLnk::loadPixmap()
{
	if(m_params.count() >= 3){
		m_pixmap = Opie::Core::OResource::loadPixmap( m_params[2], Opie::Core::OResource::SmallIcon );
	}
}

void AnyLnk::parseText()
{
	if(m_params.count() >= 2){
		QString& str = m_params[1];
		if(str != QString::null && str.length() > 0){
			replaceKeyword(str);
			replaceDate(str);
		}
	}
}

void AnyLnk::replaceText(QString& str, const QString& s1, const QString& s2)
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

void AnyLnk::replaceDate(QString& str)
{
	time_t t;
	struct tm lct;
	char buf[4096];
	int nLen;
	QString group;

	t = ::time(NULL);
	::localtime_r(&t, &lct);

	ConfigEx& cfg = ConfigEx::getInstance("keyhelper");
	group = cfg.getGroup();
	cfg.setGroup("Global");
	QString charset = cfg.readEntry("SystemCharSet", "eucJP");
	if(charset.length() == 0){
		charset = "eucJP";
	}
	cfg.setGroup(group);

	QTextCodec* codec = QTextCodec::codecForName(charset);
	if(codec == NULL){
		codec = QTextCodec::codecForLocale();
	}
	QTextDecoder* decoder = codec->makeDecoder();
	QTextEncoder* encoder = codec->makeEncoder();
	nLen = str.length();
	QCString localeString = encoder->fromUnicode(str, nLen);

	memset(buf, '\0', sizeof(buf));
	int n = ::strftime(buf, sizeof(buf), localeString, &lct);
	if(n > 0){
		str = decoder->toUnicode(buf, n);
	}
	delete decoder;
	delete encoder;
}

void AnyLnk::replaceKeyword(QString& str)
{
	QString txt;
	/* clipboard text */
	QClipboard* cb = QApplication::clipboard();
	if(cb == NULL){
		txt == "";
	} else {
		txt = cb->text();
	}
	replaceText(str, "%clipboard%", txt);
	/* current app */
	txt = KHUtil::currentApp();
	replaceText(str, "%currentapp%", txt);
}
