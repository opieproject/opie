/* This file is part of the KDE libraries
    Copyright (C) 1997 Martin Jones (mjones@kde.org)

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License as published by the Free Software Foundation; either
    version 2 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public License
    along with this library; see the file COPYING.LIB.  If not, write to
    the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
    Boston, MA 02111-1307, USA.
*/
//-----------------------------------------------------------------------------
// Selector widgets for KDE Color Selector, but probably useful for other
// stuff also.

#ifndef __OSELECT_H__
#define __OSELECT_H__

#include <qwidget.h>
#include <qrangecontrol.h>
#include <qpixmap.h>


namespace Opie {
namespace Ui   {
/**
 * OXYSelector is the base class for other widgets which
 * provides the ability to choose from a two-dimensional
 * range of values. The currently chosen value is indicated
 * by a cross. An example is the @ref OHSSelector which
 * allows to choose from a range of colors, and which is
 * used in OColorDialog.
 *
 * A custom drawing routine for the widget surface has
 * to be provided by the subclass.
 */
class OXYSelector : public QWidget
{
  Q_OBJECT

public:
  /**
   * Constructs a two-dimensional selector widget which
   * has a value range of [0..100] in both directions.
   */
  OXYSelector( QWidget *parent=0, const char *name=0 );
  /**
   * Destructs the widget.
   */
  ~OXYSelector();

  /**
   * Sets the current values in horizontal and
   * vertical direction.
   */
  void setValues( int xPos, int yPos );
  /**
   * Sets the range of possible values.
   */
  void setRange( int minX, int minY, int maxX, int maxY );

  /**
   * @return the current value in horizontal direction.
   */
  int xValue() const {	return xPos; }
  /**
   * @return the current value in vertical direction.
   */
  int yValue() const {	return yPos; }

  /**
   * @return the rectangle on which subclasses should draw.
   */
  QRect contentsRect() const;

signals:
  /**
   * This signal is emitted whenever the user chooses a value,
   * e.g. by clicking with the mouse on the widget.
   */
  void valueChanged( int x, int y );

protected:
  /**
   * Override this function to draw the contents of the widget.
   * The default implementation does nothing.
   *
   * Draw within @ref contentsRect() only.
   */
  virtual void drawContents( QPainter * );
  /**
   * Override this function to draw the cursor which
   * indicates the currently selected value pair.
   */
  virtual void drawCursor( QPainter *p, int xp, int yp );
  /**
   * @reimplemented
   */
  virtual void paintEvent( QPaintEvent *e );
  /**
   * @reimplemented
   */
  virtual void mousePressEvent( QMouseEvent *e );
  /**
   * @reimplemented
   */
  virtual void mouseMoveEvent( QMouseEvent *e );
  /**
   * @reimplemented
   */
  virtual void wheelEvent( QWheelEvent * );
  /**
   * Converts a pixel position to its corresponding values.
   */
  void valuesFromPosition( int x, int y, int& xVal, int& yVal ) const;

private:
  void setPosition( int xp, int yp );
  int px;
  int py;
  int xPos;
  int yPos;
  int minX;
  int maxX;
  int minY;
  int maxY;
  QPixmap store;

private:
  class OXYSelectorPrivate;
  OXYSelectorPrivate *d;
};


/**
 * OSelector is the base class for other widgets which
 * provides the ability to choose from a one-dimensional
 * range of values. An example is the @ref OGradientSelector
 * which allows to choose from a range of colors.
 *
 * A custom drawing routine for the widget surface has
 * to be provided by the subclass.
 */
class OSelector : public QWidget, public QRangeControl
{
  Q_OBJECT
  Q_PROPERTY( int value READ value WRITE setValue )
  Q_PROPERTY( int minValue READ minValue WRITE setMinValue )
  Q_PROPERTY( int maxValue READ maxValue WRITE setMaxValue )
public:

  /**
   * Constructs a horizontal one-dimensional selection widget.
   */
  OSelector( QWidget *parent=0, const char *name=0 );
  /**
   * Constructs a one-dimensional selection widget with
   * a given orientation.
   */
  OSelector( Orientation o, QWidget *parent = 0L, const char *name = 0L );
  /*
   * Destructs the widget.
   */
  ~OSelector();

