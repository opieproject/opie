/*
 * GPLv2 zecke@handhelds.org
 * No WArranty...
 */

#ifndef PHUNK_ICON_VIEW_H
#define PHUNK_ICON_VIEW_H

#include <qvbox.h>

#include <qpe/config.h>

class QIconView;
class QIconViewItem;
class QComboBox;
class PIconViewItem;
class PDirLister;
class Ir;
class PIconView : public QVBox {
    Q_OBJECT
    friend class PIconViewItem;
public:
    PIconView( QWidget* wid, Config *cfg );
    ~PIconView();
    void resetView();

private:
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
    Config *m_cfg;
    QComboBox* m_views;
    QIconView* m_view;
    QString m_path;
};

#endif
