#include <qdialog.h>
#include <qlayout.h>
#include <qmultilineedit.h>
#include <qmessagebox.h>
#include <qtextstream.h>
#include <qfile.h>
#include <qfileinfo.h>

class editWindowWidget : public QDialog
{
    Q_OBJECT

    public:
        editWindowWidget(QWidget* parent = 0, const char* name = 0, bool modal = FALSE, WFlags fl = 0);
        ~editWindowWidget();
        void loadFile(QString fileName);
        void saveFile(QString fileName);

    private:
        QMultiLineEdit *editArea;

};
