#include "stringutil.h"

#include "qpemessagebox.h"
#include <qmessagebox.h>

static void never_called_tr_function_um_libqpe_ts_etwas_unter_zu_jubeln() {
(void)QMessageBox::tr("Yes");
(void)QMessageBox::tr("No");
}


/*!
  \class QPEMessageBox qpemessagebox.h
  \brief A message box that provides yes, no and cancel options.

  \ingroup qtopiaemb
 */

/*!
  Displays a QMessageBox with parent \a parent and caption \a caption.
  The message displayed is "Are you sure you want to delete: ",
  followed by \a object.
 */
bool QPEMessageBox::confirmDelete( QWidget *parent, const QString & caption,
			       const QString & object )
{
    QString msg = QObject::tr("<qt>Are you sure you want to delete\n %1?</qt>").arg( Qtopia::escapeString( object ) ); // use <qt> an esacpe String....
    int r = QMessageBox::warning( parent, caption, msg, QMessageBox::Yes,
				  QMessageBox::No|QMessageBox::Default|
				  QMessageBox::Escape, 0 );

    return r == QMessageBox::Yes;
}
