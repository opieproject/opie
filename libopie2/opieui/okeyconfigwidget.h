#ifndef ODP_KEY_CONFIG_WIDGET_H
#define ODP_KEY_CONFIG_WIDGET_H

#include <opie2/oconfig.h>

#include <qstring.h>
#include <qpixmap.h>
#include <qbytearray.h>
#include <qhbox.h>
#include <qvaluelist.h>

class QKeyEvent;

namespace Opie {
namespace Ui {

class OKeyConfigItem {
    friend class OKeyConfigManager;
public:
    typedef QValueList<OKeyConfigItem> OKeyConfigItemList;
    OKeyConfigItem( const QString& text = QString::null , const QCString& config_key = QCString(),
                    const QPixmap& symbol  = QPixmap(),
                    int key = 0, int mod = 0,
                    int default_key = 0, int default_modified = 0 );
    ~OKeyConfigItem();

    QString text()const;
    QPixmap pixmap()const;
    int key()const;
    int modifier()const;
    int defaultKey()const;
    int defaultModifier()const;

    void setText( const QString& text );
    void setPixmap( const QPixmap& );
    void setKey( int );
    void setModied( int );
    void setDefaultKey( int );
    void setDefaultModifier( int );

    bool isConfigured()const;
    bool isEmpty()const;
private:
    QString  m_text;
    QCString m_config;
    QPixmap  m_pix;
    int      m_key;
    int      m_mod;
    int      m_defKey;
    int      m_defMod;
    class Private;
    Private *d;
};


class OKeyConfig : public QObject {
    Q_OBJECT
public:
    OKeyConfig(Opie::Core::OConfig *conf = 0,  bool grabkeyboard);
    ~OKeyConfig();

    void load();
    void save();

    OKeyConfigItem handleKeyEvent( QKeyEvent* );
    QString        handleKeyEventString( QKeyEvent* );

    void addKeyConfig( const OKeyConfigItem& );
    void removeKeyConfig( const OKeyConfigItem& );
signals:
    void keyConfigChanged( OKeyConfig* );

private:
    OKeyConfigItemList m_keys;
};


class OKeyConfigWidget : public QHBox {
    Q_OBJECT
public:
    enum ChangeMode { Imediate, Queu };
    OKeyConfigWidget( QWidget* parent = 0, const char* name = 0, WFlags fl = 0 );
    OKeyConfigWidget( OKeyConfig *, QWidget* parent = 0, const char* = 0, WFlags = 0 );
    ~OKeyConfigWidget();

    void setChangeMode( enum ChangeMode );
    ChangeMode changeMode()const;

    void reload();
};

}
}


#endif
