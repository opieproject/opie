#ifndef __OPIE_LIQUIDDECO_PLUGIN_H__
#define __OPIE_LIQUIDDECO_PLUGIN_H__

#include <qpe/windowdecorationinterface.h>

class LiquidSettings;

class LiquidDecoInterface : public WindowDecorationInterface {
public:
	LiquidDecoInterface ( );
	virtual ~LiquidDecoInterface ( );
	
	QRESULT queryInterface ( const QUuid &, QUnknownInterface ** );
	Q_REFCOUNT

	virtual void drawArea( Area a, QPainter *, const WindowData * ) const;
        	
	virtual QString name ( ) const;
	virtual QPixmap icon ( ) const;
	
private:
	ulong ref;
};

#endif
