
#ifndef BLUEBASE_H
#define BLUEBASE_H

#include <qvariant.h>
#include <qwidget.h>
#include <qscrollview.h>
#include <qsplitter.h>

class QVBox;
class QHBoxLayout;
class QGridLayout;
class QFrame;
class QLabel;
class QPushButton;
class QTabWidget;
class QCheckBox;

class BlueBase : public QWidget
{
    Q_OBJECT

public:
    BlueBase( QWidget* parent = 0, const char* name = 0, WFlags fl = 0 );
    ~BlueBase();



protected:
    QTabWidget *TabWidget;
    QWidget *Tab;
    QWidget *TabConf;
    QWidget *TabConn;
    QCheckBox *AuthCheckBox;
    QCheckBox *CryptCheckBox;
    QLabel *Test;
    QPushButton *ScanButton;
};

#endif
