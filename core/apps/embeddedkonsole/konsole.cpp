
/* ---------------------------------------------------------------------- */
/*                                                                        */
/* [main.C]                        Konsole                                */
/*                                                                        */
/* ---------------------------------------------------------------------- */
/*                                                                        */
/* Copyright (c) 1997,1998 by Lars Doelle <lars.doelle@on-line.de>        */
/*                                                                        */
/* This file is part of Konsole, an X terminal.                           */
/*                                                                        */
/* The material contained in here more or less directly orginates from    */
/* kvt, which is copyright (c) 1996 by Matthias Ettrich <ettrich@kde.org> */
/*                                                                        */
/* ---------------------------------------------------------------------- */
/*                        */
/* Ported Konsole to Qt/Embedded                                              */
/*                        */
/* Copyright (C) 2000 by John Ryland <jryland@trolltech.com>                  */
/*                        */
/* -------------------------------------------------------------------------- */
// enhancements added by L.J. Potter <ljp@llornkcor.com>
// enhancements added by Phillip Kuhn

#include <stdlib.h>

#ifdef  QT_QWS_OPIE
#include <opie2/ocolorpopupmenu.h>
#endif

#include <qpe/resource.h>

#include <qdir.h>
#include <qevent.h>
#include <qdragobject.h>
#include <qobjectlist.h>
#include <qtoolbutton.h>
#include <qtoolbar.h>
#include <qpushbutton.h>
#include <qfontdialog.h>
#include <qglobal.h>
#include <qpainter.h>
#include <qmenubar.h>
#include <qmessagebox.h>
#include <qaction.h>
#include <qapplication.h>
#include <qfontmetrics.h>
#include <qcombobox.h>
#include <qevent.h>
#include <qtabwidget.h>
#include <qtabbar.h>
#include <qpe/config.h>
#include <qstringlist.h>
#include <qpalette.h>
#include <qfontdatabase.h>
#include <qfile.h>
#include <qspinbox.h>
#include <qlayout.h>
#include <qvbox.h>

#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "konsole.h"
#include "keytrans.h"
#include "commandeditdialog.h"

class EKNumTabBar : public QTabBar
{
public:
    EKNumTabBar(QWidget *parent = 0, const char *name = 0) :
            QTabBar(parent, name)
    {}

    //  QList<QTab> *getTabList() { return(tabList()); }

    void numberTabs()
    {
        // Yes, it really is this messy. QTabWidget needs functions
        // that provide acces to tabs in a sequential way.
        int m=INT_MIN;
        for (int i=0; i<count(); i++)
        {
            QTab* left=0;
            QListIterator<QTab> it(*tabList());
            int x=INT_MAX;
            for( QTab* t; (t=it.current()); ++it )
            {
                int tx = t->rect().x();
                if ( tx<x && tx>m )
                {
                    x = tx;
                    left = t;
                }
            }
            if ( left )
            {
                left->setText(QString::number(i+1));
                m = left->rect().x();
            }
        }
    }

    virtual QSize sizeHint() const
    {
        if (isHidden())
        {
            return(QSize(0,0));
        }
        else
        {
            QSize size = QTabBar::sizeHint();
            int shrink = 5;
            if (qApp->desktop()->width() > 600 || qApp->desktop()->height() > 600)
            {
                shrink = 10;
            }
            size.setHeight(size.height() - shrink);
            return(size);
        }
    }

};

class EKNumTabWidget : public QTabWidget
{
public:
    EKNumTabWidget(QWidget* parent) : QTabWidget(parent)
    {
        setTabBar(new EKNumTabBar(parent,"EKTabBar"));
        setSizePolicy( QSizePolicy( QSizePolicy::Expanding, QSizePolicy::Expanding ) );
    }

    EKNumTabBar *getTabBar() const
    {
        return ((EKNumTabBar*)tabBar());
    }


    void addTab(QWidget* w)
    {
        QTab* t = new QTab(QString::number(tabBar()->count()+1));
        QTabWidget::addTab(w,t);
    }

    void removeTab(QWidget* w)
    {
        removePage(w);
        ((EKNumTabBar*)tabBar())->numberTabs();
    }
};

// This could be configurable or dynamicly generated from the bash history
// file of the user
static const char *commonCmds[] =
    {
        "ls ", // I left this here, cause it looks better than the first alpha
        "cardctl eject",
        "cat ",
        "cd ",
        "chmod ",
        "clear",
        "cp ",
        "dc ",
        "df ",
        "dmesg",
        "echo ",
        "env",
        "find ",
        "free",
        "grep ",
        "ifconfig ",
        "ipkg ",
        "mkdir ",
        "mv ",
        "nc localhost 7776",
        "nc localhost 7777",
        "netstat ",
        "nslookup ",
        "ping ",
        "ps aux",
        "pwd ",
        "qcop QPE/System 'linkChanged(QString)' ''",
        "qcop QPE/System 'restart()'",
        "qcop QPE/System 'quit()'",
        "rm ",
        "rmdir ",
        "route ",
        "set ",
        "traceroute",

        /*
            "gzip",
            "gunzip",
            "chgrp",
            "chown",
            "date",
            "dd",
            "df",
            "dmesg",
            "fuser",
            "hostname",
            "kill",
            "killall",
            "ln",
            "ping",
            "mount",
            "more",
            "sort",
            "touch",
            "umount",
            "mknod",
            "netstat",
        */

        "exit",
        NULL
    };


Konsole::Konsole(QWidget* parent, const char* name, WFlags fl) :
        QMainWindow(parent, name, fl)
{
    QStrList args;
    init("/bin/bash",args);
}

Konsole::Konsole(const char* name, const char* _pgm, QStrList & _args, int)
        : QMainWindow(0, name)
{
    init(_pgm,_args);
}

struct HistoryItem
{
    HistoryItem(int c, const QString &l)
    {
        count = c;
        line = l;
    }
    int count;
    QString line;
};

class HistoryList : public QList<HistoryItem>
{
    virtual int compareItems( QCollection::Item item1, QCollection::Item item2)
    {
        int c1 = ((HistoryItem*)item1)->count;
        int c2 = ((HistoryItem*)item2)->count;
        if (c1 > c2)
            return(1);
        if (c1 < c2)
            return(-1);
        return(0);
    }
};

void Konsole::initCommandList()
{
    //    qDebug("Konsole::initCommandList");
    Config cfg( "Konsole" );
    cfg.setGroup("Commands");
    //    commonCombo->setInsertionPolicy(QComboBox::AtCurrent);
    commonCombo->clear();

    if (cfg.readEntry("ShellHistory","TRUE") == "TRUE")
    {
        QString histfilename = QString(getenv("HOME")) + "/.bash_history";
        histfilename = cfg.readEntry("ShellHistoryPath",histfilename);
        QFile histfile(histfilename);
        // note: compiler barfed on:
        // QFile histfile(QString(getenv("HOME")) + "/.bash_history");
        if (histfile.open( IO_ReadOnly ))
        {
            QString line;
            uint i;
            HistoryList items;

            int lineno = 0;
            while(!histfile.atEnd())
            {
                if (histfile.readLine(line, 200) < 0)
                {
                    break;
                }
                line = line.left(line.length()-1);
                lineno++;

                for(i=0; i<items.count(); i++)
                {
                    if (line == items.at(i)->line)
                    {
                        // weight recent commands & repeated commands more
                        // by adding up the index of each command
                        items.at(i)->count += lineno;
                        break;
                    }
                }
                if (i >= items.count())
                {
                    items.append(new HistoryItem(lineno, line));
                }
            }
            items.sort();
            int n = items.count();
            if (n > 40)
            {
                n = 40;
            }
            for(int i=0; i<n; i++)
            {
                // should insert start of command, but keep whole thing
                if (items.at(items.count()-i-1)->line.length() < 30)
                {
                    commonCombo->insertItem(items.at(items.count()-i-1)->line);
                }
            }
            histfile.close();
        }
    }
    if (cfg.readEntry("Commands Set","FALSE") == "FALSE")
    {
        for (int i = 0; commonCmds[i] != NULL; i++)
        {
            commonCombo->insertItem(commonCmds[i]);
        }
    }
    else
    {
        for (int i = 0; i < 100; i++)
        {
            if (!(cfg.readEntry( QString::number(i),"")).isEmpty())
                commonCombo->insertItem(cfg.readEntry( QString::number(i),""));
        }
    }


}

