#ifndef __OPIE_LIQUID_PLUGIN_H__
#define __OPIE_LIQUID_PLUGIN_H__

#include <qpe/styleinterface.h>

class LiquidSettings;

class LiquidInterface : public StyleInterface {
public:
	LiquidInterface ( );
	virtual ~LiquidInterface ( );
	
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

class LiquidSettingsInterface : public StyleSettingsInterface {
public:
	LiquidSettingsInterface ( );
	virtual ~LiquidSettingsInterface ( );

	QRESULT queryInterface ( const QUuid &, QUnknownInterface ** );
	Q_REFCOUNT
       
	virtual QWidget *create ( QWidget *parent, const char *name = 0 );
    
	virtual bool accept ( );            
	virtual void reject ( );
	
private:
	LiquidSettings *m_widget;
	ulong ref;
};

#endif
