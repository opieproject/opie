
#ifndef INPUTDIALOG_H
#define INPUTDIALOG_H

#include <opie2/oprocess.h>

#include <qvariant.h>
#include <qdialog.h>
#include <qmainwindow.h>

class QLineEdit;
class QCheckBox;

class InputDialog : public  QMainWindow {
    Q_OBJECT

public:
    InputDialog(  );
    ~InputDialog();

private:
    QLineEdit* LineEdit1;
private slots:
    void doLookup();
    void showBrowser(Opie::Core::OProcess*);
protected slots:

};

#endif // INPUTDIALOG_H
