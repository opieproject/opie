#ifndef TEST_WIDGET_H
#define TEST_WIDGET_H

#include <qmainwindow.h>

#include <opie2/olistview.h>

namespace Opie{
namespace Ui{
    class OTabWidget;
    class OListView;
}
namespace Core {
    class OConfig;
    class OKeyConfigManager;
    class OKeyConfigItem;
}
}
class QKeyEvent;


class TestMainWindow : public Opie::Ui::OListView {
    Q_OBJECT
public:

    TestMainWindow(QWidget* parent, const char*, WFlags fl );
    ~TestMainWindow();

    Opie::Core::OKeyConfigManager *manager();
protected:
    void keyPressEvent( QKeyEvent* );
private slots:
    void slotDelete( QWidget*, QKeyEvent* );
    void slotAction( QWidget*, QKeyEvent*, const Opie::Core::OKeyConfigItem& );
private:
    Opie::Core::OConfig *m_config;
    Opie::Core::OKeyConfigManager *m_manager;
};

#endif
