#include "qcheckviewbase.h"
#include "qrestrictedline.h"
#include "qcheckentry.h"
#include "qcheckdetails.h"

#include <qstring.h>
#include "config.h"
#include <qtable.h>
#include <qpoint.h>
#include <qlayout.h>

class QCheckView : public QCheckViewBase
{
	Q_OBJECT
	public:
		QCheckView(QWidget *, QString filename);
		void load(const QString filename);
	private:
		Config *config;
		QString calculator(QString largervalue, QString smallervalue, bool subtract);
		int lastSortCol;
		bool ascend;
		QCheckDetails *qcd;
		QString m_filename;
	private slots:
//		void entryActivated(int);
		void newClicked();
		void tableClicked(int, int, int, const QPoint &mousePos);
		void editClicked(int, int);
		void deleteClicked(int, int);
	signals:
		void reload(const QString &filename);
};
