#ifndef __QT_OVERRIDE_H__
#define __QT_OVERRIDE_H__

namespace Opie {
	
enum ForceAppearance {
	Force_Style      = 0x01,
	Force_Font       = 0x02,
	Force_Decoration = 0x04,
	
	Force_All        = 0xff, 
	Force_None       = 0x00,
};

extern const char *binaryName ( );

extern int force_appearance;

}

#endif
