
#ifndef DIALDIALOG_H
#define DIALDIALOG_H

#include <qdialog.h>
#include <qstring.h>
#include <qlcdnumber.h>

class DialDialog : public QDialog {

    Q_OBJECT

public:
    DialDialog( QWidget* parent = 0,  const char* name = 0, bool modal = FALSE, WFlags fl = 0 );

    ~DialDialog();

    QString number();

private:
    float m_number;
    QLCDNumber *LCD;

private slots:
    void slotEnterNumber( int );
};


#endif
