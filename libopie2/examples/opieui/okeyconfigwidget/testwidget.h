#ifndef TEST_WIDGET_H
#define TEST_WIDGET_H

#include <qmainwindow.h>

#include <opie2/olistview.h>

namespace Opie{
namespace Ui{
    class OTabWidget;
    class OKeyConfigManager;
    class OListView;
    class OKeyConfigItem;
}
namespace Core {
    class OConfig;
}
}
class QKeyEvent;


class TestMainWindow : public Opie::Ui::OListView {
    Q_OBJECT
public:

    TestMainWindow(QWidget* parent, const char*, WFlags fl );
    ~TestMainWindow();

    Opie::Ui::OKeyConfigManager *manager();
protected:
    void keyPressEvent( QKeyEvent* );
private slots:
    void slotDelete( QWidget*, QKeyEvent* );
    void slotAction( QWidget*, QKeyEvent*, const Opie::Ui::OKeyConfigItem& );
private:
    Opie::Core::OConfig *m_config;
    Opie::Ui::OKeyConfigManager *m_manager;
};

#endif
