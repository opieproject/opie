#include "qchecknamebase.h"
#include <qstring.h>
#include <qwidget.h>

class QCheckName : public QCheckNameBase
{
	Q_OBJECT
	public:
		QCheckName();
		static QString getName();
	private:
		QWidget *m_parent;
	private slots:
		void clicked();
};
