#ifndef OPIE_PROFILE_MANAGER_H
#define OPIE_PROFILE_MANAGER_H

#include <qlist.h>

#include "session.h"
#include "profile.h"

class MetaFactory;
class ConfigWidget;
class ProfileManager {
public:
    ProfileManager(MetaFactory*);
    ~ProfileManager();

    void load();
    Profile::ValueList all()const;
    void clear();
    /**
     * also replaces the item
     */
    void add( const Profile& prof );
    void remove( const Profile& prof );
    Session* fromProfile( const Profile& );
    void save();
private:
    MetaFactory* m_fact;
    Profile::ValueList m_list;
};
#endif
