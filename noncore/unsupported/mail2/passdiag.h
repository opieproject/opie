#ifndef PASSDIAG_H
#define PASSDIAG_H

#include "passdiagbase.h"

class PassDiag : public PassDiagBase
{
	Q_OBJECT

public:
	static QString getPassword(QWidget *parent, QString text);

protected:
	PassDiag(QString text, QWidget *parent, const char *name = 0, bool modal = false, WFlags fl = 0);

protected slots:
	void accept();

};

#endif
