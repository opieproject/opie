#ifndef __OPIE_METAL_PLUGIN_H__
#define __OPIE_METAL_PLUGIN_H__

#include <qpe/styleinterface.h>


class MetalInterface : public StyleInterface {
public:
	MetalInterface ( );
	virtual ~MetalInterface ( );
	
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

#endif
