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
        FullScreen
    };

public:
    ImageView(Opie::Core::OConfig *cfg, QWidget* parent, const char* name = 0, WFlags fl = 0 );
    virtual ~ImageView();
    Opie::Core::OKeyConfigManager* manager();
    void setFullScreen(bool how){m_isFullScreen = how;}
    bool fullScreen(){return m_isFullScreen;}

signals:
    void dispImageInfo(const QString&);
    void dispNext();
    void dispPrev();
    void toggleFullScreen();
    void hideMe();

protected:
    Opie::Core::OConfig * m_cfg;
    Opie::Core::OKeyConfigManager*m_viewManager;
    void initKeys();
    bool m_isFullScreen:1;

protected slots:
    virtual void slotShowImageInfo();
    virtual void slotDispNext();
    virtual void slotDispPrev();
    virtual void keyReleaseEvent(QKeyEvent * e);
};
#endif
