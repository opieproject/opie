#include "KHCWidget.h"

KHCWidget::KHCWidget(QWidget* parent, const char* name, WFlags fl)
	: KHCWidgetBase(parent, name, fl)
{
	m_isEnable = true;
	setLayout();
	setHandler();

	txtOrgKey->installEventFilter(this);
	txtMapKey->installEventFilter(this);
}

KHCWidget::~KHCWidget()
{
}

void KHCWidget::setLayout()
{
	QBoxLayout* topLayout = new QVBoxLayout(this, 5);

	QBoxLayout* orgLayout1 = new QHBoxLayout(topLayout);
	orgLayout1->addWidget(lblOriginal);
	orgLayout1->addWidget(txtOrgKey);

	QBoxLayout* orgLayout2 = new QHBoxLayout(topLayout);
	orgLayout2->addWidget(lblOrgK);
	orgLayout2->addWidget(lblOrgKeycode);
#if 0
	orgLayout2->addWidget(lblOrgM);
	orgLayout2->addWidget(lblOrgModifiers);
#endif
	orgLayout2->addWidget(lblOrgU);
	orgLayout2->addWidget(lblOrgUnicode);
	orgLayout2->addWidget(chkOrgShift);
	orgLayout2->addWidget(chkOrgCtrl);
	orgLayout2->addWidget(chkOrgAlt);

	QBoxLayout* mapLayout1 = new QHBoxLayout(topLayout);
	mapLayout1->addWidget(lblMapping);
	mapLayout1->addWidget(txtMapKey);

	QBoxLayout* mapLayout2 = new QHBoxLayout(topLayout);
	mapLayout2->addWidget(lblMapK);
	mapLayout2->addWidget(lblMapKeycode);
#if 0
	mapLayout2->addWidget(lblMapM);
	mapLayout2->addWidget(lblMapModifiers);
#endif
	mapLayout2->addWidget(lblMapU);
	mapLayout2->addWidget(lblMapUnicode);
	mapLayout2->addWidget(chkMapShift);
	mapLayout2->addWidget(chkMapCtrl);
	mapLayout2->addWidget(chkMapAlt);

	QBoxLayout* btnLayout = new QHBoxLayout(topLayout);
	btnLayout->addWidget(btnGen);
	btnLayout->addWidget(btnCopy);
	btnLayout->addWidget(btnCtrl);

	topLayout->addWidget(mleDefine);
}

void KHCWidget::setHandler()
{
	connect(btnGen, SIGNAL(clicked()), this, SLOT(onClick_Gen()));
	connect(btnCopy, SIGNAL(clicked()), this, SLOT(onClick_Copy()));
	connect(btnCtrl, SIGNAL(clicked()), this, SLOT(onClick_Ctrl()));
}

bool KHCWidget::eventFilter(QObject* o, QEvent* e)
{
	if(::strcmp(o->name(), "txtOrgKey") == 0){
		if(e->type() == QEvent::KeyPress){
			QKeyEvent* ke = (QKeyEvent*)e;
			if(ke->isAutoRepeat() == false){
				onPress_Org(ke);
			}
			return(true);
		}
	} else if(::strcmp(o->name(), "txtMapKey") == 0){
		if(e->type() == QEvent::KeyPress){
			QKeyEvent* ke = (QKeyEvent*)e;
			if(ke->isAutoRepeat() == false){
				onPress_Map(ke);
			}
			return(true);
		}
	}

	return QWidget::eventFilter(o, e);
}

void KHCWidget::onPress_Org(QKeyEvent* ke)
{
	/* keycode */
	const QString& name = KeyNames::getName(ke->key());
	if(name == QString::null){
		lblOrgKeycode->setText(QString::number(ke->key(), 16));
	} else {
		lblOrgKeycode->setText(name);
	}
	/* modifiers */
	chkOrgShift->setChecked(ke->state() & Qt::ShiftButton);
	chkOrgCtrl->setChecked(ke->state() & Qt::ControlButton);
	chkOrgAlt->setChecked(ke->state() & Qt::AltButton);

	/* unicode */
	if(ke->text()[0].isPrint()){
		lblOrgUnicode->setText(ke->text());
	} else {
		lblOrgUnicode->setText(QString::number(ke->ascii(), 16));
	}
	m_OrgkeyData.setData(ke);
}

void KHCWidget::onPress_Map(QKeyEvent* ke)
{
	/* keycode */
	const QString& name = KeyNames::getName(ke->key());
	if(name == QString::null){
		lblMapKeycode->setText(QString::number(ke->key(), 16));
	} else {
		lblMapKeycode->setText(name);
	}
	/* modifiers */
	chkMapShift->setChecked(ke->state() & Qt::ShiftButton);
	chkMapCtrl->setChecked(ke->state() & Qt::ControlButton);
	chkMapAlt->setChecked(ke->state() & Qt::AltButton);

	/* unicode */
	if(ke->text()[0].isPrint()){
		lblMapUnicode->setText(ke->text());
	} else {
		lblMapUnicode->setText(QString::number(ke->ascii(), 16));
	}
	m_MapkeyData.setData(ke);
}

