/*
 * examplepluginimpl.h
 *
 * email       : harlekin@handhelds.org
 *
 */

#ifndef EXAMPLE_PLUGIN_IMPL_H
#define EXAMPLE_PLUGIN_IMPL_H

#include <opie/todayplugininterface.h>

class ExamplePlugin;

class ExamplePluginImpl : public TodayPluginInterface{

public:
    ExamplePluginImpl();
    virtual ~ExamplePluginImpl();

    QRESULT queryInterface( const QUuid &,  QUnknownInterface** );
    Q_REFCOUNT

    virtual TodayPluginObject *guiPart();

private:
    ExamplePlugin *examplePlugin;
};

#endif
