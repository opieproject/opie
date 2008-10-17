#ifndef OPIE_DATEBOOK_EDITOR_H
#define OPIE_DATEBOOK_EDITOR_H

#include <qdialog.h>

#include <opie/oevent.h>

#include "descriptionmanager.h"
#include "locationmanager.h"

namespace Datebook {
    class MainWindow;
    /**
     * This is the editor interface
     */
    class Editor {
    public:
        Editor( MainWindow*, QWidget* parent );
        virtual ~Editor();

        virtual bool newEvent( const QDate& ) = 0;
        virtual bool newEvent( const QDateTime& start, const QDateTime& end ) = 0;
        virtual bool edit( const OEvent&, bool showRec = TRUE ) = 0;

        virtual OEvent event()const = 0;

    protected:
        DescriptionManager descriptions()const;
        LocationManager locations()const;
        void setDescriptions( const DescriptionManager& );
        void setLocations( const LocationManager& );

    private:
        MainWindow* m_win;

    };
}

#endif
