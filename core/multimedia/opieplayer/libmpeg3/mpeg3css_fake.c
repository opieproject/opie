/* Stubs for deCSS which can't be distributed in source form */

#include "mpeg3css.h"
#include "mpeg3private.h"

int mpeg3_init_css(mpeg3_t *file, mpeg3_css_t *css)
{
	return 0;
}

int mpeg3_get_keys(mpeg3_css_t *css, char *path)
{
	return 1;
}

int mpeg3_decrypt_packet(mpeg3_css_t *css, unsigned char *sector)
{
	return 1;
}