static void sig_handler(int x)
{
    printf("got signal %d\n",x);
}

void Konsole::init(const char* _pgm, QStrList & _args)
{

#if 0
    for(int i=1; i<=31; i++)
    {
        if (i != SIGPIPE && i != SIGPROF && i != SIGSEGV
                && i != SIGINT && i != SIGILL && i != SIGTERM
                && i != SIGBUS)
            signal(i,sig_handler);
    }
#endif
    signal(SIGSTOP, sig_handler);
    signal(SIGCONT, sig_handler);
    signal(SIGTSTP, sig_handler);

    b_scroll = TRUE; // histon;
    n_keytab = 0;
    n_render = 0;
    startUp=0;
    fromMenu = FALSE;
    fullscreen = false;

    setCaption( tr( "Konsole" ) );
    setIcon( Resource::loadPixmap( "konsole/Terminal" ) );

    Config cfg( "Konsole" );
    cfg.setGroup("Font");
    QString tmp;

    // initialize the list of allowed fonts ///////////////////////////////////

    QString cfgFontName = cfg.readEntry("FontName","Lcfont");
    int cfgFontSize = cfg.readNumEntry("FontSize",18);

    cfont = -1;

    // this code causes repeated access to all the font files
    // which does slow down startup
    QFontDatabase fontDB;
    QStringList familyNames;
    familyNames = fontDB.families( FALSE );
    QString s;
    int fontIndex = 0;
    int familyNum = 0;
    fontList = new QPopupMenu( this );

    for(uint j = 0; j < (uint)familyNames.count(); j++)
    {
        s = familyNames[j];
        if ( s.contains('-') )
        {
            int i = s.find('-');
            s = s.right( s.length() - i - 1 ) + " [" + s.left( i ) + "]";
        }
        s[0] = s[0].upper();

        QValueList<int> sizes = fontDB.pointSizes( familyNames[j] );

        printf("family[%d] = %s with %d sizes\n", j, familyNames[j].latin1(),
               sizes.count());

        if (sizes.count() > 0)
        {
            QPopupMenu *sizeMenu;
            QFont f;
            int last_width = -1;
            sizeMenu = NULL;

            for(uint i = 0; i < (uint)sizes.count() + 4; i++)
            {
                //    printf("family %s size %d  ", familyNames[j].latin1(), sizes[i]);
                // need to divide by 10 on the Z, but not otherwise
                int size;

                if (i >= (uint)sizes.count())
                {
                    // try for expandable fonts
                    size = sizes[sizes.count()-1] + 2 * (i - sizes.count() + 1);
                }
                else
                {
                    printf("sizes[%d] = %d\n", i, sizes[i]);
                    size = sizes[i];
                }
                // a hack, sizes on Z seem to be points*10
                size /= 10;

                f = QFont(familyNames[j], size);
                f.setFixedPitch(true);
                QFontMetrics fm(f);
                // don't trust f.fixedPitch() or f.exactMatch(), they lie!!
                if (fm.width("l") == fm.width("m")
                        && (i < (uint)sizes.count()
                            || fm.width("m") > last_width))
                {
                    if (i < (uint)sizes.count())
                    {
                        last_width = fm.width("m");
                    }
                    if (sizeMenu == NULL)
                    {
                        sizeMenu = new QPopupMenu();
                    }
                    int id = sizeMenu->insertItem(QString("%1").arg(size), fontIndex);
                    sizeMenu->setItemParameter(id, fontIndex);
                    sizeMenu->connectItem(id, this, SLOT(setFont(int)));
                    QString name = s + " " + QString::number(size);
                    fonts.append(new VTFont(name, f, familyNames[j], familyNum, size));
                    if (familyNames[j] == cfgFontName && size == cfgFontSize)
                    {
                        cfont = fontIndex;
                    }
                    printf("FOUND: %s family %s size %d\n", name.latin1(), familyNames[j].latin1(), size);
                    fontIndex++;
                }
            }
            if (sizeMenu)
            {
                fontList->insertItem(s, sizeMenu, familyNum + 1000);

                familyNum++;
            }
        }

    }

    if (cfont < 0 || cfont >= (int)fonts.count())
    {
        cfont = 0;
    }

    // create terminal emulation framework ////////////////////////////////////
    nsessions = 0;

    tab = new EKNumTabWidget(this);
    //  tab->setMargin(tab->margin()-5);
    connect(tab, SIGNAL(currentChanged(QWidget*)), this, SLOT(switchSession(QWidget*)));

    // create terminal toolbar ////////////////////////////////////////////////
    setToolBarsMovable( FALSE );
    menuToolBar = new QToolBar( this );
    menuToolBar->setHorizontalStretchable( TRUE );

    QMenuBar *menuBar = new QMenuBar( menuToolBar );

    bool c7xx = false;
    if (qApp->desktop()->width() > 600 || qApp->desktop()->height() > 600)
    {
        c7xx = true;
    }
    QFont menuFont;
    menuFont.setPointSize(c7xx? 18 : 10);
    qApp->setFont(menuFont, true);

    setFont(cfont);

    configMenu = new QPopupMenu( this);
    colorMenu = new QPopupMenu( this);
    scrollMenu = new QPopupMenu( this);
    editCommandListMenu = new QPopupMenu( this);

    configMenu->insertItem(tr("Command List"), editCommandListMenu);

    bool listHidden;
    cfg.setGroup("Menubar");
    if( cfg.readEntry("Hidden","FALSE") == "TRUE")
    {
        ec_cmdlist = editCommandListMenu->insertItem( tr( "Show command list" ));
        listHidden=TRUE;
    }
    else
    {
        ec_cmdlist = editCommandListMenu->insertItem( tr( "Hide command list" ));
        listHidden=FALSE;
    }

    cfg.setGroup("Tabs");

    tabMenu = new QPopupMenu(this);
    tm_bottom = tabMenu->insertItem(tr("Bottom" ));
    tm_top = tabMenu->insertItem(tr("Top"));
    tm_hidden = tabMenu->insertItem(tr("Hidden"));

    configMenu->insertItem(tr("Tabs"), tabMenu);

    tmp=cfg.readEntry("Position","Top");
    if(tmp=="Top")
    {
        tab->setTabPosition(QTabWidget::Top);
        tab->getTabBar()->show();
        tabPos = tm_top;
    }
    else if (tmp=="Bottom")
    {
        tab->setTabPosition(QTabWidget::Bottom);
        tab->getTabBar()->show();
        tabPos = tm_bottom;
    }
    else
    {
        tab->getTabBar()->hide();
        tab->setMargin(tab->margin());
        tabPos = tm_hidden;
    }

    cm_bw = colorMenu->insertItem(tr( "Black on White"));
    cm_wb = colorMenu->insertItem(tr( "White on Black"));
    cm_gb = colorMenu->insertItem(tr( "Green on Black"));
    //  cm_bt = colorMenu->insertItem(tr( "Black on Transparent"));
    cm_br = colorMenu->insertItem(tr( "Black on Pink"));
    cm_rb = colorMenu->insertItem(tr( "Pink on Black"));
    cm_gy = colorMenu->insertItem(tr( "Green on Yellow"));
    cm_bm = colorMenu->insertItem(tr( "Blue on Magenta"));
    cm_mb = colorMenu->insertItem(tr( "Magenta on Blue"));
    cm_cw = colorMenu->insertItem(tr( "Cyan on White"));
    cm_wc = colorMenu->insertItem(tr( "White on Cyan"));
    cm_bb = colorMenu->insertItem(tr( "Blue on Black"));
    cm_ab = colorMenu->insertItem(tr( "Amber on Black"));
    cm_default = colorMenu->insertItem(tr("default"));

#ifdef QT_QWS_OPIE

    colorMenu->insertItem(tr( "Custom"));
#endif

    configMenu->insertItem(tr( "Colors") ,colorMenu);

    sessionList = new QPopupMenu(this);
    sessionList-> insertItem ( Resource::loadPixmap ( "konsole/Terminal" ), tr( "new session" ),  this,
                               SLOT(newSession()) );

    //  connect( fontList, SIGNAL( activated(int) ), this, SLOT( fontChanged(int) ));
    connect( configMenu, SIGNAL( activated(int) ), this, SLOT( configMenuSelected(int) ));
    connect( colorMenu, SIGNAL( activated(int) ), this, SLOT( colorMenuIsSelected(int) ));
    connect( tabMenu, SIGNAL( activated(int) ), this, SLOT( tabMenuSelected(int) ));
    connect( scrollMenu, SIGNAL(activated(int)),this,SLOT(scrollMenuSelected(int)));
    connect( editCommandListMenu,SIGNAL(activated(int)),this,SLOT(editCommandListMenuSelected(int)));
    connect( sessionList, SIGNAL(activated(int)), this, SLOT( sessionListSelected(int) ) );

    menuBar->insertItem( tr("View"), configMenu );
    menuBar->insertItem( tr("Fonts"), fontList );
    menuBar->insertItem( tr("Sessions"), sessionList );

    toolBar = new QToolBar( this );

    QAction *a;

    // Button Commands
    a = new QAction( tr("New"), Resource::loadPixmap( "konsole/Terminal" ), QString::null, 0, this, 0 );
    connect( a, SIGNAL( activated() ), this, SLOT( newSession() ) );
    a->addTo( toolBar );

    a = new QAction( tr("Full Screen"), Resource::loadPixmap( "fullscreen" ), QString::null, 0, this, 0 );
    connect( a, SIGNAL( activated() ), this, SLOT( toggleFullScreen() ) );
    a->addTo( toolBar );

    a = new QAction( tr("Zoom"), Resource::loadPixmap( "zoom" ), QString::null, 0, this, 0 );
    connect( a, SIGNAL( activated() ), this, SLOT( cycleZoom() ) );
    a->addTo( toolBar );


    /*
    a = new QAction( tr("Enter"), Resource::loadPixmap( "konsole/enter" ), QString::null, 0, this, 0 );
    connect( a, SIGNAL( activated() ), this, SLOT( hitEnter() ) ); a->addTo( toolBar );
    a = new QAction( tr("Space"), Resource::loadPixmap( "konsole/space" ), QString::null, 0, this, 0 );
    connect( a, SIGNAL( activated() ), this, SLOT( hitSpace() ) ); a->addTo( toolBar );
    a = new QAction( tr("Tab"), Resource::loadPixmap( "konsole/tab" ), QString::null, 0, this, 0 );
    connect( a, SIGNAL( activated() ), this, SLOT( hitTab() ) ); a->addTo( toolBar );
    */
    /*
    a = new QAction( tr("Up"), Resource::loadPixmap( "konsole/up" ), QString::null, 0, this, 0 );
    connect( a, SIGNAL( activated() ), this, SLOT( hitUp() ) ); a->addTo( toolBar );
    a = new QAction( tr("Down"), Resource::loadPixmap( "konsole/down" ), QString::null, 0, this, 0 );
    connect( a, SIGNAL( activated() ), this, SLOT( hitDown() ) ); a->addTo( toolBar );
    */
    a = new QAction( tr("Paste"), Resource::loadPixmap( "paste" ), QString::null, 0, this, 0 );
    connect( a, SIGNAL( activated() ), this, SLOT( hitPaste() ) );
    a->addTo( toolBar );

    secondToolBar = new QToolBar( this );
    secondToolBar->setHorizontalStretchable( TRUE );

    commonCombo = new QComboBox( secondToolBar );
    //  commonCombo->setMaximumWidth(236);

    ec_quick = editCommandListMenu->insertItem( tr( "Quick Edit" ) );
    if( listHidden)
    {
        secondToolBar->hide();
        editCommandListMenu->setItemEnabled(ec_quick ,FALSE);
    }
    ec_edit = editCommandListMenu->insertItem(tr(  "Edit..." ) );

    cfg.setGroup("Commands");
    commonCombo->setInsertionPolicy(QComboBox::AtCurrent);

    initCommandList();
    //   for (int i = 0; commonCmds[i] != NULL; i++) {
    //        commonCombo->insertItem( commonCmds[i], i );
    //        tmp = cfg.readEntry( QString::number(i),"");
    //        if(tmp != "")
    //            commonCombo->changeItem( tmp,i );
    //    }

    connect( commonCombo, SIGNAL( activated(int) ), this, SLOT( enterCommand(int) ));

    sm_none = scrollMenu->insertItem(tr( "None" ));
    sm_left = scrollMenu->insertItem(tr( "Left" ));
    sm_right = scrollMenu->insertItem(tr( "Right" ));
    //   scrollMenu->insertSeparator(4);
    //   scrollMenu->insertItem(tr( "Horizontal" ));

    configMenu->insertItem(tr( "ScrollBar" ),scrollMenu);

    configMenu->insertItem(tr( "History..." ), this, SLOT(historyDialog()));

    cm_wrap = configMenu->insertItem(tr( "Wrap" ));
    cfg.setGroup("ScrollBar");
    configMenu->setItemChecked(cm_wrap, cfg.readBoolEntry("HorzScroll",0));

    cm_beep = configMenu->insertItem(tr( "Use Beep" ));
    cfg.setGroup("Menubar");
    configMenu->setItemChecked(cm_beep, cfg.readBoolEntry("useBeep",0));

    fullscreen_msg = new QLabel(this);
    fullscreen_msg-> setAlignment ( AlignCenter | SingleLine );
    fullscreen_msg-> hide();
    fullscreen_msg-> setSizePolicy ( QSizePolicy ( QSizePolicy::Expanding, QSizePolicy::Expanding ));
    fullscreen_msg-> setAutoResize(true);
    fullscreen_msg-> setFrameStyle(QFrame::PopupPanel | QFrame::Raised);
    fullscreen_msg-> setText(tr("To exit fullscreen, tap here."));

    fullscreen_timer = new QTimer(this);
    connect(fullscreen_timer, SIGNAL(timeout()),
            this, SLOT(fullscreenTimeout()));
    show_fullscreen_msg = true;

    //scrollMenuSelected(-29);
    //   cfg.setGroup("ScrollBar");
    //   if(cfg.readBoolEntry("HorzScroll",0)) {
    //       if(cfg.readNumEntry("Position",2) == 0)
    //           te->setScrollbarLocation(1);
    //       else
    //           te->setScrollbarLocation(0);
    //       te->setScrollbarLocation( cfg.readNumEntry("Position",2));
    //       te->setWrapAt(120);
    //   }
    // create applications /////////////////////////////////////////////////////
    setCentralWidget(tab);

    // load keymaps ////////////////////////////////////////////////////////////
    KeyTrans::loadAll();
    for (int i = 0; i < KeyTrans::count(); i++)
    {
        KeyTrans* s = KeyTrans::find(i);
        assert( s );
    }

    se_pgm = _pgm;
    se_args = _args;

    cfg.setGroup("CommandLine");

    if (cfg.hasKey("shell_args"))
    {
        QStringList se_args_list = cfg.readListEntry("shell_args",'|');
        for(uint i = 0; i < se_args_list.count(); i++)
        {
            se_args.prepend(se_args_list[se_args_list.count() - i - 1].latin1());
        }
    }
    else
    {
        se_args.prepend("--login");
    }

    se_pgm = cfg.readEntry("shell_bin", QString(se_pgm));

    // this is the "documentation" for those who know to look
    if (! cfg.hasKey("shell_args"))
    {
        cfg.writeEntry("shell_args",QStringList::fromStrList(se_args),'|');
    }
    if (! cfg.hasKey("shell_bin"))
    {
        cfg.writeEntry("shell_bin",QString(se_pgm));
    }

    parseCommandLine();

    // read and apply default values ///////////////////////////////////////////
    resize(321, 321); // Dummy.
    QSize currentSize = size();
    if (currentSize != size())
        defaultSize = size();


    /* allows us to catch cancel/escape */
    reparent ( 0, WStyle_Customize | WStyle_NoBorder,
               QPoint ( 0, 0 ));
}

