/**********************************************************************
** BenchmarkInfo
**
** A benchmark for Qt/Embedded
**
** Copyright (C) 2004 Michael Lauer <mickey@vanille.de>
** Inspired by ZBench (C) 2002 Satoshi <af230533@im07.alpha-net.ne.jp>
**
** This file may be distributed and/or modified under the terms of the
** GNU General Public License version 2 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
**********************************************************************/

#include <qwidget.h>
#include <qdialog.h>
#include <qdict.h>
#include <qstringlist.h>

class QClipboard;
class QComboBox;
namespace Opie {namespace Ui {class OCheckListItem;}}
class QPushButton;
namespace Opie {namespace Ui {class OListView;}}

class BenchmarkInfo : public QWidget
{
    Q_OBJECT

public:
    BenchmarkInfo( QWidget *parent = 0, const char *name = 0, int wFlags = 0 );
    ~BenchmarkInfo();

    Opie::Ui::OCheckListItem* test_alu;
    Opie::Ui::OCheckListItem* test_fpu;
    Opie::Ui::OCheckListItem* test_txt;
    Opie::Ui::OCheckListItem* test_gfx;
    Opie::Ui::OCheckListItem* test_ram;
#ifndef QT_QWS_RAMSES
    Opie::Ui::OCheckListItem* test_sd;
    Opie::Ui::OCheckListItem* test_cf;
#endif

    bool main_rd;
    bool main_wt;
#ifndef QT_QWS_RAMSES
    bool sd_rd;
    bool sd_wt;
    bool cf_rd;
    bool cf_wt;
#endif

    QClipboard* clb;
    QComboBox* machineCombo;
    Opie::Ui::OListView* tests;
    QPushButton* startButton;
    QDict <QStringList> machines;

    int textRendering( int );
    int gfxRendering( int );
    void performFileTest( const QString& fname, Opie::Ui::OCheckListItem* item );

private slots:
    void run();
    void machineActivated( int );
};

