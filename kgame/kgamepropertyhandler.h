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
    the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
    Boston, MA 02111-1307, USA.
*/

#ifndef __KGAMEPROPERTYHANDLER_H_
#define __KGAMEPROPERTYHANDLER_H_

#include <qobject.h>
#include <qdatastream.h>
#include <qintdict.h>
#include <qmap.h>

#include <kdebug.h>

#include "kgameproperty.h"

class KGame;
class KPlayer;
// class KGamePropertyHandlerBase;

class KGamePropertyHandlerPrivate; // wow - what a name ;-)

class KGamePropertyHandler : public QObject
// QIntDict<KGamePropertyBase>
{
  Q_OBJECT

public:
	KGamePropertyHandler();
	KGamePropertyHandler(int id,const QObject * receiver, const char * sendf, const char *emitf);
	~KGamePropertyHandler();

	/**
	 * Register the handler with a parent. This is to use
	 * if the constructor without arguments has been choosen.
	 * Otherwise you need not call this.
	 *
	 * @param id The id of the message to listen for
	 * @param owner the parent object
	 **/
	void registerHandler(int id,const QObject *receiver, const char * send, const char *emit); 

	/**
	 * Main message process function. This has to be called by
	 * the parent's message event handler. If the id of the message
	 * agrees with the id of the handler, the message is extracted 
	 * and processed. Otherwise false is returned.
	 * Example:
	 * <pre>
	 *   if (mProperties.processMessage(stream,msgid)) return ;
	 * </pre>
	 * 
	 * @param stream The data stream containing the message
	 * @param id the message id of the message
	 * @return true on message processed otherwise false
	 **/
	bool processMessage(QDataStream &stream, int id);
	
	/**
	 * @return the id of the handler
	 **/
	int id() const { return mId; }
	
	/**
	 * Adds a @ref KGameProperty property to the handler
	 * @param data the property
	 * @return true on success
	 **/
	bool addProperty(KGamePropertyBase *data,QString name=0);

	/**
	 * Removes a property from the handler
	 * @param data the property
	 * @return true on success
	 **/
	bool removeProperty(KGamePropertyBase *data);

	/**
	 * Loads properties from the datastream
	 *
	 * @param stream the datastream to save from
	 * @return true on success otherwise false
	 **/
	virtual bool load(QDataStream &stream);

	/**
	 * Saves properties into the datastream
	 *
	 * @param stream the datastream to save to
	 * @return true on success otherwise false
	 **/
	virtual bool save(QDataStream &stream);
	
	/**
	 * called by a property to send itself into the
	 * datastream. This call is simply forwarded to
	 * the parent object
	 **/ 
	void sendProperty(QDataStream &s);

	/**
	 * called by a property to emit a signal 
	 * This call is simply forwarded to
	 * the parent object
	 **/ 
	void emitSignal(KGamePropertyBase *data);

  QString propertyName(int id);

  KGamePropertyBase *find(int id);
  void clear();

	void setId(int id)//AB: TODO: make this protected in KGamePropertyHandler!!
	{
		mId = id;
	}

      /**
       * Calls @ref KGamePropertyBase::setLocked(false) for all properties of this
       * player
       **/
      void unlockProperties();
      /**
       * Calls @ref KGamePropertyBase::setLocked(true) for all properties of this
       * player
       *
       * Use with care! This will even lock the core properties, like name,
       * group and myTurn!!
       **/
      void lockProperties();

      /**
       * Reference to the internal dictionary
       **/
      QIntDict<KGamePropertyBase> &dict() {return mIdDict;}


signals:
      void signalPropertyChanged(KGamePropertyBase *);
      void signalSendMessage(QDataStream &);

private:
	void init();
	KGamePropertyHandlerPrivate* d;
	int mId;
  QMap<int,QString> mNameMap;
  QIntDict<KGamePropertyBase> mIdDict;
};


#endif
