/****************************************************************************
** Form implementation generated from reading ui file 'Prefs.ui'
**
** Created: Tue Feb 11 23:53:35 2003
**      by:  The User Interface Compiler (uic)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/
#include "useqpe.h"
#include "Prefs.h"

#include <qcheckbox.h>
#include <qlabel.h>
#include <qpushbutton.h>
#include <qspinbox.h>
#include <qlayout.h>
#include <qvariant.h>
#include <qtooltip.h>
#include <qwhatsthis.h>
#include <qcombobox.h>
#include <qbuttongroup.h>
#include <qlineedit.h>
#ifdef USEQPE
#include <qpe/menubutton.h>
#include <qpe/fontdatabase.h>
#else
#include <qfontdatabase.h>
#endif

#include <qpe/qpeapplication.h>

CLayoutPrefs::CLayoutPrefs( QWidget* parent,  const char* name, WFlags fl )
    : QWidget( parent, name, fl )
{
    QHBoxLayout* hb = new QHBoxLayout(this);
    QButtonGroup* bg = new QButtonGroup(2, Qt::Horizontal, tr("Text"), this);
    hb->addWidget(bg);

    StripCR = new QCheckBox( bg );
    StripCR->setText( tr( "Strip CR" ) );

    Dehyphen = new QCheckBox( bg );
    Dehyphen->setText( tr( "Dehyphen" ) );

    SingleSpace = new QCheckBox( bg );
    SingleSpace->setText( tr( "Single Space" ) );

    Unindent = new QCheckBox( bg );
    Unindent->setText( tr( "Unindent" ) );

    Reparagraph = new QCheckBox( bg );
    Reparagraph->setText( tr( "Reparagraph" ) );

    DoubleSpace = new QCheckBox( bg );
    DoubleSpace->setText( tr( "Double Space" ) );

    Remap = new QCheckBox( bg );
    Remap->setText( tr( "Remap" ) );

    Embolden = new QCheckBox( bg );
    Embolden->setText( tr( "Embolden" ) );

    FullJustify = new QCheckBox( bg );
    FullJustify->setText( tr( "Full Justify" ) );

}

/*  
 *  Destroys the object and frees any allocated resources
 */
CLayoutPrefs::~CLayoutPrefs()
{
    // no need to delete child widgets, Qt does it all for us
}



CLayoutPrefs2::CLayoutPrefs2( QWidget* parent,  const char* name, WFlags fl )
    : QWidget( parent, name, fl )
{
    
    QVBoxLayout* vb = new QVBoxLayout(this);
    QGridLayout* gl = new QGridLayout(vb, 4, 3);

    QLabel *TextLabel;

    TextLabel = new QLabel( this, "TextLabel1" );
    TextLabel->setText( tr( "Indent" ) );
    gl->addWidget(TextLabel, 0, 0);


    TextLabel = new QLabel( this );
    TextLabel->setText( tr( "Page\nOverlap" ) );
    gl->addWidget(TextLabel, 0, 1);

    TextLabel = new QLabel( this );
    TextLabel->setText( tr( "Graphics\nZoom" ) );
    gl->addWidget(TextLabel, 0, 2);

    Indent = new QSpinBox( this, "Indent" );
    Indent->setRange(0,20);
    gl->addWidget(Indent, 1, 0);

    pageoverlap = new QSpinBox( this );
    pageoverlap->setRange(0,20);
    gl->addWidget(pageoverlap, 1, 1);

    gfxzoom = new QSpinBox( this );
    gfxzoom->setRange(0,100);
    gl->addWidget(gfxzoom, 1, 2);

    TextLabel = new QLabel( this, "TextLabel4" );
    TextLabel->setText( tr( "Margin" ) );
    gl->addWidget(TextLabel, 2, 0);

    TextLabel = new QLabel( this );
    TextLabel->setText( tr( "Paragraph\nLeading" ) );
    gl->addWidget(TextLabel, 2, 1);

    TextLabel = new QLabel( this );
    TextLabel->setText( tr( "Line\nLeading" ) );
    gl->addWidget(TextLabel, 2, 2);

    Margin = new QSpinBox( this, "Margin" );
    Margin->setRange(0, 100);
    gl->addWidget(Margin, 3, 0);

    ParaLead = new QSpinBox( this );
    ParaLead->setRange(-5, 50);
    gl->addWidget(ParaLead, 3, 1);

    LineLead = new QSpinBox( this );
    LineLead->setRange(-5, 50);
    gl->addWidget(LineLead, 3, 2);

    gl = new QGridLayout(vb, 2, 2);

    TextLabel = new QLabel( this);
    TextLabel->setText( tr( "Markup" ) );
    gl->addWidget(TextLabel, 0, 0, Qt::AlignBottom);
    TextLabel = new QLabel( this);
    TextLabel->setText( tr( "Font" ) );
    gl->addWidget(TextLabel, 0, 1, Qt::AlignBottom);


#ifdef USECOMBO
    Markup = new QComboBox( this);
#else
    Markup = new MenuButton( this);
#endif
    Markup->setSizePolicy(QSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed));
    Markup->insertItem("Auto");
    Markup->insertItem("None");
    Markup->insertItem("Text");
    Markup->insertItem("HTML");
    Markup->insertItem("PML");
    gl->addWidget(Markup, 1, 0, Qt::AlignTop);

