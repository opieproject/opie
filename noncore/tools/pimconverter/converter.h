#ifndef _CONVERTER_H_
#define _CONVERTER_H_


#include "converter_base.h"


class Converter: public converter_base {
public:
	Converter();

	// Slots defined in the ui-description file
	void start_conversion();
	void selectedDatabase( int num );
	void selectedDestFormat( int num );
	void selectedSourceFormat( int num );
	
private:
	// Caution:
	// The order and value of the following enums must be regarding
	// the predefinition in the UI-File !!
	enum DataBases{
		ADDRESSBOOK = 0,
		TODOLIST    = 1,
		DATEBOOK    = 2,
	};

	enum DbFormats{
		XML = 0,
		SQL = 1,
	};

	int m_selectedDatabase;
	int m_selectedSourceFormat;
	int m_selectedDestFormat;

};


#endif
