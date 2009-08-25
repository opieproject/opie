#include <opie2/opimmemo.h>

#include <qdialog.h>
#include <qlayout.h>
#include <qmultilineedit.h>
#include <qmessagebox.h>
#include <qtextstream.h>
#include <qfile.h>
#include <qfileinfo.h>

namespace Opie {
namespace Notes {

class editWindowWidget : public QDialog
{
    Q_OBJECT

    public:
        editWindowWidget(QWidget* parent = 0, const char* name = 0, bool modal = FALSE, WFlags fl = 0);
        ~editWindowWidget();
        void readMemo( const OPimMemo &memo );
        void writeMemo( OPimMemo &memo );

    private:
        QMultiLineEdit *editArea;

};

}
}
