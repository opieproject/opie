#ifndef OBEXDIALOG_H
#define OBEXDIALOG_H


#include <qdialog.h>
#include <qlabel.h>
#include <qmultilineedit.h>
#include "obexpush.h"

class QVBoxLayout;
class QPushButton;
class QMultiLineEdit;
class QLineEdit;

namespace Opie {namespace Core {class OProcess;}}
namespace OpieTooth {

    class ObexDialog : public QDialog {

        Q_OBJECT

    public:
        ObexDialog( const QString& device = 0, int port = 0, 
            QWidget* parent = 0, const char* name = 0, 
            bool modal = TRUE, WFlags fl = 0);
        ~ObexDialog();


private slots:
        void browse();
        void sendData();
        void slotPushStatus(QCString&);
        void slotPushComplete(int);
        void slotPushError(int);
        
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
        int m_port; //Port used
        ObexPush* obexSend;
    };
}
#endif
