#ifndef __INFOWIN_H
#define __INFOWIN_H

#include <qlabel.h>
class QString;
class QMultiLineEdit;

class infowin : public QWidget
{
Q_OBJECT
    QLabel* fileSize;
    QLabel* textSize;
    QLabel* ratio;
    QLabel* location;
    QLabel* read;
    QLabel* docread;
    QLabel* docSize;
    QLabel* docLocation;
    QLabel* zoom;
    QMultiLineEdit* aboutbox;
public:
    infowin::infowin( QWidget *parent=0, const char *name=0, WFlags f = 0);
    void setFileSize(int sz) { fileSize->setNum(sz); }
    void setTextSize(int sz) { textSize->setNum(sz); }
    void setRatio(int sz) { ratio->setText(QString().setNum(sz)+"%"); }
    void setLocation(int sz) { location->setNum(sz); }
    void setRead(int sz) { read->setText(QString().setNum(sz)+"%"); }
    void setDocRead(int sz) { docread->setText(QString().setNum(sz)+"%"); }
    void setZoom(int sz) { zoom->setText(QString().setNum(sz)+"%"); }
    void setDocSize(int sz) { docSize->setNum(sz); }
    void setDocLocation(int sz) { docLocation->setNum(sz); }
    void setAbout(const QString& s);
private slots:
    void infoClose() { emit Close(); }
 signals:
 void Close(); 
};

#endif
