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
        virtual Editor();

        bool newEvent( const QDate& );
        bool newEvent( const QDateTime& start, const QDateTime& end );
        bool edit( const OEvent& );

        OEvent event()const;

    protected:
        DescriptionManager descriptions()const;
        LocationManager locations()const;
        void setDescriptions( const DescriptionManager& );
        void setLocations( const LocationManager& );

    };
}

#endif
