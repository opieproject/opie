#ifndef DATE_BOOK_MAIN_WINDOW_H
#define DATE_BOOK_MAIN_WINDOW_H

#include <qlist.h>

#include <opie/opimmainwindow.h>

namespace Datebook {

    class MainWindow : public OPimMainWindow {
        Q_OBJECT
    public:
        MainWindow();
        ~MainWindow();

    private slots:
        void initUI(); // init the UI
        void initConfig(); // apply the config
        void initView(); // init the Views..
        void initManagers(); // init the Managers including templates, location and description
        void raiseCurrentView(); // raise the current View

    protected slots:
        void populate();
        void doSetDocument( const QString& str );
        void flush();
        void reload();
        int create();
        bool remove( int uid );
        void beam( int uid );
        void show( int uid );
        void edit( int uid );
        void add( const OPimRecord& );

    private:
        QList<View> m_view; // the Views.. not autoDelete

    };
}

#endif
