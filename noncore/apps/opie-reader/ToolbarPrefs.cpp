/****************************************************************************
** Form implementation generated from reading ui file 'Prefs.ui'
**
** Created: Tue Feb 11 23:53:35 2003
**      by:  The User Interface Compiler (uic)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/
#include "useqpe.h"
#include "ToolbarPrefs.h"

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
#endif

CBarPrefs::CBarPrefs(const QString& appdir, bool fs, QWidget* parent, const char* name) : QDialog(parent, name, true), config( appdir )
{
    setCaption(tr( "Toolbar Settings" ) );
    QTabWidget* td = new QTabWidget(this);
    misc = new CMiscBarPrefs(this);
    filebar = new CFileBarPrefs(config, this);
    navbar = new CNavBarPrefs(config, this);
    viewbar = new CViewBarPrefs(config, this);
    markbar = new CMarkBarPrefs(config, this);
    indbar = new CIndBarPrefs(config, this);
    td->addTab(filebar, tr("File"));
    td->addTab(navbar, tr("Navigation"));
    td->addTab(viewbar, tr("View"));
    td->addTab(markbar, tr("Marks"));
    td->addTab(indbar, tr("Indicators"));
    td->addTab(misc, tr("Policy"));
    QVBoxLayout* v = new QVBoxLayout(this);
    v->addWidget(td);

    if (fs) showMaximized();
}

/*
CBarPrefs1::CBarPrefs1( Config& _config, QWidget* parent,  const char* name, WFlags fl )
    : QWidget( parent, name, fl ), config(_config)
{
    config.setGroup( "Toolbar" );
    QVBoxLayout* vb = new QVBoxLayout(this);

    QGroupBox* bg = new QGroupBox(3, Qt::Horizontal, "File", this);
    vb->addWidget(bg);

    open = new QCheckBox( tr("Open"), bg );
    open->setChecked(config.readBoolEntry( "Open", false ));
    connect(open, SIGNAL(stateChanged(int)), this, SLOT( isChanged(int) ) );
    close = new QCheckBox( tr("Close"), bg );
    close->setChecked(config.readBoolEntry( "Close", false ));
    connect(close, SIGNAL(stateChanged(int)), this, SLOT( isChanged(int) ) );
    info = new QCheckBox( tr("Info"), bg );
    info->setChecked(config.readBoolEntry( "Info", false ));
    connect(info, SIGNAL(stateChanged(int)), this, SLOT( isChanged(int) ) );
    twotouch = new QCheckBox( tr("Two/One\nTouch"), bg );
    twotouch->setChecked(config.readBoolEntry( "Two/One Touch", false ));
    connect(twotouch, SIGNAL(stateChanged(int)), this, SLOT( isChanged(int) ) );
    find = new QCheckBox( tr("Find"), bg );
    find->setChecked(config.readBoolEntry( "Find", false ));
    connect(find, SIGNAL(stateChanged(int)), this, SLOT( isChanged(int) ) );

    bg = new QGroupBox(2, Qt::Horizontal, "Navigation", this);
    vb->addWidget(bg);
    scroll = new QCheckBox( tr("Scroll"), bg );
    scroll->setChecked(config.readBoolEntry( "Scroll", false ));
    connect(scroll, SIGNAL(stateChanged(int)), this, SLOT( isChanged(int) ) );
    navigation = new QCheckBox( tr("Back/Home/Forward"), bg );
    navigation->setChecked(config.readBoolEntry( "Back/Home/Forward", false ));
    connect(navigation, SIGNAL(stateChanged(int)), this, SLOT( isChanged(int) ) );
    page = new QCheckBox( tr("Page\nUp/Down"), bg );
    page->setChecked(config.readBoolEntry( "Page Up/Down", false ));
    connect(page, SIGNAL(stateChanged(int)), this, SLOT( isChanged(int) ) );
    startend = new QCheckBox( tr("Goto Start/End"), bg );
    startend->setChecked(config.readBoolEntry( "Goto Start/End", false ));
    connect(startend, SIGNAL(stateChanged(int)), this, SLOT( isChanged(int) ) );
    jump = new QCheckBox( tr("Jump"), bg );
    jump->setChecked(config.readBoolEntry( "Jump", false ));
    connect(jump, SIGNAL(stateChanged(int)), this, SLOT( isChanged(int) ) );
    pageline = new QCheckBox( tr("Page/Line Scroll"), bg );
    pageline->setChecked(config.readBoolEntry( "Page/Line Scroll", false ));
    connect(pageline, SIGNAL(stateChanged(int)), this, SLOT( isChanged(int) ) );
    m_isChanged = false;
}

void CBarPrefs1::saveall()
{
    config.setGroup( "Toolbar" );
    config.writeEntry( "Open", open->isChecked());
    config.writeEntry( "Close", close->isChecked());
    config.writeEntry( "Info", info->isChecked());
    config.writeEntry( "Two/One Touch", twotouch->isChecked());
    config.writeEntry( "Find", find->isChecked());
    config.writeEntry( "Scroll", scroll->isChecked());
    config.writeEntry( "Back/Home/Forward", navigation->isChecked());
    config.writeEntry( "Page Up/Down", page->isChecked());
    config.writeEntry( "Goto Start/End", startend->isChecked());
    config.writeEntry( "Jump", jump->isChecked());
    config.writeEntry( "Page/Line Scroll", pageline->isChecked());
}

CBarPrefs1::~CBarPrefs1()
{
}
*/
/*
CBarPrefs2::CBarPrefs2( Config& _config, QWidget* parent,  const char* name, WFlags fl )
    : QWidget( parent, name, fl ), config(_config)
{
    QVBoxLayout* vb = new QVBoxLayout(this);

    QGroupBox* bg = new QGroupBox(3, Qt::Horizontal, "View", this);
    vb->addWidget(bg);

    config.setGroup( "Toolbar" );

    fullscreen = new QCheckBox( tr("Fullscreen"), bg );
    fullscreen->setChecked(config.readBoolEntry( "Fullscreen", false ));
    connect(fullscreen, SIGNAL(stateChanged(int)), this, SLOT( isChanged(int) ) );
    zoom = new QCheckBox( tr("Zoom"), bg );
    zoom->setChecked(config.readBoolEntry( "Zoom In/Out", false ));
    connect(zoom, SIGNAL(stateChanged(int)), this, SLOT( isChanged(int) ) );
    setfont = new QCheckBox( tr("Set Font"), bg );
    setfont->setChecked(config.readBoolEntry( "Set Font", false ));
    connect(setfont, SIGNAL(stateChanged(int)), this, SLOT( isChanged(int) ) );

    encoding = new QCheckBox( tr("Encoding"), bg );
    encoding->setChecked(config.readBoolEntry("Encoding Select", false));
    connect(encoding, SIGNAL(stateChanged(int)), this, SLOT( isChanged(int) ) );
    ideogram = new QCheckBox( tr("Ideogram"), bg );
    ideogram->setChecked(config.readBoolEntry("Ideogram Mode", false));
    connect(ideogram, SIGNAL(stateChanged(int)), this, SLOT( isChanged(int) ) );

    bg = new QGroupBox(3, Qt::Horizontal, "Marks", this);
    vb->addWidget(bg);
    mark = new QCheckBox( tr("Bookmark"), bg );
    mark->setChecked(config.readBoolEntry( "Mark", false ));
    connect(mark, SIGNAL(stateChanged(int)), this, SLOT( isChanged(int) ) );
    annotate = new QCheckBox( tr("Annotate"), bg );
    annotate->setChecked(config.readBoolEntry( "Annotate", false ));
    connect(annotate, SIGNAL(stateChanged(int)), this, SLOT( isChanged(int) ) );
    go_to = new QCheckBox( tr("Goto"), bg );
    go_to->setChecked(config.readBoolEntry( "Goto", false ));
    connect(go_to, SIGNAL(stateChanged(int)), this, SLOT( isChanged(int) ) );
    Delete = new QCheckBox( tr("Delete"), bg );
    Delete->setChecked(config.readBoolEntry( "Delete", false ));
    connect(Delete, SIGNAL(stateChanged(int)), this, SLOT( isChanged(int) ) );
    autogen = new QCheckBox( tr("Autogen"), bg );
    autogen->setChecked(config.readBoolEntry( "Autogen", false ));
    connect(autogen, SIGNAL(stateChanged(int)), this, SLOT( isChanged(int) ) );
    clear = new QCheckBox( tr("Clear"), bg );
    clear->setChecked(config.readBoolEntry( "Clear", false ));
    connect(clear, SIGNAL(stateChanged(int)), this, SLOT( isChanged(int) ) );
    save = new QCheckBox( tr("Save"), bg );
    save->setChecked(config.readBoolEntry( "Save", false ));
    connect(save, SIGNAL(stateChanged(int)), this, SLOT( isChanged(int) ) );
    tidy = new QCheckBox( tr("Tidy"), bg );
    tidy->setChecked(config.readBoolEntry( "Tidy", false ));
    connect(tidy, SIGNAL(stateChanged(int)), this, SLOT( isChanged(int) ) );
    block = new QCheckBox( tr("Mark/Copy"), bg );
    block->setChecked(config.readBoolEntry( "Start/Copy Block", false ));
    connect(block, SIGNAL(stateChanged(int)), this, SLOT( isChanged(int) ) );

    bg = new QGroupBox(1, Qt::Horizontal, "Indicators", this);
    vb->addWidget(bg);
    indannotate = new QCheckBox( tr("Annotation"), bg );
    indannotate->setChecked(config.readBoolEntry( "Annotation indicator", false ));
    connect(indannotate, SIGNAL(stateChanged(int)), this, SLOT( isChanged(int) ) );
    m_isChanged = false;
}

void CBarPrefs2::saveall()
{
    config.setGroup( "Toolbar" );
    config.writeEntry( "Fullscreen", fullscreen->isChecked());
    config.writeEntry( "Zoom In/Out", zoom->isChecked());
    config.writeEntry( "Set Font", setfont->isChecked());
    config.writeEntry("Encoding Select", encoding->isChecked());
    config.writeEntry("Ideogram Mode", ideogram->isChecked());

    config.writeEntry( "Mark", mark->isChecked());
    config.writeEntry( "Annotate", annotate->isChecked());
    config.writeEntry( "Goto", go_to->isChecked());
    config.writeEntry( "Delete", Delete->isChecked());
    config.writeEntry( "Autogen", autogen->isChecked());
    config.writeEntry( "Clear", clear->isChecked());
    config.writeEntry( "Save", save->isChecked());
    config.writeEntry( "Tidy", tidy->isChecked());
    config.writeEntry( "Start/Copy Block", block->isChecked());
    config.writeEntry( "Annotation indicator", indannotate->isChecked());
}

CBarPrefs2::~CBarPrefs2()
{
}
*/

