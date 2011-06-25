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
class QActionGroup;
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
    void changeListMode(int);

public slots:
    void slotShowInfo( const QString& inf );
    void slotDisplay( const QString& inf );
    void slotUpdateDisplay( const QString& inf );
    void slotReturn();
    void slotRotateToggled(bool);
    void slotScaleToggled(bool);
    void slotZoomerToggled(bool);
    void slotToggleZoomer();
    void slotToggleAutorotate();
    void slotToggleAutoscale();
    void setDocument( const QString& );
    virtual void slotToggleFullScreen();
    virtual void slotFullScreenToggled(bool);
    virtual void polish();

protected slots:
    void raiseIconView();
    void closeEvent( QCloseEvent* );
    void showToolbar(bool);
    void listviewselected(QAction*);
    void slotFullScreenButton(bool);
    void check_view_fullscreen();
    virtual void setupBrightness();

private:
    template<class T>  void initT( const char* name, T**, int );
    void initInfo();
    void initDisp();
    void setupViewWindow(bool full, bool forceDisplay);

private:
    Opie::Core::OConfig *m_cfg;
    Opie::Ui::OWidgetStack *m_stack;
    PIconView* m_view;
    imageinfo *m_info;
    ImageView *m_disp;
    bool autoSave:1;
    bool m_setDocCalled:1;
    bool m_polishDone:1;
    bool m_SmallWindow:1;
    int m_Intensity;
    QToolButton*fsButton;
    QToolBar *toolBar;
    QPopupMenu *fileMenu,*dispMenu,*fsMenu,*listviewMenu,*settingsMenu;
    QAction*m_aShowInfo,*m_aBeam,*m_aTrash,*m_aViewfile,*m_aDirUp,*m_aStartSlide;
    QAction*m_aHideToolbar,*m_aSetup,*m_aDirName,*m_aDirShort,*m_aDirLong;
    QActionGroup *m_gListViewMode,*m_gDisplayType,*m_gPrevNext,*m_hGroup,*m_hBright;
    QAction *m_aNext,*m_aPrevious,*m_aFullScreen;
    QAction *m_aAutoRotate,*m_aUnscaled,*m_aZoomer,*m_aForceSmall,*m_setCurrentBrightness;
    QAction *m_IncBrightness,*m_DecBrightness;

    /* init funs */
    void readConfig();
    void setupActions();
    void setupToolbar();
    void setupMenu();
    /* for the device submenu - ToDo: Merge with the special button */
    StorageInfo *m_storage;
    QMap<QString, QString> m_dev;
    static bool Valuebox(QWidget*parent,int min, int max, int current,int&store);

private slots:
    void slotConfig();
    void slotSelectDir(int);
    void dirChanged();
    void slotForceSmall(bool);
};

#endif
