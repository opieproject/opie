#ifndef COLORLISTITEM_H
#define COLORLISTITEM_H

#include <qlistbox.h>
#include <qpalette.h>

class ColorListItem : public QListBoxText {
public:
	ColorListItem ( const QString &t, const QPalette &pal ) : QListBoxText ( t ) 
	{ 
		m_pal = pal;
	}

	virtual ~ColorListItem ( )
	{
	}
	
	QPalette palette ( )
	{
		return m_pal;
	}
	
private:
	QPalette m_pal;
};


#endif