  /**
   * @return the orientation of the widget.
   */
  Orientation orientation() const
  {	return _orientation; }

  /**
   * @return the rectangle on which subclasses should draw.
   */
  QRect contentsRect() const;

  /**
   * Sets the indent option of the widget to i.
   * This determines whether a shaded frame is drawn.
   */
  void setIndent( bool i )
  {	_indent = i; }
  /**
   * @return whether the indent option is set.
   */
  bool indent() const
  {	return _indent; }

  /**
   * Sets the value.
   */
  void setValue(int value)
  { QRangeControl::setValue(value); }

  /**
   * @returns the value.
   */
  int value() const
  { return QRangeControl::value(); }

  /**
   * Sets the min value.
   */
  #if ( QT_VERSION > 290 )
  void setMinValue(int value) { QRangeControl::setMinValue(value); }
  #else
  void setMinValue(int value) { QRangeControl::setRange(value,QRangeControl::maxValue()); }
  #endif

  /**
   * @return the min value.
   */
  int minValue() const
  { return QRangeControl::minValue(); }

  /**
   * Sets the max value.
   */
  #if ( QT_VERSION > 290 )
  void setMaxValue(int value) { QRangeControl::setMaxValue(value); }
  #else
  void setMaxValue(int value) { QRangeControl::setRange(QRangeControl::minValue(),value); }
  #endif

  /**
   * @return the max value.
   */
  int maxValue() const
  { return QRangeControl::maxValue(); }

signals:
  /**
   * This signal is emitted whenever the user chooses a value,
   * e.g. by clicking with the mouse on the widget.
   */
  void valueChanged( int value );

protected:
  /**
   * Override this function to draw the contents of the control.
   * The default implementation does nothing.
   *
   * Draw only within contentsRect().
   */
  virtual void drawContents( QPainter * );
  /**
   * Override this function to draw the cursor which
   * indicates the current value. This function is
   * always called twice, once with argument show=false
   * to clear the old cursor, once with argument show=true
   * to draw the new one.
   */
  virtual void drawArrow( QPainter *painter, bool show, const QPoint &pos );

  /**
   * @reimplemented
   */
  virtual void valueChange();
  /**
   * @reimplemented
   */
  virtual void paintEvent( QPaintEvent * );
  /**
   * @reimplemented
   */
  virtual void mousePressEvent( QMouseEvent *e );
  /**
   * @reimplemented
   */
  virtual void mouseMoveEvent( QMouseEvent *e );
  /**
   * @reimplemented
   */
  virtual void wheelEvent( QWheelEvent * );

private:
  QPoint calcArrowPos( int val );
  void moveArrow( const QPoint &pos );

  Orientation _orientation;
  bool _indent;

private:
  class OSelectorPrivate;
  OSelectorPrivate *d;
};


/**
 * The OGradientSelector widget allows the user to choose
 * from a one-dimensional range of colors which is given as a
 * gradient between two colors provided by the programmer.
 */
class OGradientSelector : public OSelector
{
  Q_OBJECT

  Q_PROPERTY( QColor firstColor READ firstColor WRITE setFirstColor )
  Q_PROPERTY( QColor secondColor READ secondColor WRITE setSecondColor )
  Q_PROPERTY( QString firstText READ firstText WRITE setFirstText )
  Q_PROPERTY( QString secondText READ secondText WRITE setSecondText )

public:
  /**
   * Constructs a horizontal color selector which
   * contains a gradient between white and black.
   */
  OGradientSelector( QWidget *parent=0, const char *name=0 );
  /**
   * Constructs a colors selector with orientation o which
   * contains a gradient between white and black.
   */
  OGradientSelector( Orientation o, QWidget *parent=0, const char *name=0 );
  /**
   * Destructs the widget.
   */
  ~OGradientSelector();
  /**
   * Sets the two colors which span the gradient.
   */
  void setColors( const QColor &col1, const QColor &col2 )
  {	color1 = col1; color2 = col2; update();}
  void setText( const QString &t1, const QString &t2 )
  {	text1 = t1; text2 = t2; update(); }

