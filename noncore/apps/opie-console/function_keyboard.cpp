#include "function_keyboard.h"

#include <qpe/resource.h>
#include <qpe/qpeapplication.h>
#include <qsizepolicy.h>
#include <qevent.h>
#include <qwindowsystem_qws.h>
#include <qapplication.h>
#include <qlayout.h>
#include <qspinbox.h>
#include <qlistbox.h>
#include <qlabel.h>
#include <qcombobox.h>
#include <qdir.h>

/* FunctionKeyboard {{{1 */

FunctionKeyboard::FunctionKeyboard(QWidget *parent) :
    QFrame(parent), numRows(2), numCols(11),
    pressedRow(0), pressedCol(0) {

    setSizePolicy(QSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Fixed));

    /*
     * all the saving/loading is now done in a profile. downside is that you cant modify
     * the keyboard for all profiles, but must do it on a profile-basis
     *

    Config conf("opie-console-keys");
    conf.setGroup("keys");
    for (uint r = 0; r < numRows; r++)
        for (uint c = 0; c < numCols; c++) {

            QString handle = "r" + QString::number(r) + "c" + QString::number(c);
            QStringList value_list = conf.readListEntry( handle, '|');

            if (value_list.isEmpty()) continue;

            keys.insert(

                 handle,
                 FKey (value_list[0], value_list[1], value_list[2].toUShort(), value_list[3].toUShort())
            );
        }
    //qWarning("loaded %d keys", keys.count());
    */
    if (keys.isEmpty()) loadDefaults();



}

FunctionKeyboard::~FunctionKeyboard() {}

void FunctionKeyboard::changeRows(int r) {

    numRows = r;
    repaint(false);
}
void FunctionKeyboard::changeCols(int c) {

    numCols = c;
    keyWidth = (double)width()/numCols; // have to reset this thing too
    repaint(false);
}

void FunctionKeyboard::paintEvent(QPaintEvent *e) {

    QPainter p(this);
    p.setClipRect(e->rect());
    p.fillRect(0, 0, width(), height(), QColor(255,255,255));

    p.setPen(QColor(0,0,0));

    /* those decimals do count! becomes short if use plain int */
    for (double i = 0; i <= width(); i += keyWidth) {

        p.drawLine((int)i, 0, (int)i, height());
    }

    // sometimes the last line doesnt get drawn
    p.drawLine(width() -1, 0, width() -1, height());

    for (int i = 0; i <= height(); i += keyHeight) {

        p.drawLine(0, i, width(), i);
    }

    for (uint r = 0; r < numRows; r++) {
        for (uint c = 0; c < numCols; c++) {

            QString handle = "r" + QString::number(r) + "c" + QString::number(c);
            if (keys.contains(handle)) {

                if (keys[handle].pixFile.isEmpty())
                    p.drawText( c * keyWidth + 1, r * keyHeight + 1,
                                keyWidth, keyHeight,
                                Qt::AlignHCenter | Qt::AlignVCenter,
                                keys[handle].label
                    );
                else  {

                    ushort centerX = (ushort)(c *keyWidth) + (ushort)(keyWidth - keys[handle].pix->width()) / 2;
                    ushort centerY = r * keyHeight + (keyHeight - keys[handle].pix->height()) / 2;
                    p.drawPixmap(centerX, centerY, *keys[handle].pix);
                }
            }
        }
    }
}

void FunctionKeyboard::paintKey(uint row, uint col) {

    QPainter p(this);

    p.fillRect(QRect(QPoint(col * keyWidth + 1, row * keyHeight + 1),
                     QPoint((col + 1) * keyWidth - 1, row * keyHeight + keyHeight- 1)),
               (pressedRow != -1 && pressedCol != -1 ) ? QColor(97,119,155) : QColor(255,255,255));

    QString handle ("r" + QString::number(row) + "c" + QString::number(col));
    if (keys[handle].pixFile.isEmpty())
        p.drawText(
                    col * keyWidth + 1, row * keyHeight + 1,
                    keyWidth, keyHeight,
                    Qt::AlignHCenter | Qt::AlignVCenter,
                    keys[handle].label
        );
    else {

        ushort centerX = (ushort)(col *keyWidth) + (ushort)(keyWidth - keys[handle].pix->width()) / 2;
        ushort centerY = row * keyHeight + (keyHeight - keys[handle].pix->height()) / 2;
        p.drawPixmap(centerX, centerY, *keys[handle].pix);
    }

    if (col == numCols - 1) {

        // sometimes it doesnt draw the last line

        p.drawLine((col+1) * keyWidth -1, row * keyHeight,
                   (col+1) * keyWidth -1, (row + 1) * keyHeight
        );
    }

}

