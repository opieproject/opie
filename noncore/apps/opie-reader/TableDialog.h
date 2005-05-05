#ifndef __TABLEDIALOG_H
#define __TABLEDIALOG_H

#include <qdialog.h>
#include <qtextview.h>
#include <qlayout.h>

class CTableDialog : public QDialog
{
Q_OBJECT
#ifndef USEQPE
    void keyPressEvent(QKeyEvent* e)
	{
	    switch (e->key())
	    {
		case Key_Escape:
		    e->accept();
		    reject();
		    break;
		case Key_Space:
		case Key_Return:
		    e->accept();
		    accept();
		    break;
		default:
		    QWidget::keyPressEvent(e);
	    }
	}
#endif
 public:
 CTableDialog(const QFont& f, const QString& tabtext, bool fs = true, QWidget* parent = 0, const char* name = 0);
 ~CTableDialog() {}
};
#endif // CPREFS_H
