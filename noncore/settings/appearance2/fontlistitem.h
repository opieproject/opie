#ifndef FONTLISTITEM_H
#define FONTLISTITEM_H

#include <qlistbox.h>

class FontListItem : public QListBoxText {
public:
	FontListItem ( const QString &t, const QStringList &styles, const QValueList<int> &sizes ) : QListBoxText ( )
	{
		m_name = t;
		m_styles = styles;
		m_sizes = sizes;
		
		QString str = t;
		str [0] = str [0]. upper ( );
		setText ( str );
	}

	QString family ( ) const
	{
		return m_name;
	}
	
	const QStringList &styles ( ) const
	{
		return m_styles;
	}
	
	const QValueList<int> &sizes ( ) const
	{
		return m_sizes;
	}
	
private:
	QStringList m_styles;
	QValueList<int> m_sizes;
	QString m_name;
};

#endif