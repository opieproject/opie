#ifndef __OPIE_LIQUID_PLUGIN_H__
#define __OPIE_LIQUID_PLUGIN_H__

#include <qpe/styleinterface.h>

class LiquidSettings;

class LiquidInterface : public StyleExtendedInterface {
public:
	LiquidInterface ( );
	virtual ~LiquidInterface ( );
	
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
	LiquidSettings *m_widget;
	ulong ref;
};

#endif
