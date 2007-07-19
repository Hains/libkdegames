/*
Copyright (c) 2007, Nicolas Roffet, <nicolas-kde@roffet.com>
Copyright (c) 2007, Pino Toscano, <toscano.pino@tiscali.it>

This library is free software; you can redistribute it and/or modify it under the terms of the GNU Library General Public License as published by the Free Software Foundation; either version 2 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Library General Public License for more details.

You should have received a copy of the GNU Library General Public License along with this program; if not, write to the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA
*/

#include "kgamedifficulty.h"



#include <QComboBox>
#include <QMap>


#include <kactioncollection.h>
#include <kicon.h>
#include <klocale.h>
#include <kmessagebox.h>
#include <kstatusbar.h>
#include <kselectaction.h>
#include <kxmlguiwindow.h>



class KGameDifficultyPrivate : public QObject
{
	Q_OBJECT

	public:
		~KGameDifficultyPrivate();

		void init(KXmlGuiWindow* window, const QObject* recvr, const char* slotStandard, const char* slotCustom);

		void rebuildActions();

		/**
		 * @return standard string for standard level
		 */
		QString standardLevelString(KGameDifficulty::standardLevel level);

		void setLevel(KGameDifficulty::standardLevel level);
		void setLevelCustom(int key);


		/**
		* @brief Current custom difficulty level
		*/
		int m_levelCustom;

		KGameDifficulty::standardLevel m_level;
		QList<KGameDifficulty::standardLevel> m_standardLevels;
		QMap<int, QString> m_customLevels;

		KSelectAction* m_menu;
		KGameDifficulty::onChange m_restartOnChange;
		bool m_running;
		int m_oldSelection;
		QComboBox* m_comboBox;


	public Q_SLOTS:
		/**
		 * @brief Player wants to change the difficulty level to a standard level
		 *
		 * The difference with the methode "setSelection" is that the player may have to confirm that he agrees to end the current game (if needed).
		 * @param newSelection Selected item.
		 */
		void changeSelection(int newSelection);

	Q_SIGNALS:
		/**
		 * @brief Current difficulty level changed to a standard level
		 *
		 * The game catchs this signal and restarts a game with the new standard difficulty level.
		 * @param level New standard level.
		 */
		void standardLevelChanged(KGameDifficulty::standardLevel level);

		/**
		 * @brief Current difficulty level changed to a custom level
		 *
		 * The game catchs this signal and restarts a game with the new standard difficulty level.
		 * @param key Custom level identifier.
		 */
		void customLevelChanged(int key);


	private:
		void setSelection(int newSelection);
};


KGameDifficultyPrivate::~KGameDifficultyPrivate()
{
	delete KGameDifficulty::self();
}


void KGameDifficultyPrivate::init(KXmlGuiWindow* window, const QObject* recvr, const char* slotStandard, const char* slotCustom = 0)
{
	Q_ASSERT(recvr!=0);

	m_oldSelection = -1; // No valid selection
	m_level = KGameDifficulty::noLevel;
	m_running = false;

	QObject::connect(this, SIGNAL(standardLevelChanged(KGameDifficulty::standardLevel)), recvr, slotStandard);
	if (slotCustom!=0)
		QObject::connect(this, SIGNAL(customLevelChanged(int)), recvr, slotCustom);

	m_menu = new KSelectAction(KIcon("games-difficult"), i18n("Difficulty"), window);
	m_menu->setToolTip(i18n("Set the difficulty level"));
	m_menu->setWhatsThis(i18n("Set the difficulty level of the game."));
	QObject::connect(m_menu, SIGNAL(triggered(int)), this, SLOT(changeSelection(int)));
	window->actionCollection()->addAction("game_difficulty", m_menu);

	setParent(window);

	m_comboBox = new QComboBox(window);
	m_comboBox->setToolTip(i18n("Difficulty"));
	QObject::connect(m_comboBox, SIGNAL(activated(int)), this, SLOT(changeSelection(int)));
	window->statusBar()->addPermanentWidget(m_comboBox);

	KGameDifficulty::setRestartOnChange(KGameDifficulty::restartOnChange);
}