CFileBarPrefs::CFileBarPrefs( Config& _config, QWidget* parent,  const char* name, WFlags fl )
    : QWidget( parent, name, fl ), config(_config)
{
    config.setGroup( "Toolbar" );
    QVBoxLayout* vb = new QVBoxLayout(this);

    QGroupBox* bg = new QGroupBox(2, Qt::Horizontal, "File", this);
    vb->addWidget(bg);

    open = new QCheckBox( tr("Open"), bg );
    open->setChecked(config.readBoolEntry( "Open", false ));
    connect(open, SIGNAL(stateChanged(int)), this, SLOT( isChanged(int) ) );
    close = new QCheckBox( tr("Close"), bg );
    close->setChecked(config.readBoolEntry( "Close", false ));
    connect(close, SIGNAL(stateChanged(int)), this, SLOT( isChanged(int) ) );
    info = new QCheckBox( tr("Info"), bg );
    info->setChecked(config.readBoolEntry( "Info", false ));
    connect(info, SIGNAL(stateChanged(int)), this, SLOT( isChanged(int) ) );
    twotouch = new QCheckBox( tr("Two/One\nTouch"), bg );
    twotouch->setChecked(config.readBoolEntry( "Two/One Touch", false ));
    connect(twotouch, SIGNAL(stateChanged(int)), this, SLOT( isChanged(int) ) );
    find = new QCheckBox( tr("Find"), bg );
    find->setChecked(config.readBoolEntry( "Find", false ));
    connect(find, SIGNAL(stateChanged(int)), this, SLOT( isChanged(int) ) );
    m_isChanged = false;
}

