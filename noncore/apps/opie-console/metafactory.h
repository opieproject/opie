#ifndef OPIE_META_FACTORY_H
#define OPIE_META_FACTORY_H

/**
 * The MetaFactory is used to keep track of all IOLayers, FileTransferLayers and ConfigWidgets
 * and to instantiate these implementations on demand
 */

#include <qwidget.h>
#include <qmap.h>

#include <qpe/config.h>

#include "io_layer.h"
#include "file_layer.h"
#include "profile.h"
#include "profiledialogwidget.h"
#include "emulation_layer.h"

class WidgetLayer;
class MetaFactory {
public:
    typedef ProfileDialogWidget* (*configWidget)(const QString&, QWidget* parent);
    typedef IOLayer* (*iolayer)(const Profile& );
    typedef FileTransferLayer* (*filelayer)(IOLayer*);
    typedef EmulationLayer* (*emulationLayer)(WidgetLayer* );

    MetaFactory();
    ~MetaFactory();

    /**
     *  add a ProfileDialogWidget to the factory
     * name is the name shown to the user
     */
    void addConnectionWidgetFactory( const QCString& internalName,
                                     const QString& uiString,
                                     configWidget );
    void addTerminalWidgetFactory  ( const QCString& internalName,
                                     const QString& name,
                                     configWidget );

    /**
     *  adds an IOLayer factory
     */
    void addIOLayerFactory( const QCString&,
                            const QString&,
                            iolayer );

    /**
     * adds a FileTransfer Layer
     */
    void addFileTransferLayer( const QCString& name,
                               const QString&,
                               filelayer );

    /**
     * adds a Factory for Emulation to the Layer..
     */
    void addEmulationLayer ( const QCString& name,
                              const QString& uiString,
                              emulationLayer );

    /* translated UI Strings */
    QStringList ioLayers()const;
    QStringList connectionWidgets()const;

    /**
     * Terminal Configuration widgets
     */
    QStringList terminalWidgets()const;
    QStringList fileTransferLayers()const;
    QStringList emulationLayers()const;

    /**
     * the generation...
     */
    IOLayer* newIOLayer( const QString&,const Profile& );
    ProfileDialogWidget *newConnectionPlugin ( const QString&, QWidget* );
    ProfileDialogWidget* newTerminalPlugin( const QString&, QWidget* );
    EmulationLayer* newEmulationLayer(const QString&, WidgetLayer* );
    FileTransferLayer* newFileTransfer(const QString&, IOLayer* );

    /*
     * internal takes the maybe translated
     * public QString and maps it to the internal
     * not translatable QCString
     */
    QCString internal( const QString& )const;

    /*
     * external takes the internal name
     * it returns a translated name
     */
    QString external( const QCString& )const;


private:
    QMap<QString, QCString> m_strings;
    QMap<QString, configWidget> m_conFact;
    QMap<QString, configWidget> m_termFact;
    QMap<QString, iolayer> m_layerFact;
    QMap<QString, filelayer> m_fileFact;
    QMap<QString, emulationLayer> m_emu;
};


#endif
