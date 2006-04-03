#ifndef PANDIALOG_H
#define DUNDIALOG_H


#include <qdialog.h>
#include <opie2/oprocess.h>

class QVBoxLayout;
class QMultiLineEdit;
class QPushButton;
class QCheckBox;

namespace OpieTooth {
    class PanDialog : public QDialog {

        Q_OBJECT

    public:
        PanDialog(const QString& device = 0, QWidget* parent = 0, const char* name = 0, bool modal = TRUE, WFlags fl = 0);
        ~PanDialog();

    private slots:
        void connectToDevice();
        void fillOutPut( Opie::Core::OProcess* pppDial, char* cha, int len );
        void fillErr(Opie::Core::OProcess*, char*, int);
        void slotProcessExited(Opie::Core::OProcess* proc);
        void closeEvent(QCloseEvent* e);
    protected:
        QVBoxLayout* layout;
        QPushButton* connectButton;
        QMultiLineEdit* outPut;
        QCheckBox* doEncryption;
        QCheckBox* doSecure;
        Opie::Core::OProcess* m_panConnect;
        
    private:
        QString m_device; //device BT address
    };
}
#endif
