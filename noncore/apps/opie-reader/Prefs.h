/****************************************************************************
** Form interface generated from reading ui file 'Prefs.ui'
**
** Created: Tue Feb 11 23:53:32 2003
**      by:  The User Interface Compiler (uic)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/
#ifndef CPREFS_H
#define CPREFS_H

#include <qvariant.h>
#include <qwidget.h>
#include <qtabdialog.h>
#include <qtabwidget.h>
#include <qspinbox.h>
#include <qcheckbox.h>
#include <qlineedit.h>

#define USECOMBO

#ifdef USECOMBO
#include <qcombobox.h>
#else
#include <qpe/menubutton.h>
#endif

#include "orkey.h"

class QVBoxLayout; 
class QHBoxLayout; 
class QGridLayout; 
//class QCheckBox;
class QLabel;
//class QSpinBox;
class QListViewItem;

class CLayoutPrefs : public QWidget
{ 

public:

    friend class CPrefs;

    CLayoutPrefs( QWidget* parent = 0, const char* name = 0, WFlags fl = 0 );
    ~CLayoutPrefs();

    QCheckBox* StripCR;
    QCheckBox* Dehyphen;
    QCheckBox* SingleSpace;
    QCheckBox* Unindent;
    QCheckBox* Reparagraph;
    QCheckBox* DoubleSpace;
    QCheckBox* Remap;
    QCheckBox* Embolden;
    QCheckBox* FullJustify;
    QCheckBox* FixGraphics;
    QCheckBox* hyphenate;
    //    QCheckBox* customhyphen;
    QCheckBox* prepalm;
    QCheckBox* pkern;
    //    QCheckBox* Inverse;
    //    QCheckBox* Negative;
};

class CLayoutPrefs2 : public QWidget
{ 

public:

    friend class CPrefs;

    CLayoutPrefs2( int w, QWidget* parent = 0, const char* name = 0, WFlags fl = 0 );
    ~CLayoutPrefs2();

    QSpinBox* ParaLead;
    QSpinBox* LineLead;
    QSpinBox* Indent;
    QSpinBox *TopMargin, *BottomMargin, *LeftMargin, *RightMargin, *gfxzoom, *pageoverlap;
#ifdef USECOMBO
    QComboBox *Markup, *fontselector, *fgsel, *bgsel;
#else
    MenuButton *Markup, *fontselector, *fgsel, *bgsel;
#endif
};

/*
class CPluckerPrefs : public QWidget
{ 

public:

    friend class CPrefs;

    CPluckerPrefs( QWidget* parent = 0, const char* name = 0, WFlags fl = 0 );
    ~CPluckerPrefs();

    QCheckBox* Depluck;
    QCheckBox* Dejpluck;
    QCheckBox* Continuous;
protected:

    QHBoxLayout* Layout5;
    QVBoxLayout* Layout11;
    QGridLayout* Layout4;

};
*/
class CMiscPrefs : public QWidget
{ 

public:

    friend class CPrefs;

    CMiscPrefs( QWidget* parent = 0, const char* name = 0, WFlags fl = 0 );
    ~CMiscPrefs();

    QCheckBox *annotation, *dictionary, *clipboard;
    QCheckBox *Depluck, *Dejpluck, *Continuous;

    QSpinBox *scrollstep;

#ifdef USECOMBO
    QComboBox *scrollcolor, *bgtype, *scrolltype, *minibarcol;
#else
    MenuButton *scrollcolor, *bgtype, *scrolltype, *minibarcol;
#endif
};
/*
class QListView;
class QListViewItem;

class CButtonPrefs : public QWidget
{
Q_OBJECT
  QMap<orKey, int> *kmap;
  QMap<orKey, QListViewItem*> listmap;
  QListView* lb;
  void keyPressEvent(QKeyEvent* e);
#ifdef USECOMBO
    void populate(QComboBox*);
#else
    void populate(MenuButton*);
#endif
public:

    friend class CPrefs;

    CButtonPrefs( QMap<orKey, int>*, QWidget* parent = 0, const char* name = 0, WFlags fl = 0 );
    ~CButtonPrefs();

#ifdef USECOMBO
    QComboBox *action;
#else
    MenuButton *action;
#endif
    QSpinBox* debounce;

    //    QCheckBox *leftScroll, *rightScroll, *upScroll, *downScroll;
 private slots:
    void erasemapping(QListViewItem*);
};
*/
class CInterPrefs : public QWidget
{ 

public:

    friend class CPrefs;

    CInterPrefs( QWidget* parent = 0, const char* name = 0, WFlags fl = 0 );
    ~CInterPrefs();

