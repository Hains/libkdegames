/* **************************************************************************
                           KGameProcess class
                           -------------------
    begin                : 1 January 2001
    copyright            : (C) 2001 by Martin Heni and Andreas Beckermann
    email                : martin@heni-online.de and b_mann@gmx.de
 ***************************************************************************/

/* **************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   Additional license: Any of the above copyright holders can add an     *
 *   enhanced license which complies with the license of the KDE core      *
 *   libraries so that this file resp. this library is compatible with     *
 *   the KDE core libraries.                                               *
 *   The user of this program shall have the choice which license to use   *
 *                                                                         *
 ***************************************************************************/
/*
    $Id$
*/
#ifndef __KGAMEPROCESS_H_
#define __KGAMEPROCESS_H_

#include <qstring.h>
#include <qobject.h>
#include <kgameproperty.h>
#include <qfile.h>
#include <krandomsequence.h>

//class KGameClientProcess;
class KPlayer;
//class KMessageClient;
//class KMessageServer;
class KMessageFilePipe;
//class KRandomSequence;

/**
 * This is the process class used on the computer player
 * side to communicate with its counterpart KProcessIO class.
 * Using these two classes will give fully transparent communication
 * via QDataStreams.
 */
class KGameProcess:  public QObject
{
  Q_OBJECT

  public:
    /** 
     * Creates a KGameProcess class. Done only in the computer
     * player. To activate the communication you have to call
     * the @ref exec function of this class which will listen
     * to the communication and emit signals to notify you of
     * any incoming messages.
     * Note: This function will only return after you set
     * @ref setTerminate(true) in one of the received signals.
     * So you can not do any computer calculation after the exec function.
     * Instead you react on the signals which are emitted after a
     * message is received and perform the calculations there!
     * Example:
     * <pre>
     *  KGameProcess proc;
     *  return proc.exec();
     *  </pre>
     */
    KGameProcess();
    /**
     * Destruct the process
     */
    ~KGameProcess();

    /**
     * Enters the event loop of the computer process. Does only
     * return on setTerminate(true)!
     */
    bool exec(int argc, char *argv[]);
    /**
     * Should the computer process leave its exec function?
     *
     * @return true/false
     */
    bool terminate() const {return mTerminate;}
    /**
     * Set this to true if the computer process should end, ie
     * leave its exec function.
     *
     * @param b true for exit the exec function
     */
    void setTerminate(bool b) {mTerminate=b;}
    /**
     * Sends a message to the corresponding KGameIO
     * device.
     *
     * @param the QDataStream containing the message
     */
    void sendMessage(QDataStream &stream,int msgid,int receiver=0);
    /**
     * Sends a system message to the corresonding KGameIO device.
     * This will normally be either a perfomred move or a query
     * (IdProcessQuery)
     *
     * @param the QDataStream containing the message
     * @param msgid - the message id for the message
     * @param receiver - unused
     */
    void sendSystemMessage(QDataStream &stream,int msgid,int receiver=0);
    /**
     * Returns a pointer to a KRandomSequence. You can generate
     * random numbers via e.g. random->getLong(100);
     * 
     * @return @ref KRandomSequence pointer
     */
    KRandomSequence *random() {return mRandom;}

  protected:
    /**
     * processes the command line argumens to set up the computer player
     * Pass the argumens exactely as given by main()
     */
    void processArgs(int argc, char *argv[]);

  protected slots:
      void receivedMessage(const QByteArray& receiveBuffer);

  signals:
    /**
     * The generic communication signal. You have to connect to this
     * signal to generate a valid computer response onto arbitrary messages.
     * All signals but IdIOAdded and IdTurn end up here!
     * Example:
     * <pre>
     * void Computer::slotCommand(int &msgid,QDataStream &in,QDataStream &out)
     * {
     *   Q_INT32 data,move;
     *   in >> data;
     *   // compute move ...
     *   move=data*2;
     *   out << move;
     * }
     * </pre>
     *
     * @param id the message id of the message which got transmitted to the computer
     * @param inputStream the incoming data stream
     * @param outputStream the outgoing data stream for the move
     */
     void signalCommand(QDataStream &inputStream,int msgid,int receiver,int sender);

     /**
      * This signal is emmited if the computer player should perform a turn.
      * Calculatisn can be made here and the move can then be send back with 
      * sendSystemMessage with the message id KGameMessage::IdPlayerInput.
      * These must provide a move which complies to your other move syntax as
      * e.g. produces by keyboard or mouse input.
      * Additonal data which have been written into the stream from the
      * ProcessIO's signal signalPrepareTurn can be retrieved from the
      * stream here.
      *
      * @param The datastream which contains user data 
      * @param True or false whether the turn is activated or deactivated
      * 
      */
     void signalTurn(QDataStream &stream,bool turn);
     /**
      * This signal is emmited when the process is initialized, i.e. added
      * to a KPlayer. Initial initialisation can be performed here be reacting
      * to the KProcessIO signal signalIOAdded and retrieving the data here
      * from the stream. 
      *
      * @param The datastream which contains user data 
      * @param The userId of the player. (Careful to rely on it yet)
      */
     void signalInit(QDataStream &stream,int userid);

  protected:
    bool mTerminate;
    KMessageFilePipe *mMessageIO;
  private:
    QFile rFile;
    QFile wFile;
    KRandomSequence* mRandom;
};
#endif
