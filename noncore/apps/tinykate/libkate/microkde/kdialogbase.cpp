#include <qtabwidget.h>
#include <qpushbutton.h>
#include <qlayout.h>
#include <qframe.h>

#include "klocale.h"
#include "kdebug.h"

#include "kdialogbase.h"

KDialogBase::KDialogBase()
{
}

KDialogBase::KDialogBase( QWidget *parent, const char *name, bool modal,
             const QString &caption,
             int buttonMask, ButtonCode defaultButton,
             bool separator, 
             const QString &user1,
             const QString &user2,
             const QString &user3) :
    KDialog( parent, name, modal )
{
  init( caption, buttonMask, user1 );
}

KDialogBase::KDialogBase( int dialogFace, const QString &caption, 
             int buttonMask, ButtonCode defaultButton, 
             QWidget *parent, const char *name, bool modal, 
             bool separator,
             const QString &user1,
             const QString &user2,
             const QString &user3) :
    KDialog( parent, name, modal )
{
  init( caption, buttonMask, user1 );
}

KDialogBase::~KDialogBase()
{
}

void KDialogBase::init( const QString &caption, int buttonMask,
                        const QString &user1 )
{
  mMainWidget = 0;
  mTabWidget = 0;
  mPlainPage = 0;
  mTopLayout = 0;

  if ( !caption.isEmpty() ) {
    setCaption( caption );
  }

  if ( buttonMask & User1 ) {
    mUser1Button = new QPushButton( user1, this );
    connect( mUser1Button, SIGNAL( clicked() ), SLOT( slotUser1() ) );
  } else {
    mUser1Button = 0;
  }

  if ( buttonMask & Ok ) {
    mOkButton = new QPushButton( i18n("Ok"), this );
    connect( mOkButton, SIGNAL( clicked() ), SLOT( slotOk() ) );
  } else {
    mOkButton = 0;
  }

  if ( buttonMask & Apply ) {
    mApplyButton = new QPushButton( i18n("Apply"), this );
    connect( mApplyButton, SIGNAL( clicked() ), SLOT( slotApply() ) );
  } else {
    mApplyButton = 0;
  }

  if ( buttonMask & Cancel ) {
    mCancelButton = new QPushButton( i18n("Cancel"), this );
    connect( mCancelButton, SIGNAL( clicked() ), SLOT( slotCancel() ) );
  } else {
    mCancelButton = 0;
  }

  if ( buttonMask & Close ) {
    mCloseButton = new QPushButton( i18n("Close"), this );
    connect( mCloseButton, SIGNAL( clicked() ), SLOT( slotClose() ) );
  } else {
    mCloseButton = 0;
  }
}

QTabWidget *KDialogBase::tabWidget()
{
  if ( !mTabWidget ) {
    mTabWidget = new QTabWidget( this );
    setMainWidget( mTabWidget );
  }
  return mTabWidget;
}

void KDialogBase::initLayout()
{
  delete mTopLayout;
  mTopLayout = new QVBoxLayout( this );
  mTopLayout->setMargin( marginHint() );
  mTopLayout->setSpacing( spacingHint() );
  
  mTopLayout->addWidget( mMainWidget );
  
  QBoxLayout *buttonLayout = new QHBoxLayout;
  mTopLayout->addLayout( buttonLayout );
  
  if ( mUser1Button ) buttonLayout->addWidget( mUser1Button );
  if ( mOkButton ) buttonLayout->addWidget( mOkButton );
  if ( mApplyButton ) buttonLayout->addWidget( mApplyButton );
  if ( mCancelButton ) buttonLayout->addWidget( mCancelButton );
  if ( mCloseButton ) buttonLayout->addWidget( mCloseButton );
}

QFrame *KDialogBase::addPage( const QString &name )
{
//  kdDebug() << "KDialogBase::addPage(): " << name << endl;

  QFrame *frame = new QFrame( tabWidget() );
  tabWidget()->addTab( frame, name );
  return frame;
}

QFrame *KDialogBase::addPage( const QString &name, int, const QPixmap & )
{
  return addPage( name );
}


void KDialogBase::setMainWidget( QWidget *widget )
{
  kdDebug() << "KDialogBase::setMainWidget()" << endl;

  mMainWidget = widget;
  initLayout();
}


void KDialogBase::enableButton( ButtonCode id, bool state )
{
  QPushButton *button = 0;
  switch ( id ) {
    case Ok:
      button = mOkButton;
      break;
    case Apply:
      button = mApplyButton;
      break;
    default:
      break;      
  }
  if ( button ) {
    button->setEnabled( state );
  }
}

void KDialogBase::enableButtonOK( bool state )
{
  enableButton( Ok, state );
}

void KDialogBase::enableButtonApply( bool state )
{
  enableButton( Apply, state );
}


int KDialogBase::pageIndex( QWidget *widget ) const
{
  return 0;
}


bool KDialogBase::showPage( int index )
{
  return false;
}

QFrame *KDialogBase::plainPage()
{
  if ( !mPlainPage ) {
    mPlainPage = new QFrame( this );
    setMainWidget( mPlainPage );
  }
  return mPlainPage;
}

void KDialogBase::slotOk()
{
  accept();
}

void KDialogBase::slotApply()
{
}

void KDialogBase::slotCancel()
{
  reject();
}

void KDialogBase::slotClose()
{
  accept();
}

void KDialogBase::slotUser1()
{
  emit user1Clicked();
}
