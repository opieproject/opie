#ifndef _IMAGE_VIEW_H
#define _IMAGE_VIEW_H

/* must be changed when it will moved to Opie::MM */
#include "imagescrollview.h"

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
        ViewInfo
    };

public:
    ImageView(Opie::Core::OConfig *cfg, QWidget* parent, const char* name = 0, WFlags fl = 0 );
    virtual ~ImageView();
    Opie::Core::OKeyConfigManager* manager();

signals:
    void dispImageInfo(const QString&);

protected:
    Opie::Core::OConfig * m_cfg;
    Opie::Core::OKeyConfigManager*m_viewManager;
    void initKeys();
protected slots:
    virtual void slotShowImageInfo();
};
#endif
