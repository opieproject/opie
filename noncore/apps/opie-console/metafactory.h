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

class MetaFactory {
public:
    typedef ProfileDialogWidget* (*configWidget)(const QString&, QWidget* parent);
    typedef IOLayer* (*iolayer)(const Profile& );
    typedef FileTransferLayer* (*filelayer)(IOLayer*);

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

    /* translated UI Strings */
    QStringList ioLayers()const;
    QStringList connectionWidgets()const;
    QStringList terminalWidgets()const;
    QStringList fileTransferLayers()const;
    IOLayer* newIOLayer( const QString&,const Profile& );
    ProfileDialogWidget *newConnectionPlugin ( const QString&, QWidget* );
    ProfileDialogWidget* newTerminalPlugin( const QString&, QWidget* );

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
};


#endif
