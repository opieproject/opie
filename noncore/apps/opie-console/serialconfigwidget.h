#ifndef OPIE_SERIAL_CONFIG_WIDGET_H
#define OPIE_SERIAL_CONFIG_WIDGET_H

#include "profiledialogwidget.h"

class QVBoxLayout;
class QLabel;
class QComboBox;
class IOLayerBase;
class SerialConfigWidget : public ProfileDialogConnectionWidget {
    Q_OBJECT
public:
    SerialConfigWidget( const QString& name, QWidget* parent, const char* name = 0l );
    ~SerialConfigWidget();

    void load( const Profile& );
    void save( Profile& );
private:
    QVBoxLayout* m_lay;
    QLabel* m_device;
    QComboBox* m_deviceCmb;
    IOLayerBase* m_base;

};


#endif
