/*
    This file is part of the KDE games library
    Copyright (C) 2001 Andreas Beckermann (b_mann@gmx.de)
    Copyright (C) 2001 Martin Heni (martin@heni-online.de)

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License version 2 as published by the Free Software Foundation.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public License
    along with this library; see the file COPYING.LIB.  If not, write to
    the Free Software Foundation, Inc., 51 Franklin Steet, Fifth Floor,
    Boston, MA 02110-1301, USA.
*/

#include <qlayout.h>
#include <q3vbox.h>
//Added by qt3to4:
#include <QVBoxLayout>
#include <Q3PtrList>

#include <klocale.h>

#include "kgame.h"
#include "kplayer.h"
#include "kgamedialogconfig.h"

#include "kgamedialog.h"

#include "kgamedialog.moc"

class KGameDialogPrivate
{
public:
	KGameDialogPrivate() 
	{
		mGamePage = 0;
		mNetworkPage = 0;
		mMsgServerPage = 0;
		mTopLayout = 0;

		mNetworkConfig = 0;
		mGameConfig = 0;

		mOwner = 0;
		mGame = 0;
	}

	Q3VBox* mGamePage;
	Q3VBox* mNetworkPage;
	Q3VBox* mMsgServerPage;// unused here?
	QVBoxLayout* mTopLayout;
	KGameDialogNetworkConfig* mNetworkConfig;
	KGameDialogGeneralConfig* mGameConfig;

// a list of all config widgets added to this dialog
	Q3PtrList<KGameDialogConfig> mConfigWidgets;

// just pointers:
	KPlayer* mOwner;
	KGame* mGame;
};

KGameDialog::KGameDialog(KGame* g, KPlayer* owner, const QString& title,
		QWidget* parent, bool modal)
	: KDialogBase(Tabbed, title, Ok|Default|Apply,
	Ok, parent, 0, modal, true)
{
 init(g, owner);
}

KGameDialog::KGameDialog(KGame* g, KPlayer* owner, const QString& title, 
		QWidget* parent, long initConfigs, int chatMsgId, bool modal)
	: KDialogBase(Tabbed, title, Ok|Default|Apply,
	Ok, parent, 0, modal, true)
{
 init(g, owner);
 if ((ConfigOptions)initConfigs!=NoConfig) {
	initDefaultDialog((ConfigOptions)initConfigs, chatMsgId);
 }
}

void KGameDialog::init(KGame* g, KPlayer* owner)
{
//AB: do we need a "Cancel" Button? currently removed

// kdDebug(11001) << k_funcinfo << ": this=" << this << endl;
 d = new KGameDialogPrivate;

 setOwner(owner);
 setKGame(g);
 if (g) {
	setAdmin(g->isAdmin());
 } else {
	setAdmin(false);
 }
}

void KGameDialog::initDefaultDialog(ConfigOptions initConfigs, int chatMsgId)
{
 if (initConfigs & GameConfig) {
	kdDebug() << "add gameconf" << endl;
	addGameConfig(new KGameDialogGeneralConfig(0));
 }
 if (initConfigs & NetworkConfig) {
	addNetworkConfig(new KGameDialogNetworkConfig(0));
 }
 if (initConfigs & (MsgServerConfig) ) {
	addMsgServerConfig(new KGameDialogMsgServerConfig(0));
 }
 if (initConfigs & ChatConfig) {
	KGameDialogChatConfig * c = new KGameDialogChatConfig(chatMsgId, 0);
	if (d->mGamePage) {
		addChatWidget(c, d->mGamePage);
	} else {
		addConfigPage(c, i18n("&Chat"));
	}
 }
 if (initConfigs & BanPlayerConfig) {
	// add the connection management system - ie the widget where the ADMIN can
	// kick players out
	if (d->mNetworkPage) {
		// put it on the network page
		addConnectionList(new KGameDialogConnectionConfig(0), d->mNetworkPage);
	} else {
		// if no network page available put it on an own page
		addConfigPage(new KGameDialogConnectionConfig(0), i18n("C&onnections"));
	}
 }
}

KGameDialog::~KGameDialog()
{
// kdDebug(11001) << "DESTRUCT KGameDialog" << this << endl;
 d->mConfigWidgets.setAutoDelete(true);
 d->mConfigWidgets.clear();
 delete d;
}

void KGameDialog::addGameConfig(KGameDialogGeneralConfig* conf)
{
 if (!conf) {
	return;
 }
 d->mGameConfig = conf;
 d->mGamePage = addConfigPage(d->mGameConfig, i18n("&Game"));
}

void KGameDialog::addNetworkConfig(KGameDialogNetworkConfig* netConf)
{
 if (!netConf) {
	return;
 }
 d->mNetworkConfig = netConf;
 d->mNetworkPage = addConfigPage(netConf, i18n("&Network"));
}

void KGameDialog::addMsgServerConfig(KGameDialogMsgServerConfig* msgConf)
{
 if (!msgConf) {
	return;
 }
 d->mMsgServerPage = addConfigPage(msgConf, i18n("&Message Server"));
}

