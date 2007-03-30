/** @file
 * This file contains the KGameSvgDocument class, used for manipulating
 * an SVG file using DOM.
 */

/***************************************************************************
 *   Copyright (C) 2007 Mark A. Taff <kde@marktaff.com>                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU Library General Public License          *
 *   version 2 as published by the Free Software Foundation                *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU Library General Public License for more details.                  *
 *                                                                         *
 *   You should have received a copy of the GNU Library General Public     *
 *   License along with this program; if not, write to the                 *
 *   Free Software Foundation, Inc.,                                       *
 *   51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.         *
 ***************************************************************************/

#ifndef _KGAMESVGDOCUMENT_H_
#define _KGAMESVGDOCUMENT_H_

#include <QDomDocument>
#include <QHash>
#include <QMatrix>
#include <QStringList>

class KGameSvgDocumentPrivate;

/**
 * @brief A class for manipulating an SVG file using DOM
 *
 * This class is a wrapper around QDomDocument for SVG files.
 * It:
 * @li implements elementById();
 * @li manipulates a node's style properties; and,
 * @li manipulates a node's transform properties.
 *
 * @note The DOM standard requires all changes to be "live", so we cannot cache any values
 *     from the file; instead, we always have to query the DOM for the current value.  This also
 *     means that style & matrix changes we make happen to the DOM immediately.
 *
 * A typical use is to read in an SVG file, edit the style or transform attributes
 * in DOM as desired, and then output a QByteArray suitable for being loaded with 
 * KSvgRenderer::load().
 *
 * To read an SVG file into DOM:
 * @code
 * KGameSvgDocument svgDom;
 * KGameSvgDocument *svgDom_ptr;
 * svgDom.load("/path/to/svgFile.svg");
 * svgDom_ptr = &svgDom;
 * @endcode
 *
 * To find a node with a specific value in its id attribute, for example where id="playerOneGamepiece":
 * @code
 * QDomNode playerOneGamepiece = svgDom_ptr->elementById("playerOneGamepiece");
 *
 * // This works too
 * QDomNode playerOneGamepiece = svgDom_ptr->elementByUniqueAttributeValue("id", "playerOneGamepiece");
 * @endcode
 *
 * Most methods operate on the last node found by @c elementById() or @c elementByUniqueAttributeValue().
 * If the methods are working on the wrong node, then you are mistaken about which node was
 * the last node (or you found a bug).  Try calling @c setCurrentNode() with the node you are
 * wanting to modify to see if this is the issue.  Consider the following code for example:
 * @code
 * QDomNode playerOneGamepiece = svgDom_ptr->elementById("playerOneGamepiece");
 * QDomNode playerTwoGamepiece = svgDom_ptr->elementById("playerTwoGamepiece");
 *
 * // Set player one's game piece to have a white fill
 * svgDom_ptr->setStyleProperty("fill", "#ffffff");  // INCORRECT: playerTwoGamepiece is the last node, not playerOneGamepiece
 *
 * svgDom_ptr->setCurrentNode(playerOneGamepiece);   // CORRECT: Set current node to the node we want,
 * svgDom_ptr->setStyleProperty("fill", "#ffffff");  // then we modify the node
 * @endcode
 *
 * To skew the @c currentNode():
 * @code
 * // Skew the horizontal axis 7.5 degrees
 * svgDom_ptr->skew(-7.5, 0, KGameSvgDocument::ReplaceCurrentMatrix);
 * @endcode
 * 
 * @warning Be careful when using the KGameSvgDocument::ApplyToCurrentMatrix flag. It multiplies the matrices,
 *     so if you repeatedly apply the same matrix to a node, you have a polynomial series @c x^2, and you will
 *     very quickly run into overflow issues.
 *
 * To output @c currentNode() to be rendered:
 * @code
 * KSvgRenderer svgRenderer;
 * QByteArray svg = svgDom_ptr->nodeToByteArray();
 * svgRenderer.load(svg);
 * @endcode
 *
 * To output the whole document to be rendered (See QDomDocument::toByteArray()):
 * @code
 * KSvgRenderer svgRenderer;
 * QByteArray svg = svgDom_ptr->toByteArray();
 * svgRenderer.load(svg);
 * @endcode
 * 
 * @see QDomDocument, KSvgRenderer
 * @author Mark A. Taff \<kde@marktaff.com\>
 * @version 0.1
 *
 * @todo Add convenience functions for getting/setting individual style properties.
 *     I haven't completely convinced myself of the utility of this, so don't hold your breathe. ;-)
 */
class KGameSvgDocument : public QDomDocument
{
public:
    /**
     * Constructor
     */
    explicit KGameSvgDocument();

    /**
     * Destructor
     */
    virtual ~KGameSvgDocument();

