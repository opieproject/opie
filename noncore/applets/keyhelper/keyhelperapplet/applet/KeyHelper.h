#ifndef _KEY_HELPER_H_
#define _KEY_HELPER_H_

#include <qdir.h>
#include <qwindowsystem_qws.h>
#include "KeyNames.h"
#include "KeyMappings.h"
#include "KeyModifiers.h"
#include "KeyExtensions.h"
#include "KeyRepeater.h"
#include "KeyAction.h"
#include "KeycfgReader.h"

class KeyHelper : public QWSServer::KeyboardFilter
{
public:
    KeyHelper();
    virtual ~KeyHelper();
    bool filter(int unicode, int keycode, int modifiers,
            bool isPress, bool autoRepeat);

    void enable();
    void disable();
    void set();
    void unset();
    void statistics();
    void dumpkeymap();
    bool reload(const QString& file=QString::null);
private:
    friend class KeyHelperWidget;

    bool load(const QString& file=QString::null);
    void setDefault();

    KeyAction m_oAction;
    KeyMappings m_oMappings;
    KeyModifiers m_oModifiers;
    KeyExtensions m_oExtensions;
    KeyRepeater m_oRepeater;
};

#endif /* _KEY_HELPER_H_ */
