#ifndef ATTACHDIAG_H
#define ATTACHDIAG_H

#include <qdialog.h>

class DocLnk;
class FileSelector;

class AttachDiag : public QDialog
{
	Q_OBJECT

public:
	DocLnk selectedFile();
	static DocLnk getFile(QWidget *parent);

protected:
	AttachDiag(QWidget *parent = 0, const char* name = 0, bool modal = false, WFlags fl = 0);

private:
	FileSelector *_fileSelector;

};

#endif

