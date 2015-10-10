#ifndef GRAPHICSITEM_H
#define GRAPHICSITEM_H

#include <QGraphicsItem>
#include <QHash>

#include "graph/conllgraph.h"

class Node;
class EdgeLabel;

/**
 * @brief The EdgeInfo struct
 * Structure grouping all we need to know when drawing a node
 * It's a helping structure for tree layout
 *
 * It contains :
 * - source node and target node
 * - x position of source node and target node
 * - edge id
 * - edge properties
 * - edge level (which y position it must have)
 * - does it need to be under the tree (to avoid edge crossing)
 */
struct EdgeInfo {
    Node *src, *tar;
    float x_src, x_tar;
    unsigned eid;
    ConllEdgeProperties edgeProperties;
    int level;
    bool under;

    EdgeInfo(){}

    EdgeInfo(Node *_src, Node *_tar, float _p_src, float _p_tar, unsigned _eid,
             ConllEdgeProperties prop, int _level, bool _under):
        src(_src), tar(_tar), x_src(_p_src), x_tar(_p_tar),
        eid(_eid), edgeProperties(prop), level(_level), under(_under)
    {}
};


/**
 * @brief The Edge class
 * A graphical edge
 */
class Edge : public QObject, public QGraphicsItem
{
    Q_OBJECT
public:
    /**
     * @brief Edge
     * Constructor
     * @param sourceNode : source node
     * @param destNode : target node
     * @param id : edge id
     * @param level : y position of the edge
     * @param _x_src : x position of the source
     * @param _x_target : x position of the target
     * @param under : does it need to be under the tree
     * @param color : edge color
     */
    Edge(Node *sourceNode, Node *destNode, unsigned id,
         unsigned level, float _x_src, float _x_target, bool under, QColor color);

    /**
     * @brief sourceNode
     * @return source node
     */
    Node *sourceNode() const;

    /**
     * @brief destNode
     * @return target node
     */
    Node *destNode() const;


    enum { Type = UserType + 2 };
    int type() const { return Type; }

    /**
     * @brief id
     * @return edge id
     */
    unsigned id()const {return _id;}

    /**
     * @brief adjust
     * Update edge position when dragging (disabled)
     */
    void adjust();

    /**
     * @brief isUnder
     * @return true if the edge is under the tree, false otherwise
     */
    bool isUnder()const {return _under;}

protected:
    /**
     * @brief boundingRect
     * @return bounding rect of the edge (when updating the tree or the edge, the
     * bounding rect is the only updated space
     */
    QRectF boundingRect() const;

    /**
     * @brief paint
     * Method to draw the edge with the right position
     * and right color
     * @param painter
     * @param option
     * @param widget
     */
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

    /**
     * @brief mouseDoubleClickEvent
     * @param event
     * Event released when double clicking on an edge
     */
    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event);

signals:
    /**
     * @brief doubleClicked
     * Signal emitted when double clicking on a node
     */
    void doubleClicked();

private:
    /**
     * @brief calculateEdge
     * Edge position calculations
     * @param rounded : size of the rounded radius
     * @return a map with all the properties
     */
    QHash<QString, QPointF> calculateEdge(int rounded);

private:
    Node *source, *dest;
    unsigned _id;

    QPointF sourcePoint;
    QPointF destPoint;
    QPointF topLeft;
    QPointF bottomRight;
    qreal arrowSize;
    unsigned _level;

    float x_src, x_target;
    bool _under;
    QColor _color;

    //Distance between node and top of the edge
    const int edge_dist;
};

/**
 * @brief The EdgeLabel class
 * Graphical edge label
 */
class EdgeLabel: public QGraphicsTextItem
{
    Q_OBJECT
public:
    /**
     * @brief EdgeLabel
     * @param parent : edge parent of the label
     * @param label : label text
     * @param scene : parent's scene
     */
    EdgeLabel(Edge *parent, QString label, QColor color);

    /**
     * @brief id
     * @return edge id
     */
    unsigned id()const {return mId;}

protected:
    /**
     * @brief mouseDoubleClickEvent
     * @param event
     * Event released when double clicking on an edge
     */
    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event);

signals:
    /**
     * @brief doubleClicked
     * Signal emitted when double clicking on a node
     */
    void doubleClicked();

private:
    unsigned mId;
    QString mLabel;
    QColor mColor;
};

/**
 * @brief The Node class
 * Graphical node
 */
class Node : public QGraphicsTextItem
{
    Q_OBJECT
public:
    /**
     * @brief Node
     * @param level : node level
     * @param id : node id
     * @param edges_number : number of edges for the node (in edges and out edges)
     */
    Node(int level, unsigned id, int edges_number);

    /**
     * @brief addEdge
     * @param edge
     * Add a new edge to the node
     */
    void addEdge(Edge *edge);

    /**
     * @brief edges
     * @return a list of edges
     */
    QList<Edge *> edges() const;

    enum { Type = UserType + 1 };
    int type() const { return Type; }

    /**
     * @brief calculateForces
     * @obsolete
     * It's taken care of an elastic
     * layout of the graph
     */
    void calculateForces();

    /**
     * @brief advance
     * @obsolete
     * @see calculateForces
     * @return true or false
     */
    bool advance();

    /**
     * @brief getOptimalPoint
     * @param dist
     * @return the optimal point for the an edge
     */
    QPointF getOptimalPoint(QPointF dist);

    /**
     * @brief id
     * @return node id
     */
    unsigned id()const {return _id;}

    /**
     * @brief getLevel
     * @return node level
     */
    int getLevel()const {return level;}

    /**
     * @brief getEdgesNumber
     * @return edges number
     */
    int getEdgesNumber() const {return _edges_number;}


    int getNumberOfAttachedEdges() const{return _number_of_attached_edges;}
    void attachOneEdge() {_number_of_attached_edges++;}

signals:
    /**
     * @brief doubleClicked
     * Signal emitted when double clicking on a node
     */
    void doubleClicked();

protected:    
    QVariant itemChange(GraphicsItemChange change, const QVariant &value);

    /**
     * @brief mousePressEvent
     * @param event
     * Event released when the mouse is pressed
     */
    void mousePressEvent(QGraphicsSceneMouseEvent *event);

    /**
     * @brief mouseReleaseEvent
     * @param event
     * Event emitted when the mouse is released
     */
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);

    /**
     * @brief mouseDoubleClickEvent
     * @param event
     * Event emitted when the mouse is double clicked.
     */
    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event);

private:
    QList<Edge *> edgeList;
    QPointF newPos;
    int level;
    unsigned _id;
    int _edges_number;
    int _number_of_attached_edges;
};

#endif // GRAPHICSITEM_H
