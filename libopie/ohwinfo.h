#ifndef _LIBOPIE_OHWINFO_H_
#define _LIBOPIE_OHWINFO_H_

#include <qstring.h>

enum OHwModel {
	OMODEL_Unknown,

	OMODEL_iPAQ_H31xx,
	OMODEL_iPAQ_H36xx,
	OMODEL_iPAQ_H37xx,
	OMODEL_iPAQ_H38xx,
	
	OMODEL_Zaurus_SL5000
};

enum OHwVendor {	
	OVENDOR_Unknown,

	OVENDOR_HP,
	OVENDOR_Sharp,
};

class OHwInfoData;


class OHwInfo
{
public:
	static OHwInfo *inst ( );
	
	QString modelString ( ); 
	OHwModel model ( );
	
	QString vendorString ( );
	OHwVendor vendor ( );

	virtual ~OHwInfo ( );

private:
	OHwInfo ( );
	OHwInfo ( const OHwInfo & );

	OHwInfoData *m_data;
};

#endif