#ifdef USECOMBO
    fontselector = new QComboBox( this);
#else
    fontselector = new MenuButton( this);
#endif
    fontselector->setSizePolicy(QSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed));
    {
#ifdef USEQPE
	FontDatabase f;
#else
	QFontDatabase f;
#endif
	QStringList flist = f.families();
	for (QStringList::Iterator nm = flist.begin(); nm != flist.end(); nm++)
	{
	    fontselector->insertItem(*nm);
	}
    } // delete the FontDatabase!!!
    gl->addWidget(fontselector, 1, 1, Qt::AlignTop);
}
/*
CLayoutPrefs2::CLayoutPrefs2( QWidget* parent,  const char* name, WFlags fl )
    : QWidget( parent, name, fl )
{
    QHBoxLayout* hb = new QHBoxLayout(this);

    QVBoxLayout* vb = new QVBoxLayout;
    hb->addLayout(vb);

    QLabel *TextLabel;

    TextLabel = new QLabel( this, "TextLabel1" );
    TextLabel->setText( tr( "Indent" ) );
    vb->addWidget( TextLabel, 0, Qt::AlignBottom );
    Indent = new QSpinBox( this, "Indent" );
    Indent->setRange(0,20);
    vb->addWidget( Indent, 0, Qt::AlignLeft );

    TextLabel = new QLabel( this );
    TextLabel->setText( tr( "Page\nOverlap" ) );
    vb->addWidget( TextLabel, 0, Qt::AlignBottom );
    pageoverlap = new QSpinBox( this );
    pageoverlap->setRange(0,20);
    vb->addWidget( pageoverlap, 0, Qt::AlignLeft );

    TextLabel = new QLabel( this );
    TextLabel->setText( tr( "Graphics\nZoom" ) );
    vb->addWidget( TextLabel, 0, Qt::AlignBottom );
    gfxzoom = new QSpinBox( this );
    gfxzoom->setRange(0,100);
    vb->addWidget( gfxzoom, 0, Qt::AlignLeft );

    vb->addStretch();

    vb = new QVBoxLayout;
    hb->addLayout(vb);

    TextLabel = new QLabel( this, "TextLabel4" );
    TextLabel->setText( tr( "Margin" ) );
    vb->addWidget( TextLabel, 0, Qt::AlignBottom );
    Margin = new QSpinBox( this, "Margin" );
    Margin->setRange(0, 100);
    vb->addWidget( Margin, 0, Qt::AlignLeft );

    TextLabel = new QLabel( this );
    TextLabel->setText( tr( "Paragraph\nLeading" ) );
    vb->addWidget( TextLabel, 0, Qt::AlignBottom );
    ParaLead = new QSpinBox( this );
    ParaLead->setRange(-5, 50);
    vb->addWidget( ParaLead, 0, Qt::AlignLeft );

    TextLabel = new QLabel( this );
    TextLabel->setText( tr( "Line\nLeading" ) );
    vb->addWidget( TextLabel, 0, Qt::AlignBottom );
    LineLead = new QSpinBox( this );
    LineLead->setRange(-5, 50);
    vb->addWidget( LineLead, 0, Qt::AlignLeft );

    vb->addStretch();

    vb = new QVBoxLayout;
    hb->addLayout(vb);


    TextLabel = new QLabel( this);
    TextLabel->setText( tr( "Markup" ) );
    vb->addWidget( TextLabel, 0, Qt::AlignBottom );
    Markup = new MenuButton( this);
    Markup->insertItem("Auto");
    Markup->insertItem("None");
    Markup->insertItem("Text");
    Markup->insertItem("HTML");
    Markup->insertItem("PML");
    vb->addWidget( Markup, 0, Qt::AlignLeft );

    TextLabel = new QLabel( this);
    TextLabel->setText( tr( "Font" ) );
    vb->addWidget( TextLabel, 0, Qt::AlignBottom );
    fontselector = new MenuButton( this);
    fontselector->setSizePolicy(QSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed));
    {
	FontDatabase f;
	QStringList flist = f.families();
	for (QStringList::Iterator nm = flist.begin(); nm != flist.end(); nm++)
	{
	    fontselector->insertItem(*nm);
	}
    } // delete the FontDatabase!!!
    vb->addWidget( fontselector, 0, Qt::AlignLeft );

    vb->addStretch();
}
*/
/*  
 *  Destroys the object and frees any allocated resources
 */