void Konsole::show()
{
    if ( !nsessions )
    {
        newSession();
    }
    QMainWindow::show();

}

void Konsole::initSession(const char*, QStrList &)
{
    QMainWindow::show();
}

Konsole::~Konsole()
{
    while (nsessions > 0)
    {
        doneSession(getTe(), 0);
    }
}

void
Konsole::historyDialog()
{
    QDialog *d = new QDialog ( this, "histdlg", true );
    //    d-> setCaption ( tr( "History" ));

    QBoxLayout *lay = new QVBoxLayout ( d, 4, 4  );

    QLabel *l = new QLabel ( tr( "History Lines:" ), d );
    lay-> addWidget ( l );

    Config cfg( "Konsole" );
    cfg.setGroup("History");
    int hist = cfg.readNumEntry("history_lines",300);
    int avg_line = cfg.readNumEntry("avg_line_length",60);

    QSpinBox *spin = new QSpinBox ( 1, 100000, 20, d );
    spin-> setValue ( hist );
    spin-> setWrapping ( true );
    spin-> setButtonSymbols ( QSpinBox::PlusMinus );
    lay-> addWidget ( spin );

    if ( d-> exec ( ) == QDialog::Accepted )
    {
        cfg.writeEntry("history_lines", spin->value());
        cfg.writeEntry("avg_line_length", avg_line);
        if (getTe() != NULL)
        {
            getTe()->currentSession->setHistory(true);
        }
    }

    delete d;
}


