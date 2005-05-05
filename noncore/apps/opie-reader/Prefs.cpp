/****************************************************************************
** Form implementation generated from reading ui file 'Prefs.ui'
**
** Created: Tue Feb 11 23:53:35 2003
**      by:  The User Interface Compiler (uic)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/
#include "Prefs.h"

#include <stdlib.h>

#include <qcheckbox.h>
#include <qlabel.h>
#include <qpushbutton.h>
#include <qspinbox.h>
#include <qlayout.h>
#include <qvariant.h>
#include <qtooltip.h>
#include <qwhatsthis.h>
#include <qbuttongroup.h>
#include <qlineedit.h>
#include <qdir.h>
#ifdef USECOMBO
#include <qcombobox.h>
#else
#include <qpe/menubutton.h>
#endif
#include <qfontdatabase.h>

#ifdef USECOMBO
void populate_colours(QComboBox *mb)
#else
void populate_colours(MenuButton *mb)
#endif
{
    mb->setSizePolicy(QSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed));
    mb->insertItem("White");
    mb->insertItem("Black");
    mb->insertItem("Dark Gray");
    mb->insertItem("Gray");
    mb->insertItem("Light Gray");
    mb->insertItem("Red");
    mb->insertItem("Green");
    mb->insertItem("Blue");
    mb->insertItem("Cyan");
    mb->insertItem("Magenta");
    mb->insertItem("Yellow");
    mb->insertItem("Dark Red");
    mb->insertItem("Dark Green");
    mb->insertItem("Dark Blue");
    mb->insertItem("Dark Cyan");
    mb->insertItem("Dark Magenta");
    mb->insertItem("Dark Yellow");
}

CLayoutPrefs::CLayoutPrefs( QWidget* parent,  const char* name, WFlags fl )
    : QWidget( parent, name, fl )
{
    QHBoxLayout* hb = new QHBoxLayout(this);
    QButtonGroup* bg = new QButtonGroup(2, Qt::Horizontal, "Text", this);
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

    FixGraphics = new QCheckBox( bg );
    FixGraphics->setText( tr( "Fix Graphic Size" ) );

    hyphenate = new QCheckBox( bg );
    hyphenate->setText( tr( "Hyphenate" ) );

    /*
    customhyphen = new QCheckBox( bg );
    customhyphen->setText( tr( "Custom Hyphen'n" ) );
    */

    prepalm = new QCheckBox( bg );
    prepalm->setText( tr( "Repalm(Baen)" ) );

    pkern = new QCheckBox( bg );
    pkern->setText( tr( "Kern" ) );

    InlineTables = new QCheckBox( bg );
    InlineTables->setText( tr( "Inline Tables" ) );

    Underlinelinks = new QCheckBox( bg );
    Underlinelinks->setText( tr( "Underline Links" ) );
    /*
    Negative = new QCheckBox( bg );
    Negative->setText( tr( "Negative" ) );
    */

    //    Inverse = new QCheckBox( bg );
    //    Inverse->setText( tr( "Inverse" ) );
}

/*  
 *  Destroys the object and frees any allocated resources
 */
CLayoutPrefs::~CLayoutPrefs()
{
    // no need to delete child widgets, Qt does it all for us
}