CLayoutPrefs2::~CLayoutPrefs2()
{
    // no need to delete child widgets, Qt does it all for us
}

/*
CPluckerPrefs::CPluckerPrefs( QWidget* parent,  const char* name, WFlags fl )
    : QWidget( parent, name, fl )
{
    Layout11 = new QVBoxLayout(this); 
    Layout11->setMargin( 0 );

    Depluck = new QCheckBox( this, "Depluck" );
    Depluck->setText( tr( "Depluck" ) );
    Layout11->addWidget( Depluck );

    Dejpluck = new QCheckBox( this, "Dejpluck" );
    Dejpluck->setText( tr( "Dejpluck" ) );
    Layout11->addWidget( Dejpluck );

    Continuous = new QCheckBox( this, "Continuous" );
    Continuous->setText( tr( "Continuous" ) );
    Layout11->addWidget( Continuous );

}

CPluckerPrefs::~CPluckerPrefs()
{
    // no need to delete child widgets, Qt does it all for us
}
*/
/*
CMiscPrefs::CMiscPrefs( QWidget* parent,  const char* name, WFlags fl )
    : QWidget( parent, name, fl )
{

    QVBoxLayout* Layout11 = new QVBoxLayout(this); 
    Layout11->setMargin( 0 );


    QHBoxLayout* hl = new QHBoxLayout;

    QLabel* TextLabel = new QLabel( this );
    TextLabel->setText( tr( "Action for\nSelect Button" ) );
    hl->addWidget(TextLabel);

    action = new QComboBox( this );
    action->insertItem("Open file");
    action->insertItem("Autoscroll");
    action->insertItem("Mark");
    action->insertItem("Annotate");
    action->insertItem("Fullscreen");
    hl->addWidget( action );

    Layout11->addLayout(hl);

    hl = new QHBoxLayout;

    TextLabel = new QLabel( this );
    TextLabel->setText( tr( "Dictionary\nApplication" ) );
    hl->addWidget(TextLabel);
    target = new QLineEdit(this);
    hl->addWidget( target );

    Layout11->addLayout(hl);


    QButtonGroup* bg = new QButtonGroup(1, Qt::Horizontal, "Selection Target", this);
    Layout11->addWidget( bg );

    annotation = new QCheckBox( bg );
    annotation->setText( tr( "Annotation" ) );

    dictionary = new QCheckBox( bg );
    dictionary->setText( tr( "Dictionary" ) );

    clipboard = new QCheckBox( bg );
    clipboard->setText( tr( "Clipboard" ) );

}

CMiscPrefs::~CMiscPrefs()
{
    // no need to delete child widgets, Qt does it all for us
}
*/

CMiscPrefs::CMiscPrefs( QWidget* parent,  const char* name, WFlags fl )
    : QWidget( parent, name, fl )
{

    QGridLayout* hl = new QGridLayout(this,1,2);

    hl->setMargin( 0 );

    QGroupBox* gb = new QGroupBox(1, Qt::Horizontal, tr("Select Action"), this);
    hl->addWidget( gb, 0, 0 );

    annotation = new QCheckBox( gb );
    annotation->setText( tr( "Annotation" ) );

    dictionary = new QCheckBox( gb );
    dictionary->setText( tr( "Dictionary" ) );

    clipboard = new QCheckBox( gb );
    clipboard->setText( tr( "Clipboard" ) );

    QButtonGroup* bg = new QButtonGroup(1, Qt::Horizontal, tr("Plucker"), this);
    hl->addWidget( bg, 0 , 1 );

    Depluck = new QCheckBox( bg );
    Depluck->setText( tr( "Depluck" ) );

    Dejpluck = new QCheckBox( bg );
    Dejpluck->setText( tr( "Dejpluck" ) );

    Continuous = new QCheckBox( bg );
    Continuous->setText( tr( "Continuous" ) );


/*

    QGroupBox* gb = new QGroupBox(1, Qt::Horizontal, "Navigation", this);
    TextLabel = new QLabel( gb );
    TextLabel->setText( tr( "Overlap" ) );
    QSpinBox* sb = new QSpinBox( gb );

    Internationalisation
    Ideogram/Word
    Set Width
    Set Encoding
    Set Font
*/
}