void Konsole::cycleZoom()
{
    TEWidget* te = getTe();
    QFont font = te->getVTFont();
    int size = font.pointSize();
    changeFontSize(1);
    font = te->getVTFont();
    if (font.pointSize() <= size)
    {
        do
        {
            font = te->getVTFont();
            size = font.pointSize();
            changeFontSize(-1);
            font = te->getVTFont();
        }
        while (font.pointSize() < size);
    }
}

void Konsole::changeFontSize(int delta)
{
    //  printf("delta font size %d\n", delta);
    TEWidget* te = getTe();
    QFont font = te->getVTFont();
    int size = font.pointSize();
    int closest = delta > 0? 10000 : -10000;
    int closest_font = -1;
    for(uint i = 0; i < fonts.count(); i++)
    {
        if (fonts.at(i)->getFont() == font)
        {
            if (delta > 0)
            {
                if (i+1 < fonts.count()
                        && fonts.at(i+1)->getFamilyNum() == fonts.at(i)->getFamilyNum())
                {
                    setFont(i+1);
                    printf("font %d\n", i+1);
                    return;
                }
            }
            else if (delta < 0)
            {
                if (i > 0
                        && fonts.at(i-1)->getFamilyNum() == fonts.at(i)->getFamilyNum())
                {
                    setFont(i-1);
                    printf("font %d\n", i-1);
                    return;
                }
            }
        }
        int fsize = fonts.at(i)->getSize();
        printf("%d size=%d fsize=%d closest=%d\n", i, size, fsize, closest);
        if ((delta > 0 && fsize > size && fsize < closest)
                || (delta < 0 && fsize < size && fsize > closest))
        {
            closest = fsize;
            closest_font = i;
        }
    }
    if (closest_font >= 0)
    {
        printf("font closest %d (%d)\n", closest_font, closest);
        setFont(closest_font);
    }
}

int Konsole::findFont(const QString& name, int size, bool exactMatch)
{
    for(uint i = 0; i < fonts.count(); i++)
    {
        if (fonts.at(i)->getName() == name
                && fonts.at(i)->getSize() == size)
        {
            return(i);
        }
    }
    if (exactMatch)
    {
        return(-1);
    }
    for(uint i = 0; i < fonts.count(); i++)
    {
        if (fonts.at(i)->getSize() == size)
        {
            return(i);
        }
    }
    return(-1);
}

void Konsole::setFont(int f)
{
    VTFont* font = fonts.at(f);
    if (font)
    {
        TEWidget* te = getTe();
        if (te != 0)
        {
            te->setVTFont(font->getFont());
        }
        cfont = f;

        int familyNum = font->getFamilyNum();
        int size = font->getSize();
        printf("familyNum = %d  size = %d  count=%d\n", familyNum, size,
               fontList->count());
        for(int i = 0; i < (int)fontList->count(); i++)
        {
            fontList->setItemChecked(i + 1000, i == familyNum);
        }
        for(int i = 0; i < (int)fonts.count(); i++)
        {
            fontList->setItemChecked(i, fonts.at(i)->getFamilyNum() == familyNum
                                     && fonts.at(i)->getSize() == size);
        }
        Config cfg( "Konsole" );
        cfg.setGroup("Font");
        QString ss = "Session"+ QString::number(tab->currentPageIndex()+1);
        if (tab->currentPageIndex() == 0)
        {
            cfg.writeEntry("FontName", fonts.at(cfont)->getFamily());
            cfg.writeEntry("FontSize", fonts.at(cfont)->getSize());
        }
        cfg.writeEntry("FontName"+ss, fonts.at(cfont)->getFamily());
        cfg.writeEntry("FontSize"+ss, fonts.at(cfont)->getSize());
    }
}

