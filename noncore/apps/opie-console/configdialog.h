#ifndef OPIE_CONFIG_DIALOG_H
#define OPIE_CONFIG_DIALOG_H

#include <qdialog.h>

#include "configurebase.h"
#include "profile.h"

class MetaFactory;
class ConfigDialog : public ConfigureBase {
    Q_OBJECT
public:
    ConfigDialog( const Profile::ValueList&, MetaFactory*,  QWidget* parent = 0l);
    ~ConfigDialog();

    Profile::ValueList list()const;
protected slots:
    void slotRemove();
    void slotEdit();
    void slotAdd();
private:
    MetaFactory* m_fact;

};

#endif
