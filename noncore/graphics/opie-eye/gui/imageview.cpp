#include "imageview.h"

#include <opie2/odebug.h>
#include <opie2/oconfig.h>
#include <opie2/okeyconfigwidget.h>

#include <qpe/resource.h>

using namespace Opie::Core;

ImageView::ImageView(Opie::Core::OConfig *cfg, QWidget* parent, const char* name, WFlags fl )
    : ImageScrollView(parent,name,fl)
{
    m_viewManager = 0;
    m_cfg = cfg;
    initKeys();
}

ImageView::~ImageView()
{
    if (m_viewManager) {
        delete m_viewManager;
    }
}

Opie::Ui::OKeyConfigManager* ImageView::manager()
{
    if (!m_viewManager) {
        initKeys();
    }
    return m_viewManager;
}

void ImageView::initKeys()
{
    odebug << "init imageview keys" << oendl;
    if (!m_cfg) {
        m_cfg = new Opie::Core::OConfig("phunkview");
        m_cfg->setGroup("image_view_keys" );
    }
    Opie::Ui::OKeyPair::List lst;
    lst.append( Opie::Ui::OKeyPair::upArrowKey() );
    lst.append( Opie::Ui::OKeyPair::downArrowKey() );
    lst.append( Opie::Ui::OKeyPair::leftArrowKey() );
    lst.append( Opie::Ui::OKeyPair::rightArrowKey() );
    lst.append( Opie::Ui::OKeyPair::returnKey() );

    m_viewManager = new Opie::Ui::OKeyConfigManager(m_cfg, "image_view_keys",
                                                    lst, false,this, "image_view_keys" );
    m_viewManager->addKeyConfig( Opie::Ui::OKeyConfigItem(tr("View Image Info"), "imageviewinfo",
                                                Resource::loadPixmap("1to1"), ViewInfo,
                                                Opie::Ui::OKeyPair(Qt::Key_I,Qt::ShiftButton),
                                                this, SLOT(slotShowImageInfo())));
    m_viewManager->handleWidget( this );
    m_viewManager->load();
}

void ImageView::slotShowImageInfo()
{
    emit dispImageInfo(m_lastName);
}