void CFileBarPrefs::saveall()
{
    config.setGroup( "Toolbar" );
    config.writeEntry( "Open", open->isChecked());
    config.writeEntry( "Close", close->isChecked());
    config.writeEntry( "Info", info->isChecked());
    config.writeEntry( "Two/One Touch", twotouch->isChecked());
    config.writeEntry( "Find", find->isChecked());
}

CFileBarPrefs::~CFileBarPrefs()
{
}

CNavBarPrefs::CNavBarPrefs( Config& _config, QWidget* parent,  const char* name, WFlags fl )
    : QWidget( parent, name, fl ), config(_config)
{
    config.setGroup( "Toolbar" );
    QVBoxLayout* vb = new QVBoxLayout(this);

    QGroupBox* bg = new QGroupBox(2, Qt::Horizontal, "Navigation", this);
    vb->addWidget(bg);
    scroll = new QCheckBox( tr("Scroll"), bg );
    scroll->setChecked(config.readBoolEntry( "Scroll", false ));
    connect(scroll, SIGNAL(stateChanged(int)), this, SLOT( isChanged(int) ) );

    navback = new QCheckBox( tr("Back"), bg );
    navback->setChecked(config.readBoolEntry( "Back", false ));
    connect(navback, SIGNAL(stateChanged(int)), this, SLOT( isChanged(int) ) );
    navhome = new QCheckBox( tr("Home"), bg );
    navhome->setChecked(config.readBoolEntry( "Home", false ));
    connect(navhome, SIGNAL(stateChanged(int)), this, SLOT( isChanged(int) ) );
    navforward = new QCheckBox( tr("Forward"), bg );
    navforward->setChecked(config.readBoolEntry( "Forward", false ));
    connect(navforward, SIGNAL(stateChanged(int)), this, SLOT( isChanged(int) ) );

    pageup = new QCheckBox( tr("Page Up"), bg );
    pageup->setChecked(config.readBoolEntry( "Page Up", false ));
    connect(pageup, SIGNAL(stateChanged(int)), this, SLOT( isChanged(int) ) );
    pagedown = new QCheckBox( tr("Page Down"), bg );
    pagedown->setChecked(config.readBoolEntry( "Page Down", false ));
    connect(pagedown, SIGNAL(stateChanged(int)), this, SLOT( isChanged(int) ) );
    gotostart = new QCheckBox( tr("Goto Start"), bg );
    gotostart->setChecked(config.readBoolEntry( "Goto Start", false ));
    connect(gotostart, SIGNAL(stateChanged(int)), this, SLOT( isChanged(int) ) );
    gotoend = new QCheckBox( tr("Goto End"), bg );
    gotoend->setChecked(config.readBoolEntry( "Goto End", false ));
    connect(gotoend, SIGNAL(stateChanged(int)), this, SLOT( isChanged(int) ) );
    jump = new QCheckBox( tr("Jump"), bg );
    jump->setChecked(config.readBoolEntry( "Jump", false ));
    connect(jump, SIGNAL(stateChanged(int)), this, SLOT( isChanged(int) ) );
    pageline = new QCheckBox( tr("Page/Line Scroll"), bg );
    pageline->setChecked(config.readBoolEntry( "Page/Line Scroll", false ));
    connect(pageline, SIGNAL(stateChanged(int)), this, SLOT( isChanged(int) ) );
    m_isChanged = false;
}

