#ifndef THEMEDIALOG_H
#define THEMEDIALOG_H

#include <qcombobox.h>
#include <qdialog.h>
#include <qlineedit.h>

struct ImageNames
{
    QString theme;
    QString board;
    QString pieces1;
    QString pieces2;
    QString dice1;
    QString dice2;
    QString odds;
    QString table;
};

class ThemeDialog : public QDialog
{
    Q_OBJECT
private:
    QLineEdit* lineName;
    QComboBox* boxboard;
    QComboBox* boxpiecesA;
    QComboBox* boxpiecesB;
    QComboBox* boxdiceA;
    QComboBox* boxdiceB;
    QComboBox* boxodds;
    QComboBox* boxtable;
public:
    ThemeDialog(QWidget* parent=0,const char* name=0,bool modal=TRUE,WFlags f=0);
    ~ThemeDialog();
public:
    ImageNames getNames();
    void setCurrent(const ImageNames& current);
private:
    void fillBox(QString dirname,QComboBox* thebox);
};

#endif //THEMEDIALOG_H
