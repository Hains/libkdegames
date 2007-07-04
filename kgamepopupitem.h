/*******************************************************************
    Copyright 2007 Dmitry Suzdalev <dimsuz@gmail.com>

    This library is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 ********************************************************************/
#ifndef K_GAME_POPUP_ITEM_H
#define K_GAME_POPUP_ITEM_H

#include <libkdegames_export.h>

#include <QtGui/QGraphicsItem>
#include <QtCore/QObject>

class KGamePopupItemPrivate;

/**
 * QGraphicsItem capable of showing short popup messages
 * which do not interrupt the gameplay.
 * Messages can stay on screen for specified amount of time
 * and automatically hide after (unless user hovers it with mouse).
 *
 * Example of use:
 * \code
 * KGamePopupItem *messageItem = new KGamePopupItem();
 * myGraphicsScene->addItem(messageItem);
 * ...
 * messageItem->setMessageTimeOut( 3000 ); // 3 sec
 * messageItem->setPosition( BottomLeft );
 * messageItem->showMessage("Hello, I'm a game message! How do you do?");
 * \endcode
 */
class KDEGAMES_EXPORT KGamePopupItem : public QObject, public QGraphicsItem
{
    Q_OBJECT
public:
    /**
     * The possible places in the scene where a message can be shown
     */
    enum Position { TopLeft, TopRight, BottomLeft, BottomRight };
    /**
     * Constructs a message item. It is hidden by default.
     */
    KGamePopupItem();
    /**
     * Destructs a message item
     */
    ~KGamePopupItem();
    /**
     * Shows the message: item will appear at specified place
     * of the scene using simple animation
     * Item will be automatically hidden after timeout set in setMessageTimeOut() passes
     * If item is hovered with mouse it won't hide until user moves
     * the mouse away
     *
     * @param text holds the message to show
     * @param pos position on the scene where the message will appear
     */
    void showMessage( const QString& text, Position pos );
    /**
     * Sets the amount of time the item will stay visible on screen
     * before it goes away.
     * By default item is shown for 2000 msec
     * If item is hovered with mouse it will hide only after
     * user moves the mouse away
     *
     * @param msec amount of time in milliseconds
     */
    void setMessageTimeout( int msec );
    /**
     * @return timeout that is currently set
     */
    int messageTimeout() const;
    /**
     * Sets the message opacity from 0 (fully transparent) to 1 (fully opaque)
     * For example 0.5 is half transparent
     * It defaults to 1.0
     */
    void setMessageOpacity( qreal opacity );
    /**
     * @return current message opacity
     */
    qreal messageOpacity() const;
    /**
     * Sets custom pixmap to show instead of default icon on the left
     */
    void setMessageIcon( const QPixmap& pix );
    /**
     * Requests the item to be hidden immediately. No hide-animation
     * is played. It just dissapears immediately
     */
    void forceHide();
    /**
     * Sets brush used to paint item backgound
     * By default system-default brush is used
     * @see KColorScheme
     */
    void setBackgroundBrush( const QBrush& brush );
    /**
     * Sets default color for unformatted text
     * By default system-default color is used
     * @see KColorScheme
     */
    void setTextColor( const QColor& color );
    /**
     * @return the bounding rect of this item. Reimplemented from QGraphicsItem
     */
    virtual QRectF boundingRect() const;
    /**
     * Paints item. Reimplemented from QGraphicsItem
     */
    virtual void paint( QPainter* p, const QStyleOptionGraphicsItem *option, QWidget* widget );
Q_SIGNALS:
    /**
     * Emitted when user clicks on a link in item
     */
    void linkActivated( const QString& link );
    /**
     * Emitted when user hovers a link in item
     */
    void linkHovered( const QString& link );
private Q_SLOTS:
    void animationFrame(int);
    void hideMe();
    void playHideAnimation();
private:
    virtual void hoverEnterEvent( QGraphicsSceneHoverEvent* );
    virtual void hoverLeaveEvent( QGraphicsSceneHoverEvent* );
    KGamePopupItemPrivate * const d;
};

#endif
