
#include "portable.h"

#if defined( KDE2_PORT )
#include <kpacman.h>
#include <kpacman.moc>
#include <kcolordlg.h>
#elif defined( QPE_PORT )
#include <qmenubar.h>
#include "config.h"
#include <qapplication.h>
#include "kpacman.h"
#endif

#include <qkeycode.h>
#include <qcolor.h>
#include <qstring.h>
#include <qpopmenu.h>
#include <qmsgbox.h>

Kpacman::Kpacman(QWidget *parent, const char *name)
        : KTMainWindow(parent, name)
{
    schemesPopup = new QList<QPopupMenu>;
    schemesPopup->setAutoDelete(TRUE);

    menu();

	m_view = new QWidget( this, "m_view" );
    m_view->setBackgroundColor( black );
	m_layout = new QGridLayout( m_view );
	m_layout->setMargin( 7 );

	view = new KpacmanWidget( this, QString(name)+"widget");
	m_layout->addWidget( view, 0, 0 );

    setCaption( "KPacman" );

    view->referee->setFocus();

    connect(view->referee, SIGNAL(setScore(int, int)),
            view->score, SLOT(setScore(int, int)));
    connect(view->referee, SIGNAL(setPoints(int)),
            view->score, SLOT(set(int)));
    connect(view->referee, SIGNAL(setLifes(int)),
            view->status, SLOT(setLifes(int)));
    connect(view->referee, SIGNAL(setLevel(int)),
            view->status, SLOT(setLevel(int)));
    connect(view->referee, SIGNAL(forcedHallOfFame(bool)),
            this, SLOT(forcedHallOfFame(bool)));
    connect(view->referee, SIGNAL(togglePaused()), this, SLOT(togglePaused()));
    connect(view->referee, SIGNAL(toggleNew()), this, SLOT(toggleNew()));

    connect(view->score, SIGNAL(toggleNew()), this, SLOT(toggleNew()));
    connect(view->score, SIGNAL(forcedHallOfFame(bool)),
            this, SLOT(forcedHallOfFame(bool)));

    APP_CONFIG_BEGIN( cfg );
    focusOutPause = !cfg->readBoolEntry("FocusOutPause", TRUE);
    focusInContinue = !cfg->readBoolEntry("FocusInContinue", TRUE);
    hideMouseCursor = !cfg->readBoolEntry("HideMouseCursor", TRUE);
    APP_CONFIG_END( cfg );

    toggleFocusOutPause();
    toggleFocusInContinue();
    toggleHideMouseCursor();

    setCentralWidget( m_view );
}

Kpacman::~Kpacman()
{
    APP_CONFIG_BEGIN( cfg );
    cfg->writeEntry("FocusOutPause", focusOutPause);
    cfg->writeEntry("FocusInContinue", focusInContinue);
    cfg->writeEntry("HideMouseCursor", hideMouseCursor);
    APP_CONFIG_END( cfg );
    delete _menuBar;
}

