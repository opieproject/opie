#ifndef OPIE_META_FACTORY_H
#define OPIE_META_FACTORY_H

/**
 * meta factory is our factory servie
 */

#include <qwidget.h>
#include <qmap.h>

#include <qpe/config.h>

#include "io_layer.h"
#include "file_layer.h"


class MetaFactory {
public:
    typedef QWidget* (*configWidget)(QWidget* parent);
    typedef IOLayer* (*iolayer)(const Config& );
    typedef FileTransferLayer* (*filelayer)(IOLayer*);
    MetaFactory();
    ~MetaFactory();

    void addConfigWidgetFactory( const QString&,
                                 configWidget );
    void addIOLayerFactory(const QString&,
                           iolayer );
    void addFileTransferLayer( const QString&,
                               filelayer );
    QStringList ioLayers()const;
    QStringList configWidgets()const;
    QStringList fileTransferLayers()const;


private:
    QMap<QString, configWidget> m_confFact;
    QMap<QString, iolayer> m_layerFact;
    QMap<QString, filelayer> m_fileFact;



};


#endif