void KHCWidget::onClick_Gen()
{
	mleDefine->clear();
	if(m_OrgkeyData.key == 0
		|| m_MapkeyData.key == 0){
		return;
	}
	/* original key */
	QString line;
	const QString& name = KeyNames::getName(m_OrgkeyData.key);
	line = "<define ";
	if(name == QString::null){
		line.append("code=\"");
		line.append(QString::number(m_OrgkeyData.key, 16));
	} else {
		line.append("key=\"");
		line.append(name);
	}
	line.append("\">");
	mleDefine->append(line);

	/* original modifiers */
	bool need = false;
	line = "<modifier";
	if(chkOrgShift->isChecked()){
		line.append(" Shift=\"On\"");
		need = true;
	}
	if(chkOrgCtrl->isChecked()){
		line.append(" Control=\"On\"");
		need = true;
	}
	if(chkOrgAlt->isChecked()){
		line.append(" Alt=\"On\"");
		need = true;
	}
	line.append("/>");
	if(need){
		mleDefine->append(line);
	}

	/* mapping key */
	const QString& mapname = KeyNames::getName(m_MapkeyData.key);
	line = "<map";
	if(mapname == QString::null){
		line.append(" code=\"");
		line.append(QString::number(m_MapkeyData.key, 16));
	} else {
		line.append(" key=\"");
		line.append(mapname);
	}
	line.append("\"/>");
	mleDefine->append(line);

	/* mapping modifiers */
	need = false;
	line = "<map_modifier";
	bool on = chkMapShift->isChecked();
	if(chkOrgShift->isChecked() != on){
		line.append(" Shift=\"");
		if(on){
			line.append("On\"");
		} else {
			line.append("Off\"");
		}
		need = true;
	}
	on = chkMapCtrl->isChecked();
	if(chkOrgCtrl->isChecked() != on){
		line.append(" Control=\"");
		if(on){
			line.append("On\"");
		} else {
			line.append("Off\"");
		}
		need = true;
	}
	on = chkMapAlt->isChecked();
	if(chkOrgAlt->isChecked() != on){
		line.append(" Alt=\"");
		if(on){
			line.append("On\"");
		} else {
			line.append("Off\"");
		}
		need = true;
	}
	line.append("/>");
	if(need){
		mleDefine->append(line);
	}

	/* mapping unicode */
	bool found = false;
	for(const QWSServer::KeyMap* m = QWSServer::keyMap();
			m->key_code != 0; m++){
		if(m->key_code == m_MapkeyData.key){
			if(m_MapkeyData.state & Qt::ControlButton){
				if(m->ctrl_unicode == m_MapkeyData.ascii){
					found = true;
					break;
				}
			} else if(m_MapkeyData.state & Qt::ShiftButton){
				if(m->shift_unicode == m_MapkeyData.ascii){
					found = true;
					break;
				}
			} else {
				if(m->unicode == m_MapkeyData.ascii){
					found = true;
					break;
				}
			}
		}
	}
	if(found == false){
		if(m_MapkeyData.text[0].isPrint()){
			line = "<map_unicode";
			if(m_MapkeyData.state & Qt::ControlButton){
				line.append(" ctrl_unicode=\"");
			} else if(m_MapkeyData.state & Qt::ShiftButton){
				line.append(" shift_unicode=\"");
			} else {
				line.append(" unicode=\"");
			}
			line.append(m_MapkeyData.text);
			line.append("\"/>");
			mleDefine->append(line);
		} else {
			mleDefine->clear();
			mleDefine->append("Not Support");
			return;
		}
	}

	/* close tag */
	line = "</define>";
	mleDefine->append(line);
}

void KHCWidget::onClick_Copy()
{
	mleDefine->selectAll();
	mleDefine->copy();
	mleDefine->deselect();
}

void KHCWidget::onClick_Ctrl()
{
	if(m_isEnable){
		QCopEnvelope e("QPE/KeyHelper", "disable()");
		btnCtrl->setText(tr("Enable"));
		//btnCtrl->update();
	} else {
		QCopEnvelope e("QPE/KeyHelper", "enable()");
		btnCtrl->setText(tr("Disable"));
		//btnCtrl->update();
	}
	m_isEnable = !m_isEnable;
}

void KHCWidget::closeEvent(QCloseEvent* ce)
{
	qDebug("closeEvent()");
	QCopEnvelope e("QPE/KeyHelper", "enable()");
	ce->accept();
}

