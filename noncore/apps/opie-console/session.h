#ifndef OPIE_SESSION_H
#define OPIE_SESSION_H

#include <qwidgetstack.h>

#include "profile.h"

class IOLayer;
class EmulationHandler;
/**
 * This is a Session. A session contains
 * a QWidget pointer and a IOLayer
 * Imagine a session like a collection of what
 * is needed to show your widget in a tab ;)
 */
class Session {
public:
    /**
     * c'tor with widget and layer
     * ownership get's transfered
     */
    Session();
    Session( const QString&, QWidgetStack* widget, IOLayer* );
    ~Session();

    /**
     * return the name of the session
     */
    QString name()const;

    /**
     * return the widgetstack
     * this is used to be semi modal
     * for FileTransfer
     *
     * semi modal == SessionModal
     */
    QWidgetStack* widgetStack();
    QWidget* widget();

    /**
     * return the layer
     */
    IOLayer* layer();

    EmulationHandler* emulationHandler();
    Profile profile()const;

    /*
     * connects the data flow from
     * the IOLayer to the EmulationLayer
     */
    void connect();

    /*
     * disconnect the dataflow
     * this will be done for ft
     */
    void disconnect();

    void setWidgetStack( QWidgetStack* widget );
    void setEmulationHandler( EmulationHandler* lay );
    void setIOLayer( IOLayer*  );
    void setName( const QString& );
    void setProfile( const Profile& );

private:
    QString m_name;
    QWidgetStack* m_widget;
    IOLayer* m_layer;
    EmulationHandler* m_emu;
    bool m_connected : 1;
    Profile m_prof;

};

#endif