void KGameDifficultyPrivate::changeSelection(int newSelection)
{
	bool mayChange = true;


	if (mayChange && (m_restartOnChange==KGameDifficulty::restartOnChange) && m_running)
		mayChange = ( KMessageBox::warningContinueCancel(0, i18n("This will be the end of the current game!"), QString(), KGuiItem(i18n("Change the difficulty level"))) == KMessageBox::Continue );

	if (mayChange) {
		setSelection(newSelection);
	} else {
		// restore current level selection
		setSelection(m_oldSelection);
	}
}

QString KGameDifficultyPrivate::standardLevelString(KGameDifficulty::standardLevel level)
{
    switch (level) {
        case KGameDifficulty::ridiculouslyEasy:
            return i18n("Ridiculously easy");
        case KGameDifficulty::veryEasy:
            return i18n("Very easy");
        case KGameDifficulty::easy:
            return i18n("Easy");
        case KGameDifficulty::medium:
            return i18n("Medium");
        case KGameDifficulty::hard:
            return i18n("Hard");
        case KGameDifficulty::veryHard:
            return i18n("Very hard");
        case KGameDifficulty::extremelyHard:
            return i18n("Extremely hard");
        case KGameDifficulty::impossible:
            return i18n("Impossible");
        case KGameDifficulty::custom:
        case KGameDifficulty::configurable:
        case KGameDifficulty::noLevel:
            // Do nothing
            break;
    }
    return QString();
}

void KGameDifficultyPrivate::rebuildActions()
{
	m_menu->clear();
	m_comboBox->clear();
	qSort(m_standardLevels.begin(), m_standardLevels.end());

	foreach(KGameDifficulty::standardLevel level, m_standardLevels) {
		if (level!=KGameDifficulty::configurable) {
			m_menu->addAction(standardLevelString(level));
			m_comboBox->addItem(KIcon("games-difficult"), standardLevelString(level));
		}
	}

	if (m_customLevels.count()>0) {
		foreach(QString s, m_customLevels) {
			m_menu->addAction(s);
			m_comboBox->addItem(KIcon("games-difficult"), s);
		}
	}

	if (m_standardLevels.contains(KGameDifficulty::configurable)) {
		QAction* separator = new QAction(m_menu);
		separator->setSeparator(true);
		m_menu->addAction(separator);

		QString s = i18n("Custom...");
		m_menu->addAction(s);
		m_comboBox->addItem(KIcon("games-difficult"), s);
	}

	// reselect the previous selected item.
	if (m_level==KGameDifficulty::custom)
		KGameDifficulty::setLevelCustom(m_levelCustom);
	else if (m_standardLevels.contains(m_level))
		KGameDifficulty::setLevel(m_level);
}

void KGameDifficultyPrivate::setSelection(int newSelection)
{
	int countWithoutConfigurable = m_standardLevels.count();
	if (m_standardLevels.contains(KGameDifficulty::configurable))
		countWithoutConfigurable--;

	if ((m_standardLevels.contains(KGameDifficulty::configurable)) && (newSelection>m_menu->actions().count()-3))
		KGameDifficulty::setLevel(KGameDifficulty::configurable);
	else if(newSelection<countWithoutConfigurable)
		KGameDifficulty::setLevel(m_standardLevels[newSelection]);
	else
		KGameDifficulty::setLevelCustom((m_customLevels.uniqueKeys()).value(newSelection - countWithoutConfigurable));

	m_oldSelection = newSelection;
}


void KGameDifficultyPrivate::setLevel(KGameDifficulty::standardLevel level)
{
	if ((!m_standardLevels.contains(level)) && (level!=KGameDifficulty::custom))
		level = KGameDifficulty::noLevel;

	if (level==KGameDifficulty::configurable) {
		m_menu->setCurrentItem(m_menu->actions().count()-1);
		m_comboBox->setCurrentIndex(m_comboBox->count()-1);
	} else if (level!=KGameDifficulty::custom) {
		int i = m_standardLevels.indexOf(level);
		m_menu->setCurrentItem(i);
		m_comboBox->setCurrentIndex(i);
	}

	if (level != m_level)
		emit standardLevelChanged(level);
	m_level = level;

	m_oldSelection = m_menu->currentItem();
}


