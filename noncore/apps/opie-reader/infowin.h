#ifndef __INFOWIN_H
#define __INFOWIN_H

#include <qlayout.h>
#include <qpushbutton.h>
#include <qgrid.h>
#include <qlabel.h>

class infowin : public QWidget
{
Q_OBJECT
    QLabel* fileSize;
    QLabel* textSize;
    QLabel* ratio;
    QLabel* location;
    QLabel* read;
    QGridLayout* grid;
public:
    infowin::infowin( QWidget *parent=0, const char *name=0, WFlags f = 0);
/*
    void addcancel(QPushButton* exitbutton)
	{
	    grid->addWidget(exitbutton, 5, 1);
	}
*/
    void setFileSize(int sz) { fileSize->setNum(sz); }
    void setTextSize(int sz) { textSize->setNum(sz); }
    void setRatio(int sz) { ratio->setText(QString().setNum(sz)+"%"); }
    void setLocation(int sz) { location->setNum(sz); }
    void setRead(int sz) { read->setText(QString().setNum(sz)+"%"); }
/*
    virtual void keyPressEvent(QKeyEvent* e)
	{
	    printf("Received:%d\n", e->key());
	    switch (e->key())
	    {
		case Key_Escape:
		    e->accept();
		    emit Close();
		    break;
		default:
		    e->ignore();
	    }
	}
*/
private slots:
    void infoClose() { emit Close(); }
 signals:
 void Close(); 
};

#endif