CLayoutPrefs2::CLayoutPrefs2( int w, QWidget* parent,  const char* name, WFlags fl )
    : QWidget( parent, name, fl )
{
    
    QVBoxLayout* vb = new QVBoxLayout(this);
    QGridLayout* gl = new QGridLayout(vb, 4, 5);

    QLabel *TextLabel;

    TextLabel = new QLabel( this );
    TextLabel->setText( tr( "Page\nOverlap" ) );
    gl->addWidget(TextLabel, 0, 0);
    pageoverlap = new QSpinBox( this );
    pageoverlap->setRange(0,20);
    gl->addWidget(pageoverlap, 0, 1);

    TextLabel = new QLabel( this, "TextLabel1" );
    TextLabel->setText( tr( "Indent" ) );
    gl->addWidget(TextLabel, 1, 0);
    Indent = new QSpinBox( this, "Indent" );
    Indent->setRange(0,20);
    gl->addWidget(Indent, 1, 1);



    TextLabel = new QLabel( this );
    TextLabel->setText( tr( "Graphics\nZoom" ) );
    gl->addWidget(TextLabel, 1, 2);
    gfxzoom = new QSpinBox( this );
    gfxzoom->setRange(0,2000);
    gfxzoom->setLineStep(10);
    gfxzoom->setSuffix("%");
    gl->addWidget(gfxzoom, 1, 3);

    TextLabel = new QLabel( this, "TextLabel4" );
    TextLabel->setText( tr( "Top\nMargin" ) );
    gl->addWidget(TextLabel, 2, 0);
    TopMargin = new QSpinBox( this, "TMargin" );
    TopMargin->setRange(0, 1000);
    gl->addWidget(TopMargin, 2, 1);

    TextLabel = new QLabel( this );
    TextLabel->setText( tr( "Bottom\nMargin" ) );
    gl->addWidget(TextLabel, 2, 2);
    BottomMargin = new QSpinBox( this, "BMargin" );
    BottomMargin->setRange(0, 1000);
    gl->addWidget(BottomMargin, 2, 3);

    TextLabel = new QLabel( this, "TextLabel4" );
    TextLabel->setText( tr( "Left\nMargin" ) );
    gl->addWidget(TextLabel, 3, 0);
    LeftMargin = new QSpinBox( this, "LMargin" );
    LeftMargin->setRange(0, 1000);
    gl->addWidget(LeftMargin, 3, 1);

    TextLabel = new QLabel( this );
    TextLabel->setText( tr( "Right\nMargin" ) );
    gl->addWidget(TextLabel, 3, 2);
    RightMargin = new QSpinBox( this, "RMargin" );
    RightMargin->setRange(0, 1000);
    gl->addWidget(RightMargin, 3, 3);

    TextLabel = new QLabel( this );
    TextLabel->setText( tr( "Paragraph\nLeading" ) );
    gl->addWidget(TextLabel, 4, 0);
    ParaLead = new QSpinBox( this );
    ParaLead->setRange(-5, 50);
    gl->addWidget(ParaLead, 4, 1);

    TextLabel = new QLabel( this );
    TextLabel->setText( tr( "Line\nLeading" ) );
    gl->addWidget(TextLabel, 4, 2);
    LineLead = new QSpinBox( this );
    LineLead->setRange(-5, 50);
    gl->addWidget(LineLead, 4, 3);

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
	QFontDatabase f;
	QStringList flist = f.families();
	for (QStringList::Iterator nm = flist.begin(); nm != flist.end(); nm++)
	{
	    fontselector->insertItem(*nm);
	}
    } // delete the FontDatabase!!!
    gl->addWidget(fontselector, 1, 1, Qt::AlignTop);

#ifdef USECOMBO
    bgsel = new QComboBox( this );
#else
    bgsel = new MenuButton( this );
#endif
    populate_colours(bgsel);

#ifdef USECOMBO
    fgsel = new QComboBox( this );
#else
    fgsel = new MenuButton( this );