    QCheckBox *ideogram, *propfontchange, *SwapMouse, *twotouch;
#ifdef USECOMBO
    QComboBox* encoding;
#else
    MenuButton* encoding;
#endif
    QSpinBox* ideogramwidth;
    QLineEdit *application, *message;
};


class CPrefs : public QDialog
{
Q_OBJECT
    CLayoutPrefs* layout;
    CLayoutPrefs2* layout2;
 CMiscPrefs* misc;
 // CButtonPrefs* button;
 CInterPrefs* inter;

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
    CPrefs(int w, bool fs = true, QWidget* parent = 0, const char* name = 0);
    ~CPrefs()
	{
	}
    bool hyphenate() { return layout->hyphenate->isChecked(); }
    void hyphenate(bool _v) { layout->hyphenate->setChecked(_v); }
    /*
    bool customhyphen() { return layout->customhyphen->isChecked(); }
    void customhyphen(bool _v) { layout->customhyphen->setChecked(_v); }
    */
    bool StripCR() { return layout->StripCR->isChecked(); }
    bool repalm() { return layout->prepalm->isChecked(); }
    bool kern() { return layout->pkern->isChecked(); }
    bool Dehyphen() { return layout->Dehyphen->isChecked(); }
    bool SingleSpace() { return layout->SingleSpace->isChecked(); }
    bool Unindent() { return layout->Unindent->isChecked(); }
    bool Reparagraph() { return layout->Reparagraph->isChecked(); }
    bool DoubleSpace() { return layout->DoubleSpace->isChecked(); }
    bool Remap() { return layout->Remap->isChecked(); }
    bool Embolden() { return layout->Embolden->isChecked(); }
    bool FullJustify() { return layout->FullJustify->isChecked(); }
    //    bool Inverse() { return layout->Inverse->isChecked(); }
    //    bool Negative() { return layout->Negative->isChecked(); }
    bool FixGraphics() { return layout->FixGraphics->isChecked(); }
    int ParaLead() { return layout2->ParaLead->value(); }
    int LineLead() { return layout2->LineLead->value(); }
    int TopMargin() { return layout2->TopMargin->value(); }
    int BottomMargin() { return layout2->BottomMargin->value(); }
    int LeftMargin() { return layout2->LeftMargin->value(); }
    int RightMargin() { return layout2->RightMargin->value(); }
    int Indent() { return layout2->Indent->value(); }
    int Markup() { return layout2->Markup->currentItem(); }
    QString Font() { return layout2->fontselector->currentText(); }


    void StripCR(bool v) { layout->StripCR->setChecked(v); }
    void repalm(bool v) { layout->prepalm->setChecked(v); }
    void kern(bool v) { layout->pkern->setChecked(v); }
    void Dehyphen(bool v) { layout->Dehyphen->setChecked(v); }
    void SingleSpace(bool v) { layout->SingleSpace->setChecked(v); }
    void Unindent(bool v) { layout->Unindent->setChecked(v); }
    void Reparagraph(bool v) { layout->Reparagraph->setChecked(v); }
    void DoubleSpace(bool v) { layout->DoubleSpace->setChecked(v); }
    void Remap(bool v) { layout->Remap->setChecked(v); }
    void Embolden(bool v) { layout->Embolden->setChecked(v); }
    void FullJustify(bool v) { layout->FullJustify->setChecked(v); }
    //    void Negative(bool v) { layout->Negative->setChecked(v); }
    //    void Inverse(bool v) { layout->Inverse->setChecked(v); }
    void FixGraphics(bool v) { layout->FixGraphics->setChecked(v); }
    void ParaLead(int v) { layout2->ParaLead->setValue(v); }
    void LineLead(int v) { layout2->LineLead->setValue(v); }
    void TopMargin(int v) { layout2->TopMargin->setValue(v); }
    void BottomMargin(int v) { layout2->BottomMargin->setValue(v); }
    void LeftMargin(int v) { layout2->LeftMargin->setValue(v); }
    void RightMargin(int v) { layout2->RightMargin->setValue(v); }
    void Indent(int v) { layout2->Indent->setValue(v); }
#ifdef USECOMBO
    void Markup(int v) { layout2->Markup->setCurrentItem(v); }
#else
    void Markup(int v) { layout2->Markup->select(v); }
#endif
#ifdef USECOMBO
    void bgtype(int v) { misc->bgtype->setCurrentItem(v); }
    void scrollcolor(int v) { misc->scrollcolor->setCurrentItem(v); }
    void minibarcol(int v) { misc->minibarcol->setCurrentItem(v); }
#else
    void bgtype(int v) { misc->bgtype->select(v); }
    void scrollcolor(int v) { misc->scrollcolor->select(v); }
    void minibarcol(int v) { misc->minibarcol->select(v); }
#endif
    int bgtype() { return misc->bgtype->currentItem(); }
    int scrollcolor() { return misc->scrollcolor->currentItem(); }
    int minibarcol() { return misc->minibarcol->currentItem(); }


#ifdef USECOMBO
    void foreground(int v) { layout2->fgsel->setCurrentItem(v); }
#else
    void foreground(int v) { layout2->fgsel->select(v); }
#endif
    int foreground() { return layout2->fgsel->currentItem(); }

#ifdef USECOMBO
    void background(int v) { layout2->bgsel->setCurrentItem(v); }
#else
    void background(int v) { layout2->bgsel->select(v); }
#endif
    int background() { return layout2->bgsel->currentItem(); }



#ifdef USECOMBO
    void Font(QString& s)
	{
	    for (int i = 1; i <= layout2->fontselector->count(); i++)
	    {
		if (layout2->fontselector->text(i) == s)
		{
		    layout2->fontselector->setCurrentItem(i);
		    break;
		}
	    }
	}
#else
    void Font(QString& s) { layout2->fontselector->select(s); }
#endif