    /**
     * Options for applying (multiplying) or replacing the current matrix
     */ 
    enum MatrixOption {
        /**
        * Apply to current matrix
        */
        ApplyToCurrentMatrix = 0x01,
        /**
        * Replace the current matrix
        */
        ReplaceCurrentMatrix = 0x02
    };
    Q_DECLARE_FLAGS(MatrixOptions,
                        MatrixOption)

    /**
     * Options for sorting style properties when building a style attribute
     */ 
    enum StylePropertySortOption {
        /**
        * When building a style attribute, do not sort
        */
        Unsorted = 0x01,
        /**
        * When building a style attribute, sort properties the same way Inkscape does
        */
        UseInkscapeOrder = 0x02
    };
    Q_DECLARE_FLAGS(StylePropertySortOptions,
                        StylePropertySortOption)

    /**
     * @brief Returns the node with the given value for the given attribute.
     *
     * Returns the element whose attribute given in @p attributeName is equal to the value
     * given in @p attributeValue.
     *
     * QDomDocument::elementById() always returns a null node because TT says they can't know
     * which attribute is the id attribute.  Here, we allow the id attribute to be specified.
     *
     * This function also sets @p m_currentNode to this node.
     *
     * @param attributeName The name of the identifing attribute, such as "id" to find.
     * @param attributeValue The value to look for in the attribute @p attributeName
     *     The values held in this attribute must be unique in the document, or the consequences
     *     may be unpredictably incorrect.  You've been warned. ;-)
     * @returns the matching node, or a null node if no matching node found
     */
    QDomNode elementByUniqueAttributeValue(const QString& attributeName, const QString& attributeValue);

    /**
     * @brief Returns a node with the given id.
     *
     * This is a convenience function.  We call elementByUniqueAttributeValue(), but we assume
     * that the name of the attribute is "id".  This assumption will be correct for valid SVG files.
     *
     * Returns the element whose ID is equal to elementId. If no element with the ID was found,
     * this function returns a null element.
     *
     * @param attributeValue The value of the id attribute to find
     * @returns the matching node, or a null node if no matching node found
     * @see elementByUniqueAttributeValue()
     */
    QDomNode elementById(const QString& attributeValue);

    /**
     * @brief Reads the SVG file svgFilename() into DOM.
     * @returns nothing
     * @since 4.0
     */
    void load();

    /**
     * @overload
     * @brief This function permits specifying the svg file and loading it at once.
     *
     * @param svgFilename The filename of the SVG file to open.
     * @returns nothing
     */
    void load(const QString& svgFilename);

    /**
     * @brief Rotates the origin of the current node counterclockwise.
     *
     * @param degrees The amount in degrees to rotate by.
     * @param options Apply to current matrix or replace current matrix.
     * @returns nothing
     * @see QMatrix#rotate()
     */
    void rotate(double degrees, const MatrixOptions& options = ApplyToCurrentMatrix);

    /**
     * @brief Moves the origin of the current node
     *
     * @param xPixels The number of pixels to move the x-axis by.
     * @param yPixels The number of pixels to move the y-axis by.
     * @param options Apply to current matrix or replace current matrix.
     * @returns nothing
     * @see QMatrix::translate()
     */
    void translate(int xPixels, int yPixels, const MatrixOptions& options = ApplyToCurrentMatrix);

    /**
     * @brief Shears the origin of the current node.
     *
     * @param xRadians The amount in radians to shear (skew) the x-axis by.
     * @param yRadians The amount in radians to shear (skew) the y-axis by.
     * @param options Apply to current matrix or replace current matrix.
     * @returns nothing
     * @see QMatrix::shear()
     */
    void shear(double xRadians, double yRadians, const MatrixOptions& options = ApplyToCurrentMatrix);

    /**
     * @brief Skews the origin of the current node.
     *
     * This is a convenience function.  It simply coverts it's arguments to
     * radians, then calls shear().
     *
     * @param xDegrees The amount in degrees to shear (skew) the x-axis by.
     * @param yDegrees The amount in degrees to shear (skew) the y-axis by.
     * @param options Apply to current matrix or replace current matrix.
     * @returns nothing
     * @see shear()
     */
    void skew(double xDegrees, double yDegrees, const MatrixOptions& options = ApplyToCurrentMatrix);

    /**
     * @brief Scales the origin of the current node.
     *
     * @param xFactor The factor to scale the x-axis by.
     * @param yFactor The factor to scale the y-axis by.
     * @param options Apply to current matrix or replace current matrix.
     * @returns nothing
     * @see QMatrix::scale()
     */
    void scale(double xFactor, double yFactor, const MatrixOptions& options = ApplyToCurrentMatrix);

    /**
     * @brief Returns the last node found by elementById, or null if node not found
     * @returns The current node
     */
    QDomNode currentNode() const;

    /**
     * @brief Sets the current node.
     *
     * @param node The node to set currentNode to.
     * @returns nothing
     */
    void setCurrentNode(const QDomNode& node);

    /**
     * @brief Returns the name of the SVG file this DOM represents.
     * @returns The current filename.
     */
    QString svgFilename() const;

