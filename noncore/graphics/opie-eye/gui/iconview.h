/*
 * GPLv2 zecke@handhelds.org
 * No WArranty...
 */

#ifndef PHUNK_ICON_VIEW_H
#define PHUNK_ICON_VIEW_H

#include <qvbox.h>


class QIconView;
class QIconViewItem;
class QComboBox;
class PIconViewItem;
class PDirLister;
class Ir;

namespace Opie {
namespace Core{
    class OConfig;
}
namespace Ui {
    class OKeyConfigManager;
}
}

class PIconView : public QVBox {
    Q_OBJECT
    friend class PIconViewItem;
    enum ActionIds {
        BeamItem, DeleteItem, ViewItem, InfoItem
    };
public:
    PIconView( QWidget* wid, Opie::Core::OConfig *cfg );
    ~PIconView();
    void resetView();
    Opie::Ui::OKeyConfigManager* manager();

private:
    void initKeys();
    QString currentFileName(bool &isDir)const;
    void loadViews();

private slots:
    void slotDirUp();
    void slotChangeDir(const QString&);
    void slotTrash();
    void slotViewChanged( int );
    void slotReloadDir();
    void slotRename();
    void slotBeam();
    void slotBeamDone( Ir* );

    void slotShowImage();
    void slotShowImage( const QString& );
    void slotImageInfo();
    void slotImageInfo( const QString& );

    void slotStart();
    void slotEnd();

/* for performance reasons make it inline in the future */
    void addFolders(  const QStringList& );
    void addFiles( const QStringList& );
    void slotClicked(QIconViewItem* );

/**/
    void slotThumbInfo(const QString&, const QString&);
    void slotThumbNail(const QString&, const QPixmap&);
private:
    Opie::Ui::OKeyConfigManager *m_viewManager;
    Opie::Core::OConfig *m_cfg;
    QComboBox* m_views;
    QIconView* m_view;
    QString m_path;
    bool m_updatet : 1;
};

#endif