void Kpacman::menu()
{
    gamePopup = new QPopupMenu();
    CHECK_PTR( gamePopup );
    newID = gamePopup->insertItem(tr("&New"), this, SLOT(newKpacman()),Key_F2);
    pauseID = gamePopup->insertItem(tr("&Pause"),
                                    this, SLOT(pauseKpacman()), Key_F3);
    hofID = gamePopup->insertItem(tr("&Hall of fame"),
                                    this, SLOT(toggleHallOfFame()), Key_F4);
    gamePopup->insertSeparator();
    gamePopup->insertItem(tr("&Quit"), this, SLOT(quitKpacman()), CTRL+Key_Q);
    gamePopup->setCheckable(TRUE);

    optionsPopup = new QPopupMenu();
    CHECK_PTR(optionsPopup);

    modesPopup = new QPopupMenu();
    CHECK_PTR(modesPopup);

    hideMouseCursorID = optionsPopup->insertItem(tr("&Hide Mousecursor"),
                                                 this, SLOT(toggleHideMouseCursor()),
                                                 CTRL+Key_H);
    optionsPopup->insertSeparator();

    if (lookupSchemes() > 0) {
        optionsPopup->insertItem(tr("&Select graphic scheme"),  modesPopup);
        optionsPopup->insertSeparator();
    }

    focusOutPauseID = optionsPopup->insertItem(tr("&Pause in Background"),
                                               this, SLOT(toggleFocusOutPause()));
    focusInContinueID = optionsPopup->insertItem(tr("&Continue in Foreground"),
                                              this, SLOT(toggleFocusInContinue()));
    optionsPopup->insertSeparator();

    optionsPopup->insertItem(tr("Change &keys..."), this, SLOT(confKeys()));

#ifndef QWS
    QString aboutText = tr("@PACKAGE@ - @VERSION@\n\n"
                             "Joerg Thoennissen (joe@dsite.de)\n\n"
                             "A pacman game for the KDE Desktop\n\n"
                             "The program based on the source of ksnake\n"
                             "by Michel Filippi (mfilippi@sade.rhein-main.de).\n"
                             "The design was strongly influenced by the pacman\n"
                             "(c) 1980 MIDWAY MFG.CO.\n\n"
                             "I like to thank my girlfriend Elke Krueers for\n"
                             "the last 10 years of her friendship.\n");
    aboutText.replace(QRegExp("@PACKAGE@"), PACKAGE);
    aboutText.replace(QRegExp("@VERSION@"), VERSION);
    QPopupMenu *helpPopup = helpMenu(aboutText, FALSE);
#endif

    //_menuBar = new KMenuBar(this);
    //CHECK_PTR( _menuBar );
    //_menuBar->insertItem(tr("&Game"), gamePopup);
    //_menuBar->insertItem(tr("&Options"), optionsPopup);
    //_menuBar->insertSeparator();
#ifndef QWS
    _menuBar->insertItem(tr("&Help"), helpPopup);
#endif
}

int Kpacman::lookupSchemes()
{
    APP_CONFIG_BEGIN( cfg );
    int ModeCount = cfg->readNumEntry("ModeCount", -1);
    int Mode = cfg->readNumEntry("Mode", -1);
    int SchemeCount = cfg->readNumEntry("SchemeCount");
    int Scheme = cfg->readNumEntry("Scheme", -1);

    if (SchemeCount == 0 || Scheme == -1) {
        QMessageBox::warning(this, tr("Configuration Error"),
                                   tr("There are no schemes defined,\n"
                                        "or no scheme is selected."));
        APP_CONFIG_END( cfg );
        return 0;
    }

    connect(modesPopup, SIGNAL(activated(int)), this, SLOT(schemeChecked(int)));
    modeID.resize(ModeCount > 0 ? ModeCount : 0);

    if (!schemesPopup->isEmpty())
        schemesPopup->clear();

    SAVE_CONFIG_GROUP( cfg, oldgroup );

    QString ModeGroup;
    QString ModeName;

    for (int m = 0; m < ModeCount; m++) {
        ModeGroup.sprintf("Mode %d", m);
        cfg->setGroup(ModeGroup);

        ModeName = cfg->readEntry("Description", ModeGroup);

        QPopupMenu *p = new QPopupMenu;
        p->setCheckable(TRUE);
        connect(p, SIGNAL(activated(int)), this, SLOT(schemeChecked(int)));
        schemesPopup->append(p);

        modeID[m] = modesPopup->insertItem(ModeName, schemesPopup->at(m));
        modesPopup->setItemEnabled(modeID[m], FALSE);
        modesPopup->setItemChecked(modeID[m], m == Mode);
    }

    schemeID.resize(SchemeCount);
    schemeMode.resize(SchemeCount);

    QString SchemeGroup;
    QString SchemeName;
    int SchemeMode;

    for (int i = 0; i < SchemeCount; i++) {
        SchemeGroup.sprintf("Scheme %d", i);
        cfg->setGroup(SchemeGroup);

        SchemeName = cfg->readEntry("Description", SchemeGroup);
        SchemeMode = cfg->readNumEntry("Mode", -1);

        schemeMode[i] = SchemeMode;
        if (SchemeMode == -1) {
            schemeID[i] = modesPopup->insertItem(SchemeName);
            modesPopup->setItemChecked(schemeID[i], i == Scheme);
        } else {
            schemeID[i] = schemesPopup->at(SchemeMode)->insertItem(SchemeName);
            schemesPopup->at(SchemeMode)->
                setItemChecked(schemeID[i], i == Scheme);
            modesPopup->setItemEnabled(modeID[SchemeMode], TRUE);
        }
    }

    RESTORE_CONFIG_GROUP( cfg, oldgroup );

    APP_CONFIG_END( cfg );
    return SchemeCount;
}

