#include "qpemessagebox.h"
#include <qmessagebox.h>

bool QPEMessageBox::confirmDelete( QWidget *parent, const QString & caption,
			       const QString & object )
{
    QString msg = QObject::tr("Are you sure you want to delete\n %1?").arg( object );
    int r = QMessageBox::warning( parent, caption, msg, QMessageBox::Yes,
				  QMessageBox::No|QMessageBox::Default|
				  QMessageBox::Escape, 0 );

    return r == QMessageBox::Yes;
}
