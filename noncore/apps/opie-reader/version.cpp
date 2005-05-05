#include "version.h"
#include "names.h"
#include <qmessagebox.h>

#include <qmultilineedit.h>
#include <qlayout.h>
#include <qtextview.h>
class versionbox : public QDialog
{
public:
  versionbox(const QString& txt, QWidget* parent = NULL) : QDialog(parent, NULL, true)
  {
    setCaption(tr( "New Features" ));
    QVBoxLayout* v = new QVBoxLayout(this);
    /*
    QMultiLineEdit* box = new QMultiLineEdit(this);
    v->addWidget(box);
    box->setReadOnly(true);
    box->setWordWrap(QMultiLineEdit::WidgetWidth);
    box->setText(txt);
    */
    QTextView* box = new QTextView(this);
    v->addWidget(box);
    box->setText(txt);
#ifdef USEQPE
    showMaximized();
#endif
  }
};

bool CheckVersion(int& major, int& bkmktype, char& minor, QWidget* pnt)
{
    if (
	(major != MAJOR)
	||
	(bkmktype != BKMKTYPE)
	||
	(minor != MINOR)
	)
    {
	major = MAJOR;
	bkmktype = BKMKTYPE;
	minor = MINOR;
	versionbox v(
		     "<h2>New Features</h2>"
		     "<h3>Support for custom version of rbmake</h3>"
		     "<h3>Mini scrollbar now acts more like a QT scrollbar.</h3>"
		     "<p>The handle can be dragged to jump to a \"random\" location and tapping to either side of the handle does page up/down</p>"
		     "<p>Line up/down is done by tapping in the margins to either end of the scroll bar (left/right margins for a horizontal scrollbar, top/bottom for a vertical scrollbar).</p>"
		     "<p>The toolbar now stays hidden if you hide it.</p>"
		     "<p>Better support for tables. Tapping on a table brings up a table viewer. The inline display of the linearised table can be toggled using the Inline Table option"
		     ,
		     pnt
		     );
	v.exec();
	return true;
    }
    else
    {
	return false;
    }
}
