#ifndef MINIKDE_KDIALOGBASE_H
#define MINIKDE_KDIALOGBASE_H

#include <qframe.h>

#include "kdialog.h"

class QPushButton;
class QLayout;
class QTabWidget;
class QBoxLayout;

class KDialogBase : public KDialog
{
    Q_OBJECT
  public:
    enum ButtonCode
    {
      Help    = 0x00000001,
      Default = 0x00000002,
      Ok      = 0x00000004,
      Apply   = 0x00000008,
      Try     = 0x00000010,
      Cancel  = 0x00000020,
      Close   = 0x00000040,
      User1   = 0x00000080,
      User2   = 0x00000100,
      User3   = 0x00000200,
      No      = 0x00000080,
      Yes     = 0x00000100,
      Details = 0x00000400,
      Filler  = 0x40000000,
      Stretch = 0x80000000
    };

    enum DialogType
    {
      TreeList,
      Tabbed,
      Plain, 
      Swallow,
      IconList
    };

    KDialogBase();
    KDialogBase( QWidget *parent=0, const char *name=0, bool modal=true,
		 const QString &caption=QString::null,
		 int buttonMask=Ok|Apply|Cancel, ButtonCode defaultButton=Ok,
		 bool separator=false, 
		 const QString &user1=QString::null,
		 const QString &user2=QString::null,
		 const QString &user3=QString::null);
    KDialogBase( int dialogFace, const QString &caption, 
		 int buttonMask, ButtonCode defaultButton, 
		 QWidget *parent=0, const char *name=0, bool modal=true, 
		 bool separator=false,
		 const QString &user1=QString::null,
		 const QString &user2=QString::null,
		 const QString &user3=QString::null);
    virtual ~KDialogBase();
    
    QFrame *addPage( const QString & );
    QFrame *addPage( const QString &, int, const QPixmap & );

    void setMainWidget( QWidget *widget );

    void enableButton( ButtonCode id, bool state );
    void enableButtonOK( bool state );
    void enableButtonApply( bool state );

    int pageIndex( QWidget *widget ) const;

    bool showPage( int index );

    QFrame *plainPage();

  signals:
    void user1Clicked();

  protected slots:
    virtual void slotOk();
    virtual void slotApply();
    virtual void slotCancel();
    virtual void slotClose();
    virtual void slotUser1();

  private:
    QTabWidget *tabWidget();
    void init( const QString &caption, int buttonMask,
               const QString &user1=QString::null );
    void initLayout();

    QWidget *mMainWidget;
    QTabWidget *mTabWidget;
    QFrame *mPlainPage;
    QBoxLayout *mTopLayout;
    
    QPushButton *mUser1Button;
    QPushButton *mCloseButton;
    QPushButton *mOkButton;
    QPushButton *mApplyButton;
    QPushButton *mCancelButton;    
};

#endif
