#ifndef __GRAPHICWIN_H
#define __GRAPHICWIN_H

#include <qscrollview.h>
#include <qpixmap.h>
#include <qpushbutton.h>
#include <qlayout.h>

class GraphicScroll : public QScrollView
{
    Q_OBJECT
	QWidget* m_picture;
 protected:
    void hideEvent( QHideEvent * p)
	{
	    m_picture->setFixedSize(1,1);	    
	}
 public:
    GraphicScroll( QWidget *parent=0, const char *name=0, WFlags f = 0)
	: QScrollView(parent, name, f)
	{
	    m_picture = new QWidget(viewport());
	    addChild(m_picture);
	}
    void setPixmap(QPixmap& pm)
	{
	    m_picture->setFixedSize(pm.size());
	    m_picture->setBackgroundPixmap(pm);
	}
/*
 private slots:
     void graphicClose() { emit Close(); }
 signals:
 void Close(); 
*/
 
};


class GraphicWin : public QWidget
{
    Q_OBJECT
    
	GraphicScroll* m_scroll;
 signals:
    void Closed();
 private slots:
     void slotClosed() { emit Closed(); }
    
 public:
 
     void setPixmap(QPixmap& pm) { m_scroll->setPixmap(pm); }
     GraphicWin( QWidget *parent=0, const char *name=0, WFlags f = 0)
	 : QWidget(parent, name, f)
	 {
	     QVBoxLayout* grid = new QVBoxLayout(this);
	     m_scroll = new GraphicScroll(this);
	     QPushButton* exitButton = new QPushButton("Close", this);
	     connect(exitButton, SIGNAL( released() ), this, SLOT( slotClosed() ) );
	     grid->addWidget(m_scroll,1);
	     grid->addWidget(exitButton);
	 }
};

#endif