    bool Depluck() { return misc->Depluck->isChecked(); }
    void Depluck(bool v) { misc->Depluck->setChecked(v); }
    bool Dejpluck() { return misc->Dejpluck->isChecked(); }
    void Dejpluck(bool v) { misc->Dejpluck->setChecked(v); }
    bool Continuous() { return misc->Continuous->isChecked(); }
    void Continuous(bool v) { misc->Continuous->setChecked(v); }
    bool SwapMouse() { return inter->SwapMouse->isChecked(); }
    void SwapMouse(bool v) { inter->SwapMouse->setChecked(v); }


    void dictApplication(const QString& v) { inter->application->setText(v); }
    QString dictApplication() { return inter->application->text(); }
    void dictMessage(const QString& v) { inter->message->setText(v); }
    QString dictMessage() { return inter->message->text(); }

    void miscannotation(bool v) { misc->annotation->setChecked(v); }
    void miscdictionary(bool v) { misc->dictionary->setChecked(v); }
    void miscclipboard(bool v) { misc->clipboard->setChecked(v); }
    bool miscannotation() { return misc->annotation->isChecked(); }
    bool miscdictionary() { return misc->dictionary->isChecked(); }
    bool miscclipboard() { return misc->clipboard->isChecked(); }
    /*
    int Debounce() { return button->debounce->value(); }
    void Debounce(int v) { button->debounce->setValue(v); }
    */
    /*
    bool leftScroll() { return button->leftScroll->isChecked(); }
    void leftScroll(bool v) { button->leftScroll->setChecked(v); }
    bool rightScroll() { return button->rightScroll->isChecked(); }
    void rightScroll(bool v) { button->rightScroll->setChecked(v); }
    bool upScroll() { return button->upScroll->isChecked(); }
    void upScroll(bool v) { button->upScroll->setChecked(v); }
    bool downScroll() { return button->downScroll->isChecked(); }
    void downScroll(bool v) { button->downScroll->setChecked(v); }
    */

    int gfxsize() { return layout2->gfxzoom->value()/10; }
    void gfxsize(int v) { layout2->gfxzoom->setValue(10*v); }
    int pageoverlap() { return layout2->pageoverlap->value(); }
    void pageoverlap(int v) { layout2->pageoverlap->setValue(v); }

    bool twotouch() { return inter->twotouch->isChecked(); }
    void twotouch(bool v) { inter->twotouch->setChecked(v); }

    bool ideogram() { return inter->ideogram->isChecked(); }
    void ideogram(bool v) { inter->ideogram->setChecked(v); }

    int ideogramwidth() { return inter->ideogramwidth->value(); }
    void ideogramwidth(int v) { inter->ideogramwidth->setValue(v); }

    bool propfontchange() { return inter->propfontchange->isChecked(); }
    void propfontchange(bool v) { inter->propfontchange->setChecked(v); }

    int encoding() { return inter->encoding->currentItem(); }
    int scrolltype() { return misc->scrolltype->currentItem(); }
#ifdef USECOMBO
    void encoding(int v) { inter->encoding->setCurrentItem(v); }
    void scrolltype(int v) { misc->scrolltype->setCurrentItem(v); }
#else
    void encoding(int v) { inter->encoding->select(v); }
    void scrolltype(int v) { misc->scrolltype->select(v); }
#endif
    void scrollstep(int v) { misc->scrollstep->setValue(v); }
    int scrollstep() { return misc->scrollstep->value(); }
};
#endif // CPREFS_H
