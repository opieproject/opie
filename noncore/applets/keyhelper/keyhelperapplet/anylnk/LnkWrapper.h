#ifndef _LNK_WRAPPER_H_
#define _LNK_WRAPPER_H_

#include <qstring.h>
#include <qstringlist.h>

#include <qpe/applnk.h>

#include "AppLnkWrapper.h"
#include "DocLnkWrapper.h"
#include "ExecLnk.h"
#include "QCopLnk.h"
#include "TextLnk.h"
#include "MenuLnk.h"

class LnkWrapper
{
public:
	LnkWrapper(const QStringList& params);
	virtual ~LnkWrapper();

	bool isValid(){
		return(m_pLnk && m_pLnk->isValid());
	}
	AnyLnk& instance(){
		return(*m_pLnk);
	}
private:
	AnyLnk* m_pLnk;
};

#endif /* _LNK_WRAPPER_H_ */

