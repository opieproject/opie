/**********************************************************************
** Copyright (C) 2000 Trolltech AS.  All rights reserved.
**
** This file is part of Qtopia Environment.
**
** Licensees holding valid Qtopia Developer license may use this
** file in accordance with the Qtopia Developer License Agreement
** provided with the Software.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING
** THE WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR
** PURPOSE.
**
** email sales@trolltech.com for information about Qtopia License
** Agreements.
**
** Contact info@trolltech.com if any conditions of this licensing are
** not clear to you.
**
**********************************************************************/
#ifndef QPC_RECORD_FIELDS_H
#define QPC_RECORD_FIELDS_H
#include "qpcglobal.h"

// dataset = "addressbook"
namespace Qtopia
{
    static const int UID_ID = 0;
    static const int CATEGORY_ID = 1;

    enum AddressBookFields {
	AddressUid = UID_ID,	
	AddressCategory = CATEGORY_ID,

	// NOTE: Order of fields dependency in backend/contact.cpp

	Title,
	FirstName,
	MiddleName,
	LastName,
	Suffix,
	FileAs,

	JobTitle,
	Department,
	Company,
	BusinessPhone,
	BusinessFax,
	BusinessMobile,

	// email
	DefaultEmail,
	Emails,

	HomePhone,
	HomeFax,
	HomeMobile,

	// business
	BusinessStreet,
	BusinessCity,
	BusinessState,
	BusinessZip,
	BusinessCountry,
	BusinessPager,
	BusinessWebPage,

	Office,
	Profession,
	Assistant,
	Manager,

	// home
	HomeStreet,
	HomeCity,
	HomeState,
	HomeZip,
	HomeCountry,
	HomeWebPage,

	//personal
	Spouse,
	Gender,
	Birthday,
	Anniversary,
	Nickname,
	Children,

	// other
	Notes,
	Groups

	,rid,
	rinfo
    };

    // dataset = "todolist"
    enum TaskFields {
	TaskUid = UID_ID,
	TaskCategory = CATEGORY_ID,
	
	HasDate,
	Completed,
	TaskDescription,
	Priority,
	Date,

	TaskRid,
	TaskRinfo
    };

    // dataset = "categories" for todos
    enum CategoryFields {
	CatUid = UID_ID,
	CatName,
	CatAppGroup
    };


// dataset = "datebook"
    enum DatebookFields {
	DatebookUid = UID_ID,
	DatebookCategory = CATEGORY_ID,
	
	DatebookDescription,
	Location,
	TimeZone,
	Note,
	StartDateTime,
	EndDateTime,
	DatebookType,
	HasAlarm,
	SoundType,
	AlarmTime,

	RepeatPatternType,
	RepeatPatternFrequency,
	RepeatPatternPosition,
	RepeatPatternDays,
	RepeatPatternHasEndDate,
	RepeatPatternEndDate,	

	DateBookRid,
	DateBookRinfo
    };
};


#endif
