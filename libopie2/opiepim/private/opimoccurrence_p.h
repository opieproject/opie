// Private HEADER DON'T USE IT!!! WE MEAN IT!!!
//

#ifndef OPIE_PRIVATE_INTERNAL_OCCURRENCE_PIM_H
#define OPIE_PRIVATE_INTERNAL_OCCURRENCE_PIM_H

#include <qshared.h>


namespace Opie{

/*
 * The Data is shared between Occurrences
 * across multiple days.
 */
/**
 * @internal
 *
 * DO NOT USE. IT IS NOT PART OF THE API
 */
struct OPimOccurrence::Data : public QShared {
    Data() : uid( -1 ), backend( 0l ) {}

    QString summary;  // The Summary of this Occurrence
    QString location; // The location of this Occurrence
    QString note;     // The note of this Occurrence
    UID uid;          // The UID of the Record
    mutable Opie::Core::OSharedPointer<OPimRecord> record; // The Guarded Record
    OPimBase *backend;
};
}

#endif
