#include <qstring.h>
#include <qcombobox.h>
#include "qrestrictedline.h"
#include <qcombobox.h>
#include <qstringlist.h>
#include <qlabel.h>
#include <qmultilineedit.h>
#include "qcheckentrybase.h"
#include <qdialog.h>
#include <qregexp.h>
#include <qmessagebox.h>
#include <qwidget.h>
#include <qevent.h>
#include <qdatetime.h>
#include <qradiobutton.h>
#include "config.h"

class QCheckEntry : public QCheckEntryBase
{
	Q_OBJECT
	public:
		QCheckEntry();
		static QStringList popupEntry(const QStringList &originaldata);
	private slots:
		void amountChanged(const QString &);
		void transFeeChanged(const QString &);
		void paymentClicked();
		void depositClicked();
	protected slots:
		virtual void accept();
};
