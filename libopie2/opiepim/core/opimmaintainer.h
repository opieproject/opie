#ifndef OPIE_PIM_MAINTAINER_H
#define OPIE_PIM_MAINTAINER_H

#include <qstring.h>

/**
 * Who maintains what?
 */
class OPimMaintainer {
public:
    enum Mode { Undefined = -1,
                Responsible = 0,
                DoneBy,
                Coordinating };
    OPimMaintainer( enum Mode mode = Undefined, int uid = 0);
    OPimMaintainer( const OPimMaintainer& );
    ~OPimMaintainer();

    OPimMaintainer &operator=( const OPimMaintainer& );
    bool operator==( const OPimMaintainer& );
    bool operator!=( const OPimMaintainer& );


    Mode mode()const;
    int uid()const;

    void setMode( enum Mode );
    void setUid( int uid );

private:
    Mode m_mode;
    int m_uid;

};

#endif
