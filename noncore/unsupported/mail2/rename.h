#ifndef RENAME_H
#define RENAME_H

#include "renamebase.h"

class Rename : public RenameBase
{
	Q_OBJECT

public:
	static QString rename(const QString &on, QWidget *parent);
	static QString getText(const QString &caption, const QString &secondLabel, QWidget *parent);

protected:
	Rename(QWidget *parent = 0, const char *name = 0, bool modal = false, WFlags fl = 0);

};

#endif

