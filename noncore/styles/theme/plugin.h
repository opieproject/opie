#ifndef __OPIE_THEME_PLUGIN_H__
#define __OPIE_THEME_PLUGIN_H__

#include <qpe/styleinterface.h>

class ThemeSettings;

class ThemeInterface : public StyleInterface {
public:
	ThemeInterface ( );
	virtual ~ThemeInterface ( );
	
	QRESULT queryInterface ( const QUuid &, QUnknownInterface ** );
	Q_REFCOUNT
        	
	virtual QStyle *create ( );
	
	virtual QString description ( );
	virtual QString name ( );
	virtual QCString key ( );
	
	virtual unsigned int version ( );
	
private:
	ulong ref;
};

class ThemeSettingsInterface : public StyleSettingsInterface {
public:
	ThemeSettingsInterface ( );
	virtual ~ThemeSettingsInterface ( );

	QRESULT queryInterface ( const QUuid &, QUnknownInterface ** );
	Q_REFCOUNT
       
	virtual QWidget *create ( QWidget *parent, const char *name = 0 );
    
	virtual bool accept ( );            
	virtual void reject ( );
	
private:
	ThemeSettings *m_widget;
	ulong ref;
};

#endif
