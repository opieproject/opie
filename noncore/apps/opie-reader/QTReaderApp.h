/**********************************************************************
** Copyright (C) 2000 Trolltech AS.  All rights reserved.
**
** This file is part of Qt Palmtop Environment.
**
** This file may be distributed and/or modified under the terms of the
** GNU General Public License version 2 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
** See http://www.trolltech.com/gpl/ for GPL licensing information.
**
** Contact info@trolltech.com if any conditions of this licensing are
** not clear to you.
**
**********************************************************************/
#ifndef __QTREADERAPP_H
#define __QTREADERAPP_H

//#define __ISEARCH

#define MAX_ENCODING 6
#define MAX_ACTIONS 4

#include <qmainwindow.h>
#include "CExpander.h"
#include <qlist.h>
#include <qpe/filemanager.h>
#include <qmap.h>
#include <qlineedit.h>
#include <qstack.h>
#include <qlistbox.h>
//#include "Queue.h"

class QWidgetStack;
class QToolButton;
class QPopupMenu;
class QToolBar;
//class QPEToolBar;
class CBkmkSelector;
class QProgressBar;
class QAction;
class CAnnoEdit;
class QFloatBar;
class CDrawBuffer;
class QTReader;
class QPixmap;

enum ActionTypes
{
    cesOpenFile = 0,
    cesAutoScroll,
    cesActionMark,
    cesFullScreen
};

#ifdef __ISEARCH
struct searchrecord
{
  QString s;
  size_t pos;
  searchrecord(const QString& _s, size_t _pos) : s(_s), pos(_pos) {}
};
#endif

class infowin;
class GraphicWin;

class QTReaderApp : public QMainWindow
{
    Q_OBJECT

	unsigned long m_savedpos;
    bool m_annoIsEditing;

	public:
    QTReaderApp( QWidget *parent = 0, const char *name = 0, WFlags f = 0 );
    ~QTReaderApp();
    void suspend();
    void openFile( const QString & );

    void setScrollState(bool _b);

 protected:
    void setfontHelper(const QString& lcn, int size = 0);
    QAction* m_bkmkAvail, *m_actFullscreen;
    CAnnoEdit* m_annoWin;
    Bkmk* m_anno;
//    void resizeEvent(QResizeEvent* e);
    void keyPressEvent(QKeyEvent* e);
    void closeEvent( QCloseEvent *e );
	void readbkmks();
	void do_mono(const QString&);
	void do_jump(const QString&);
	void do_overlap(const QString&);
	void do_settarget(const QString&);
	int EncNameToInt(const QString&);
	ActionTypes ActNameToInt(const QString&);
    bool m_doAnnotation;
    bool m_doDictionary;
    bool m_doClipboard;
    bool m_fullscreen;

 public:
	void saveprefs();
private slots:
    void zoomin();
    void zoomout();
    void setfullscreen(bool sfs);
    void setcontinuous(bool sfs);
    void setTwoTouch(bool _b);
 void restoreFocus();
 void OnAnnotation(bool _b)
	{
	    m_doAnnotation = _b;
	}
    void OnDictionary(bool _b)
	{
	    m_doDictionary = _b;
	}
    void OnClipboard(bool _b)
	{
	    m_doClipboard = _b;
	}
    void OnWordSelected(const QString&, size_t, const QString&);
    void showgraphic(QPixmap&);
    void addAnno(const QString&, const QString&, size_t);
    void addAnno(const QString&, const QString&);
    void addanno();
    void showAnnotation();
	void do_setfont(const QString&);
	void encodingSelected(QAction*);
	void buttonActionSelected(QAction*);
	void msgHandler(const QCString&, const QByteArray&);
 void monospace(bool);
	void jump();
	void setoverlap();
	void settarget();
	void setspacing();
	void setfont();
	void clearBkmkList();
	void listBkmkFiles();
	void editMark();
	void autoScroll(bool);
	void addbkmk();
	void savebkmks();
//	void importFiles();
	void infoClose();
	//    void oldFile();
	void showinfo();
    void setDocument(const QString&);

