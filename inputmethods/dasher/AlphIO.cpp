// AlphIO.cpp
//
/////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2002 Iain Murray
//
/////////////////////////////////////////////////////////////////////////////



#include "AlphIO.h"

using namespace Dasher;
using namespace std;

CAlphIO::CAlphIO(string SystemLocation, string UserLocation)
	: SystemLocation(SystemLocation), UserLocation(UserLocation),
	  BlankInfo(), CData("")
{
	CreateDefault();
}


void CAlphIO::GetAlphabets(std::vector< std::string > * AlphabetList) const
{
	AlphabetList->clear();
	
	typedef std::map<std::string, AlphInfo>::const_iterator CI;
	CI End = Alphabets.end();
	
	for (CI Cur=Alphabets.begin(); Cur!=End; Cur++)
		AlphabetList->push_back( (*Cur).second.AlphID);
}


const CAlphIO::AlphInfo& CAlphIO::GetInfo(const std::string& AlphID)
{
	if (AlphID=="")
		return Alphabets["Default"];
	else {
		AlphInfo& CurInfo = Alphabets[AlphID];
		Alphabets[AlphID].AlphID = AlphID; // Ensure consistency
		return Alphabets[AlphID];
	}
}


void CAlphIO::SetInfo(const AlphInfo& NewInfo)
{
	Alphabets[NewInfo.AlphID] = NewInfo;
}


void CAlphIO::Delete(const std::string& AlphID)
{
	if (Alphabets.find(AlphID)!=Alphabets.end()) {
		Alphabets.erase(AlphID);
	}
}


void CAlphIO::CreateDefault()
{
	// TODO I appreciate these strings should probably be in a resource file.
	// Not urgent though as this is not intended to be used. It's just a
	// last ditch effort in case file I/O totally fails.
	AlphInfo& Default = Alphabets["Default"];
	Default.AlphID = "Default";
	Default.Type = Opts::Western;
	Default.Mutable = false;
	Default.Orientation = Opts::LeftToRight;
	Default.SpaceCharacter.Display = "_";
	Default.SpaceCharacter.Text = " ";
	Default.TrainingFile = "training_english_GB.txt";
	string Chars = "abcdefghijklmnopqrstuvwxyz";
	Default.Groups.resize(1);
	Default.Groups[0].Description = "Lower case Latin letters";
	Default.Groups[0].Characters.resize(Chars.size());
	for (unsigned int i=0; i<Chars.size(); i++) {
		Default.Groups[0].Characters[i].Text = Chars[i];
		Default.Groups[0].Characters[i].Display = Chars[i];
	}
}
