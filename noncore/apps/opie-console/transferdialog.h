#ifndef TRANSFER_DIALOG_H
#define TRANSFER_DIALOG_H

#include "qdialog.h"

class QLineEdit;
class QComboBox;
class QProgressBar;
class QLabel;
class QPushButton;
class MainWindow;
class FileTransferLayer;

class TransferDialog : public QDialog
{
	Q_OBJECT
	public:
		TransferDialog(MainWindow *parent = 0l, const char *name = 0l);
		~TransferDialog();

	public slots:
		void slotFilename();
		void slotTransfer();
		void slotCancel();
		void slotProgress(const QString&, int, int, int, int, int);
		void slotError(int error, const QString& message);
		void slotSent();
		void slotMode(int id);

	private:
		enum Modes
		{
			id_send,
			id_receive
		};

		QLineEdit *filename;
		QComboBox *protocol;
		QProgressBar *progressbar;
		QLabel *statusbar;
		QPushButton *ok, *cancel, *selector;
		MainWindow* m_win;
		FileTransferLayer* m_lay;
		int m_transfermode;
};

#endif

