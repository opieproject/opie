#ifndef __PREVIEW_H__
#define __PREVIEW_H__

#include <qwidget.h>

#include <qpe/windowdecorationinterface.h>

class QVBox;
class QPopupMenu;
class SampleText;

class SampleWindow : public QWidget
{
    Q_OBJECT
public:
    SampleWindow( QWidget *parent );

    QSize sizeHint() const;

    virtual void setFont( const QFont &f );

	void setStyle2 ( QStyle *sty );
    void setDecoration( WindowDecorationInterface *i );
    void setPalette ( const QPalette & );

    virtual void paintEvent( QPaintEvent * );

    void init();

    virtual bool eventFilter( QObject *, QEvent *e );
    virtual void paletteChange( const QPalette &old );
    virtual void resizeEvent( QResizeEvent *re );

public slots:
    void fixGeometry();

protected:
    WindowDecorationInterface *iface;
    WindowDecorationInterface::WindowData wd;
    QVBox *container;
    QPopupMenu *popup;
    int th;
    int tb;
    int lb;
    int rb;
    int bb;
};

#endif