#if 0
void Konsole::fontChanged(int f)
{
    VTFont* font = fonts.at(f);
    if (font != 0)
    {
        for(uint i = 0; i < fonts.count(); i++)
        {
            fontList->setItemChecked(i, (i == (uint) f) ? TRUE : FALSE);
        }

        cfont = f;

        TEWidget* te = getTe();
        if (te != 0)
        {
            te->setVTFont(font->getFont());
        }
    }
}
#endif


void Konsole::enterCommand(int c)
{
    TEWidget* te = getTe();
    if (te != 0)
    {
        if(!commonCombo->editable())
        {
            QString text = commonCombo->text(c); //commonCmds[c];
            te->emitText(text);
        }
        else
        {
            changeCommand( commonCombo->text(c), c);
        }
    }
}

void Konsole::hitEnter()
{
    TEWidget* te = getTe();
    if (te != 0)
    {
        te->emitText(QString("\r"));
    }
}

void Konsole::hitSpace()
{
    TEWidget* te = getTe();
    if (te != 0)
    {
        te->emitText(QString(" "));
    }
}

void Konsole::hitTab()
{
    TEWidget* te = getTe();
    if (te != 0)
    {
        te->emitText(QString("\t"));
    }
}

void Konsole::hitPaste()
{
    TEWidget* te = getTe();
    if (te != 0)
    {
        te->pasteClipboard();
    }
}

void Konsole::hitUp()
{
    TEWidget* te = getTe();
    if (te != 0)
    {
        QKeyEvent ke( QKeyEvent::KeyPress, Qt::Key_Up, 0, 0);
        QApplication::sendEvent( te, &ke );
    }
}

void Konsole::hitDown()
{
    TEWidget* te = getTe();
    if (te != 0)
    {
        QKeyEvent ke( QKeyEvent::KeyPress, Qt::Key_Down, 0, 0);
        QApplication::sendEvent( te, &ke );
    }
}

/**
   This function calculates the size of the external widget
   needed for the internal widget to be
 */
QSize Konsole::calcSize(int columns, int lines)
{
    TEWidget* te = getTe();
    if (te != 0)
    {
        QSize size = te->calcSize(columns, lines);
        return size;
    }
    else
    {
        QSize size;
        return size;
    }
}

/**
    sets application window to a size based on columns X lines of the te
    guest widget. Call with (0,0) for setting default size.
*/

void Konsole::setColLin(int columns, int lines)
{
    qDebug("konsole::setColLin:: Columns %d", columns);

    if ((columns==0) || (lines==0))
    {
        if (defaultSize.isEmpty()) // not in config file : set default value
        {
            defaultSize = calcSize(80,24);
            // notifySize(24,80); // set menu items (strange arg order !)
        }
        resize(defaultSize);
    }
    else
    {
        resize(calcSize(columns, lines));
        // notifySize(lines,columns); // set menu items (strange arg order !)
    }
}

/*
void Konsole::setFont(int fontno)
{
  QFont f;
  if (fontno == 0)
    f = defaultFont = QFont( "Helvetica", 12  );
  else
  if (fonts[fontno][0] == '-')
    f.setRawName( fonts[fontno] );
  else
  {
    f.setFamily(fonts[fontno]);
    f.setRawMode( TRUE );
  }
  if ( !f.exactMatch() && fontno != 0)
  {
    QString msg = i18n("Font `%1' not found.\nCheck README.linux.console for help.").arg(fonts[fontno]);
    QMessageBox(this,  msg);
    return;
  }
  if (se) se->setFontNo(fontno);
  te->setVTFont(f);
  n_font = fontno;
}
*/

// --| color selection |-------------------------------------------------------

void Konsole::changeColumns(int /*columns*/)
{ //FIXME this seems to cause silliness when reset command is executed
    //     qDebug("change columns");
    //   TEWidget* te = getTe();
    //   if (te != 0) {
    //   setColLin(columns,te->Lines());
    //   te->update();
    //   }
}

//FIXME: If a child dies during session swap,
//       this routine might be called before
//       session swap is completed.

void Konsole::doneSession(TEWidget* te, int )
{
    //  TEWidget *te = NULL;
    //  if (sess->currentSession == tab->currentPage()) {
    //    printf("done current session\n");
    //    te = getTe();
    //  } else {
    //    int currentPage = tab->currentPageIndex();
    //    printf("done not current session\n");
    //    for(int i = 0; i < nsessions; i++) {
    //      tab->setCurrentPage(i);
    //      printf("find session %d tab page %x session %x\n",
    //         i, tab->currentPage(), sess->currentSession);
    //      if (tab->currentPage() == sess->currentSession) {
    //    printf("found session %d\n", i);
    //    te = tab->currentPage();
    //    break;
    //      }
    //    }
    //    tab->setCurrentPage(currentPage);
    //  }
    if (te != 0)
    {
        te->currentSession->setConnect(FALSE);
        tab->removeTab(te);
        delete te->currentSession;
        delete te;
        sessionList->removeItem(nsessions);
        nsessions--;
    }
    if (nsessions == 0)
    {
        close();
    }
}

void Konsole::changeTitle(TEWidget* te, const QString& newTitle )
{
    if (te == getTe())
    {
        setCaption( newTitle + " - " + tr( "Konsole " ) );
    }
}


void Konsole::newSession()
{
    if(nsessions < 15)
    {    // seems to be something weird about 16 tabs on the Zaurus.... memory?
        TEWidget* te = new TEWidget(tab);
        Config cfg( "Konsole" );
        cfg.setGroup("Menubar");

        // FIXME use more defaults from config file
        te->useBeep=cfg.readBoolEntry("useBeep",0);

        //  te->setBackgroundMode(PaletteBase); //we want transparent!!

        cfg.setGroup("Font");
        QString sn = "Session" + QString::number(nsessions+1);
        printf("read font session %s\n", sn.latin1());
        QString fontName = cfg.readEntry("FontName"+sn,
                                         cfg.readEntry("FontName",
                                                       fonts.at(cfont)->getFamily()));
        int fontSize = cfg.readNumEntry("FontSize"+sn,
                                        cfg.readNumEntry("FontSize",
                                                         fonts.at(cfont)->getSize()));
        cfont = findFont(fontName, fontSize, false);
        printf("lookup font %s size %d got %d\n", fontName.latin1(), fontSize, cfont);
        if (cfont < 0)
            cfont = 0;
        te->setVTFont(fonts.at(cfont)->getFont());

        tab->addTab(te);
        TESession* se = new TESession(this, te, se_pgm, se_args, "xterm");
        te->currentSession = se;
        connect( se, SIGNAL(done(TEWidget*,int)), this, SLOT(doneSession(TEWidget*,int)) );
        connect( se, SIGNAL(changeTitle(TEWidget*,const QString&)), this,
                 SLOT(changeTitle(TEWidget*,const QString&)) );
        connect(te, SIGNAL(changeFontSize(int)), this, SLOT(changeFontSize(int)));
        connect(te, SIGNAL(changeSession(int)), this, SLOT(changeSession(int)));
        connect(te, SIGNAL(newSession()), this, SLOT(newSession()));
        connect(te, SIGNAL(toggleFullScreen()), this, SLOT(toggleFullScreen()));
        connect(te, SIGNAL(setFullScreen(bool)), this, SLOT(setFullScreen(bool)));
        se->run();
        se->setConnect(TRUE);
        se->setHistory(b_scroll);
        nsessions++;
        sessionList->insertItem(QString::number(nsessions), nsessions);
        sessionListSelected(nsessions);
        doWrap();
        setColor(nsessions-1);
    }
}

