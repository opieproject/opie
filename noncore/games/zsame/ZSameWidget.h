#ifndef ZSAME_WIDGET_H
#define ZSAME_WIDGET_H

#include "StoneWidget.h"
#include <qmainwindow.h>

class Config;
class QAction;
class ZSameWidget : public QMainWindow {
    Q_OBJECT
public:
    static QString appName() { return QString::fromLatin1("zsame"); }
    ZSameWidget(QWidget* parent, const char* name,  WFlags fl );
    ~ZSameWidget();

private:
    void desktop_widget(int *f)const;
    StoneWidget *stone;
    QAction *restart;
    QAction *undo;

    int multispin_item;

protected:
    void newGame( unsigned int board,int colors);

     virtual void saveProperties(Config *conf);
     virtual void readProperties(Config *conf);

     bool confirmAbort();

public slots:
    void sizeChanged();

/* File Menu */
    void m_new();
    void m_restart();
    void m_load();
    void m_save();
    void m_showhs();
    void m_quit();
    void m_undo();

/* Options Menu */
    void m_tglboard();

    void gameover();
    void setColors(int colors);
    void setBoard(int board);
    void setScore(int score);
    void setMarked(int m);
    void stonesRemoved(int,int);
};


#endif
