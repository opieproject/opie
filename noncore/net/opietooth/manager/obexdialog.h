#ifndef OBEXDIALOG_H
#define OBEXDIALOG_H


#include <qdialog.h>
#include <opie2/oprocess.h>
#include <qlabel.h>
#include <qmultilineedit.h>

class QVBoxLayout;
class QPushButton;
class QMultiLineEdit;
class QLineEdit;

namespace Opie {namespace Core {class OProcess;}}
namespace OpieTooth {

    class ObexDialog : public QDialog {

        Q_OBJECT

    public:
        ObexDialog( const QString& device = 0, QWidget* parent = 0, const char* name = 0, bool modal = TRUE, WFlags fl = 0);
        ~ObexDialog();


private slots:
        void browse();
        void sendData();
        void slotPushOut(Opie::Core::OProcess*, char*, int);
        void slotPushErr(Opie::Core::OProcess*, char*, int);
        void slotProcessExited(Opie::Core::OProcess* proc);   	
        
    protected:
        QVBoxLayout* layout;
        QLineEdit* cmdLine;
        QLineEdit* chNameLine;
        QPushButton* sendButton;
        QLabel* info;
        QMultiLineEdit* status;
        QLabel* statLine;
    private:
        // Device that is used
        QString m_device;
        Opie::Core::OProcess *obexSend;
    };
}
#endif
