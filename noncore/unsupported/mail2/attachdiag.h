#ifndef ATTACHDIAG_H
#define ATTACHDIAG_H

#include <qdialog.h>

#include <qpe/applnk.h>

class AttachDiag : public QDialog
{
	Q_OBJECT

public:
	static DocLnk getFile();

protected:
	AttachDiag(QWidget *parent = 0, const char* name = 0, bool modal = false, WFlags fl = 0);

private slots:
	void fileSelected(const DocLnk &);

private:
	DocLnk currentFile;

};

#endif

