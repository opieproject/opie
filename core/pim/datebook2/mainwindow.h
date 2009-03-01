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
        void slotSystemReceive( const QCString&, const QByteArray& ); // QPE/System
        void slotChangeView(); // View changed
        void slotConfigureTemp();
        void slotNewFromTemplate(int ); // called when someone chooses the template menu

        void slotItemNew();
        void slotItemEdit();
        void slotItemDuplicate();
        void slotItemDelete();
        void slotItemBeam();
        void slotItemBeamOccurrence();

        void setTemplateMenu(); // updates the templateMenu
    
    protected:
        void initBars();
        void saveConfig();
        void beamEvent( const OPimEvent &e );

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
        void showDayView();
        void showDefaultView();
        void add( const OPimRecord& );
        bool doAlarm( const QDateTime &when, int uid );
        void nextView();

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
        Show* eventShow();
        Editor* editor();

    private: // friend functions for Show
        void hideShow(); // to hide the view
        // off friend Show

        // friend of the view
    private:
        void viewPopup( const OPimOccurrence &ev, const QPoint &pt );
        void viewAdd( const QDateTime& start, const QDateTime& end );
        void viewAdd( const OPimEvent& );
        bool viewAP()const;
        bool viewStartMonday()const;
        // off view show

    private: // friend functions for Editor
        QString defaultLocation()const;
        QArray<int> defaultCategories()const;
        bool alarmPreset()const;
        int alarmPresetTime()const;

    private:
        QList<View> m_views; // the Views.. not autoDelete
        View *m_currView;
        QActionGroup* m_viewsGroup;
        QWidgetStack* m_stack;
        QToolBar *m_toolBar;
        QToolBar *m_toolBar2;
        QToolBar *m_viewsBar;
        QPopupMenu* m_popTemplate;
        QAction *m_itemBeamOccurrenceAction;
        BookManager* m_manager;
        TemplateManager m_tempMan;
        DescriptionManager m_descMan;
        LocationManager m_locMan;
        Show* m_show;
        Editor* m_edit;
        QDate m_initialDate;
        bool m_ampm;
        bool m_onMonday;
        int m_defaultViewIdx;
        int m_startTime;
        bool m_alarmPreset;
        int m_alarmPresetTime;
        QArray<int> m_defaultCategories;
        QString m_defaultLocation;
    };
}
}

#endif
