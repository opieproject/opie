/****************************************************************************
** $Id: main.cpp,v 1.1.8.1.2.1 2003-08-29 06:50:40 harlekin Exp $
**
** Copyright (C) 1992-2000 Trolltech AS.  All rights reserved.
**
** This file is part of an example program for Qt.  This example
** program may be used, distributed and modified without limitation.
**
*****************************************************************************/

#include <qpe/qpeapplication.h>
#include <opie/oapplicationfactory.h>
#include "tictac.h"

OPIE_EXPORT_APP( OApplicationFactory<TicTacToe> )
