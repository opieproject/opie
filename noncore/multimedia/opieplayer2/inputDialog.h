
#ifndef INPUTDIALOG_H
#define INPUTDIALOG_H

#include <qvariant.h>
#include <qdialog.h>

class QLineEdit;

class InputDialog : public QDialog {
    Q_OBJECT

public:
    InputDialog( QWidget* parent = 0, const char* name = 0, bool modal = FALSE, WFlags fl = 0 );
    ~InputDialog();
    QString text()const;
    
private:
    QLineEdit* LineEdit1;
    
protected slots:
    void browse();
};

#endif // INPUTDIALOG_H
