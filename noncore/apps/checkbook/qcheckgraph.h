#include <qstring.h>
#include <qmainwindow.h>
#include <qwidget.h>
#include <qstringlist.h>
#include <qpixmap.h>
#include "config.h"

#include "qcheckgraphbase.h"

class QCheckGraph : public QCheckGraphBase
{
	Q_OBJECT
	public:
		QCheckGraph(const QString);
	private:
		QStringList list;
		QStringList list2;
		QString calculator(QString larger, QString smaller, bool divide);
		QPixmap graph;
		void graphPixmap();
		bool pixmapready;
		Config *config;
	private slots:
		void saveGraphAsPicture();
};