void KGameDialog::addChatWidget(KGameDialogChatConfig* chat, Q3VBox* parent)
{
 if (!chat) {
	return;
 }
 if (!parent) {
	parent = d->mGamePage;
 }
 if (!parent) {
	kdError(11001) << "cannot add chat widget without page" << endl;
	return;
 }
 addConfigWidget(chat, parent);
}

void KGameDialog::addConnectionList(KGameDialogConnectionConfig* c, Q3VBox* parent)
{
 if (!c) {
	return;
 }
 if (!parent) {
	parent = d->mNetworkPage;
 }
 if (!parent) {
	kdError(11001) << "Cannot add connection list without page" << endl;
	return;
 }
 addConfigWidget(c, parent);
}

Q3VBox *KGameDialog::configPage(ConfigOptions which)
{
 Q3VBox *box = 0;
 switch(which)
 {
	case NetworkConfig:
		box = d->mNetworkPage;
		break;
	case GameConfig:
		box = d->mGamePage;
		break;
	case MsgServerConfig:
		box = d->mMsgServerPage;
		break;
	default:
		kdError(11001) << k_funcinfo << ": Parameter " << which << " not supported" << endl;
 }
 return box;
}

Q3VBox* KGameDialog::addConfigPage(KGameDialogConfig* widget, const QString& title)
{
 if (!widget) {
	kdError(11001) << "Cannot add NULL config widget" << endl;
	return 0;
 }
 Q3VBox* page = addVBoxPage(title);
 addConfigWidget(widget, page);
 return page;
}

void KGameDialog::addConfigWidget(KGameDialogConfig* widget, QWidget* parent)
{
 if (!widget) {
	kdError(11001) << "Cannot add NULL config widget" << endl;
	return;
 }
 if (!parent) {
	kdError(11001) << "Cannot reparent to NULL widget" << endl;
	return;
 }
// kdDebug(11001) << "reparenting widget" << endl;
 widget->reparent(parent, QPoint(0,0));
 d->mConfigWidgets.append(widget);
 connect(widget, SIGNAL(destroyed(QObject*)), this, SLOT(slotRemoveConfigWidget(QObject*)));
 if (!d->mGame) {
	kdWarning(11001) << "No game has been set!" << endl;
 } else {
	widget->setKGame(d->mGame);
	widget->setAdmin(d->mGame->isAdmin());
 }
 if (!d->mOwner) {
	kdWarning(11001) << "No player has been set!" << endl;
 } else {
	widget->setOwner(d->mOwner);
 }
 widget->show();
}

KGameDialogGeneralConfig* KGameDialog::gameConfig() const
{ return d->mGameConfig; }
KGameDialogNetworkConfig* KGameDialog::networkConfig() const
{ return d->mNetworkConfig; }

void KGameDialog::slotApply()
{
 submitToKGame();
}

void KGameDialog::slotDefault()
{
 if (!d->mGame) {
	return;
 }
 
//TODO *only*  call setKGame/setOwner for the *current* page!!
 setKGame(d->mGame);
 setOwner(d->mOwner);
}

void KGameDialog::slotOk()
{
 slotApply();
 QDialog::accept();
}

void KGameDialog::setOwner(KPlayer* owner)
{
//AB: note: NULL player is ok!
 d->mOwner = owner;
 for (int unsigned i = 0; i < d->mConfigWidgets.count(); i++) {
	if (d->mConfigWidgets.at(i)) {
		d->mConfigWidgets.at(i)->setOwner(d->mOwner);
		//TODO: hide playerName in KGameDialogGeneralConfig
	} else {
		kdError(11001) << "NULL widget??" << endl;
	}
 }
}

void KGameDialog::setKGame(KGame* g)
{
 if (d->mGame) {
	disconnect(d->mGame, 0, this, 0);
 }
 d->mGame = g;
 for (int unsigned i = 0; i < d->mConfigWidgets.count(); i++) {
	d->mConfigWidgets.at(i)->setKGame(d->mGame);
 }
 if (d->mGame) {
	setAdmin(d->mGame->isAdmin());
	connect(d->mGame, SIGNAL(destroyed()), this, SLOT(slotUnsetKGame()));
	connect(d->mGame, SIGNAL(signalAdminStatusChanged(bool)), 
			this, SLOT(setAdmin(bool)));
 }
}

void KGameDialog::setAdmin(bool admin)
{
 for (int unsigned i = 0; i < d->mConfigWidgets.count(); i++) {
	d->mConfigWidgets.at(i)->setAdmin(admin);
 }
}

void KGameDialog::slotUnsetKGame() // called when KGame is destroyed
{ setKGame(0); }

void KGameDialog::submitToKGame()
{
 if (!d->mGame) {
	kdError(11001) << k_funcinfo << ": no game has been set" << endl;
	return;
 }
 if (!d->mOwner) {
	kdError(11001) << k_funcinfo << ": no player has been set" << endl;
	return;
 }

 for (int unsigned i = 0; i < d->mConfigWidgets.count(); i++) {
// kdDebug(11001) << "submit to kgame " << i << endl;
	d->mConfigWidgets.at(i)->submitToKGame(d->mGame, d->mOwner);
// kdDebug(11001) << "done: submit to kgame " << i << endl;
 }
}

void KGameDialog::slotRemoveConfigWidget(QObject* configWidget)
{
 d->mConfigWidgets.removeRef((KGameDialogConfig*)configWidget);
}

