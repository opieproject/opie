#include <qfile.h>
#include <qtextstream.h>

#include "ohwinfo.h"


struct OHwInfoData {
	QString m_vendorstr;
	OHwVendor m_vendor;
	
	QString m_modelstr;
	OHwModel m_model;
};


OHwInfo *OHwInfo::inst ( )
{
	static OHwInfo *inf = 0;
	
	if ( !inf ) {
		inf = new OHwInfo ( );
	}
	return inf;
}

OHwInfo::OHwInfo ( )
{
	m_data = new OHwInfoData ( );

	QFile f ( "/proc/hal/model" );
	
	if ( f. open ( IO_ReadOnly )) {
		QTextStream ts ( &f );
		m_data-> m_modelstr = "H" + ts. readLine ( );
		
		if ( m_data-> m_modelstr == "H3100" )
			m_data-> m_model = OMODEL_iPAQ_H31xx;
		else if ( m_data-> m_modelstr == "H3600" )
			m_data-> m_model = OMODEL_iPAQ_H36xx;
		else if ( m_data-> m_modelstr == "H3700" )
			m_data-> m_model = OMODEL_iPAQ_H37xx;
		else if ( m_data-> m_modelstr == "H3800" )
			m_data-> m_model = OMODEL_iPAQ_H38xx;
		else
			m_data-> m_model = OMODEL_Unknown;
		
		m_data-> m_vendorstr = "HP";
		m_data-> m_vendor = OVENDOR_HP;	
	
		f. close ( );
	}
	else {
		m_data-> m_modelstr = "Unknown";
		m_data-> m_model = OMODEL_Unknown;
		m_data-> m_vendorstr = "Unkown";
		m_data-> m_vendor = OVENDOR_Unknown;
	}
}

OHwInfo::~OHwInfo ( )
{
	delete m_data;
}

QString OHwInfo::vendorString ( )
{
	return m_data-> m_vendorstr;
}

OHwVendor OHwInfo::vendor ( )
{
	return m_data-> m_vendor;
}

QString OHwInfo::modelString ( )
{
	return m_data-> m_modelstr;
}

OHwModel OHwInfo::model ( )
{
	return m_data-> m_model;
}