void CNavBarPrefs::saveall()
{
    config.setGroup( "Toolbar" );
    config.writeEntry( "Scroll", scroll->isChecked());
    config.writeEntry( "Back", navback->isChecked());
    config.writeEntry( "Home", navhome->isChecked());
    config.writeEntry( "Forward", navforward->isChecked());
    config.writeEntry( "Page Up", pageup->isChecked());
    config.writeEntry( "Page Down", pagedown->isChecked());
    config.writeEntry( "Goto Start", gotostart->isChecked());
    config.writeEntry( "Goto End", gotoend->isChecked());
    config.writeEntry( "Jump", jump->isChecked());
    config.writeEntry( "Page/Line Scroll", pageline->isChecked());
}

CNavBarPrefs::~CNavBarPrefs()
{
}

CViewBarPrefs::CViewBarPrefs( Config& _config, QWidget* parent,  const char* name, WFlags fl )
    : QWidget( parent, name, fl ), config(_config)
{
    QVBoxLayout* vb = new QVBoxLayout(this);

    QGroupBox* bg = new QGroupBox(2, Qt::Horizontal, "View", this);
    vb->addWidget(bg);

    config.setGroup( "Toolbar" );

    fullscreen = new QCheckBox( tr("Fullscreen"), bg );
    fullscreen->setChecked(config.readBoolEntry( "Fullscreen", false ));
    connect(fullscreen, SIGNAL(stateChanged(int)), this, SLOT( isChanged(int) ) );
    zoomin = new QCheckBox( tr("Zoom In"), bg );
    zoomin->setChecked(config.readBoolEntry( "Zoom In", false ));
    connect(zoomin, SIGNAL(stateChanged(int)), this, SLOT( isChanged(int) ) );
    zoomout = new QCheckBox( tr("Zoom Out"), bg );
    zoomout->setChecked(config.readBoolEntry( "Zoom Out", false ));
    connect(zoomout, SIGNAL(stateChanged(int)), this, SLOT( isChanged(int) ) );
    setfont = new QCheckBox( tr("Set Font"), bg );
    setfont->setChecked(config.readBoolEntry( "Set Font", false ));
    connect(setfont, SIGNAL(stateChanged(int)), this, SLOT( isChanged(int) ) );

    encoding = new QCheckBox( tr("Encoding"), bg );
    encoding->setChecked(config.readBoolEntry("Encoding Select", false));
    connect(encoding, SIGNAL(stateChanged(int)), this, SLOT( isChanged(int) ) );
    ideogram = new QCheckBox( tr("Ideogram"), bg );
    ideogram->setChecked(config.readBoolEntry("Ideogram Mode", false));
    connect(ideogram, SIGNAL(stateChanged(int)), this, SLOT( isChanged(int) ) );
    m_isChanged = false;
}

