#ifndef OPIE_DATE_BOOK_VIEW_H
#define OPIE_DATE_BOOK_VIEW_H

#include <qpixmap.h>
#include <qwidget.h>
#include <qaction.h>
#include <qobject.h>
#include <qlist.h>

#include <opie2/opimevent.h>
#include <opie2/opimoccurrence.h>
#include <opie2/odatebookaccess.h>

class Config;
namespace Opie {
namespace Datebook {
    class MainWindow;
    class View;
    
    class View: public QObject {
        Q_OBJECT
    public:
        View( MainWindow* window,  QWidget* parent );
        virtual ~View();

        static QDate dateFromWeek( int week, int year, bool startOnMonda );
        static bool calcWeek( const QDate& d, int &week, int &year, bool startOnMonday = false );

        virtual QPixmap pixmap()const = 0;

        /**
         * non translatable name or type
         */
        virtual QCString type()const = 0;

        /**
         * shown to the user
         */
        virtual QString name()const = 0;
        virtual QString description()const = 0;

        /**
         * return the uid of the current item or 0
         */
        virtual int currentItem()const = 0;

        /**
         * loadConfig
         * saveConfig
         */
        void loadConfig( Config* );
        void saveConfig( Config* );

        /**
         * the current range
         * @param src Where to write the start datetime
         * @param dest Where to write the end datetime
         */
        virtual void currentRange( const QDateTime& src, const QDateTime& from) = 0;

        /**
         * the clock format changed
         */
        virtual void clockChanged() = 0;

        /**
         * the day change means that either
         * monday or sunday is the week start
         */
        virtual void dayChanged() = 0;

        /**
         * show date in your view!!
         * make the date visible in the current view
         */
        virtual void showDay( const QDate& date ) = 0;

        /**
         * return the widget
         */
        virtual QWidget* widget() = 0;

        /**
         * the view needs an update!
         */
        virtual void reschedule() = 0;
    protected:
        void popup( int uid);
        QString toShortText(const OPimOccurrence& eff)const;
        QString toText(const OPimOccurrence& eff)const;
        virtual void doLoadConfig( Config* ) = 0;
        virtual void doSaveConfig( Config* ) = 0;

        /**
         * create a new event starting
         * on start
         */
        void add( const QDate& start );

        /**
         * create a new event in a given range
         */
        void add( const QDateTime& start, const QDateTime& end );

        /**
         * will make the MainWindow to open the editor
         */
        void edit( int uid );

        /**
         * remove item with uid
         */
        void remove( int uid );

        /**
         * give access to all events
         */
        ODateBookAccess::List allEvents()const;

        /**
         * return events
         */
        OPimOccurrence::List events( const QDate& start, const QDate& end );

        /**
         * return an Event
         */
        OPimEvent event( int uid )const;

        /**
         * Is the clock AP/PM or 24h?
         */
        bool isAP()const;

        /**
         * if the week starts on monday?
         */
        bool weekStartOnMonday()const;

        /**
         * return related records for days
         */
        QList<OPimRecord> records( const QDate& on );
        QList<OPimRecord> records( const QDate& start, const QDate& to );

    private:
        MainWindow* m_win;
    };

    class ViewAction : public QAction {
    public:
        ViewAction(View *view, int accel, QObject *parent, const char *name = 0)
            : QAction(view->name(), view->pixmap(), QString::null, accel, parent, name, TRUE) { };
    };
}
}

#endif