#endif
    populate_colours(fgsel);
    gl = new QGridLayout(vb, 2, 2);

    TextLabel = new QLabel( this);
    TextLabel->setText( tr( "Background" ) );
    gl->addWidget(TextLabel, 0, 0, Qt::AlignBottom);
    TextLabel = new QLabel( this);
    TextLabel->setText( tr( "Foreground" ) );
    gl->addWidget(TextLabel, 0, 1, Qt::AlignBottom);

    gl->addWidget(bgsel, 1, 0, Qt::AlignTop);
    gl->addWidget(fgsel, 1, 1, Qt::AlignTop);

}
/*
CLayoutPrefs2::CLayoutPrefs2( int w, QWidget* parent,  const char* name, WFlags fl )
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
    Margin->setRange(0, w/2);
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
	QFontDatabase f;
	QStringList flist = f.families();
	for (QStringList::Iterator nm = flist.begin(); nm != flist.end(); nm++)
	{
	    fontselector->insertItem(*nm);
	}
    } // delete the FontDatabase!!!
    gl->addWidget(fontselector, 1, 1, Qt::AlignTop);

#ifdef USECOMBO
    bgsel = new QComboBox( this );
#else
    bgsel = new MenuButton( this );
#endif
    populate_colours(bgsel);

#ifdef USECOMBO
    fgsel = new QComboBox( this );
#else
    fgsel = new MenuButton( this );
#endif
    populate_colours(fgsel);
    gl = new QGridLayout(vb, 2, 2);

    TextLabel = new QLabel( this);
    TextLabel->setText( tr( "Background" ) );
    gl->addWidget(TextLabel, 0, 0, Qt::AlignBottom);
    TextLabel = new QLabel( this);
    TextLabel->setText( tr( "Foreground" ) );
    gl->addWidget(TextLabel, 0, 1, Qt::AlignBottom);

    gl->addWidget(bgsel, 1, 0, Qt::AlignTop);
    gl->addWidget(fgsel, 1, 1, Qt::AlignTop);

}
*/
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

    QCheckBox* outcodec = new QCheckBox( bg );
    outcodec->setText( tr( "Output" ) );

}

CMiscPrefs::~CMiscPrefs()
{
    // no need to delete child widgets, Qt does it all for us
}
*/

CMiscPrefs::CMiscPrefs( QWidget* parent,  const char* name, WFlags fl )
    : QWidget( parent, name, fl )
{

  QVBoxLayout* vl = new QVBoxLayout(this);
    QHBoxLayout* hl = new QHBoxLayout;
    vl->addLayout(hl);

    vl->setMargin( 0 );
    hl->setMargin( 0 );

    QGroupBox* gb = new QGroupBox(1, Qt::Horizontal, "Select Action", this);
    hl->addWidget( gb );

    annotation = new QCheckBox( gb );
    annotation->setText( tr( "Annotation" ) );

    dictionary = new QCheckBox( gb );
    dictionary->setText( tr( "Dictionary" ) );

    clipboard = new QCheckBox( gb );
    clipboard->setText( tr( "Clipboard" ) );

    boutput = new QCheckBox( gb );
    boutput->setText( tr( "Output" ) );

    QButtonGroup* bg = new QButtonGroup(1, Qt::Horizontal, "Plucker", this);
    hl->addWidget( bg );

    Depluck = new QCheckBox( bg );
    Depluck->setText( tr( "Depluck" ) );

    Dejpluck = new QCheckBox( bg );
    Dejpluck->setText( tr( "Dejpluck" ) );

    Continuous = new QCheckBox( bg );
    Continuous->setText( tr( "Continuous" ) );
    bg = new QButtonGroup(2, Qt::Horizontal, "Background", this);
    vl->addWidget( bg );

//    QLabel* TextLabel = new QLabel( bg );
//    TextLabel->setText( tr( "Copy an image to \"background\" in\n~/Applications/uqtreader/Theme/" ) );

#ifdef USECOMBO
    bgtype = new QComboBox( bg );
#else
    bgtype = new MenuButton( this);
#endif
    bgtype->setSizePolicy(QSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed));

    bgtype->insertItem( tr("Centred") );
    bgtype->insertItem( tr("Tiled") );
    bgtype->insertItem( tr("Fitted") );

    DoubleBuffer = new QCheckBox( bg );
    DoubleBuffer->setText( tr( "Double Buffer" ) );

    QLabel* TextLabel = new QLabel( bg );
    TextLabel->setText( tr( "Minibar Colour" ) );
#ifdef USECOMBO
    minibarcol = new QComboBox( bg );
#else
    minibarcol = new MenuButton( this);
#endif
    populate_colours(minibarcol);
    minibarcol->setSizePolicy(QSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed));
}

CMiscPrefs::~CMiscPrefs()
{
    // no need to delete child widgets, Qt does it all for us
}

