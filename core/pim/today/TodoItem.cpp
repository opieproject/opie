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


#include "TodoItem.h"

TodoItem::TodoItem(const char *description, int completed, int priority)
{
	m_description = description;
	m_completed = completed;
	m_priority = priority;
}

QString TodoItem::getDescription()
{
	return m_description;
}

bool TodoItem::getCompleted()
{
	return (m_completed == 1);
}

int TodoItem::getPriority()
{
	return m_priority;
}