void CViewBarPrefs::saveall()
{
    config.setGroup( "Toolbar" );
    config.writeEntry( "Fullscreen", fullscreen->isChecked());
    config.writeEntry( "Zoom In", zoomin->isChecked());
    config.writeEntry( "Zoom Out", zoomout->isChecked());
    config.writeEntry( "Set Font", setfont->isChecked());
    config.writeEntry("Encoding Select", encoding->isChecked());
    config.writeEntry("Ideogram Mode", ideogram->isChecked());
}

CViewBarPrefs::~CViewBarPrefs()
{
}

CMarkBarPrefs::CMarkBarPrefs( Config& _config, QWidget* parent,  const char* name, WFlags fl )
    : QWidget( parent, name, fl ), config(_config)
{
    QVBoxLayout* vb = new QVBoxLayout(this);

    QGroupBox* bg = new QGroupBox(2, Qt::Horizontal, "Marks", this);
    vb->addWidget(bg);
    mark = new QCheckBox( tr("Bookmark"), bg );
    mark->setChecked(config.readBoolEntry( "Mark", false ));
    connect(mark, SIGNAL(stateChanged(int)), this, SLOT( isChanged(int) ) );
    annotate = new QCheckBox( tr("Annotate"), bg );
    annotate->setChecked(config.readBoolEntry( "Annotate", false ));
    connect(annotate, SIGNAL(stateChanged(int)), this, SLOT( isChanged(int) ) );
    go_to = new QCheckBox( tr("Goto"), bg );
    go_to->setChecked(config.readBoolEntry( "Goto", false ));
    connect(go_to, SIGNAL(stateChanged(int)), this, SLOT( isChanged(int) ) );
    Delete = new QCheckBox( tr("Delete"), bg );
    Delete->setChecked(config.readBoolEntry( "Delete", false ));
    connect(Delete, SIGNAL(stateChanged(int)), this, SLOT( isChanged(int) ) );
    autogen = new QCheckBox( tr("Autogen"), bg );
    autogen->setChecked(config.readBoolEntry( "Autogen", false ));
    connect(autogen, SIGNAL(stateChanged(int)), this, SLOT( isChanged(int) ) );
    clear = new QCheckBox( tr("Clear"), bg );
    clear->setChecked(config.readBoolEntry( "Clear", false ));
    connect(clear, SIGNAL(stateChanged(int)), this, SLOT( isChanged(int) ) );
    save = new QCheckBox( tr("Save"), bg );
    save->setChecked(config.readBoolEntry( "Save", false ));
    connect(save, SIGNAL(stateChanged(int)), this, SLOT( isChanged(int) ) );
    tidy = new QCheckBox( tr("Tidy"), bg );
    tidy->setChecked(config.readBoolEntry( "Tidy", false ));
    connect(tidy, SIGNAL(stateChanged(int)), this, SLOT( isChanged(int) ) );
    startblock = new QCheckBox( tr("Mark Block"), bg );
    startblock->setChecked(config.readBoolEntry( "Start Block", false ));
    connect(startblock, SIGNAL(stateChanged(int)), this, SLOT( isChanged(int) ) );
    copyblock = new QCheckBox( tr("Copy Block"), bg );
    copyblock->setChecked(config.readBoolEntry( "Copy Block", false ));
    connect(copyblock, SIGNAL(stateChanged(int)), this, SLOT( isChanged(int) ) );
    m_isChanged = false;
}

