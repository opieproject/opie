/*
 * Copyright (C) 2004
 * LGPL v2  zecke@handhelds.org
 */


#ifndef ODP_KEY_CONFIG_WIDGET_H
#define ODP_KEY_CONFIG_WIDGET_H

#include <opie2/okeyconfigmanager.h>

#include <qhbox.h>


class QKeyEvent;
class QLabel;
class QPushButton;
class QListViewItem;
class QRadioButton;
class QTimer;

namespace Opie {
namespace Ui {
namespace Internal {
    class OKeyConfigWidgetPrivate;
    typedef QValueList<OKeyConfigWidgetPrivate> OKeyConfigWidgetPrivateList;
    class OKeyListViewItem;
}
    class OListViewItem;
    class OListView;


/**
 * With this Widget you can let the Keyboard Shortcuts
 * be configured by the user.
 * There are two ways you can use this widget. Either in a tab were
 * all changes are immediately getting into effect or in a queue
 * were you ask for saving. Save won't write the data but only set
 * it to the OKeyConfigManager
 *
 * @since 1.2
 */
class OKeyConfigWidget : public QWidget {
    Q_OBJECT

public:
    /**
     * Immediate Apply the change directly to the underlying OKeyConfigManager
     * Queue Save all items and then apply when you save()
     */
    enum ChangeMode { Imediate, Queue };
    OKeyConfigWidget( QWidget* parent = 0, const char* name = 0, WFlags fl = 0 );
    ~OKeyConfigWidget();

    void setChangeMode( enum ChangeMode );
    ChangeMode changeMode()const;

    void insert( const QString& name, Opie::Core::OKeyConfigManager* );

    void load();
    void save();

private slots:
    void slotListViewItem(  QListViewItem* );
    void slotNoKey();
    void slotDefaultKey();
    void slotCustomKey();
    void slotConfigure();

private:
    static bool sanityCheck(  Opie::Ui::Internal::OKeyListViewItem* man,
                              const Opie::Core::OKeyPair& newItem );
    void updateItem( Opie::Ui::Internal::OKeyListViewItem* man,
                     const Opie::Core::OKeyPair& newItem);
    void initUi();
    Opie::Ui::OListView *m_view;
    Opie::Ui::Internal::OKeyConfigWidgetPrivateList m_list;
    QLabel *m_lbl;
    QPushButton *m_btn;
    QRadioButton *m_def, *m_cus, *m_none;
    QWidget* m_box;
    ChangeMode m_mode;
    class Private;
    Private *d;
};


/**
 * This is a small dialog that allows you to
 * capture a key sequence.
 * If you want it to close after a key was captured you
 * can use this code snippet.
 *
 * \code
 *  OKeyChooserConfigDialog diag(0,0,true);
 *  connect(&diag,SIGNAL(keyCaptured()),
 *         this,SLOT(accept()));
 *  if( QPEApplication::execDialog(&diag) == QDialog::Accept ){
 *     take_the_key_and_do_something
 *  }
 *
 * \endcode
 *
 */
class OKeyChooserConfigDialog : public QDialog {
    Q_OBJECT
public:
    OKeyChooserConfigDialog( QWidget* parent = 0, const char* name = 0, bool modal = false, WFlags fl = 0 );
    ~OKeyChooserConfigDialog();

    Opie::Core::OKeyPair keyPair()const;

protected:
    void keyPressEvent( QKeyEvent* );
    void keyReleaseEvent( QKeyEvent* );

signals:
    void keyCaptured();

private slots:
    void  slotTimeUp();

private:
    QTimer *m_timer;
    QLabel *m_lbl;
    bool m_virtKey : 1;
    Opie::Core::OKeyPair m_keyPair;
    int m_key, m_mod;
    class Private;
    Private *d;
};

}
}


#endif
