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
class QMenuBar;
class QAction;
class MetaFactory;

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    MainWindow(  );
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

private slots:
    void slotNew();
    void slotConnect();
    void slotDisconnect();
    void slotTerminate();
    void slotConfigure();

private:
    void initUI();
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

    QToolBar* m_tool;
    QMenuBar* m_bar;
    QPopupMenu* m_console;
    QPopupMenu* m_settings;
    QPopupMenu* m_sessionsPop;
    QAction* m_connect;
    QAction* m_disconnect;
    QAction* m_terminate;
    QAction* m_setProfiles;

};


#endif
