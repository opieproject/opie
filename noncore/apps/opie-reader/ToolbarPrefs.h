/****************************************************************************
 ** Form interface generated from reading ui file 'Prefs.ui'
 **
 ** Created: Tue Feb 11 23:53:32 2003
 **      by:  The User Interface Compiler (uic)
 **
 ** WARNING! All changes made in this file will be lost!
 ****************************************************************************/
#ifndef CTOOLBARPREFS_H
#define CTOOLBARPREFS_H
#include "useqpe.h"
#include <qvariant.h>
#include <qwidget.h>
#include <qtabdialog.h>
#include <qtabwidget.h>
#include <qspinbox.h>
#include <qcheckbox.h>
#include <qcombobox.h>
#include <qlineedit.h>
#ifdef USEQPE
#include <qpe/menubutton.h>
#include <qpe/config.h>
#else
#include "preferences.h"
#endif
//#include "opie.h"
//#ifdef OPIE
#define USECOMBO
//#endif

class QVBoxLayout; 
class QHBoxLayout; 
class QGridLayout; 
//class QCheckBox;
class QLabel;
//class QSpinBox;
/*
class CBarPrefs1 : public QWidget
{ 
    Q_OBJECT

	Config& config;
    QCheckBox *open, *close, *info, *twotouch,*find,*scroll,*navigation,*page,*startend,*jump,*pageline;

    bool m_isChanged;

    private slots:
	void isChanged(int _v) { m_isChanged = true; }

 public:
    CBarPrefs1( Config&, QWidget* parent = 0, const char* name = 0, WFlags fl = 0 );
    ~CBarPrefs1();
    void saveall();
    bool isChanged() { return m_isChanged; }
};
*/
class CFileBarPrefs : public QWidget
{ 
    Q_OBJECT

	Config& config;
    QCheckBox *open, *close, *info, *twotouch, *find, *scroll;

    bool m_isChanged;

    private slots:
	void isChanged(int _v) { m_isChanged = true; }

 public:
    CFileBarPrefs( Config&, QWidget* parent = 0, const char* name = 0, WFlags fl = 0 );
    ~CFileBarPrefs();
    void saveall();
    bool isChanged() { return m_isChanged; }
};

class CNavBarPrefs : public QWidget
{ 
    Q_OBJECT

	Config& config;
    QCheckBox *scroll, *navback, *navhome, *navforward;
    QCheckBox *pageup, *pagedown, *gotostart, *gotoend, *jump, *pageline;

    bool m_isChanged;

    private slots:
	void isChanged(int _v) { m_isChanged = true; }

 public:
    CNavBarPrefs( Config&, QWidget* parent = 0, const char* name = 0, WFlags fl = 0 );
    ~CNavBarPrefs();
    void saveall();
    bool isChanged() { return m_isChanged; }
};
/*
class CBarPrefs2 : public QWidget
{ 
    Q_OBJECT
	Config& config;
    QCheckBox *fullscreen, *zoom, *setfont, *mark, *annotate, *go_to, *Delete, *autogen, *clear, *save, *tidy, *block, *indannotate, *encoding, *ideogram;

    bool m_isChanged;

    private slots:
	void isChanged(int _v) { m_isChanged = true; }

 public:
    CBarPrefs2( Config&, QWidget* parent = 0, const char* name = 0, WFlags fl = 0 );
    ~CBarPrefs2();
    void saveall();
    bool isChanged() { return m_isChanged; }

};
*/
class CViewBarPrefs : public QWidget
{ 
    Q_OBJECT
	Config& config;
    QCheckBox *fullscreen, *zoomin, *zoomout, *setfont, *encoding, *ideogram;

    bool m_isChanged;

    private slots:
	void isChanged(int _v) { m_isChanged = true; }

 public:
    CViewBarPrefs( Config&, QWidget* parent = 0, const char* name = 0, WFlags fl = 0 );
    ~CViewBarPrefs();
    void saveall();
    bool isChanged() { return m_isChanged; }

};

class CMarkBarPrefs : public QWidget
{ 
    Q_OBJECT
	Config& config;
    QCheckBox *mark, *annotate, *go_to, *Delete, *autogen, *clear, *save, *tidy, *startblock, *copyblock;

    bool m_isChanged;

    private slots:
	void isChanged(int _v) { m_isChanged = true; }

 public:
    CMarkBarPrefs( Config&, QWidget* parent = 0, const char* name = 0, WFlags fl = 0 );
    ~CMarkBarPrefs();
    void saveall();
    bool isChanged() { return m_isChanged; }

};

class CIndBarPrefs : public QWidget
{ 
    Q_OBJECT
	Config& config;
    QCheckBox *indannotate;

    bool m_isChanged;

    private slots:
	void isChanged(int _v) { m_isChanged = true; }

 public:
    CIndBarPrefs( Config&, QWidget* parent = 0, const char* name = 0, WFlags fl = 0 );
    ~CIndBarPrefs();
    void saveall();
    bool isChanged() { return m_isChanged; }

};

class CMiscBarPrefs : public QWidget
{ 

public:

    CMiscBarPrefs( QWidget* parent = 0, const char* name = 0, WFlags fl = 0 );
    ~CMiscBarPrefs();

#ifdef USECOMBO
    QComboBox *tbpolicy, *tbposition;
#else
    MenuButton *tbpolicy, *tbposition;
#endif
    QCheckBox *tbmovable, *floating;
};

class CBarPrefs : public QDialog
{
    Q_OBJECT
	;
    CMiscBarPrefs* misc;
    CFileBarPrefs* filebar;
    CNavBarPrefs* navbar;
    CViewBarPrefs* viewbar;
    CMarkBarPrefs* markbar;
    CIndBarPrefs* indbar;
    Config config;

    void keyPressEvent(QKeyEvent* e)
	{
	    switch (e->key())
	    {
		case Key_Escape:
		    e->accept();
		    reject();
		    break;
		case Key_Space:
		case Key_Return:
		    e->accept();
		    accept();
		    break;
		default:
		    QWidget::keyPressEvent(e);
	    }
	}
 public:
    CBarPrefs(const QString& appdir, bool fs = true, QWidget* parent = 0, const char* name = 0);
    ~CBarPrefs()
	{
	    if (result())
	    {
		if (navbar->isChanged()) navbar->saveall();
		if (filebar->isChanged()) filebar->saveall();
		if (viewbar->isChanged()) viewbar->saveall();
		if (markbar->isChanged()) markbar->saveall();
		if (indbar->isChanged()) indbar->saveall();
	    }
	}
    bool isChanged()
	{
	    return
		(
		    filebar->isChanged()
		    ||
		    navbar->isChanged()
		    ||
		    viewbar->isChanged()
		    ||
		    markbar->isChanged()
		    ||
		    indbar->isChanged()
	        );
	}
    bool floating() { return misc->floating->isChecked(); }
    void floating(bool v) { misc->floating->setChecked(v); }
    int tbpolicy() { return misc->tbpolicy->currentItem(); }
#ifdef USECOMBO
    void tbpolicy(int v) { misc->tbpolicy->setCurrentItem(v); }
#else
    void tbpolicy(int v) { misc->tbpolicy->select(v); }
#endif
    bool tbmovable() { return misc->tbmovable->isChecked(); }
    void tbmovable(bool v) { misc->tbmovable->setChecked(v); }
    int tbposition() { return misc->tbposition->currentItem(); }
#ifdef USECOMBO
    void tbposition(int v) { misc->tbposition->setCurrentItem(v); }
#else
    void tbposition(int v) { misc->tbposition->select(v); }
#endif

};
#endif // CPREFS_H
