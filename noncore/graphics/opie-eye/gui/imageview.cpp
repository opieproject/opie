#include "imageview.h"

#include <opie2/odebug.h>
#include <opie2/oconfig.h>
#include <opie2/okeyconfigwidget.h>

#include <qpe/resource.h>
#include <qpe/qpeapplication.h>
#include <qpopupmenu.h>

using namespace Opie::Core;

ImageView::ImageView(Opie::Core::OConfig *cfg, QWidget* parent, const char* name, WFlags fl )
    : Opie::MM::OImageScrollView(parent,name,fl)
{
    m_viewManager = 0;
    m_cfg = cfg;
    m_isFullScreen = false;
    QPEApplication::setStylusOperation(viewport(),QPEApplication::RightOnHold);
    initKeys();
}

ImageView::~ImageView()
{
    odebug << "Delete Imageview" << oendl;
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
    lst.append( Opie::Core::OKeyPair(Qt::Key_Escape,0));

    m_viewManager = new Opie::Core::OKeyConfigManager(m_cfg, "image_view_keys",
                                                    lst, false,this, "image_view_keys" );

    m_viewManager->addKeyConfig( Opie::Core::OKeyConfigItem(tr("View Image Info"), "imageviewinfo",
                                                Resource::loadPixmap("1to1"), ViewInfo,
                                                Opie::Core::OKeyPair(Qt::Key_I,0),
                                                this, SLOT(slotShowImageInfo())));

    m_viewManager->addKeyConfig( Opie::Core::OKeyConfigItem(tr("Toggle autorotate"), "imageautorotate",
                                                Resource::loadPixmap("rotate"), Autorotate,
                                                Opie::Core::OKeyPair(Qt::Key_R,0),
                                                this, SIGNAL(toggleAutorotate())));
    m_viewManager->addKeyConfig( Opie::Core::OKeyConfigItem(tr("Toggle autoscale"), "imageautoscale",
                                                Resource::loadPixmap("1to1"), Autoscale,
                                                Opie::Core::OKeyPair(Qt::Key_S,0),
                                                this, SIGNAL(toggleAutoscale())));

    m_viewManager->addKeyConfig( Opie::Core::OKeyConfigItem(tr("Switch to next image"), "imageshownext",
                                                Resource::loadPixmap("forward"), ShowNext,
                                                Opie::Core::OKeyPair(Qt::Key_Return,0),
                                                this, SIGNAL(dispNext())));
    m_viewManager->addKeyConfig( Opie::Core::OKeyConfigItem(tr("Switch to previous image"), "imageshowprev",
                                                Resource::loadPixmap("back"), ShowPrevious,
                                                Opie::Core::OKeyPair(Qt::Key_P,0),
                                                this, SIGNAL(dispPrev())));
    m_viewManager->addKeyConfig( Opie::Core::OKeyConfigItem(tr("Toggle fullscreen"), "imagefullscreen",
                                                Resource::loadPixmap("fullscreen"), FullScreen,
                                                Opie::Core::OKeyPair(Qt::Key_F,0),
                                                this, SIGNAL(toggleFullScreen())));
    m_viewManager->addKeyConfig( Opie::Core::OKeyConfigItem(tr("Toggle thumbnail"), "imagezoomer",
                                                Resource::loadPixmap("mag"), Zoomer,
                                                Opie::Core::OKeyPair(Qt::Key_T,0),
                                                this, SIGNAL(toggleZoomer())));
    m_viewManager->handleWidget( this );
    m_viewManager->load();
}

void ImageView::keyReleaseEvent(QKeyEvent * e)
{
    if (!e || e->state()!=0) {
        return;
    }
    if (e->key()==Qt::Key_Escape && fullScreen()) emit hideMe();
}

void ImageView::slotShowImageInfo()
{
    emit dispImageInfo(m_lastName);
}

void ImageView::contentsMousePressEvent ( QMouseEvent * e)
{
    if (e->button()==1) {
        return OImageScrollView::contentsMousePressEvent(e);
    }
    if (!fullScreen()) return;
#if 0
    // doesn't work right (repainting problems)
    odebug << "Popup " << oendl;
    QPopupMenu *m = new QPopupMenu(0);
    if (!m) return;
    m->insertItem(tr("Previous image"),ShowPrevious);
    m->insertItem(tr("Next image"),ShowNext);
    m->insertSeparator();
    m->insertItem(tr("Toggle fullscreen"),FullScreen);
    m->insertItem(tr("Toggle autoscale"),Autoscale);
    m->insertItem(tr("Toggle autorotate"),Autorotate);
    m->insertItem(tr("Toggle thumbnail"),Zoomer);
    m->setFocus();
    m->exec( QPoint( QCursor::pos().x(), QCursor::pos().y()) );
    delete m;
    parentWidget()->showFullScreen();
#endif
}

void ImageView::setFullScreen(bool how)
{
    m_isFullScreen = how;
}
