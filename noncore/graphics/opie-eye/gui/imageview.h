#ifndef _IMAGE_VIEW_H
#define _IMAGE_VIEW_H

/* must be changed when it will moved to Opie::MM */
#include "imagescrollview.h"

namespace Opie {
    namespace Core {
        class OConfig;
    }
    namespace Ui {
        class OKeyConfigManager;
    }
}

class ImageView:public ImageScrollView
{
    Q_OBJECT

    enum ActionIds {
        ViewInfo
    };

public:
    ImageView( QWidget* parent, const char* name = 0, WFlags fl = 0 );
    virtual ~ImageView();
    Opie::Ui::OKeyConfigManager* manager();

signals:
    void dispImageInfo(const QString&);
    void sig_return();

protected:
    Opie::Core::OConfig * m_cfg;
    Opie::Ui::OKeyConfigManager*m_viewManager;
    void initKeys();
protected slots:
    virtual void slotShowImageInfo();
};
#endif