void KGameDifficultyPrivate::setLevelCustom(int key)
{
	m_level = KGameDifficulty::custom;

	int a = m_standardLevels.count();
	if (m_standardLevels.contains(KGameDifficulty::configurable))
		a -= 1;

	int i = (m_customLevels.uniqueKeys()).indexOf(key) + a;
	m_menu->setCurrentItem(i);
	m_comboBox->setCurrentIndex(i);

	if (key != m_levelCustom)
		emit customLevelChanged(key);
	m_levelCustom = key;

	m_oldSelection = m_menu->currentItem();
}



//---//



KGameDifficulty* KGameDifficulty::instance = 0;


KGameDifficulty::~KGameDifficulty()
{
	// We do not need to delete d, because d deletes us.
}


void KGameDifficulty::init(KXmlGuiWindow* window, const QObject* recvr, const char* slotStandard, const char* slotCustom)
{
	self()->d->init(window, recvr, slotStandard, slotCustom);
}


void KGameDifficulty::setRestartOnChange(onChange restart)
{
	Q_ASSERT(self()->d);

	self()->d->m_restartOnChange = restart;
	if (restart==restartOnChange)
		self()->d->m_comboBox->setWhatsThis(i18n("Select the <b>difficulty</b> of the game.<br />If you change the difficulty level while a game is running, you will have to cancel it and start a new one."));
	else
		self()->d->m_comboBox->setWhatsThis(i18n("Select the <b>difficulty</b> of the game.<br />You can change the difficulty level during a running game."));

}


void KGameDifficulty::addStandardLevel(standardLevel level)
{
	Q_ASSERT(self()->d);

	if ((level!=custom) && (level!=noLevel)) {
		self()->d->m_standardLevels.append(level);
		self()->d->rebuildActions();
	}
}


void KGameDifficulty::removeStandardLevel(standardLevel level)
{
	Q_ASSERT(self()->d);

	self()->d->m_standardLevels.removeAll(level);
	self()->d->rebuildActions();
}


void KGameDifficulty::addCustomLevel(int key, const QString& appellation)
{
	Q_ASSERT(self()->d);

	self()->d->m_customLevels.insert(key, appellation);
	self()->d->rebuildActions();
}


void KGameDifficulty::removeCustomLevel(int key)
{
	Q_ASSERT(self()->d);

	self()->d->m_customLevels.remove(key);
	self()->d->rebuildActions();
}


void KGameDifficulty::setEnabled(bool enabled)
{
	Q_ASSERT(self()->d->m_menu);

	// TODO: Doing this never disable the combobox in the toolbar (just in the menu). It seems to be a bug in the class KSelectAction of kdelibs/kdeui/actions. To check and solve...
	self()->d->m_menu->setEnabled(enabled);
}


void KGameDifficulty::setLevel(standardLevel level)
{
	Q_ASSERT(self()->d);

	self()->d->setLevel(level);
}


void KGameDifficulty::setLevelCustom(int key)
{
	Q_ASSERT(self()->d);

	self()->d->setLevelCustom(key);
}


int KGameDifficulty::levelCustom()
{
	Q_ASSERT(self()->d);

	return self()->d->m_levelCustom;
}


KGameDifficulty::standardLevel KGameDifficulty::level()
{
	Q_ASSERT(self()->d);

	return self()->d->m_level;
}


QString KGameDifficulty::levelString()
{
	Q_ASSERT(self()->d);

	return self()->d->standardLevelString(self()->d->m_level);
}


void KGameDifficulty::setRunning(bool running)
{
	Q_ASSERT(self()->d);

	self()->d->m_running = running;
}


KGameDifficulty::KGameDifficulty() : d(new KGameDifficultyPrivate())
{
}


KGameDifficulty* KGameDifficulty::self()
{
	if (instance==0)
		instance = new KGameDifficulty();
	return instance;
}

#include "kgamedifficulty.moc"