    void indentplus();
    void indentminus();

    void fileOpen();
    void fileClose();

    void editCopy();
    void editFind();

    void pageup();
    void pagedn();

    void findNext();
    void findClose();

    void regClose();

#ifdef __ISEARCH
//  void search( const QString& );
#else
    void search();
#endif

    void showEditTools();

    void stripcr(bool);
    void onespace(bool);
#ifdef REPALM
//    void repalm(bool);
#endif
    void peanut(bool _b);
    void remap(bool);
    void embolden(bool);
    void autofmt(bool);
    void textfmt(bool);
    void striphtml(bool);
    void dehyphen(bool);
    void unindent(bool);
    void repara(bool);
    void dblspce(bool);
    void pagemode(bool);
    void navkeys(bool);
    //  void gotobkmk(const QString& bm);
    void gotobkmk(int);
    void cancelbkmk();
    void do_gotomark();
    void do_delmark();
    void do_autogen();
    void do_regaction();
    void OnRedraw();
    void OnActionPressed();

 private:
/*
    void setstate(unsigned char* _sd, unsigned short _sdlen);
    void getstate(unsigned char*& data, unsigned short& len);
*/
    void fileOpen2();
    void readfilelist();
    void savefilelist();
    void updatefileinfo();
    bool openfrombkmk(Bkmk*);
  QString m_targetapp, m_targetmsg;
    void listbkmk(CList<Bkmk>*, const QString& _lab = QString::null);
    QString usefilebrowser();
    void do_regedit();
    void colorChanged( const QColor &c );
    void clear();
    void updateCaption();
    void do_autogen(const QString&);
    void do_addbkmk(const QString&);
    bool findNextBookmark(size_t start);

 private:

    QAction* m_scrollButton;

    QAction* m_EncodingAction[MAX_ENCODING];

    QAction* m_buttonAction[MAX_ACTIONS];

    CBkmkSelector* bkmkselector;

    ActionTypes m_spaceTarget;
    size_t searchStart;
#ifdef __ISEARCH
    QStack<searchrecord>* searchStack;
    bool dosearch(size_t start, CDrawBuffer& test, const QString& arg);
#else
    bool dosearch(size_t start, CDrawBuffer& test, const QRegExp& arg);
#endif
    QWidgetStack *editorStack;
    QTReader* reader;
    QComboBox* m_fontSelector;
//    QPEToolBar /* *menu,*/ *editBar;
    QToolBar /* *menu,*/ *editBar;
    QFloatBar *searchBar, *regBar/*, *m_fontBar*/;
    QToolBar /* *searchBar, *regBar,*/ *m_fontBar;
    QLineEdit *searchEdit, *regEdit;
    bool searchVisible;
    bool regVisible;
    bool m_fontVisible, m_twoTouch;
    bool bFromDocView;
    static unsigned long m_uid;
    long unsigned get_unique_id() { return m_uid++; }
    /*
      void resizeEvent( QResizeEvent * r)
      {
      qDebug("resize:(%u,%u)", r->oldSize().width(), r->oldSize().height());
      qDebug("resize:(%u,%u)", r->size().width(), r->size().height());
      //    bgroup->move( width()-bgroup->width(), 0 );
      }
    */
    CList<Bkmk>* pBkmklist;
    CList<Bkmk>* pOpenlist;
    infowin*  m_infoWin;
    GraphicWin* m_graphicwin;
    QProgressBar* pbar;
    bool m_fBkmksChanged;
    int m_nRegAction;
    QString m_autogenstr;
    bool m_dontSave;
};

const int cAutoGen = 0;
const int cAddBkmk = 1;
const int cDelBkmk = 2;
const int cGotoBkmk = 3;
const int cRmBkmkFile = 4;
const int cJump = 5;
const int cMonoSpace = 6;
const int cOverlap = 7;
const int cSetTarget = 8;
const int cOpenFile = 9;

#endif
