#ifndef TRANSFER_DIALOG_H
#define TRANSFER_DIALOG_H

#include "qwidget.h"

class QLineEdit;
class QComboBox;
class QProgressBar;
class QLabel;

class TransferDialog : public QWidget
{
	Q_OBJECT
	public:
		TransferDialog(QWidget *parent = NULL, const char *name = NULL);
		~TransferDialog();

	public slots:
		void slotFilename();
		void slotTransfer();

	private:
		QLineEdit *filename;
		QComboBox *protocol;
		QProgressBar *progressbar;
		QLabel *statusbar;
};

#endif