void CMarkBarPrefs::saveall()
{
    config.setGroup( "Toolbar" );
    config.writeEntry( "Mark", mark->isChecked());
    config.writeEntry( "Annotate", annotate->isChecked());
    config.writeEntry( "Goto", go_to->isChecked());
    config.writeEntry( "Delete", Delete->isChecked());
    config.writeEntry( "Autogen", autogen->isChecked());
    config.writeEntry( "Clear", clear->isChecked());
    config.writeEntry( "Save", save->isChecked());
    config.writeEntry( "Tidy", tidy->isChecked());
    config.writeEntry( "Start Block", startblock->isChecked());
    config.writeEntry( "Copy Block", copyblock->isChecked());
}

CMarkBarPrefs::~CMarkBarPrefs()
{
}

CMiscBarPrefs::CMiscBarPrefs( QWidget* parent,  const char* name, WFlags fl )
    : QWidget( parent, name, fl )
{

    QGridLayout* hl = new QGridLayout(this,1,2);

    hl->setMargin( 0 );

    QGroupBox* gb = new QGroupBox(1, Qt::Horizontal, "Dialogs", this);
    floating = new QCheckBox(gb);
    floating->setText(tr("Floating"));

//    QLabel* TextLabel = new QLabel( gb );
//    TextLabel->setText( tr( "Select Button" ) );


    hl->addWidget( gb, 0, 0 );


    gb = new QGroupBox(1, Qt::Horizontal, "Bars (Restart)", this);

//    QLabel* ql = new QLabel("Restart to apply changes", gb);
//    TextLabel = new QLabel( gb );
//    TextLabel->setText( tr( "Policy" ) );

#ifdef USECOMBO
    tbpolicy = new QComboBox(gb);
#else
    tbpolicy = new MenuButton(gb);
#endif
    tbpolicy->setSizePolicy(QSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed));
    tbpolicy->insertItem(tr("Single bar"));
    tbpolicy->insertItem(tr("Menu/tool bar"));
    tbpolicy->insertItem(tr("Multiple bars"));

#ifdef USECOMBO
    tbposition = new QComboBox(gb);
#else
    tbposition = new MenuButton(gb);
#endif
    tbposition->insertItem(tr("Top"));
    tbposition->insertItem(tr("Bottom"));
    tbposition->insertItem(tr("Right"));
    tbposition->insertItem(tr("Left"));
    tbposition->insertItem(tr("Minimised"));

    tbmovable = new QCheckBox( tr("Movable"), gb );
//  ch->setChecked(config.readBoolEntry( "Movable", false ));

    hl->addWidget(gb, 0, 1);

}

CMiscBarPrefs::~CMiscBarPrefs()
{
    // no need to delete child widgets, Qt does it all for us
}


CIndBarPrefs::CIndBarPrefs( Config& _config, QWidget* parent,  const char* name, WFlags fl )
    : QWidget( parent, name, fl ), config(_config)
{
    QVBoxLayout* vb = new QVBoxLayout(this);

    QGroupBox* bg = new QGroupBox(1, Qt::Horizontal, "Indicators", this);
    vb->addWidget(bg);
    indannotate = new QCheckBox( tr("Annotation"), bg );
    indannotate->setChecked(config.readBoolEntry( "Annotation indicator", false ));
    connect(indannotate, SIGNAL(stateChanged(int)), this, SLOT( isChanged(int) ) );
    m_isChanged = false;
}

void CIndBarPrefs::saveall()
{
    config.setGroup( "Toolbar" );
    config.writeEntry( "Annotation indicator", indannotate->isChecked());
}

CIndBarPrefs::~CIndBarPrefs()
{
}