void Kpacman::quitKpacman()
{
    APP_QUIT();
}

void Kpacman::newKpacman()
{
    if (!gamePopup->isItemEnabled(hofID))
        gamePopup->setItemEnabled(hofID, TRUE);

    if (gamePopup->isItemChecked(hofID))
        toggleHallOfFame();

    if (gamePopup->isItemChecked(pauseID))
        pauseKpacman();

    view->referee->play();
}

void Kpacman::pauseKpacman()
{
    view->referee->pause();
    view->score->setPause(gamePopup->isItemChecked(pauseID));
}

void Kpacman::toggleHallOfFame()
{
    gamePopup->setItemChecked(hofID, !gamePopup->isItemChecked(hofID));
    view->referee->toggleHallOfFame();

    if (gamePopup->isItemChecked(hofID)) {
        view->referee->lower();
        view->status->lower();
    } else {
        view->status->raise();
        view->referee->raise();
        view->referee->setFocus();
    }
}

/*
 * Disable or enable the "Hall of fame"-menuitem if the referee says so.
 * This is done, to disable turning off the "hall of fame"-display, in the automated
 * sequence of displaying the introduction, the demonstration (or playing) and the
 * hall of fame.
 * If on == TRUE then also lower the referee and the status widgets.
 */
void Kpacman::forcedHallOfFame(bool on)
{
    if (!on && !gamePopup->isItemChecked(hofID))
        return;

    gamePopup->setItemEnabled(hofID, !on);
    gamePopup->setItemChecked(hofID, on);

    view->referee->toggleHallOfFame();
    if (on) {
        view->referee->lower();
        view->status->lower();
    } else {
        view->status->raise();
        view->referee->raise();
        view->referee->setFocus();
        view->referee->intro();
    }
}

void Kpacman::togglePaused()
{
    static bool checked = FALSE;
    checked = !checked;
    gamePopup->setItemChecked( pauseID, checked );
    view->score->setPause(gamePopup->isItemChecked(pauseID));
}

/*
 * This disables the "New Game" menuitem to prevent interruptions of the current
 * play.
 */
void Kpacman::toggleNew()
{
    gamePopup->setItemEnabled(newID, !gamePopup->isItemEnabled(newID));
}

void Kpacman::toggleHideMouseCursor()
{
    hideMouseCursor = !hideMouseCursor;
    optionsPopup->setItemChecked(hideMouseCursorID, hideMouseCursor);
    if (hideMouseCursor)
        view->setCursor(blankCursor);
    else
        view->setCursor(arrowCursor);
}

void Kpacman::toggleFocusOutPause()
{
    focusOutPause = !focusOutPause;
    optionsPopup->setItemChecked(focusOutPauseID, focusOutPause);
    view->referee->setFocusOutPause(focusOutPause);
}

void Kpacman::toggleFocusInContinue()
{
    focusInContinue = !focusInContinue;
    optionsPopup->setItemChecked(focusInContinueID, focusInContinue);
    view->referee->setFocusInContinue(focusInContinue);
}

void Kpacman::confKeys()
{
    Keys *keys = new Keys();
    if (keys->exec() == QDialog::Accepted) {
        view->referee->initKeys();
        view->score->initKeys();
    }
    delete keys;
}

void Kpacman::schemeChecked(int id)
{
    int mode = 0, scheme = -1;

    for (uint s = 0; s < schemeID.size(); s++) {
        if (schemeID[s] == id) {
            scheme = s;
            mode = schemeMode[s];
        }
        if (schemeMode[s] == -1) {
						modesPopup->setItemChecked(schemeID[s], schemeID[s] == id);
				} else {
						modesPopup->setItemChecked(modeID[schemeMode[s]], schemeMode[s] == mode);
						schemesPopup->at(schemeMode[s])->setItemChecked(schemeID[s], schemeID[s] == id);
				}
		}

    APP_CONFIG_BEGIN( cfg );
    cfg->writeEntry("Scheme", scheme);
    cfg->writeEntry("Mode", mode);
    APP_CONFIG_END( cfg );

    view->setScheme(scheme, mode);
    view->updateGeometry();
    updateGeometry();
    update();
    repaint(TRUE);
    show();
}
