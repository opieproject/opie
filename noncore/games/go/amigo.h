/**********************************************************************
** Copyright (C) 2000-2002 Trolltech AS.  All rights reserved.
**
** This file is part of the Qtopia Environment.
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
/*=========================================================================
===									===
=== FILE	amigo.h							===
===									===
=== CONTENTS	prototypes for the various AmiGo routines.		===
===		added by neilb						===
===									===
=========================================================================*/

#ifndef __amigo_h
#define __amigo_h

#include "go.h"
#include "goplayutils.h"

#ifdef __STDC__
#define PROTO(fp)	fp
#else
#define PROTO(fp)	()
#endif


#ifdef __cplusplus
extern "C" {
#endif

/* From goplayer.c */



/* Procedures from amigo.c */

short	Connect PROTO((enum bVal, short, short, short[4], short[4], short *, short * ));
short	Maxlibs PROTO((short, short));
short	Suicide PROTO((enum bVal, short, short));
short	StoneLibs PROTO((short, short));
void	EraseMarks PROTO(());
short	GoPlaceStone PROTO((enum bVal, short, short));
void	GoRemoveStone PROTO((short, short));
void	MergeGroups PROTO((short, short));
void	DeleteGroup PROTO((short));
void	ReEvalGroups PROTO((enum bVal, short, short, short));
void	GroupCapture PROTO((short));
void	FixLibs PROTO((enum bVal, short, short, short));
int	CountUp PROTO((int*, int*));
/*void	main PROTO(());*/
void	goRestart PROTO((int));
void	RelabelGroups PROTO(());
short	CountAndMarkLibs PROTO((short, short));
void	CountLiberties PROTO((short));
void	CheckForEye PROTO((short, short, short[4], short, short *));
void	CountEyes PROTO(());
void	printGroupReport PROTO((short, short));


/* killable.c */

int	tryPlay PROTO(( short, short, short ));
int	sSpanGroup PROTO(( short, short, sPointList * ));
int	spanGroup PROTO(( short, short, pointList *));
int	pause PROTO(());

int	genState PROTO(());
int	initGPUtils PROTO(());
int	genBord PROTO((enum bVal));

short	genMove PROTO(( enum bVal, short *, short * ));
short	checkPos PROTO(( short, short, short ));
short	takeCorner PROTO(( short *, short * ));
short	extend PROTO(( short *, short * ));
short	noNbrs PROTO(( short, short ));
short	extend2 PROTO(( short *, short * ));
short	lookForSave PROTO(( short *, short * ));
short	lookForSaveN PROTO(( short *, short * ));
short	lookForKill PROTO(( short *, short * ));
short	doubleAtari PROTO(( short *, short * ));
short	lookForAttack PROTO(( short *, short * ));
short	threaten PROTO(( short *, short * ));
short	connectCut PROTO(( short *, short * ));
short	heCanCut PROTO(( short, short ));
short	safeMove PROTO(( short, short ));
short	extendWall PROTO(( short *, short * ));
short	findAttack2 PROTO(( short *, short * ));
short	blockCut PROTO(( short *, short * ));
short	cutHim PROTO(( short *, short * ));
short	atariAnyway PROTO(( short *, short * ));
short	underCut PROTO(( short *, short * ));
short	dropToEdge PROTO(( short *, short * ));
short	pushWall PROTO(( short *, short * ));
short	reduceHisLiberties PROTO(( short *, short * ));
short	dropToEdge2 PROTO(( short *, short * ));


/* goplayutils.c */

short	saveable PROTO((short, short, short *, short *));
short	killable PROTO((short, short, short *, short *));
int	initBoolBoard PROTO((boolBoard));
int	intersectPlist PROTO((pointList *, pointList *, pointList *));
int	initArray PROTO((intBoard));
int	initState PROTO(());
int	copyArray PROTO((intBoard, intBoard));
int	stake PROTO(());
int	spread PROTO(());
int	respreicen PROTO(());
int	tryPlay PROTO((short, short, short));
int	saveState PROTO(());
int	restoreState PROTO(());
short	tencen PROTO((short, short));
int	genConnects PROTO(());
int	sortLibs PROTO(());


/*-- from xinterface.c --*/
void	removestone PROTO((short, short));
void	placestone PROTO((enum bVal, short, short));

void	intrMoveReport PROTO((enum bVal,char *,char *));
void	intrPrisonerReport PROTO(( short, short ));


#ifdef __cplusplus
}
#endif

    
#endif
