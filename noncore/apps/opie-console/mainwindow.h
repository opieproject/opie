#ifndef OPIE_MAIN_WINDOW_H
#define OPIE_MAIN_WINDOW_H

#include <qmainwindow.h>
#include <qlist.h>

#include "session.h"

/**
 * this is the MainWindow of the new opie console
 * it's also the dispatcher between the different
 * actions supported by the gui
 */
class QToolBar;
class QToolButton;
class QMenuBar;
class QAction;
class MetaFactory;
class TabWidget;
class ProfileManager;
class Profile;
class FunctionKeyboard;
class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    MainWindow();
    ~MainWindow();

    /**
     * our factory to generate IOLayer and so on
     *
     */
    MetaFactory* factory();

    /**
     * A session contains a QWidget*,
     * an IOLayer* and some infos for us
     */
    Session* currentSession();

    /**
     * the session list
     */
    QList<Session> sessions();

    /**
     *
     */
    ProfileManager* manager();
    TabWidget* tabWidget();

private slots:
    void slotNew();
    void slotConnect();
    void slotDisconnect();
    void slotTerminate();
    void slotConfigure();
    void slotClose();
    void slotProfile(int);
	void slotTransfer();
    void slotOpenKeb(bool);

private:
    void initUI();
    void populateProfiles();
    void create( const Profile& );
    /**
     * the current session
     */
    Session* m_curSession;

    /**
     * the session list
     */
    QList<Session> m_sessions;

    /**
     * the metafactory
     */
    MetaFactory* m_factory;
    ProfileManager* m_manager;

    TabWidget* m_consoleWindow;
    QToolBar* m_tool;
    QToolBar* m_icons;
    QToolBar* m_keyBar;
    QMenuBar* m_bar;
    QPopupMenu* m_console;
    QPopupMenu* m_settings;
    QPopupMenu* m_sessionsPop;
    QAction* m_connect;
    QAction* m_disconnect;
    QAction* m_terminate;
	QAction* m_transfer;
    QAction* m_setProfiles;
    QAction* m_openKeys;

    FunctionKeyboard *m_kb;
};


#endif
