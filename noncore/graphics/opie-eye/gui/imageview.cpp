#include "imageview.h"

#include <opie2/odebug.h>
#include <opie2/oconfig.h>
#include <opie2/okeyconfigwidget.h>

#include <qpe/resource.h>
#include <qpe/qpeapplication.h>
#include <qpopupmenu.h>
#include <qtimer.h>
#include <qaction.h>

using namespace Opie::Core;

ImageView::ImageView(Opie::Core::OConfig *cfg, QWidget* parent, const char* name, WFlags fl )
    : Opie::MM::OImageScrollView(parent,name,fl)
{
    m_viewManager = 0;
    focus_in_count = 0;
    m_cfg = cfg;
    m_isFullScreen = false;
    m_ignore_next_in = false;
    m_slideTimer = 0;
    QPEApplication::setStylusOperation(viewport(),QPEApplication::RightOnHold);
    initKeys();
    m_slideValue = 5;
    m_gDisplayType = 0;
    m_gPrevNext = 0;
    m_hGroup = 0;
}

void ImageView::setMenuActions(QActionGroup*hGroup,QActionGroup*nextprevGroup, QActionGroup*disptypeGroup)
{
    m_gDisplayType = disptypeGroup;
    m_gPrevNext = nextprevGroup;
    m_hGroup = hGroup;
}

ImageView::~ImageView()
{
    odebug << "Delete Imageview" << oendl;
    delete m_viewManager;
}

Opie::Core::OKeyConfigManager* ImageView::manager()
{
    if (!m_viewManager) {
        initKeys();
    }
    return m_viewManager;
}

void ImageView::startSlide(int value)
{
    if (!m_slideTimer) {
        m_slideTimer = new QTimer(this);
    }
    m_slideValue=value;
    connect(m_slideTimer,SIGNAL(timeout()),SLOT(nextSlide()));
    m_slideTimer->start(m_slideValue*1000,true);
}

void ImageView::stopSlide()
{
    if (!m_slideTimer) {
        return;
    }
    m_slideTimer->stop();
    delete m_slideTimer;
    m_slideTimer = 0;
}

void ImageView::nextSlide()
{
    if (!m_slideTimer) {
        return;
    }
    if (isHidden()) {
        delete m_slideTimer;
        m_slideTimer = 0;
        return;
    }
    emit dispNext();
    m_slideTimer->start(m_slideValue*1000,true);
}
void ImageView::initKeys()
{
    odebug << "init imageview keys" << oendl;
    if (!m_cfg) {
        m_cfg = new Opie::Core::OConfig("opie-eye");
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

    /**
     * Handle KeyEvents when they're pressed. This avoids problems
     * with 'double next' on Return.
     * The Return press would switch to this view and the return
     * release would emit the dispNext Signal.
     */
    m_viewManager->setEventMask( Opie::Core::OKeyConfigManager::MaskPressed );

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
    odebug << "Popup " << oendl;
    QPopupMenu *m = new QPopupMenu(0);
    if (!m) return;
    if (m_hGroup) {
        m_hGroup->addTo(m);
    }
    if (fullScreen()) {
        if (m_gPrevNext) {
            m->insertSeparator();
            m_gPrevNext->addTo(m);
        }
        if (m_gDisplayType) {
            m->insertSeparator();
            m_gDisplayType->addTo(m);
        }
    }
    m->setFocus();
    m->exec( QPoint( QCursor::pos().x(), QCursor::pos().y()) );
    if (m_hGroup) {
        m_hGroup->removeFrom(m);
    }
    if (m_gPrevNext) {
        m_gPrevNext->removeFrom(m);
    }
    if (m_gDisplayType) {
        m_gDisplayType->removeFrom(m);
    }
    delete m;
}

void ImageView::setFullScreen(bool how,bool force)
{
    m_isFullScreen = how;
    if (how) {
        m_ignore_next_in = true;
        setFixedSize(qApp->desktop()->size());
        if (force) showFullScreen();
    } else {
        setMinimumSize(10,10);
    }
}

void ImageView::focusInEvent(QFocusEvent *)
{
      // Always do it here, no matter the size.
    odebug << "Focus in (view)" << oendl;
    //if (fullScreen()) parentWidget()->showNormal();
    if (m_ignore_next_in){m_ignore_next_in=false;return;}
    if (fullScreen()) enableFullscreen();
}

void ImageView::hide()
{
    if (fullScreen()) {
        m_ignore_next_in = true;
        showNormal();
    }
    QWidget::hide();
}
void ImageView::enableFullscreen()
{
      if (!fullScreen()) return;
      if (m_ignore_next_in) {m_ignore_next_in = false;return;}

      setUpdatesEnabled(false);
      // This is needed because showNormal() forcefully changes the window
      // style to WSTyle_TopLevel.
      reparent(0, WStyle_Customize | WStyle_NoBorder, QPoint(0,0));
      // Enable fullscreen.
      /* this is the trick - I don't now why, but after a showFullScreen QTE toggles the focus
       * so we must block it here! */
      m_ignore_next_in = true;
      showFullScreen();
      setUpdatesEnabled(true);
}
