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
};

class CLayoutPrefs2 : public QWidget
{ 

public:

    friend class CPrefs;

    CLayoutPrefs2( QWidget* parent = 0, const char* name = 0, WFlags fl = 0 );
    ~CLayoutPrefs2();

    QSpinBox* ParaLead;
    QSpinBox* LineLead;
    QSpinBox* Indent;
    QSpinBox* Margin, *gfxzoom, *pageoverlap;
#ifdef USECOMBO
    QComboBox *Markup, *fontselector;
#else
    MenuButton *Markup, *fontselector;
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
};

class CButtonPrefs : public QWidget
{ 
#ifdef USECOMBO
    void populate(QComboBox*);
#else
    void populate(MenuButton*);
#endif
public:

    friend class CPrefs;

    CButtonPrefs( QWidget* parent = 0, const char* name = 0, WFlags fl = 0 );
    ~CButtonPrefs();

#ifdef USECOMBO
    QComboBox *returnAction, *spaceAction, *escapeAction, *leftAction, *rightAction, *upAction, *downAction;
#else
    MenuButton *returnAction, *spaceAction, *escapeAction, *leftAction, *rightAction, *upAction, *downAction;
#endif
    QCheckBox *leftScroll, *rightScroll, *upScroll, *downScroll;
};

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
 CButtonPrefs* button;
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
    CPrefs(bool fs = true, QWidget* parent = 0, const char* name = 0);
    ~CPrefs()
	{
	}
    bool StripCR() { return layout->StripCR->isChecked(); }
    bool Dehyphen() { return layout->Dehyphen->isChecked(); }
    bool SingleSpace() { return layout->SingleSpace->isChecked(); }
    bool Unindent() { return layout->Unindent->isChecked(); }
    bool Reparagraph() { return layout->Reparagraph->isChecked(); }
    bool DoubleSpace() { return layout->DoubleSpace->isChecked(); }
    bool Remap() { return layout->Remap->isChecked(); }
    bool Embolden() { return layout->Embolden->isChecked(); }
    bool FullJustify() { return layout->FullJustify->isChecked(); }
    int ParaLead() { return layout2->ParaLead->value(); }
    int LineLead() { return layout2->LineLead->value(); }
    int Margin() { return layout2->Margin->value(); }
    int Indent() { return layout2->Indent->value(); }
    int Markup() { return layout2->Markup->currentItem(); }
    QString Font() { return layout2->fontselector->currentText(); }


    void StripCR(bool v) { layout->StripCR->setChecked(v); }
    void Dehyphen(bool v) { layout->Dehyphen->setChecked(v); }
    void SingleSpace(bool v) { layout->SingleSpace->setChecked(v); }
    void Unindent(bool v) { layout->Unindent->setChecked(v); }
    void Reparagraph(bool v) { layout->Reparagraph->setChecked(v); }
    void DoubleSpace(bool v) { layout->DoubleSpace->setChecked(v); }
    void Remap(bool v) { layout->Remap->setChecked(v); }
    void Embolden(bool v) { layout->Embolden->setChecked(v); }
    void FullJustify(bool v) { layout->FullJustify->setChecked(v); }
    void ParaLead(int v) { layout2->ParaLead->setValue(v); }
    void LineLead(int v) { layout2->LineLead->setValue(v); }
    void Margin(int v) { layout2->Margin->setValue(v); }
    void Indent(int v) { layout2->Indent->setValue(v); }
#ifdef USECOMBO
    void Markup(int v) { layout2->Markup->setCurrentItem(v); }
#else
    void Markup(int v) { layout2->Markup->select(v); }
#endif
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

    int spaceAction() { return button->spaceAction->currentItem(); }
#ifdef USECOMBO
    void spaceAction(int v) { button->spaceAction->setCurrentItem(v); }
#else
    void spaceAction(int v) { button->spaceAction->select(v); }
#endif
    int escapeAction() { return button->escapeAction->currentItem(); }
#ifdef USECOMBO
    void escapeAction(int v) { button->escapeAction->setCurrentItem(v); }
#else
    void escapeAction(int v) { button->escapeAction->select(v); }
#endif
    int returnAction() { return button->returnAction->currentItem(); }
#ifdef USECOMBO
    void returnAction(int v) { button->returnAction->setCurrentItem(v); }
#else
    void returnAction(int v) { button->returnAction->select(v); }
#endif
    int leftAction() { return button->leftAction->currentItem(); }
#ifdef USECOMBO
    void leftAction(int v) { button->leftAction->setCurrentItem(v); }
#else
    void leftAction(int v) { button->leftAction->select(v); }
#endif
    int rightAction() { return button->rightAction->currentItem(); }
#ifdef USECOMBO
    void rightAction(int v) { button->rightAction->setCurrentItem(v); }
#else
    void rightAction(int v) { button->rightAction->select(v); }
#endif
    int upAction() { return button->upAction->currentItem(); }
#ifdef USECOMBO
    void upAction(int v) { button->upAction->setCurrentItem(v); }
#else
    void upAction(int v) { button->upAction->select(v); }
#endif
    int downAction() { return button->downAction->currentItem(); }
#ifdef USECOMBO
    void downAction(int v) { button->downAction->setCurrentItem(v); }
#else
    void downAction(int v) { button->downAction->select(v); }
#endif
    bool leftScroll() { return button->leftScroll->isChecked(); }
    void leftScroll(bool v) { button->leftScroll->setChecked(v); }
    bool rightScroll() { return button->rightScroll->isChecked(); }
    void rightScroll(bool v) { button->rightScroll->setChecked(v); }
    bool upScroll() { return button->upScroll->isChecked(); }
    void upScroll(bool v) { button->upScroll->setChecked(v); }
    bool downScroll() { return button->downScroll->isChecked(); }
    void downScroll(bool v) { button->downScroll->setChecked(v); }


    int gfxsize() { return layout2->gfxzoom->value(); }
    void gfxsize(int v) { layout2->gfxzoom->setValue(v); }
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
#ifdef USECOMBO
    void encoding(int v) { inter->encoding->setCurrentItem(v); }
#else
    void encoding(int v) { inter->encoding->select(v); }
#endif



};
#endif // CPREFS_H
