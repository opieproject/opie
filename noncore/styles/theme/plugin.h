#ifndef __OPIE_THEME_PLUGIN_H__
#define __OPIE_THEME_PLUGIN_H__

#include <qpe/styleinterface.h>

class ThemeSettings;

class ThemeInterface : public StyleExtendedInterface {
public:
	ThemeInterface ( );
	virtual ~ThemeInterface ( );
	
	QRESULT queryInterface ( const QUuid &, QUnknownInterface ** );
	Q_REFCOUNT
        	
	virtual QStyle *style ( );
	virtual QString name ( ) const;
	
	virtual QString description ( ) const;
	
	virtual bool hasSettings ( ) const;
	
	virtual QWidget *create ( QWidget *parent, const char *name = 0 );
    
	virtual bool accept ( );            
	virtual void reject ( );
	
private:
	ThemeSettings *m_widget;
	ulong ref;
};

#endif
