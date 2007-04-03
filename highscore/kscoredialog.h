/****************************************************************
Copyright (c) 1998 Sandro Sigala <ssigala@globalnet.it>.
Copyright (c) 2001 Waldo Bastian <bastian@kde.org>
All rights reserved.

Permission to use, copy, modify, and distribute this software
and its documentation for any purpose and without fee is hereby
granted, provided that the above copyright notice appear in all
copies and that both that the copyright notice and this
permission notice and warranty disclaimer appear in supporting
documentation, and that the name of the author not be used in
advertising or publicity pertaining to distribution of the
software without specific, written prior permission.

The author disclaim all warranties with regard to this
software, including all implied warranties of merchantability
and fitness.  In no event shall the author be liable for any
special, indirect or consequential damages or any damages
whatsoever resulting from loss of use, data or profits, whether
in an action of contract, negligence or other tortious action,
arising out of or in connection with the use or performance of
this software.
****************************************************************/

#ifndef KSCOREDIALOG_H
#define KSCOREDIALOG_H

#include <QMap>
#include <QFlags>

#include <kdialog.h>
#include <libkdegames_export.h>

/**
 * A simple high score dialog.
 */
class KDEGAMES_EXPORT KScoreDialog : public KDialog {
    Q_OBJECT
   
public:
   enum Fields { Name = 1 << 0, 
                 Level = 1 << 1, 
                 
                 Custom1 = 1 << 10,
                 Custom2 = 1 << 11,
                 Custom3 = 1 << 12,   	
                 
                 Date = 1 << 27, 
                 Time = 1 << 28, 
                 Score = 1 << 29 };
		 
    enum AddScoreFlag { AskName = 0x1,
                         LessIsMore = 0x2 };
    Q_DECLARE_FLAGS(AddScoreFlags, AddScoreFlag)
        
    typedef QMap<int, QString> FieldInfo;

   /**
     * @param fields Which fields should be listed.
     * @param parent passed to parent QWidget constructor.
    */
    KScoreDialog(int fields, QWidget *parent);

    ~KScoreDialog();

   /**
     * @param group to use for reading/writing highscores from/to. By default
     * the class will use "High Score"
    */
    void setConfigGroup(const QString &group);

   /**
     * @param comment to add when showing high-scores.
     * The comment is only used once.  
    */
    void setComment(const QString &comment);

   /**
     * Define an extra FieldInfo entry.
     * @param field Id of this field
     * @param header Header shown in the dialog for this field
     * @param key used to store this field with.
    */
    void addField(int field, const QString &header, const QString &key); 

    /**
     * @deprecated
     * 
     * Adds a new score to the list.
     *
     * @param newScore the score of this game.
     * @param newInfo additional info about the score.
     * @param askName Whether to prompt for the players name.
     * @param lessIsMore If true, the lowest score is the best score.
     *
     * @returns The highscore position if the score was good enough to 
     * make it into the list (1 being topscore) or 0 otherwise.
    */
    int addScore(int newScore, const FieldInfo &newInfo, bool askName, bool lessIsMore);
    /// @deprecated
    int addScore(int newScore, const FieldInfo &newInfo, bool askName);
   
   /**
     * Adds a new score to the list.
     *
     * @param newScore the score of this game.
     * @param newInfo additional info about the score.
     * @param flags set wheter the user should be prompted for their name and how the scores should be sorted
     * @param name the name of the player
     *
     * @returns The highscore position if the score was good enough to 
     * make it into the list (1 being topscore) or 0 otherwise.
    */
    int addScore(int newScore, const FieldInfo &newInfo, AddScoreFlags flags=AskName, const QString &name=QString::null);

   /**
     * Returns the current best score.
    */
    int highScore();

    virtual void show();
    virtual void exec();

    private slots:
        void slotGotReturn();
        void slotGotName();

    private:
        /* read scores */
        void loadScores();   
        void saveScores();
   
        void aboutToShow();
        void setupDialog();
        void keyPressEvent( QKeyEvent *ev);

    private:           
        class KScoreDialogPrivate;
        KScoreDialogPrivate* const d;
};
Q_DECLARE_OPERATORS_FOR_FLAGS(KScoreDialog::AddScoreFlags)

#endif // !KSCOREDIALOG_H
