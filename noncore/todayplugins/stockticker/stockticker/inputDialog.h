
#ifndef INPUTDIALOG_H
#define INPUTDIALOG_H

#include <qvariant.h>
#include <qdialog.h>
#include <qmainwindow.h>

class QLineEdit;
class QCheckBox;

class InputDialog : public  QMainWindow {
    Q_OBJECT

public:
    InputDialog(  );
//    InputDialog( QWidget* parent = 0, const char* name = 0, WFlags fl = 0 );
    ~InputDialog();
    QString text()const;
    
private:
    QLineEdit* LineEdit1;
    QCheckBox *timeCheck, *dateCheck, *symbolCheck, *nameCheck, *currentPriceCheck, *lastPriceCheck, *openPriceCheck, *minPriceCheck, *maxPriceCheck, *variationCheck, *volumeCheck;
private slots:
    void cleanUp(); 
protected slots:
    
};

#endif // INPUTDIALOG_H
