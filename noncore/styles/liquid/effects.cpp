
#include <qimage.h>
#include <qpixmap.h>
#include <qcolor.h>

#include "effects.h"



QImage & QImageEffect::fade (QImage & img, float val, const QColor & color)
{
	if ( img. width ( ) == 0 || img. height ( ) == 0 )
		return img;

// We don't handle bitmaps
      if (img.depth () == 1)
        return img;

    unsigned char
    tbl[256];
    for (int i = 0; i < 256; i++)
        tbl[i] = (int) (val * i + 0.5);

    int
    red =
    color.
    red ();
    int
    green =
    color.
    green ();
    int
    blue =
    color.
    blue ();

    QRgb col;
    int
    r,
    g,
    b,
    cr,
    cg,
    cb;

    if (img.depth () <= 8)
    {
          			// pseudo color
	    for (int i = 0; i < img.numColors (); i++)
	{
	        col = img.color (i);
	        cr = qRed (col);
	  cg = qGreen (col);
		cb=qBlue(col);
			        if (cr > red)
	            r = cr - tbl[cr - red];
	        
	  else
	            r = cr + tbl[red - cr];
	        if (cg > green)
	            g = cg - tbl[cg - green];
	        
	  else
	            g = cg + tbl[green - cg];
	        if (cb > blue)
	            b = cb - tbl[cb - blue];
	        
	  else
	            b = cb + tbl[blue - cb];
	        img.setColor (i, qRgb (r, g, b));
	    }

      }
  else
    {
          			// truecolor
	    for (int y = 0; y < img.height (); y++)
	{
	        QRgb * data = (QRgb *) img.scanLine (y);
	        for (int x = 0; x < img.width (); x++)
	    {
	              col = *data;
	              cr = qRed (col);
cg=qGreen(col);
cb=qBlue(col);
	              if (cr > red)
		          r = cr - tbl[cr - red];
	              
	      else
		          r = cr + tbl[red - cr];
	              if (cg > green)
		          g = cg - tbl[cg - green];
	              
	      else
		          g = cg + tbl[green - cg];
	              if (cb > blue)
		          b = cb - tbl[cb - blue];
	              
	      else
		          b = cb + tbl[blue - cb];
	              *data++ = qRgb (r, g, b);
	          }
	    }
      }

    return img;
}



QPixmap& QPixmapEffect::fade(QPixmap &pixmap, float val, const QColor &color)
{
	QImage img = pixmap.convertToImage();
	QImageEffect::fade(img, val, color);
	pixmap.convertFromImage(img);
   
	return pixmap;
}
