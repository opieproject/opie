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
    class OWidgetStack;
}
namespace Core{
    class OKeyConfigManager;
}
}

class ImageView;
class ImageWidget;
class PIconView;
class imageinfo;
class QMenuBar;
class QToolBar;
class QPopupMenu;
class QAction;
class StorageInfo;

class PMainWindow : public QMainWindow {
    Q_OBJECT
    enum Views { IconView,  ImageInfo, ImageDisplay };
public:
    static QString appName() { return QString::fromLatin1("opie-eye" ); }
    PMainWindow(QWidget*, const char*, WFlags );
    ~PMainWindow();

signals:
    void configChanged();
    void changeDir( const QString& );

public slots:
    void slotShowInfo( const QString& inf );
    void slotDisplay( const QString& inf );
    void slotReturn();
    void slotRotateToggled(bool);
    void slotScaleToggled(bool);
    void slotZoomerToggled(bool);
    void slotToggleZoomer();
    void slotToggleAutorotate();
    void slotToggleAutoscale();
    void setDocument( const QString& );
    virtual void slotToggleFullScreen();

protected slots:
    void raiseIconView();
    void closeEvent( QCloseEvent* );
    void showToolbar(bool);

private:
    template<class T>  void initT( const char* name, T**, int );
    void initInfo();
    void initDisp();

private:
    Opie::Core::OConfig *m_cfg;
    Opie::Ui::OWidgetStack *m_stack;
    PIconView* m_view;
    imageinfo *m_info;
    ImageView *m_disp;
    bool autoRotate;
    bool autoScale;
    bool zoomerOn;
    QToolButton*rotateButton,*fsButton,*viewModeButton;
    QToolButton*nextButton,*prevButton,*zoomButton,*scaleButton;
    QMenuBar *menuBar;
    QToolBar *toolBar;
    QPopupMenu *fileMenu,*dispMenu,*fsMenu;
    QAction*m_aShowInfo,*m_aBeam,*m_aTrash,*m_aViewfile,*m_aDirUp,*m_aStartSlide;
    QAction*m_aHideToolbar;

    /* for the device submenu - ToDo: Merge with the special button */
    StorageInfo *m_storage;
    QMap<QString, QString> m_dev;

private slots:
    void slotConfig();
    void slotSelectDir(int);
    void dirChanged();
};

#endif
