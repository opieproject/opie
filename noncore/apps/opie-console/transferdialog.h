#ifndef TRANSFER_DIALOG_H
#define TRANSFER_DIALOG_H

#include "qdialog.h"

class QLineEdit;
class QComboBox;
class QProgressBar;
class QLabel;
class QPushButton;
class FileTransfer;

class TransferDialog : public QDialog
{
	Q_OBJECT
	public:
		TransferDialog(QWidget *parent = NULL, const char *name = NULL);
		~TransferDialog();

	public slots:
		void slotFilename();
		void slotTransfer();
		void slotCancel();
		void slotProgress(const QString&, int, int, int, int, int);
		void slotError(int error, const QString& message);
		void slotSent();

	private:
		QLineEdit *filename;
		QComboBox *protocol;
		QProgressBar *progressbar;
		QLabel *statusbar;
		QPushButton *ok, *cancel;
		FileTransfer *transfer;
};

#endif

