#include "qcheckdetailsbase.h"
#include <qstring.h>
#include <qtextview.h>
#include <qtoolbar.h>
#include <qpe/resource.h>
#include <qpixmap.h>
#include <qdialog.h>
#include <qmainwindow.h>
#include <qtoolbutton.h>
#include <qwidget.h>

class QCheckDetails : public QMainWindow
{
	Q_OBJECT
	public:
		QCheckDetails(int row, int col, const QStringList item);
	signals:
		void editClicked(int, int);
		void deleteClicked(int, int);
	public slots:
		void editCheck();
		void deleteCheck();
	private:
		int m_row;
		int m_col;
		QCheckDetailsBase *m_view;
};
