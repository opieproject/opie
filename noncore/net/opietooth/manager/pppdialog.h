#ifndef PPPDIALOG_H
#define PPPDIALOG_H


#include <qdialog.h>
#include <opie/oprocess.h>

class QVBoxLayout;
class QPushButton;
class QMultiLineEdit;
class QLineEdit;

class PPPDialog : public QDialog {

    Q_OBJECT

public:
    PPPDialog( QWidget* parent = 0, const char* name = 0, bool modal = FALSE, WFlags fl = 0);
    ~PPPDialog();


private slots:
    void connectToDevice();
    void fillOutPut( OProcess* pppDial, char* cha, int len );
protected:
    QVBoxLayout* layout;
    QLineEdit* cmdLine;
    QPushButton* connectButton;
    QMultiLineEdit* outPut;

};
#endif