TEWidget* Konsole::getTe()
{
    if (nsessions)
    {
        return (TEWidget *) tab->currentPage();
    }
    else
    {
        return 0;
    }
}

void Konsole::sessionListSelected(int id)
{
    if (id < 0)
    {
        return;
    }
    QString selected = sessionList->text(id);
    EKNumTabBar *tabBar = tab->getTabBar();

    int n = 0;
    for(int i = 0; n < tabBar->count(); i++)
    {
        if (tabBar->tab(i))
        {
            //      printf("selected = %s  tab %d = %s\n", selected.latin1(),
            //         i, tabBar->tab(i)->text().latin1());
            if (tabBar->tab(i)->text() == selected)
            {
                tab->setCurrentPage(i);
                break;
            }
            n++;
        }
    }
}


void Konsole::changeSession(int delta)
{
    printf("delta session %d\n", delta);
    QTabBar *tabBar = tab->getTabBar();
    int i = tabBar->tab(tabBar->currentTab())->text().toInt() - 1;
    i += delta;
    if (i < 0)
        i += tabBar->count();
    if (i >= tabBar->count())
        i -= tabBar->count();

    QString selected = QString::number(i+1);
    int n = 0;
    for(int i = 0; n < tabBar->count(); i++)
    {
        if (tabBar->tab(i))
        {
            printf("selected = %s  tab %d = %s\n", selected.latin1(),
                   i, tabBar->tab(i)->text().latin1());
            if (tabBar->tab(i)->text() == selected)
            {
                tab->setCurrentPage(i);
                break;
            }
            n++;
        }
    }
}

void Konsole::switchSession(QWidget* w)
{
    TEWidget* te = (TEWidget *) w;
    QFont teFnt = te->getVTFont();
    int familyNum = -1;

    for(uint i = 0; i < fonts.count(); i++)
    {
        VTFont *fnt = fonts.at(i);
        bool cf = fnt->getFont() == teFnt;
        fontList->setItemChecked(i, cf);
        if (cf)
        {
            cfont = i;
            familyNum = fnt->getFamilyNum();
        }
    }
    for(int i = 0; i < (int)fontList->count(); i++)
    {
        fontList->setItemChecked(i + 1000, i == familyNum);
    }
    if (! te->currentSession->Title().isEmpty() )
    {
        setCaption( te->currentSession->Title() + " - " + tr( "Konsole" ) );
    }
    else
    {
        setCaption( tr( "Konsole" ) );
    }
    //  colorMenuSelected(te->color_menu_item);
}


void Konsole::toggleFullScreen()
{
    setFullScreen(! fullscreen);
}

void Konsole::setFullScreen ( bool b )
{
    static QSize normalsize;
    static bool listHidden;

    if (b == fullscreen)
    {
        return;
    }

    fullscreen = b;

    if ( b )
    {
        if ( !normalsize. isValid ( ))
        {
            normalsize = size ( );
        }

        setFixedSize ( qApp-> desktop ( )-> size ( ));
        showNormal ( );
        reparent ( 0, WStyle_Customize | WStyle_NoBorder,
                   QPoint ( 0, 0 ));
        showFullScreen ( );

        menuToolBar->hide();
        toolBar->hide();
        listHidden = secondToolBar->isHidden();
        secondToolBar->hide();
        //    commonCombo->hide();
        tab->getTabBar()->hide();
        tab->setMargin(tab->margin());

        if (show_fullscreen_msg)
        {
            fullscreen_msg-> move(tab->x() + tab->width()/2 - fullscreen_msg->width()/2,
                                  qApp->desktop()->height()/16 - fullscreen_msg->height()/2);
            fullscreen_msg->show();
            fullscreen_timer->start(3000, true);
            show_fullscreen_msg = false;
        }
    }
    else
    {
        showNormal ( );
        reparent ( 0, WStyle_Customize, QPoint ( 0, 0 ));
        resize ( normalsize );
        showMaximized ( );
        normalsize = QSize ( );

        menuToolBar->show();
        toolBar->show();
        if(! listHidden)
        {
            secondToolBar->show();
        }
        //    commonCombo->show();
        menuToolBar->show();
        if (tabPos != tm_hidden)
        {
            tab->getTabBar()->show();
        }
    }
    tab->setMargin(tab->margin());  // cause setup to run
}


void Konsole::fullscreenTimeout()
{
    fullscreen_msg->hide();
}

void Konsole::colorMenuIsSelected(int iD)
{
    fromMenu = TRUE;
    colorMenuSelected(iD);
}

/// -------------------------------   some new stuff by L.J. Potter


