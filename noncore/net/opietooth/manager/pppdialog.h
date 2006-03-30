#ifndef PPPDIALOG_H
#define PPPDIALOG_H


#include <qdialog.h>
#include <opie2/oprocess.h>

class QVBoxLayout;
class QPushButton;
class QMultiLineEdit;
class QLineEdit;
class QComboBox;
#define NCONNECTS 10 //Maximal 

namespace OpieTooth {
    typedef struct {
      Opie::Core::OProcess proc; //Connection process
      QString btAddr; //MAC address
      int port; //port
    } Connection;

    class PPPDialog : public QDialog {

        Q_OBJECT

    public:
        PPPDialog(const QString& device = 0, int port = 0, QWidget* parent = 0, const char* name = 0, bool modal = TRUE, WFlags fl = 0);
        ~PPPDialog();

    private slots:
        void connectToDevice();
        void fillOutPut( Opie::Core::OProcess* pppDial, char* cha, int len );
        void fillErr(Opie::Core::OProcess*, char*, int);
        void slotProcessExited(Opie::Core::OProcess* proc);
        void closeEvent(QCloseEvent* e);
    public:
        //Array of connections indexed by rfcomm device number
        static Connection conns[NCONNECTS];
    protected:
        QVBoxLayout* layout;
        QLineEdit* cmdLine;
        QPushButton* connectButton;
        QMultiLineEdit* outPut;
        QComboBox* serPort;

    private:
        QString m_device;
        int m_port;
    };
}
#endif
