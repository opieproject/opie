#ifndef __EFFECTS_H__
#define __EFFECTS_H__

class QImage;
class QPixmap;
class QColor;

class QImageEffect {
public:	
	static QImage &fade ( QImage &, float, const QColor & );
};

class QPixmapEffect {
public:	
	static QPixmap &fade ( QPixmap &, float, const QColor & );
};


#endif

