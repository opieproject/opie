/*
 * GPLv2 zecke@handhelds.org
 * No WArranty...
 */

#ifndef PHUNK_MAIN_WINDOW_H
#define PHUNK_MAIN_WINDOW_H

#include <opie2/oconfig.h>

#include <qmainwindow.h>



namespace Opie {
namespace Ui{
    class OKeyConfigManager;
    class OWidgetStack;
}
}

class PIconView;
class imageinfo;
class ImageScrollView;
class PMainWindow : public QMainWindow {
    Q_OBJECT
    enum Views { IconView,  ImageInfo, ImageDisplay };
public:
    static QString appName() { return QString::fromLatin1("opie-eye" ); }
    PMainWindow(QWidget*, const char*, WFlags );
    ~PMainWindow();

signals:
    void configChanged();

public slots:
    void slotShowInfo( const QString& inf );
    void slotDisplay( const QString& inf );
    void slotReturn();

protected:
    void raiseIconView();
    void closeEvent( QCloseEvent* );

private:
    template<class T>  void initT( const char* name, T**, int );
    void initInfo();
    void initDisp();

private:
    Opie::Core::OConfig *m_cfg;
    Opie::Ui::OWidgetStack *m_stack;
    PIconView* m_view;
    imageinfo *m_info;
    ImageScrollView *m_disp;


private slots:
    void slotConfig();
};

#endif
