#ifndef OPIE_PIM_STATE_H
#define OPIE_PIM_STATE_H

#include <qstring.h>

/**
 * The State of a Task
 * This class encapsules the state of a todo
 * and it's shared too
 */
/*
 * in c a simple struct would be enough ;)
 * g_new_state();
 * g_do_some_thing( state_t*  );
 * ;)
 */
class OPimState {
public:
    enum State {
        Started = 0,
        Postponed,
        Finished,
        NotStarted,
        Undefined
    };
    OPimState( int state = Undefined );
    OPimState( const OPimState& );
    ~OPimState();

    bool operator==( const OPimState& );
    OPimState &operator=( const OPimState& );
    void setState( int state);
    int state()const;
private:
    void deref();
    inline void copyInternally();
    struct Data;
    Data* data;
    class Private;
    Private *d;
};


#endif