void Konsole::colorMenuSelected(int iD)
{
    // this is NOT pretty, elegant or anything else besides functional
    //       QString temp;
    //      qDebug( temp.sprintf("colormenu %d", iD));

    TEWidget* te = getTe();
    Config cfg( "Konsole" );
    cfg.setGroup("Colors");

    ColorEntry m_table[TABLE_COLORS];
    const ColorEntry * defaultCt=te->getdefaultColorTable();

    int i;

    //    te->color_menu_item = iD;

    colorMenu->setItemChecked(cm_ab,FALSE);
    colorMenu->setItemChecked(cm_bb,FALSE);
    colorMenu->setItemChecked(cm_wc,FALSE);
    colorMenu->setItemChecked(cm_cw,FALSE);
    colorMenu->setItemChecked(cm_mb,FALSE);
    colorMenu->setItemChecked(cm_bm,FALSE);
    colorMenu->setItemChecked(cm_gy,FALSE);
    colorMenu->setItemChecked(cm_rb,FALSE);
    colorMenu->setItemChecked(cm_br,FALSE);
    colorMenu->setItemChecked(cm_wb,FALSE);
    colorMenu->setItemChecked(cm_bw,FALSE);
    colorMenu->setItemChecked(cm_gb,FALSE);

    if(iD==cm_default)
    { // default default
        printf("default colors\n");
        for (i = 0; i < TABLE_COLORS; i++)
        {
            m_table[i].color = defaultCt[i].color;
            if(i==1 || i == 11)
                m_table[i].transparent=1;
            colorMenu->setItemChecked(cm_default,TRUE);
        }
        te->setColorTable(m_table);
    }
    if(iD==cm_gb)
    { // green black
        foreground.setRgb(100,255,100); // (0x18,255,0x18);
        background.setRgb(0x00,0x00,0x00);
        colorMenu->setItemChecked(cm_gb,TRUE);
    }
    if(iD==cm_bw)
    { // black white
        foreground.setRgb(0x00,0x00,0x00);
        background.setRgb(0xFF,0xFF,0xFF);
        colorMenu->setItemChecked(cm_bw,TRUE);
    }
    if(iD==cm_wb)
    { // white black
        foreground.setRgb(0xFF,0xFF,0xFF);
        background.setRgb(0x00,0x00,0x00);
        colorMenu->setItemChecked(cm_wb,TRUE);
    }
    if(iD==cm_br)
    {// Black, Red
        foreground.setRgb(0x00,0x00,0x00);
        background.setRgb(255,85,85); //(0xB2,0x18,0x18);
        colorMenu->setItemChecked(cm_br,TRUE);
    }
    if(iD==cm_rb)
    {// Red, Black
        foreground.setRgb(255,85,85);
        background.setRgb(0x00,0x00,0x00);
        colorMenu->setItemChecked(cm_rb,TRUE);
    }
    if(iD==cm_gy)
    {// Green, Yellow - is ugly
        //            foreground.setRgb(0x18,0xB2,0x18);
        foreground.setRgb(15,115,0);
        //            background.setRgb(0xB2,0x68,0x18);
        background.setRgb(255,255,0);
        colorMenu->setItemChecked(cm_gy,TRUE);
    }
    if(iD==cm_bm)
    {// Blue,  Magenta
        foreground.setRgb(3,24,132);
        background.setRgb(225,2,255);
        colorMenu->setItemChecked(cm_bm,TRUE);
    }
    if(iD==cm_mb)
    {// Magenta, Blue
        foreground.setRgb(225,2,255);
        background.setRgb(3,24,132);
        colorMenu->setItemChecked(cm_mb,TRUE);
    }
    if(iD==cm_cw)
    {// Cyan,  White
        foreground.setRgb(8,91,129);
        background.setRgb(0xFF,0xFF,0xFF);
        colorMenu->setItemChecked(cm_cw,TRUE);
    }
    if(iD==cm_wc)
    {// White, Cyan
        background.setRgb(8,91,129);
        foreground.setRgb(0xFF,0xFF,0xFF);
        colorMenu->setItemChecked(cm_wc,TRUE);
    }
    if(iD==cm_bb)
    {// Black, Blue
        background.setRgb(0x00,0x00,0x00);
        foreground.setRgb(127,147,225);
        colorMenu->setItemChecked(cm_bb,TRUE);
    }
    if(iD==cm_ab)
    {// Black, Gold
        background.setRgb(0x00,0x00,0x00);
        foreground.setRgb(255,215,105);
        colorMenu->setItemChecked(cm_ab,TRUE);
    }
#ifdef QT_QWS_OPIE
    if(iD==-19)
    {
        // Custom
        qDebug("do custom");
        if(fromMenu)
        {
            Opie::OColorPopupMenu* penColorPopupMenu = new Opie::OColorPopupMenu(Qt::black, this, "foreground color");
            connect(penColorPopupMenu, SIGNAL(colorSelected(const QColor&)), this,
                    SLOT(changeForegroundColor(const QColor&)));
            penColorPopupMenu->exec();
        }
        if(!fromMenu)
        {
            foreground.setNamedColor(cfg.readEntry("foreground",""));
            background.setNamedColor(cfg.readEntry("background",""));
        }
        fromMenu=FALSE;
        colorMenu->setItemChecked(-19,TRUE);
    }
#endif

    lastSelectedMenu = iD;

    setColors(foreground, background);

    QTabBar *tabBar = tab->getTabBar();
    QString ss = QString("Session%1").arg(tabBar->currentTab());
    //    printf("current tab = %d\n", tabBar->currentTab());

    if (tabBar->currentTab() == 0)
    {
        cfg.writeEntry("foregroundRed",QString::number(foreground.red()));
        cfg.writeEntry("foregroundGreen",QString::number(foreground.green()));
        cfg.writeEntry("foregroundBlue",QString::number(foreground.blue()));
        cfg.writeEntry("backgroundRed",QString::number(background.red()));
        cfg.writeEntry("backgroundGreen",QString::number(background.green()));
        cfg.writeEntry("backgroundBlue",QString::number(background.blue()));
    }
    cfg.writeEntry("foregroundRed"+ss,QString::number(foreground.red()));
    cfg.writeEntry("foregroundGreen"+ss,QString::number(foreground.green()));
    cfg.writeEntry("foregroundBlue"+ss,QString::number(foreground.blue()));
    cfg.writeEntry("backgroundRed"+ss,QString::number(background.red()));
    cfg.writeEntry("backgroundGreen"+ss,QString::number(background.green()));
    cfg.writeEntry("backgroundBlue"+ss,QString::number(background.blue()));

    update();
}

void Konsole::setColors(QColor foreground, QColor background)
{
    int i;
    ColorEntry m_table[TABLE_COLORS];
    TEWidget* te = getTe();
    const ColorEntry * defaultCt=te->getdefaultColorTable();

    for (i = 0; i < TABLE_COLORS; i++)
    {
        if(i==0 || i == 10)
        {
            m_table[i].color = foreground;
        }
        else if(i==1 || i == 11)
        {
            m_table[i].color = background;
            m_table[i].transparent=0;
        }
        else
            m_table[i].color = defaultCt[i].color;
    }
    te->setColorTable(m_table);
}

void Konsole::tabMenuSelected(int id)
{
    Config cfg( "Konsole" );
    cfg.setGroup("Tabs");
    tabMenu->setItemChecked(tabPos, false);
    if (id == tm_bottom)
    {
        printf("set bottom tab\n");
        tab->getTabBar()->show();
        tab->setTabPosition(QTabWidget::Bottom);
        tab->getTabBar()->show();
        cfg.writeEntry("Position","Bottom");
    }
    else if (id == tm_top)
    {
        printf("set top tab\n");
        tab->getTabBar()->show();
        tab->setTabPosition(QTabWidget::Bottom);
        tab->setTabPosition(QTabWidget::Top);
        tab->getTabBar()->show();
        cfg.writeEntry("Position","Top");
    }
    else if (id == tm_hidden)
    {
        tab->getTabBar()->hide();
        tab->setMargin(tab->margin());
        cfg.writeEntry("Position","Hidden");
    }
    tabMenu->setItemChecked(id, true);
    tabPos = id;
}


void Konsole::configMenuSelected(int iD)
{
    //      QString temp;
    //      qDebug( temp.sprintf("configmenu %d",iD));

    TEWidget* te = getTe();
    Config cfg( "Konsole" );
    cfg.setGroup("Menubar");
    if(iD == cm_wrap)
    {
        cfg.setGroup("ScrollBar");
        bool b=cfg.readBoolEntry("HorzScroll",0);
        b=!b;
        cfg.writeEntry("HorzScroll", b );
        cfg.write();
        doWrap();
        if(cfg.readNumEntry("Position",2) == 0)
        {
            te->setScrollbarLocation(1);
        }
        else
        {
            te->setScrollbarLocation(0);
        }
        te->setScrollbarLocation( cfg.readNumEntry("Position",2));
    }
    if(iD ==  cm_beep)
    {
        cfg.setGroup("Menubar");
        bool b=cfg.readBoolEntry("useBeep",0);
        b=!b;
        cfg.writeEntry("useBeep", b );
        cfg.write();
        configMenu->setItemChecked(cm_beep,b);
        te->useBeep=b;
    }
}

void Konsole::changeCommand(const QString &text, int c)
{
    Config cfg( "Konsole" );
    cfg.setGroup("Commands");
    if(commonCmds[c] != text)
    {
        cfg.writeEntry(QString::number(c),text);
        commonCombo->clearEdit();
        commonCombo->setCurrentItem(c);
    }
}

