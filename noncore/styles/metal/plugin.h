#ifndef __OPIE_METAL_PLUGIN_H__
#define __OPIE_METAL_PLUGIN_H__

#include <qpe/styleinterface.h>


class MetalInterface : public StyleInterface {
public:
	MetalInterface ( );
	virtual ~MetalInterface ( );
	
	QRESULT queryInterface ( const QUuid &, QUnknownInterface ** );
	Q_REFCOUNT
        	
	virtual QStyle *style ( );	
	virtual QString name ( ) const;
	
private:
	ulong ref;
};

#endif
