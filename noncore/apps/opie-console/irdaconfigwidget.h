#ifndef OPIE_IRDA_CONFIG_WIDGET_H
#define OPIE_IRDA_CONFIG_WIDGET_H

#include "profiledialogwidget.h"

class QVBoxLayout;
class QLabel;
class QComboBox;
class IOLayerBase;
class IrdaConfigWidget : public ProfileDialogConnectionWidget {

    Q_OBJECT

public:
    IrdaConfigWidget( const QString& name, QWidget* parent, const char* name = 0l );
    ~IrdaConfigWidget();

    void load( const Profile& );
    void save( Profile& );
private:
    QVBoxLayout* m_lay;
    QLabel* m_device;
    QComboBox* m_deviceCmb;
    IOLayerBase* m_base;

};


#endif
