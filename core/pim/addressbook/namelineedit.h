/*
 * (C) 2004
 * GPLv2 zecke@handhelds.org
 *
 */

#ifndef ABOOK_NAME_LINE_EDIT
#define ABOOK_NAME_LINE_EDIT

#include <qlineedit.h>

namespace ABOOK {
    /**
     * small class to ease the input of names to capitalize them
     *
     */
    class NameLineEdit : public QLineEdit {
        Q_OBJECT
    public:
        NameLineEdit( QWidget* parent, const char* name = 0 );
        NameLineEdit( const QString& str, QWidget* par,
                      const char *name = 0);
        ~NameLineEdit();

    protected:
        void keyPressEvent( QKeyEvent* ev );

    private:
        bool m_prevSpace : 1;
    };
}

#endif
