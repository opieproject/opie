#ifndef OBEXDIALOG_H
#define OBEXDIALOG_H


#include <qdialog.h>
#include <opie/oprocess.h>

class QVBoxLayout;
class QPushButton;
class QMultiLineEdit;
class QLineEdit;


namespace OpieTooth {

    class ObexDialog : public QDialog {

        Q_OBJECT

    public:
        ObexDialog( QWidget* parent = 0, const char* name = 0, bool modal = TRUE, WFlags fl = 0,  const QString& device = 0);
        ~ObexDialog();


private slots:
        void browse();
        void sendData();

    protected:
        QVBoxLayout* layout;
        QLineEdit* cmdLine;
        QLineEdit* chNameLine;
        QPushButton* sendButton;
    private:
        // Device that is used
        QString m_device;
    };
}
#endif