  /**
   * Set each color on its own.
   */
  void setFirstColor( const QColor &col )
  { color1 = col; update(); }
  void setSecondColor( const QColor &col )
  { color2 = col; update(); }

  /**
   * Set each description on its own
   */
  void setFirstText( const QString &t )
  { text1 = t; update(); }
  void setSecondText( const QString &t )
  { text2 = t; update(); }

  const QColor firstColor() const
  { return color1; }
  const QColor secondColor() const
  { return color2; }

  const QString firstText() const
  { return text1; }
  const QString secondText() const
  { return text2; }

protected:
  /**
   * @reimplemented
   */
  virtual void drawContents( QPainter * );

  /**
   * @reimplemented
   */
  virtual QSize minimumSize() const
  { return sizeHint(); }

private:
  void init();
  QColor color1;
  QColor color2;
  QString text1;
  QString text2;

private:
  class OGradientSelectorPrivate;
  OGradientSelectorPrivate *d;
};

/**
 * Widget for Hue/Saturation selection.
 * The actual values can be fetched using the inherited xValue and yValue
 * methods.
 *
 * @see OXYSelector, OValueSelector, OColorDialog
 * @author Martin Jones (mjones@kde.org)
 * @version $Id: oselector.h,v 1.2 2004-03-13 19:51:50 zecke Exp $
*/
class OHSSelector : public OXYSelector
{
  Q_OBJECT

public:
  /**
   * Constructs a hue/saturation selection widget.
   */
  OHSSelector( QWidget *parent=0, const char *name=0 );

protected:
  /**
   * Draws the contents of the widget on a pixmap,
   * which is used for buffering.
   */
  virtual void drawPalette( QPixmap *pixmap );
  /**
   * @reimplemented
   */
  virtual void resizeEvent( QResizeEvent * );
  /**
   * Reimplemented from OXYSelector. This drawing is
   * buffered in a pixmap here. As real drawing
   * routine, drawPalette() is used.
   */
  virtual void drawContents( QPainter *painter );

private:
  void updateContents();
  QPixmap pixmap;

private:
  class OHSSelectorPrivate;
  OHSSelectorPrivate *d;
};


class OValueSelectorPrivate;
/**
 * Widget for color value selection.
 *
 * @see OHSSelector, OColorDialog
 * @author Martin Jones (mjones@kde.org)
 * @version $Id: oselector.h,v 1.2 2004-03-13 19:51:50 zecke Exp $
 */
class OValueSelector : public OSelector
{
  Q_OBJECT

public:
  /**
   * Constructs a widget for color selection.
   */
  OValueSelector( QWidget *parent=0, const char *name=0 );
  /**
   * Constructs a widget for color selection with a given orientation
   */
  OValueSelector( Orientation o, QWidget *parent = 0, const char *name = 0 );

  int hue() const
        { return _hue; }
  void setHue( int h )
        { _hue = h; }
  int saturation() const
        { return _sat; }
  void setSaturation( int s )
        { _sat = s; }

  void updateContents();
protected:
  /**
   * Draws the contents of the widget on a pixmap,
   * which is used for buffering.
   */
  virtual void drawPalette( QPixmap *pixmap );
  /**
   * @reimplemented
   */
  virtual void resizeEvent( QResizeEvent * );
  /**
   * Reimplemented from OSelector. The drawing is
   * buffered in a pixmap here. As real drawing
   * routine, drawPalette() is used.
   */
  virtual void drawContents( QPainter *painter );

private:
  int _hue;
  int _sat;
  QPixmap pixmap;

private:
  class OValueSelectorPrivate;
  OValueSelectorPrivate *d;
};


class OColor : public QColor
{
public:
  OColor();
  OColor( const OColor &col);
  OColor( const QColor &col);

  OColor& operator=( const OColor& col);

  bool operator==( const OColor& col) const;

  void setHsv(int _h, int _s, int _v);
  void setRgb(int _r, int _g, int _b);

  void rgb(int *_r, int *_g, int *_b) const;
  void hsv(int *_h, int *_s, int *_v) const;
protected:
  int h;
  int s;
  int v;
  int r;
  int g;
  int b;

private:
  class OColorPrivate;
  OColorPrivate *d;
};

}
}


#endif		// __OSELECT_H__

