/*
 * TodoItem.h
 *
 * ---------------------
 *
 * begin       : Sun 10 17:20:00 CEST 2002
 * copyright   : (c) 2002 by Maximilian Reiﬂ
 * email       : max.reiss@gmx.de
 *
 */
/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/


#ifndef TODO_ITEM_H
#define TODO_ITEM_H

#include <qstring.h>

class TodoItem
{
	public:
		TodoItem(const char *description, int completed, int priority);
		QString getDescription();
		bool getCompleted();
		int getPriority();
	private:
		QString m_description;
		int m_priority;
		int m_completed;
};

#endif

