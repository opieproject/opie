#ifndef OPIE_PIM_MAINTAINER_H
#define OPIE_PIM_MAINTAINER_H

#include <qstring.h>

/**
 * Who maintains what?
 */
class OPimMaintainer {
public:
    enum Mode { Undefined = -1,
                Nothing = 0,
                Responsible,
                DoneBy,
                Coordinating,
    };
    OPimMaintainer( int mode = Undefined, int uid = 0);
    OPimMaintainer( const OPimMaintainer& );
    ~OPimMaintainer();

    OPimMaintainer &operator=( const OPimMaintainer& );
    bool operator==( const OPimMaintainer& );
    bool operator!=( const OPimMaintainer& );


    int mode()const;
    int uid()const;

    void setMode( int mode );
    void setUid( int uid );

private:
    int m_mode;
    int m_uid;
    class Private;
    Private *d;

};

#endif