CMiscPrefs::~CMiscPrefs()
{
    // no need to delete child widgets, Qt does it all for us
}

CPrefs::CPrefs(bool fs, QWidget* parent, const char* name) : QDialog(parent, name, true)
{
    setCaption(tr( "OpieReader Settings" ) );
    QTabWidget* td = new QTabWidget(this);
    layout = new CLayoutPrefs(this);
    layout2 = new CLayoutPrefs2(this);
    misc = new CMiscPrefs(this);
    button = new CButtonPrefs(this);
    inter = new CInterPrefs(this);
    td->addTab(layout, tr("Layout"));
    td->addTab(layout2, tr("Layout(2)"));
    td->addTab(inter, tr("Locale"));
    td->addTab(misc, tr("Misc"));
    td->addTab(button, tr("Buttons"));
    QVBoxLayout* v = new QVBoxLayout(this);
    v->addWidget(td);

    if (fs)
        QPEApplication::showDialog( this );
}


/*

Unicode
  Ideo/Word
  Width
  Encoding

*/

#include "CEncoding_tables.h"

CInterPrefs::CInterPrefs( QWidget* parent,  const char* name, WFlags fl )
    : QWidget( parent, name, fl )
{
    QHBoxLayout* hb = new QHBoxLayout(this);

    QGroupBox* gb = new QGroupBox(1, Qt::Horizontal, tr("International"), this);

    hb->addWidget(gb);

    QLabel *TextLabel;

    ideogram = new QCheckBox( gb );
    ideogram->setText( tr( "Ideograms" ) );

    TextLabel = new QLabel( gb );
    TextLabel->setText( tr( "Ideogram Width" ) );
    ideogramwidth = new QSpinBox( gb );
    ideogramwidth->setSizePolicy(QSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed));
    ideogramwidth->setRange(1,200);

    propfontchange = new QCheckBox( gb );
    propfontchange->setText( tr( "Apply font\nto dialogs" ) );

    TextLabel = new QLabel( gb );
    TextLabel->setText( tr( "Encoding" ) );
#ifdef USECOMBO
    encoding = new QComboBox(gb);
#else
    encoding = new MenuButton(gb);
#endif
    encoding->insertItem("Ascii");
    encoding->insertItem("UTF-8");
    encoding->insertItem("UCS-2(BE)");
    encoding->insertItem("USC-2(LE)");
    encoding->insertItem("Palm");
    for (unicodetable::iterator iter = unicodetable::begin(); iter != unicodetable::end(); iter++)
    {
	encoding->insertItem(iter->mime);
    }

    QVBoxLayout* vb = new QVBoxLayout;

    gb = new QGroupBox(1, Qt::Horizontal, tr("Dictionary"), this);

    TextLabel = new QLabel( gb );
    TextLabel->setText( tr( "Application" ) );
    application = new QLineEdit(gb);
    application->setFixedWidth(80);

    TextLabel = new QLabel( gb );
    TextLabel->setText( tr( "Message" ) );
    message = new QLineEdit(gb);
    message->setFixedWidth(80);
//    message->setSizePolicy(QSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed));
    twotouch = new QCheckBox( gb );
    twotouch->setText( tr( "Two/One\nTouch" ) );

    SwapMouse = new QCheckBox( gb );
    SwapMouse->setText(tr("Swap Tap\nActions") );


    vb->addWidget(gb);

//    vb->addStretch();
    hb->addLayout(vb);
}

CInterPrefs::~CInterPrefs()
{
    // no need to delete child widgets, Qt does it all for us
}



