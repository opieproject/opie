#ifndef __STRINGTOKENIZER_H
#define __STRINGTOKENIZER_H

#include <vector>
using namespace std;

class StringTokenizer : public vector<string>
{
	public:
		StringTokenizer(const string &rStr, const string &rDelimiters = " ,\n")
		{
			string::size_type lastPos(rStr.find_first_not_of(rDelimiters, 0));
			string::size_type pos(rStr.find_first_of(rDelimiters, lastPos));
			while (string::npos != pos || string::npos != lastPos)
			{
				push_back(rStr.substr(lastPos, pos - lastPos));
				lastPos = rStr.find_first_not_of(rDelimiters, pos);
				pos = rStr.find_first_of(rDelimiters, lastPos);
			}
		}
};

#endif
