/**********************************************************************
** Copyright (C) 2000 Trolltech AS.  All rights reserved.
**
** This file is part of Qtopia Environment.
**
** This file may be distributed and/or modified under the terms of the
** GNU General Public License version 2 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
** See http://www.trolltech.com/gpl/ for GPL licensing information.
**
** Contact info@trolltech.com if any conditions of this licensing are
** not clear to you.
**
**********************************************************************/
#ifndef LAYERDATA_H
#define LAYERDATA_H

typedef struct 
{
/* sequence header */
	int intra_quantizer_matrix[64], non_intra_quantizer_matrix[64];
	int chroma_intra_quantizer_matrix[64], chroma_non_intra_quantizer_matrix[64];
	int mpeg2;
	int qscale_type, altscan;      /* picture coding extension */
	int pict_scal;                /* picture spatial scalable extension */
	int scalable_mode;            /* sequence scalable extension */
} mpeg3_layerdata_t;


#endif
