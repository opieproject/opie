#ifndef _IMAGE_VIEW_H
#define _IMAGE_VIEW_H

#include <opie2/oimagescrollview.h>

#include <qwidget.h>

class QTimer;
class QActionGroup;
class QAction;
class QToolButton;
class QPopupMenu;

class QCopChannel;

namespace Opie {
    namespace Core {
        class OConfig;
        class OKeyConfigManager;
    }
}

class ImageView:public Opie::MM::OImageScrollView
{
    Q_OBJECT

    enum ActionIds {
        ViewInfo,
        FullScreen,
        ShowNext,
        ShowPrevious,
        Zoomer,
        Autorotate,
        Autoscale,
        Incbrightness,
        Decbrightness
    };

public:
    ImageView(Opie::Core::OConfig *cfg,QWidget* parent,const char* name = 0, WFlags fl = 0 );
    virtual ~ImageView();
    Opie::Core::OKeyConfigManager* manager();
    void setFullScreen(bool how,bool force=true);
    bool fullScreen(){return m_isFullScreen;}
    virtual void enableFullscreen();
    void stopSlide();
    void setMenuActions(QActionGroup*hGroup,QActionGroup*nextprevGroup, QActionGroup*disptypeGroup,QActionGroup*brightGroup);
    void setCloseIfHide(bool);
    void showPopupMenu( QPoint p );

signals:
    void dispImageInfo(const QString&);
    void dispNext();
    void dispPrev();
    void toggleFullScreen();
    void hideMe();
    void toggleZoomer();
    void toggleAutoscale();
    void toggleAutorotate();
    void incBrightness();
    void decBrightness();

protected:
    Opie::Core::OConfig * m_cfg;
    Opie::Core::OKeyConfigManager*m_viewManager;
    void initKeys();
    bool m_isFullScreen:1;
    bool m_ignore_next_in:1;
    bool m_Rotated:1;

    int focus_in_count;
    QTimer*m_slideTimer;
    int m_slideValue;
    virtual void focusInEvent ( QFocusEvent * );
    QActionGroup *m_gDisplayType,*m_gPrevNext,*m_hGroup,*m_gBright;
    bool closeIfHide:1;
    QCopChannel* m_sysChannel;
    int m_rotation;
    QToolButton *m_fullScreenButton;
    QPopupMenu *m_popupMenu;
    QTimer *m_fullScreenButtonTimer;

    virtual void resizeEvent( QResizeEvent *e );

public slots:
    virtual void hide();
    virtual void startSlide(int);
    virtual void nextSlide();
    virtual void systemMessage( const QCString&, const QByteArray& );
    virtual void slotIncBrightness();
    virtual void slotDecBrightness();

protected slots:
    virtual void slotShowImageInfo();
    virtual void keyReleaseEvent(QKeyEvent * e);
    virtual void contentsMousePressEvent ( QMouseEvent * e);
    virtual void slotHideFullScreenButton();
    virtual void slotFullScreenButton();
};

#endif