#ifdef USECOMBO
void CButtonPrefs::populate(QComboBox *mb)
#else
void CButtonPrefs::populate(MenuButton *mb)
#endif
{
    mb->setSizePolicy(QSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed));
    mb->insertItem(tr("<Nothing>") );
    mb->insertItem(tr("Open file") );
    mb->insertItem(tr("Autoscroll") );
    mb->insertItem(tr("Bookmark") );
    mb->insertItem(tr("Annotate") );
    mb->insertItem(tr("Fullscreen") );

    mb->insertItem(tr("Zoom in") );
    mb->insertItem(tr("Zoom out") );
    mb->insertItem(tr("Back") );
    mb->insertItem(tr("Forward") );
    mb->insertItem(tr("Home") );
    mb->insertItem(tr("Page up") );
    mb->insertItem(tr("Page down") );
    mb->insertItem(tr("Line up") );
    mb->insertItem(tr("Line down") );
    mb->insertItem(tr("Beginning") );
    mb->insertItem(tr("End") );
}

CButtonPrefs::CButtonPrefs( QWidget* parent,  const char* name, WFlags fl )
    : QWidget( parent, name, fl )
{

    QGridLayout* hl = new QGridLayout(this,10,2);

    hl->setMargin( 0 );

    QLabel* ql = new QLabel(tr("Escape Button"), this);
    hl->addWidget(ql, 0, 0, Qt::AlignBottom);
#ifdef USECOMBO
    escapeAction = new QComboBox( this );
#else
    escapeAction = new MenuButton( this );
#endif
    populate(escapeAction);
    hl->addWidget(escapeAction, 1, 0, Qt::AlignTop | Qt::AlignLeft);

    ql = new QLabel(tr("Space Button"), this);
    hl->addWidget(ql, 2, 0, Qt::AlignBottom);
#ifdef USECOMBO
    spaceAction = new QComboBox( this );
#else
    spaceAction = new MenuButton( this );
#endif
    populate(spaceAction);
    hl->addWidget(spaceAction, 3, 0, Qt::AlignTop | Qt::AlignLeft);

    ql = new QLabel(tr("Return Button"), this);
    hl->addWidget(ql, 2, 1, Qt::AlignBottom);
#ifdef USECOMBO
    returnAction = new QComboBox( this );
#else
    returnAction = new MenuButton( this );
#endif
    populate(returnAction);
    hl->addWidget(returnAction, 3, 1, Qt::AlignTop | Qt::AlignLeft);

    ql = new QLabel(tr("Left Arrow"), this);
    hl->addWidget(ql, 4, 0, Qt::AlignBottom);
#ifdef USECOMBO
    leftAction = new QComboBox( this );
#else
    leftAction = new MenuButton( this );
#endif
    populate(leftAction);
    hl->addWidget(leftAction, 5, 0, Qt::AlignTop | Qt::AlignLeft);
    leftScroll = new QCheckBox( tr("Scroll Speed"), this );
    hl->addWidget(leftScroll, 6, 0, Qt::AlignTop | Qt::AlignLeft);

    ql = new QLabel(tr("Right Arrow"), this);
    hl->addWidget(ql, 4, 1, Qt::AlignBottom);
#ifdef USECOMBO
    rightAction = new QComboBox( this );
#else
    rightAction = new MenuButton( this );
#endif
    populate(rightAction);
    hl->addWidget(rightAction, 5, 1, Qt::AlignTop | Qt::AlignLeft);
    rightScroll = new QCheckBox( tr("Scroll Speed"), this );
    hl->addWidget(rightScroll, 6, 1, Qt::AlignTop | Qt::AlignLeft);

    ql = new QLabel(tr("Down Arrow"), this);
    hl->addWidget(ql, 7, 0, Qt::AlignBottom);
#ifdef USECOMBO
    downAction = new QComboBox( this );
#else
    downAction = new MenuButton( this );
#endif
    populate(downAction);
    hl->addWidget(downAction, 8, 0, Qt::AlignTop | Qt::AlignLeft);
    downScroll = new QCheckBox( tr("Scroll Speed"), this );
    hl->addWidget(downScroll, 9, 0, Qt::AlignTop | Qt::AlignLeft);

    ql = new QLabel(tr("Up Arrow"), this);
    hl->addWidget(ql, 7, 1, Qt::AlignBottom);
#ifdef USECOMBO
    upAction = new QComboBox( this );
#else
    upAction = new MenuButton( this );
#endif
    populate(upAction);
    hl->addWidget(upAction, 8, 1, Qt::AlignTop | Qt::AlignLeft);
    upScroll = new QCheckBox( tr("Scroll Speed"), this );
    hl->addWidget(upScroll, 9, 1, Qt::AlignTop | Qt::AlignLeft);
}

CButtonPrefs::~CButtonPrefs()
{
    // no need to delete child widgets, Qt does it all for us
}
