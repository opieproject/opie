#include "imageview.h"

#include <opie2/odebug.h>
#include <opie2/oconfig.h>
#include <opie2/okeyconfigwidget.h>

#include <qpe/resource.h>

using namespace Opie::Core;

ImageView::ImageView(Opie::Core::OConfig *cfg, QWidget* parent, const char* name, WFlags fl )
    : Opie::MM::OImageScrollView(parent,name,fl)
{
    m_viewManager = 0;
    m_cfg = cfg;
    m_isFullScreen = false;
    initKeys();
}

ImageView::~ImageView()
{
    if (m_viewManager) {
        delete m_viewManager;
    }
}

Opie::Core::OKeyConfigManager* ImageView::manager()
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
    Opie::Core::OKeyPair::List lst;
    lst.append( Opie::Core::OKeyPair::upArrowKey() );
    lst.append( Opie::Core::OKeyPair::downArrowKey() );
    lst.append( Opie::Core::OKeyPair::leftArrowKey() );
    lst.append( Opie::Core::OKeyPair::rightArrowKey() );
    lst.append( Opie::Core::OKeyPair(Qt::Key_N,0));
    lst.append( Opie::Core::OKeyPair(Qt::Key_P,0));
    lst.append( Opie::Core::OKeyPair(Qt::Key_F,0));

    m_viewManager = new Opie::Core::OKeyConfigManager(m_cfg, "image_view_keys",
                                                    lst, false,this, "image_view_keys" );
    m_viewManager->addKeyConfig( Opie::Core::OKeyConfigItem(tr("View Image Info"), "imageviewinfo",
                                                Resource::loadPixmap("1to1"), ViewInfo,
                                                Opie::Core::OKeyPair(Qt::Key_I,0),
                                                this, SLOT(slotShowImageInfo())));
    m_viewManager->handleWidget( this );
    m_viewManager->load();
}

void ImageView::keyReleaseEvent(QKeyEvent * e)
{
    if (!e || e->state()!=0) {
        return;
    }
    if (e->key()==Qt::Key_N) slotDispNext();
    if (e->key()==Qt::Key_P) slotDispPrev();
    if (e->key()==Qt::Key_F) emit toggleFullScreen();
}

void ImageView::slotDispNext()
{
    emit dispNext();
}

void ImageView::slotDispPrev()
{
    emit dispPrev();
}

void ImageView::slotShowImageInfo()
{
    emit dispImageInfo(m_lastName);
}
