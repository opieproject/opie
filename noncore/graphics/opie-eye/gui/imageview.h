#ifndef _IMAGE_VIEW_H
#define _IMAGE_VIEW_H

#include <opie2/oimagescrollview.h>

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
        Autoscale
    };

public:
    ImageView(Opie::Core::OConfig *cfg, QWidget* parent, const char* name = 0, WFlags fl = 0 );
    virtual ~ImageView();
    Opie::Core::OKeyConfigManager* manager();
    void setFullScreen(bool how);
    bool fullScreen(){return m_isFullScreen;}

signals:
    void dispImageInfo(const QString&);
    void dispNext();
    void dispPrev();
    void toggleFullScreen();
    void hideMe();
    void toggleZoomer();
    void toggleAutoscale();
    void toggleAutorotate();

protected:
    Opie::Core::OConfig * m_cfg;
    Opie::Core::OKeyConfigManager*m_viewManager;
    void initKeys();
    bool m_isFullScreen:1;
    void enableFullscreen();

protected slots:
    virtual void slotShowImageInfo();
    virtual void keyReleaseEvent(QKeyEvent * e);
    virtual void contentsMousePressEvent ( QMouseEvent * e);
    virtual void focusInEvent ( QFocusEvent * );
};

#endif