void FunctionKeyboard::mousePressEvent(QMouseEvent *e) {

    pressedRow = e->y() / keyHeight;
    pressedCol = (int) (e->x() / keyWidth);

    paintKey(pressedRow, pressedCol);

    // emit that sucker!
    FKey k = keys["r" + QString::number(pressedRow) + "c" + QString::number(pressedCol)];
    emit keyPressed(k, pressedRow, pressedCol, 1);

}

void FunctionKeyboard::mouseReleaseEvent(QMouseEvent *) {

    if (pressedRow != -1 && pressedRow != -1) {

        int row = pressedRow; pressedRow = -1;
        int col = pressedCol; pressedCol = -1;
        paintKey(row, col);

        FKey k = keys["r" + QString::number(row) + "c" + QString::number(col)];
        emit keyPressed(k, row, col, 0);
    }

}


void FunctionKeyboard::resizeEvent(QResizeEvent*) {

    /* set he default font height/width */
    QFontMetrics fm=fontMetrics();
    keyHeight = fm.lineSpacing() + 2;
    keyWidth = (double)width()/numCols;

}

QSize FunctionKeyboard::sizeHint() const {

    return QSize(width(), keyHeight * numRows + 1);
}

void FunctionKeyboard::loadDefaults() {

    keys.insert( "r0c0", FKey ("Enter", "enter", Qt::Key_Enter, 0));
    keys.insert( "r0c1", FKey ("Space", "space", Qt::Key_Space, Qt::Key_Space));
    keys.insert( "r0c2", FKey ("Tab", "tab", Qt::Key_Tab, 0));
    keys.insert( "r0c3", FKey ("Up", "up", Qt::Key_Up, 0));
    keys.insert( "r0c4", FKey ("Down", "down", Qt::Key_Down, 0));

    keys.insert( "r0c7", FKey ("Ho", 0, 4112, 0));
    keys.insert( "r0c8", FKey ("End", 0, 4113, 0));
    keys.insert( "r0c9", FKey ("PU", 0, 4118, 0));
    keys.insert( "r0c10", FKey ("PD", 0, 4119, 0));

    keys.insert( "r1c0", FKey ("F1", 0, 4144, 0));
    keys.insert( "r1c1", FKey ("F2", 0, 4145, 0));
    keys.insert( "r1c2", FKey ("F3", 0, 4146, 0));
    keys.insert( "r1c3", FKey ("F4", 0, 4147, 0));
    keys.insert( "r1c4", FKey ("F5", 0, 4148, 0));
    keys.insert( "r1c5", FKey ("F6", 0, 4149, 0));
    keys.insert( "r1c6", FKey ("F7", 0, 4150, 0));
    keys.insert( "r1c7", FKey ("F8", 0, 4151, 0));
    keys.insert( "r1c8", FKey ("F9", 0, 4152, 0));
    keys.insert( "r1c9", FKey ("F10", 0, 4153, 0));
    keys.insert( "r1c10", FKey ("F11", 0, 4154, 0));


}

/* FunctionKeyboardConfig {{{1 */