void Konsole::setColor(int sess)
{
    Config cfg( "Konsole" );
    cfg.setGroup("Colors");
    QColor foreground, background;
    QString ss = QString("Session") + QString::number(sess);
    foreground.setRgb(cfg.readNumEntry("foregroundRed"+ss,
                                       cfg.readNumEntry("foregroundRed",0xff)),
                      cfg.readNumEntry("foregroundGreen"+ss,
                                       cfg.readNumEntry("foregroundGreen",0xff)),
                      cfg.readNumEntry("foregroundBlue"+ss,
                                       cfg.readNumEntry("foregroundBlue",0xff)));
    background.setRgb(cfg.readNumEntry("backgroundRed"+ss,
                                       cfg.readNumEntry("backgroundRed",0)),
                      cfg.readNumEntry("backgroundGreen"+ss,
                                       cfg.readNumEntry("backgroundGreen",0)),
                      cfg.readNumEntry("backgroundBlue"+ss,
                                       cfg.readNumEntry("backgroundBlue",0)));
    setColors(foreground, background);
}

void Konsole::scrollMenuSelected(int index)
{
    //    qDebug( "scrollbar menu %d",index);

    TEWidget* te = getTe();
    Config cfg( "Konsole" );
    cfg.setGroup("ScrollBar");

    if(index == sm_none)
    {
        te->setScrollbarLocation(0);
        cfg.writeEntry("Position",0);
    }
    else if(index == sm_left)
    {
        te->setScrollbarLocation(1);
        cfg.writeEntry("Position",1);
    }
    else if(index == sm_right)
    {
        te->setScrollbarLocation(2);
        cfg.writeEntry("Position",2);
    }
    scrollMenu->setItemChecked(sm_none, index == sm_none);
    scrollMenu->setItemChecked(sm_left, index == sm_left);
    scrollMenu->setItemChecked(sm_right, index == sm_right);
}

//       case -29: {
//           bool b=cfg.readBoolEntry("HorzScroll",0);
//           cfg.writeEntry("HorzScroll", !b );
//           cfg.write();
//           if(cfg.readNumEntry("Position",2) == 0) {
//               te->setScrollbarLocation(1);
//           te->setWrapAt(0);
//           } else {
//               te->setScrollbarLocation(0);
//           te->setWrapAt(120);
//           }
//           te->setScrollbarLocation( cfg.readNumEntry("Position",2));
//       }
//         break;

void Konsole::editCommandListMenuSelected(int iD)
{
    //      QString temp;
    //      qDebug( temp.sprintf("edit command list %d",iD));

    // FIXME: more cleanup needed here


    TEWidget* te = getTe();
    Config cfg( "Konsole" );
    cfg.setGroup("Menubar");
    if( iD  == ec_cmdlist)
    {
        if(!secondToolBar->isHidden())
        {
            secondToolBar->hide();
            configMenu->changeItem( iD,tr( "Show Command List" ));
            cfg.writeEntry("Hidden","TRUE");
            configMenu->setItemEnabled(ec_edit ,FALSE);
            configMenu->setItemEnabled(ec_quick ,FALSE);
        }
        else
        {
            secondToolBar->show();
            configMenu->changeItem( iD,tr( "Hide Command List" ));
            cfg.writeEntry("Hidden","FALSE");
            configMenu->setItemEnabled(ec_edit ,TRUE);
            configMenu->setItemEnabled(ec_quick ,TRUE);

            if(cfg.readEntry("EditEnabled","FALSE")=="TRUE")
            {
                configMenu->setItemChecked(ec_edit,TRUE);
                commonCombo->setEditable( TRUE );
            }
            else
            {
                configMenu->setItemChecked(ec_edit,FALSE);
                commonCombo->setEditable( FALSE );
            }
        }
    }
    if( iD  == ec_quick)
    {
        cfg.setGroup("Commands");
        //        qDebug("enableCommandEdit");
        if( !configMenu->isItemChecked(iD) )
        {
            commonCombo->setEditable( TRUE );
            configMenu->setItemChecked(iD,TRUE);
            commonCombo->setCurrentItem(0);
            cfg.writeEntry("EditEnabled","TRUE");
        }
        else
        {
            commonCombo->setEditable( FALSE );
            configMenu->setItemChecked(iD,FALSE);
            cfg.writeEntry("EditEnabled","FALSE");
            commonCombo->setFocusPolicy(QWidget::NoFocus);
            te->setFocus();
        }
    }
    if(iD == ec_edit)
    {
        // "edit commands"
        CommandEditDialog *m = new CommandEditDialog(this);
        connect(m,SIGNAL(commandsEdited()),this,SLOT(initCommandList()));
        m->showMaximized();
    }

}

// $QPEDIR/bin/qcop QPE/Application/embeddedkonsole 'setDocument(QString)' 'ssh -V'
void Konsole::setDocument( const QString &cmd)
{
    newSession();
    TEWidget* te = getTe();
    if(cmd.find("-e", 0, TRUE) != -1)
    {
        QString cmd2;
        cmd2=cmd.right(cmd.length()-3)+" &";
        system(cmd2.latin1());
        if(startUp <= 1 && nsessions < 2)
        {
            doneSession(getTe(), 0);
            exit(0);
        }
        else
            doneSession(getTe(), 0);
    }
    else
    {
        if (te != 0)
        {
            te->emitText(cmd+"\r");
        }
    }
    startUp++;
}


// what is the point of this when you can just
// run commands by using the shell directly??
void Konsole::parseCommandLine()
{
    QString cmd;
    //    newSession();
    for (int i=1;i< qApp->argc();i++)
    {
        if( QString(qApp->argv()[i]) == "-e")
        {
            i++;
            for ( int j=i;j< qApp->argc();j++)
            {
                cmd+=QString(qApp->argv()[j])+" ";
            }
            cmd.stripWhiteSpace();
            system(cmd.latin1());
            exit(0);//close();
        } // end -e switch
    }
    startUp++;
}

void Konsole::changeForegroundColor(const QColor &color)
{
    Config cfg( "Konsole" );
    cfg.setGroup("Colors");
    int r, g, b;
    color.rgb(&r,&g,&b);
    foreground.setRgb(r,g,b);

    cfg.writeEntry("foreground",color.name());
    qDebug("foreground "+color.name());
    cfg.write();

    qDebug("do other dialog");
#ifdef QT_QWS_OPIE

    Opie::OColorPopupMenu* penColorPopupMenu2 = new Opie::OColorPopupMenu(Qt::black, this,"background color");
    connect(penColorPopupMenu2, SIGNAL(colorSelected(const QColor&)), this,
            SLOT(changeBackgroundColor(const QColor&)));
    penColorPopupMenu2->exec();
#endif
}

void Konsole::changeBackgroundColor(const QColor &color)
{

    qDebug("Change background");
    Config cfg( "Konsole" );
    cfg.setGroup("Colors");
    int r, g, b;
    color.rgb(&r,&g,&b);
    background.setRgb(r,g,b);
    cfg.writeEntry("background",color.name());
    qDebug("background "+color.name());
    cfg.write();
}

void Konsole::doWrap()
{
    Config cfg( "Konsole" );
    cfg.setGroup("ScrollBar");
    TEWidget* te = getTe();
    if( !cfg.readBoolEntry("HorzScroll",0))
    {
        te->setWrapAt(0);
        configMenu->setItemChecked( cm_wrap,TRUE);
    }
    else
    {
        //        te->setWrapAt(90);
        te->setWrapAt(120);
        configMenu->setItemChecked( cm_wrap,FALSE);
    }
}
