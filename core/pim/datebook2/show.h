#ifndef OPIE_DATEBOOK_SHOW_H
#define OPIE_DATEBOOK_SHOW_H

#include <qstring.h>

#include <opie/oevent.h>

namespace Datebook {
    class MainWindow;

    /**
     * here is the show
     * The show interface will
     * show the richtext information
     * of the OEvent
     */
    class Show {
    public:
        /**
         * mainwindow as parent
         */
        Show(MainWindow* );
        virtual ~Show();

        /**
         * show the OEvent
         */
        void show(const OEvent& str);

        /**
         * the Widget
         */
        virtual QWidget* widget() = 0;
    protected:
        /**
         * the show is over
         * ask the mainwindow to hide
         */
        void  hideMe();

    };
}

#endif
