/*
  This is based on code and ideas of
  L. J. Potter ljp@llornkcor.com
  Thanks a lot


               =.            This file is part of the OPIE Project
             .=l.            Copyright (c)  2002,2003 Holger Freyther <zecke@handhelds.org>
           .>+-=
 _;:,     .>    :=|.         This library is free software; you can
.> <`_,   >  .   <=          redistribute it and/or  modify it under
:`=1 )Y*s>-.--   :           the terms of the GNU Library General Public
.="- .-=="i,     .._         License as published by the Free Software
 - .   .-<_>     .<>         Foundation; either version 2 of the License,
     ._= =}       :          or (at your option) any later version.
    .%`+i>       _;_.
    .i_,=:_.      -<s.       This library is distributed in the hope that
     +  .  -:.       =       it will be useful,  but WITHOUT ANY WARRANTY;
    : ..    .:,     . . .    without even the implied warranty of
    =_        +     =;=|`    MERCHANTABILITY or FITNESS FOR A
  _.=:.       :    :=>`:     PARTICULAR PURPOSE. See the GNU
..}^=.=       =       ;      Library General Public License for more
++=   -.     .`     .:       details.
 :     =  ...= . :.=-
 -.   .:....=;==+<;          You should have received a copy of the GNU
  -_. . .   )=.  =           Library General Public License along with
    --        :-=`           this library; see the file COPYING.LIB.
                             If not, write to the Free Software Foundation,
                             Inc., 59 Temple Place - Suite 330,
                             Boston, MA 02111-1307, USA.

*/


#ifndef OPIE_OFILESELECTOR_FOO_H
#define OPIE_OFILESELECTOR_FOO_H

#include <qlist.h>
#include <qwidget.h>
#include <qmap.h>
#include <qvaluelist.h>
#include <qstringlist.h>

#include <qpe/applnk.h>

typedef QMap<QString, QStringList> MimeTypes;

class OFileViewInterface;
class OFileViewFileListView;
class QLineEdit;
class QComboBox;
class QWidgetStack;
class QHBox;
class OFileSelector : public QWidget {
    Q_OBJECT
    friend class OFileViewInterface;
    friend class OFileViewFileListView;
public:
    enum Mode { Open=1, Save=2, FileSelector=4, OPEN=1, SAVE=2, FILESELECTOR=4 };
//    enum OldMode { OPEN=1, SAVE=2, FILESELECTOR = 4 };
    enum Selector { Normal = 0, Extended=1,  ExtendedAll =2, Default=3,  NORMAL=0,EXTENDED=1, EXTENDED_ALL =2, DEFAULT=3 };
//    enum OldSelector { NORMAL = 0, EXTENDED =1, EXTENDED_ALL = 2};

    OFileSelector(QWidget* parent, int mode, int selector,
                  const QString& dirName,
                  const QString& fileName,
                  const MimeTypes& mimetypes = MimeTypes(),
                  bool newVisible = FALSE, bool closeVisible = FALSE );

    OFileSelector(const QString& mimeFilter, QWidget* parent,
                  const char* name = 0,  bool newVisible = TRUE, bool closeVisible = FALSE );
    ~OFileSelector();

    const DocLnk* selected();

    QString selectedName()const;
    QString selectedPath()const;
    QString directory()const;

    DocLnk selectedDocument()const;

    int fileCount()const;
    void reread();

    int mode()const;
    int selector()const;


    void setNewVisible( bool b );
    void setCloseVisible( bool b );
    void setNameVisible( bool b );

signals:
    void dirSelected( const QString& );
    void fileSelected( const DocLnk& );
    void fileSelected( const QString& );
    void newSelected( const DocLnk& );
    void closeMe();
    void ok();
    void cancel();

/* used by the ViewInterface */
private:
    bool showNew()const;
    bool showClose()const;
    MimeTypes mimeTypes()const;
    QStringList currentMimeType()const;

private:
    /* inits the Widgets */
    void initUI();
    /* inits the MimeType ComboBox content + connects signals and slots */
    void initMime();
    /* init the Views :) */
    void initViews();

private:
    QLineEdit* m_lneEdit; // the LineEdit for the Name
    QComboBox *m_cmbView, *m_cmbMime; // two ComboBoxes to select the View and MimeType
    QWidgetStack* m_stack;  // our widget stack which will contain the views
    OFileViewInterface* currentView()const; // returns the currentView
    OFileViewInterface* m_current; // here is the view saved
    bool m_shNew   : 1; // should we show New?
    bool m_shClose : 1; // should we show Close?
    MimeTypes m_mimeType; // list of mimetypes

    QMap<QString, OFileViewInterface*> m_views; // QString translated view name + ViewInterface Ptr
    QHBox* m_nameBox; // the LineEdit + Label is hold here
    QHBox* m_cmbBox; // this holds the two combo boxes

    QString m_startDir;
    int m_mode;
    int m_selector;

    struct Data; // used for future versions
    Data *d;

private slots:
    void slotMimeTypeChanged();

    /* will set the text of the lineedit and emit a fileChanged signal */
    void slotDocLnkBridge( const DocLnk& );
    void slotFileBridge( const QString& );
    void slotViewChange( const QString& );

    bool eventFilter (QObject *o, QEvent *e);

};

#endif