CScrollPrefs::CScrollPrefs( QWidget* parent,  const char* name, WFlags fl )
    : QWidget( parent, name, fl )
{

    QHBoxLayout* hl = new QHBoxLayout(this);

    hl->setMargin( 0 );

    QButtonGroup* bg = new QButtonGroup(2, Qt::Horizontal, "Scroll", this);
    hl->addWidget( bg );

    //    scrollinplace = new QCheckBox( bg );
    //    scrollinplace->setText( tr( "In Place" ) );
    QLabel* TextLabel = new QLabel( bg );
    TextLabel->setText( tr( "Scroll step" ) );
    //    gl->addWidget(TextLabel, 2, 0);
    scrollstep = new QSpinBox( bg );
    scrollstep->setRange(1, 10);
    scrollstep->setSizePolicy(QSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed));

    TextLabel = new QLabel( bg );
    TextLabel->setText( tr( "Scroll type" ) );
#ifdef USECOMBO
    scrolltype = new QComboBox( bg );
#else
    scrolltype = new MenuButton( this);
#endif
    scrolltype->insertItem("In Place");
    scrolltype->insertItem("Rolling (moving bg)");
    scrolltype->insertItem("Rolling (window)");
    scrolltype->insertItem("Rolling (static bg)");
    scrolltype->insertItem("Send to output");
    scrolltype->setSizePolicy(QSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed));

    TextLabel = new QLabel( bg );
    TextLabel->setText( tr( "Colour of scroll\nprogress indicator" ) );

#ifdef USECOMBO
    scrollcolor = new QComboBox( bg );
#else
    scrollcolor = new MenuButton( this);
#endif
    populate_colours(scrollcolor);
    scrollcolor->setSizePolicy(QSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed));

    TextLabel = new QLabel( bg );
    TextLabel->setText( tr( "Output" ) );

#ifdef USECOMBO
    outcodec = new QComboBox( bg );
#else
    outcodec = new MenuButton( this);
#endif
#ifdef USEQPE
#ifdef OPIE
      QString codecpath(getenv("OPIEDIR"));
#else
      QString codecpath(getenv("QTDIR"));
#endif
      codecpath += "/plugins/reader/outcodecs";
#else
      QString codecpath(getenv("READERDIR"));
      codecpath += "/outcodecs";
#endif
      QDir ocd(codecpath, "lib*.so");
      for (int i = 0; i < ocd.count(); ++i)
	{
	  QString tmp(ocd[i]);
	  outcodec->insertItem(tmp.mid(3,tmp.length()-6));
	}
    outcodec->setSizePolicy(QSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed));
}

CScrollPrefs::~CScrollPrefs()
{
    // no need to delete child widgets, Qt does it all for us
}


CPrefs::CPrefs( int w, bool fs, QWidget* parent, const char* name) : QDialog(parent, name, true)
{
    setCaption(tr( "OpieReader Settings" ) );
    QTabWidget* td = new QTabWidget(this);
    layout = new CLayoutPrefs(this);
    layout2 = new CLayoutPrefs2(w, this);
    scroll = new CScrollPrefs(this);
    misc = new CMiscPrefs(this);
    //    button = new CButtonPrefs(kmap, this);
    inter = new CInterPrefs(this);
    td->addTab(layout, tr("Layout"));
    td->addTab(layout2, tr("Layout(2)"));
    td->addTab(inter, tr("Locale"));
    td->addTab(scroll, tr("Scroll"));
    td->addTab(misc, tr("Misc"));
    //    td->addTab(button, tr("Buttons"));
    QVBoxLayout* v = new QVBoxLayout(this);
    v->addWidget(td);

    if (fs) showMaximized();
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

    gb = new QGroupBox(1, Qt::Horizontal, "Dictionary", this);

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
    SwapMouse->setText("Swap Tap\nActions");


    vb->addWidget(gb);

//    vb->addStretch();
    hb->addLayout(vb);
}

CInterPrefs::~CInterPrefs()
{
    // no need to delete child widgets, Qt does it all for us
}
