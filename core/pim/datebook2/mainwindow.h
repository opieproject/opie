#ifndef DATE_BOOK_MAIN_WINDOW_H
#define DATE_BOOK_MAIN_WINDOW_H

#include <qlist.h>

#include <opie2/opimmainwindow.h>
#include <opie2/opimevent.h>

#include "descriptionmanager.h"
#include "locationmanager.h"
#include "templatemanager.h"
#include "view.h"

class QAction;
class QWidgetStack;
class QPopupMenu;
class QToolBar;
namespace Opie {
namespace Datebook {
    class BookManager;
    class Show;
    class Editor;
    class MainWindow : public OPimMainWindow {
        Q_OBJECT
        friend class Show; // to avoid QObject
        friend class View; // to avoid QObject
        friend class Editor;
    public:
        MainWindow();
        ~MainWindow();

    private slots:
        void initUI(); // init the UI
        void initConfig(); // apply the config
        void initViews(); // init the Views..
        void initManagers(); // init the Managers including templates, location and description
        void raiseCurrentView(); // raise the current View
        void slotGoToNow(); // will switch the currentView to the curren date time
        void slotFind(); // find!!!
        void slotConfigure(); // configure the app
        void slotClockChanged(bool); // clock changed
        void slotWeekChanged( bool ); // week changed
        void slotAppMessage( const QCString&, const QByteArray& ); // qApp message QPE/Application/datebook
        void slotReceive( const QCString&, const QByteArray& ); // QPE/System and QPE/Datebook
        void slotChangeView(); // View changed
        void slotConfigureLocs(); // Configure the Locations
        void slotConfigureDesc(); // Configure the Desc
        void slotConfigureTemp();
        void slotNewFromTemplate(int ); // called when someone chooses the template menu

        void slotItemNew();
        void slotItemEdit();
        void slotItemDuplicate();
        void slotItemDelete();
        void slotItemBeam();

        void setTemplateMenu(); // updates the templateMenu
    
    protected:
        void initBars();

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
        void edit();
        void add( const OPimRecord& );

    private:
        QToolBar* toolbar();
        QPopupMenu* viewMenu();
        QPopupMenu* settingsMenu();
        QPopupMenu* templateMenu();
        View* currentView();
        BookManager* manager();
        TemplateManager templateManager();
        LocationManager locationManager();
        DescriptionManager descriptionManager();
        void setLocationManager( const LocationManager& );
        void setDescriptionManager( const DescriptionManager& );
        void edit ( OPimEvent &event, bool duplicate = FALSE );
        Show* eventShow();
        Editor* editor();

    private: // friend functions for Show
        void hideShow(); // to hide the view
        // off friend Show

        // friend of the view
    private:
        void viewPopup( const OPimOccurrence &ev, const QPoint &pt );
        void viewAdd( const QDate& date );
        void viewAdd( const OPimEvent& );
        bool viewAP()const;
        bool viewStartMonday()const;
        // off view show
    private:
        QList<View> m_views; // the Views.. not autoDelete
        QActionGroup* m_viewsGroup;
        QWidgetStack* m_stack;
        QToolBar *m_toolBar;
        QToolBar *m_toolBar2;
        QToolBar *m_viewsBar;
        QPopupMenu* m_popTemplate;
        BookManager* m_manager;
        TemplateManager m_tempMan;
        DescriptionManager m_descMan;
        LocationManager m_locMan;
        Show* m_show;
        Editor* m_edit;
        bool m_ampm;
    };
}
}

#endif
