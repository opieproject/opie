#ifndef ADDRESSPICKER_H
#define ADDRESSPICKER_H

#include "addresspickerbase.h"

class AddressPicker : public AddressPickerBase
{
	Q_OBJECT

public:
	static QString getNames();

protected:
	AddressPicker(QWidget *parent = 0, const char *name = 0, 
		bool modal = false, WFlags fl = 0);

	QString selectedNames;

	void accept();

};

#endif

