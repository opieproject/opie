#ifndef COMPOSERBASE_H
#define COMPOSERBASE_H

#include <qmainwindow.h>

class QMultiLineEdit;
class ListViewPlus;
class QPopupMenu;
class QComboBox;
class QLineEdit;
class QToolBar;
class QAction;
class QLabel;

class ComposerBase : public QMainWindow
{
	Q_OBJECT

public:
	ComposerBase(QWidget *parent = 0, const char *name = 0, WFlags fl = 0);

protected:
	static const int POPUP_FROM_FROM = 0;
	static const int POPUP_FROM_REPLYTO = 1;
	static const int POPUP_RECV_TO = 0;
	static const int POPUP_RECV_CC = 1;
	static const int POPUP_RECV_BCC = 2;
	static const int POPUP_SUBJ_SUBJECT = 0;
	static const int POPUP_SUBJ_PRIORITY = 1;
	static const int POPUP_PRIO_LOW = 0;
	static const int POPUP_PRIO_NORMAL = 1;
	static const int POPUP_PRIO_HIGH = 2;
	static const int POPUP_ATTACH_RENAME = 0;
	static const int POPUP_ATTACH_DESC = 1;
	static const int POPUP_ATTACH_REMOVE = 2;

	QMultiLineEdit *message;
	ListViewPlus *attachView;
	QMainWindow *attachWindow;
	QPopupMenu *attachPopup;
	QComboBox *fromBox, *from, *receiversBox, *subjectBox, *priority;
	QLineEdit *replyto, *to, *cc, *bcc, *subject;
	QToolBar *toolbar, *attachToolbar;
	QAction *sendmail, *attachfile, *addressbook, *abort, *addattach, *delattach;
	QLabel *fromLabel, *status;

protected slots:
	void slotAttachfileChanged(bool toggled);
	void slotFromMenuChanged(int id);
	void slotReceiverMenuChanged(int id);
	void slotSubjectMenuChanged(int id);

};

#endif