    /**
     * @brief Sets the current SVG filename.
     *
     * @param svgFilename The filename of the SVG file to open.
     * @returns nothing
     */
    void setSvgFilename(const QString& svgFilename);

    /**
     * @brief Returns the value of the style property given for the current node.
     * @note Internally, we create a hash with @c styleProperties, then return the value
     *     of the @c propertyName property.  As such, if you need the values of multiple
     *     properties, it will be more efficient to call @c styleProperties()
     *     and then use the hash directly.
     *
     * See KGameSvgDocumentPrivate::m_inkscapeOrder for a list of common SVG style properties 
     *
     * @param propertyName the name of the property to return
     * @returns The value style property given, or null if no such property for this node.
     */
    QString styleProperty(const QString& propertyName) const;

    /**
     * @brief Sets the value of the style property given for the current node.
     * @note Internally, we create a hash with @c styleProperties, then update the
     *  @p propertyName to @p propertyValue, before finally applying the hash to
     *      DOM via @c setStyleProperties().  Because of this, if you need to set multiple
     *      properties per node, it will be more efficient to call @c styleProperties(), 
     *      modify the hash it returns, and then apply the hash with @c setStyleProperties().
     *
     * @param propertyName The name of the property to set.
     * @param propertyValue The value of the property to set.
     * @returns nothing
     */
    void setStyleProperty(const QString& propertyName, const QString& propertyValue);

    /**
     * @brief Returns the current node and it's children as a new xml svg document.
     * @returns The xml for the new svg document
     */
    QString nodeToSvg() const;

    /**
     * @brief Builds a new svg document and returns a QByteArray suitable for passing to KSvgRenderer::load().
     *
     * Internally, we call @c nodeToSvg() and then convert to a QByteArray, so this method 
     * should be called @b instead of @c nodeToSvg().
     *
     * @returns the QByteArray
     */
    QByteArray nodeToByteArray() const;

    /**
     * @brief Returns the style attribute of the current node.
     *
     * Unless you are parsing your own style attribute for some reason, you probably
     * want to use styleProperty() or styleProperties().
     * 
     * @returns The style atttibute.
     * @see styleProperty() stylyProperties()
     */
    QString style() const;

    /**
     * @brief Sets the style attribute of the current node.
     *
     * Unless you are parsing your own style attribute for some reason, you probably
     * want to use setStyleProperty() or setStyleProperties().
     * 
     * @param styleAttribute The style attribute to apply.
     * @returns nothing
     * 
     * @see setStyleProperty() setStylyProperties()
     */
    void setStyle(const QString& styleAttribute);

    /**
     * @brief Returns the transform attribute of the current node.
     * @returns The transform atttibute.
     * @since 4.0
     */
    QString transform() const;

    /**
     * @brief Sets the transform attribute of the current node.
     * 
     * As this function works on QStrings, it <b>replaces</b> the existing
     * transform attribute.  If you need to multiply, use setTransformMatrix() instead.
     *
     * @param transformAttribute The transform attribute to apply.
     * @returns nothing
     * @see setTransformMatrix()
     */
    void setTransform(const QString& transformAttribute);

    /**
     * @brief Returns a hash of the style properties of the current node.
     * @returns The style properties.
     */
    QHash<QString, QString> styleProperties() const;

    /**
     * @brief Sets the style properties of the current node.
     *
     * The only(?) reason to set useInkscapeOrder to true is if you are saving the svg xml to a file
     * that may be human-edited later, for consistency. There is a performance hit, since hashes store
     * their data unsorted.
     *
     * @param styleProperties The hash of style properties to apply.
     * @param options Apply the hash so the properties are in the same order as Inkscape writes them.
     * @returns nothing
     */
    void setStyleProperties(const QHash<QString, QString> styleProperties, const StylePropertySortOptions& options = Unsorted);

    /**
     * @brief Returns the transform attribute of the current node as a matrix.
     * @returns The matrix for the transform atttibute.
     */
    QMatrix transformMatrix() const;

    /**
     * @brief Sets the transform attribute of the current node.
     *
     * @param matrix The matrix to apply.
     * @param options Should we apply matrix to the current matrix?
     *     We modify matrix internally if @p options includes ApplyToCurrentMatrix, so it can't
     *     be passed as const.
     *     Normally we want to apply the existing matrix. If we apply the matrix,
     *     we potentially end up squaring with each call, e.g. x^2. 
     * @returns nothing
     */
    void setTransformMatrix(QMatrix& matrix, const MatrixOptions& options = ApplyToCurrentMatrix);

private:


    /**
     * @brief d-pointer
     */
    KGameSvgDocumentPrivate * const d;

};
Q_DECLARE_OPERATORS_FOR_FLAGS(KGameSvgDocument::MatrixOptions)
Q_DECLARE_OPERATORS_FOR_FLAGS(KGameSvgDocument::StylePropertySortOptions)

#endif // _KGAMESVGDOCUMENT_H_