FunctionKeyboardConfig::FunctionKeyboardConfig(const QString& name, QWidget* parent, const char* na )
    : ProfileDialogKeyWidget(name, parent, na),
      selectedRow(0), selectedCol(0)
{
    qWarning("FunctionKeyboardConfig");


    kb = new FunctionKeyboard(this);
    connect (kb, SIGNAL(keyPressed(FKey, ushort, ushort, bool)),
             this, SLOT(slotKeyPressed(FKey, ushort, ushort, bool)));

    QGroupBox *dimentions = new QGroupBox(2, Qt::Horizontal, tr("Dimentions"), this);
    QLabel *l = new QLabel("Rows", dimentions);
    m_rowBox = new QSpinBox(1, 15, 1, dimentions);
    connect (m_rowBox, SIGNAL(valueChanged(int)), this, SLOT(slotChangeRows(int)));
    l = new QLabel("Columns", dimentions);
    m_colBox = new QSpinBox(1, 15, 1, dimentions);
    connect (m_colBox, SIGNAL(valueChanged(int)), this, SLOT(slotChangeCols(int)));

    QGroupBox *editKey = new QGroupBox(2, Qt::Horizontal, tr("Edit Key"), this);
    l = new QLabel("Label", editKey);
    m_labels = new QComboBox(true, editKey);
    m_labels->setInsertionPolicy(QComboBox::AtCurrent);
    m_labels->insertItem("");

    QStringList files = QDir( QPEApplication::qpeDir() + "pics/console/keys/", "*.png").entryList();

    for (uint i = 0; i < files.count(); i++) {

        m_labels->insertItem( Resource::loadPixmap("console/keys/" + files[i]), files[i]);
    }
    connect (m_labels, SIGNAL(activated(int)), this, SLOT(slotChangeIcon(int)));
    connect (m_labels, SIGNAL(textChanged(const QString &)), this, SLOT(slotChangeLabelText(const QString&)));

    l = new QLabel("Q Keycode", editKey);
    m_qvalues = new QComboBox(true, editKey);
    m_qvalues->setInsertionPolicy(QComboBox::AtTop);
    m_qvalues->setDuplicatesEnabled(false);
    m_qvalues->insertItem("");
    connect (m_qvalues, SIGNAL(textChanged(const QString &)), this, SLOT(slotChangeQCode(const QString&)));

    l = new QLabel("Unicode Value", editKey);
    m_uniValues = new QComboBox(true, editKey);
    m_uniValues->setInsertionPolicy(QComboBox::AtTop);
    m_uniValues->setDuplicatesEnabled(false);
    m_uniValues->insertItem("");
    connect (m_uniValues, SIGNAL(textChanged(const QString &)), this, SLOT(slotChangeUnicode(const QString&)));

    QVBoxLayout *root = new QVBoxLayout(this, 2);
    root->addWidget(kb);
    root->addWidget(dimentions);
    root->addWidget(editKey);
}
FunctionKeyboardConfig::~FunctionKeyboardConfig() {

}
void FunctionKeyboardConfig::load (const Profile& prof) {

    m_rowBox->setValue(prof.readNumEntry("keb_rows", 2));
    m_colBox->setValue(prof.readNumEntry("keb_cols", 10));

    /* load all the keys to the keyboard */
    for (int i = 0; i <= m_rowBox->value() -1; i++)
        for (int j = 0; j <= m_colBox->value() -1; j++) {

            QString h = "r" + QString::number(i) + "c" + QString::number(j);
            QString values = prof.readEntry("keb_" + h);

            if (!values.isEmpty()) {

                QStringList l = QStringList::split(QChar('|'), values, TRUE);
                kb->keys[h] = FKey(l[0], l[1], l[2].toInt(), l[3].toInt());

                // load pixmap if used
                if (!l[1].isEmpty()) {

                    kb->keys[h].pix = new QPixmap( Resource::loadPixmap( "console/keys/" + l[1] ) );
                }
            }
        }

}
void FunctionKeyboardConfig::save (Profile& prof) {

    prof.writeEntry("keb_rows", m_rowBox->value());
    prof.writeEntry("keb_cols", m_colBox->value());

    QMap<QString, FKey>::Iterator it;
    for ( it = kb->keys.begin(); it != kb->keys.end(); it++) {

        FKey k = it.data();
        QString entry = k.label + "|"
                        + k.pixFile + "|"
                        + QString::number(k.qcode) + "|"
                        + QString::number(k.unicode);

        prof.writeEntry("keb_" + it.key(), entry);

    }

}
void FunctionKeyboardConfig::slotChangeRows(int r) {

    kb->changeRows(r);

    // have to do this so the whole thing gets redrawn
    kb->hide(); kb->show();
}
void FunctionKeyboardConfig::slotChangeCols(int c) {

    kb->changeCols(c);
}
void FunctionKeyboardConfig::slotKeyPressed(FKey k, ushort r, ushort c, bool pressed) {

    if (!pressed) return;

    selectedHandle = "r" + QString::number(r) +
                     "c" + QString::number(c);
    selectedRow = r;
    selectedCol = c;

    if (k.pixFile.isEmpty()) {

        m_labels->setEditable(true);
        m_labels->setCurrentItem(0);
        m_labels->changeItem(k.label, 0);

    } else {

        // any better way to select the pixmap?
        m_labels->setCurrentItem((m_labels->listBox())->index((m_labels->listBox())->findItem(kb->keys[selectedHandle].pixFile)));
        m_labels->setEditable(false);
    }
    m_qvalues->changeItem(QString::number(k.qcode), 0);
    m_uniValues->changeItem(QString::number(k.unicode), 0);
}
void FunctionKeyboardConfig::slotChangeIcon(int index) {

    if (index == 0) {

        // is text
        m_labels->setEditable(true);
        // why tf does the text get erased unless i do this?
        m_labels->changeItem(m_labels->text(0), 0);

        kb->keys[selectedHandle].pixFile = "";
        delete kb->keys[selectedHandle].pix;

    } else {

        // is a pixmap
        m_labels->setEditable(false);
        kb->keys[selectedHandle].pixFile = m_labels->currentText();
        kb->keys[selectedHandle].pix = new QPixmap(  Resource::loadPixmap( "console/keys/" + m_labels->currentText() ) );
    }
    kb->paintKey(selectedRow, selectedCol);
}
void FunctionKeyboardConfig::slotChangeLabelText(const QString &label) {

    kb->keys[selectedHandle].label = label;

    kb->paintKey(selectedRow, selectedCol);
}
void FunctionKeyboardConfig::slotChangeQCode(const QString& qcode) {

    kb->keys[selectedHandle].qcode = qcode.toUInt();
}
void FunctionKeyboardConfig::slotChangeUnicode(const QString& uni) {

    kb->keys[selectedHandle].unicode = uni.toUInt();
}
