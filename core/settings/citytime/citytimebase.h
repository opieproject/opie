/****************************************************************************
** Form interface generated from reading ui file 'citytimebase.ui'
**
** Created: Sat Oct 12 11:23:02 2002
**      by:  The User Interface Compiler (uic)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/
#ifndef CITYTIMEBASE_H
#define CITYTIMEBASE_H

#include <qvariant.h>
#include <qwidget.h>
class QVBoxLayout;
class QHBoxLayout;
class QGridLayout;
class QLabel;
class QToolButton;
class ZoneMap;

class CityTimeBase : public QWidget
{
    Q_OBJECT

public:
    CityTimeBase( QWidget* parent = 0, const char* name = 0, WFlags fl = 0 );
    ~CityTimeBase();

    ZoneMap* frmMap;
    QToolButton* cmdCity7;
    QLabel* lblCTime9;
    QToolButton* cmdCity8;
    QToolButton* cmdCity2;
    QLabel* lblCTime6;
    QToolButton* cmdCity6;
    QToolButton* cmdCity4;
    QLabel* lblCTime5;
    QLabel* lblCTime2;
    QLabel* lblCTime1;
    QToolButton* cmdCity3;
    QLabel* lblCTime8;
    QToolButton* cmdCity1;
    QLabel* lblCTime7;
    QLabel* lblCTime4;
    QToolButton* cmdCity5;
    QLabel* lblCTime3;
    QToolButton* cmdCity9;

public slots:
    virtual void beginNewTz();
    virtual void slotNewTz(const QString &, const QString &);

protected:
    QVBoxLayout* CityTimeBaseLayout;
    QGridLayout* Layout2;
    bool event( QEvent* );
    QWidget* buttonWidget;
};

#endif // CITYTIMEBASE_H
