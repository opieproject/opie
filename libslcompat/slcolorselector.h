/*
 * Copyright (C) 2002 SHARP CORPORATION All rights reserved.
 */
#ifndef _SLCOLORSELECTOR_H_INCLUDED
#define _SLCOLORSELECTOR_H_INCLUDED

#include <opie2/ocolorpopupmenu.h>
using namespace Opie;
#include <qtableview.h>
#include <qpopupmenu.h>
#include <qvbox.h>

#define DEF_COLOR_R (6)
#define DEF_COLOR_C (8)
struct SlColorTablePrivate;
class SlColorTable : public QTableView
{
	Q_OBJECT
public:
	SlColorTable(QWidget *parent=0,const char *name=NULL,WFlags f=0,
				 int r=DEF_COLOR_R,int c=DEF_COLOR_C,const QRgb *a=NULL);
	~SlColorTable();
	void setCurrentColor(QRgb);

protected:
	QSize sizeHint() const;
	void paintCell(QPainter*,int,int);
	void mousePressEvent(QMouseEvent*);
	void mouseMoveEvent(QMouseEvent*);
	void mouseReleaseEvent(QMouseEvent*);
	void keyPressEvent(QKeyEvent*);

signals:


	void selected(QRgb rgb);


	void reSelected(QRgb rgb);

private:
	SlColorTablePrivate *p;
	void setCurrent(int,int);
	void setSelected(int,int);
	QRgb getRgbInCell(int,int);
};


struct SlColorSelectorPrivate;
class SlColorSelector : public QVBox
{
	Q_OBJECT
public:
	SlColorSelector(bool isTransparentAccept,
					QWidget *parent=0,const char *name=NULL,WFlags f=0);
	~SlColorSelector();
	void setCurrentColor(QRgb);
	void setCurrentColorTransparent();

signals:


	void selected(QRgb rgb);


	void reSelected(QRgb rgb);


	void transparentSelected();


	void transparentReSelected();
/*
private slots:
	void slotTransparentToggled(int);
*/
private:
	SlColorSelectorPrivate *p;
};

struct SlColorSelectorPopupPrivate;
class SlColorSelectorPopup : public OColorPopupMenu //QPopupMenu
{
	Q_OBJECT
public:
	SlColorSelectorPopup(bool isTransparentAccept,
						 QWidget *parent=0,const char *name=NULL,WFlags f=0);
	~SlColorSelectorPopup();
	void setCurrentColor(QRgb);
	void setCurrentColorTransparent();

signals:


	void selected(QRgb rgb);


	void transparentSelected();
/*
private slots:
	void slotSelected(QRgb);
	void slotReSelected(QRgb);
	void slotTransparentSelected();
	void slotTransparentReSelected();
*/
private:
	SlColorSelectorPopupPrivate *p;
};

#endif


