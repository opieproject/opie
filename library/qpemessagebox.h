#include <qstring.h>

class QWidget;

class QPEMessageBox
{
public:
    static bool confirmDelete( QWidget *parent, const QString & caption,
			       const QString & object );

};
