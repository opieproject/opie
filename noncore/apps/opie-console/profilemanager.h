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
    Profile profile(const QString& name )const;
    Session* fromProfile( const Profile& , QWidget* parent = 0l);
    void setProfiles( const Profile::ValueList& );
    void save();
private:
    MetaFactory* m_fact;
    Profile::ValueList m_list;
};
#endif
