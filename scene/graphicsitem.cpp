#include "graphicsitem.h"

#include <cmath>
#if QT_VERSION >= 0x050000
    #include <QtWidgets>
    #include <QtGui>
#else
    #include <QtGui>
#endif

//----------------------------
// EDGE LABEL
//----------------------------
EdgeLabel::EdgeLabel(Edge *parent, QString label, QColor color)
    :QGraphicsTextItem(parent), mId(parent->id()), mLabel(label), mColor(color)
{    
    QTextCursor cursor = QTextCursor(this->document());
    QTextCharFormat format;
    format.setForeground(QBrush(mColor));
    cursor.setCharFormat(format);
    cursor.insertText(label, format);

    setFlag(QGraphicsItem::ItemIsSelectable);
    setTextInteractionFlags(Qt::NoTextInteraction);
    setAcceptedMouseButtons(Qt::LeftButton);
}

void EdgeLabel::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event)
{
    emit doubleClicked();
    QGraphicsItem::mouseDoubleClickEvent(event);
}

//----------------------------
// EDGE
//----------------------------

static const double Pi = 3.14159265358979323846264338327950288419717;
static double TwoPi = 2.0 * Pi;

Edge::Edge(Node *sourceNode, Node *destNode, unsigned id, unsigned level,
           float _x_src, float _x_target, bool under, QColor color)
    : arrowSize(5), edge_dist(23), _id(id), _level(level),
      x_src(_x_src), x_target(_x_target), _under(under), _color(color)
{
    //setAcceptedMouseButtons(0);

    source = sourceNode;
    dest = destNode;
    source->addEdge(this);
    dest->addEdge(this);
    adjust();

    setFlag(QGraphicsItem::ItemIsSelectable, true);
}

Node *Edge::sourceNode() const
{
    return source;
}

Node *Edge::destNode() const
{
    return dest;
}

void Edge::adjust()
{
    prepareGeometryChange();
    calculateEdge(10);
}

QHash<QString, QPointF> Edge::calculateEdge(int rounded)
{
    this->setZValue(200.0/_level);

    QPointF srcPoint = source->pos();
    QPointF tarPoint = dest->pos();
    QPointF bottomLeft, topRight;

    //For the arrow
    QLineF line;
    QPointF arrowPoint;

    QHash<QString, QPointF> points;

    //Calculate all the point
    if(srcPoint.x() < tarPoint.x()) // The edge goes to right
    {
        bottomLeft = QPointF(x_src, tarPoint.y());
        topLeft = QPointF(x_src, tarPoint.y() - edge_dist * _level);
        bottomRight = QPointF(x_target, tarPoint.y());
        topRight = QPointF(x_target, tarPoint.y() - edge_dist * _level);

        //Calculate the arrow
        //Loria orientation
        //line = QLineF(topLeft, bottomLeft);
        //arrowPoint = QPointF(line.p2().x()-rounded, line.p2().y());
        //Corentin orientation
        line = QLineF(topRight, bottomRight);
        arrowPoint = line.p2();

        points.insert("lineP1", topRight);
        points.insert("lineP2", bottomRight);
        points.insert("arrowPoint", arrowPoint);
    }
    else //The edge goes to left
    {
        bottomRight = QPointF(x_src, srcPoint.y());
        topRight = QPointF(x_src, srcPoint.y() - edge_dist * _level);
        bottomLeft = QPointF(x_target, tarPoint.y());
        topLeft = QPointF(x_target, tarPoint.y() - edge_dist * _level);

        //Calculate the arrow
        //Loria orientation
        //line = QLineF(topRight, bottomRight);
        //arrowPoint = line.p2();
        //Corentin orientation
        line = QLineF(topLeft, bottomLeft);
        arrowPoint = QPointF(line.p2().x()-rounded, line.p2().y());

        points.insert("lineP1", topLeft);
        points.insert("lineP2", bottomLeft);
        points.insert("arrowPoint", arrowPoint);
    }
    /////

    points.insert("bottomLeft", bottomLeft);
    points.insert("topRight", topRight);

    return points;
}

QRectF Edge::boundingRect() const
{
    if (!source || !dest)
        return QRectF();

    qreal penWidth = 2;
    qreal extra = penWidth + arrowSize;

    QRectF bounding( QPointF(topLeft.x()-8, topLeft.y()-20),
                     QPointF(bottomRight.x()+8, bottomRight.y()+5) );
    bounding = bounding.normalized().adjusted(-extra, -extra, extra, extra);

    if(_under){
        QTransform matrix;
        matrix.rotate(180, Qt::XAxis);
        matrix.translate(0, -source->boundingRect().height());
        bounding = matrix.mapRect(bounding);
    }

    return bounding;
}

void Edge::paint(QPainter *painter, const QStyleOptionGraphicsItem * option, QWidget *)
{
    if (!source || !dest)
        return;

    //For the rounded corner
    int rounded = 10;

    //Calculate edge
    QHash<QString, QPointF> points = calculateEdge(rounded);
    QPointF bottomLeft = points.value("bottomLeft"), topRight = points.value("topRight");

    //For the arrow
    QLineF line(points.value("lineP1"), points.value("lineP2"));
    QPointF arrowPoint = points.value("arrowPoint");

    //Calculate text label
    //int text_w = QFontMetrics(QFont()).width(label->toPlainText());
    //int between_nodes = qAbs(topLeft.x() - topRight.x());
    //QRectF boundingText = QRectF(topLeft.x() + between_nodes/2.0 - text_w/2.0, topLeft.y()-25, text_w, QFontMetrics(QFont()).height());

    //Calculate the arrow (end)
    double angle = acos(line.dx() / line.length());
    if (line.dy() >= 0)
        angle = TwoPi - angle;
    QPointF destArrowP1 = arrowPoint + QPointF(sin(angle - Pi / 3) * arrowSize,
                                              cos(angle - Pi / 3) * arrowSize);
    QPointF destArrowP2 = arrowPoint + QPointF(sin(angle - Pi + Pi / 3) * arrowSize,
                                              cos(angle - Pi + Pi / 3) * arrowSize);

    painter->setRenderHint(QPainter::Antialiasing);


    // Set the line color and weight
    if (option->state & QStyle::State_Selected)
        painter->setPen(QPen(Qt::black, 2.5, Qt::DotLine, Qt::RoundCap, Qt::RoundJoin));
    else        
        painter->setPen(QPen(_color, 1, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));

    // Draw the edge
    QPainterPath path;   
    path.moveTo(bottomRight);
    path.lineTo(topRight);
    path.arcTo(topRight.x()-rounded, topRight.y()-5, rounded, rounded, 0.0, 90.0);
    path.lineTo(topLeft.x(), topLeft.y()-5);
    path.arcTo(topLeft.x()-rounded, topLeft.y()-5, rounded, rounded, 90.0, 90.0);
    path.lineTo(bottomLeft.x()-rounded, bottomLeft.y()-5);

    if(_under){
        QTransform matrix;
        matrix.rotate(180, Qt::XAxis);
        matrix.translate(0, -source->boundingRect().height());
        path = matrix.map(path);
    }

    painter->drawPath(path);
    //painter->drawText(boundingText , label);

    //Draw the arrow
    if (option->state & QStyle::State_Selected)
        painter->setBrush(Qt::black);
    else
        painter->setBrush(_color);

    QPolygonF arrow(QVector<QPointF>() << arrowPoint << destArrowP1 << destArrowP2);

    if (option->state & QStyle::State_Selected)
        painter->setPen(QPen(Qt::black, 2.5, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));

    if(_under){
        QTransform matrix;
        matrix.rotate(180, Qt::XAxis);
        matrix.translate(0, -source->boundingRect().height());
        arrow = matrix.map(arrow);
    }

    painter->drawPolygon(arrow);
    //////

    source->attachOneEdge();
    dest->attachOneEdge();
}

void Edge::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event)
{
    emit doubleClicked();
    QGraphicsItem::mouseDoubleClickEvent(event);
}

//----------------------------
// NODE
//----------------------------
Node::Node(int level, unsigned id, int edges_number):
    level(level), _id(id), _edges_number(edges_number)
{
    //setFlag(ItemIsMovable);
    setFlag(ItemIsSelectable);
    setFlag(ItemSendsGeometryChanges);
    setCacheMode(DeviceCoordinateCache);
    setZValue(-1);
    _number_of_attached_edges = 0;
}

QPointF Node::getOptimalPoint(QPointF dist)
{
    //Init pos of node
    QPointF position = this->pos();
    //Get center of the point
    int center = this->boundingRect().width()/2;
    return QPointF(position.x()+dist.x()+center, position.y());
}

void Node::addEdge(Edge *edge)
{
    edgeList << edge;
    edge->adjust();
}

QList<Edge *> Node::edges() const
{
    return edgeList;
}



void Node::calculateForces()
{
    if (!scene() || scene()->mouseGrabberItem() == this) {
        newPos = pos();
        return;
    }



    // Sum up all forces pushing this item away
    qreal xvel = 0;
    qreal yvel = 0;
    foreach (QGraphicsItem *item, scene()->items()) {
        Node *node = qgraphicsitem_cast<Node *>(item);
        if (!node)
            continue;

        QPointF vec = mapToItem(node, 0, 0);
        qreal dx = vec.x();
        qreal dy = vec.y();
        double l = 2.0 * (dx * dx + dy * dy);
        if (l > 0) {
            xvel += (dx * 150.0) / l;
            yvel += (dy * 150.0) / l;
        }
    }



    // Now subtract all forces pulling items together
    double weight = (edgeList.size() + 1) * 10;
    foreach (Edge *edge, edgeList) {
        QPointF vec;
        if (edge->sourceNode() == this)
            vec = mapToItem(edge->destNode(), 0, 0);
        else
            vec = mapToItem(edge->sourceNode(), 0, 0);
        xvel -= vec.x() / weight;
        yvel -= vec.y() / weight;
    }

    if (qAbs(xvel) < 0.1 && qAbs(yvel) < 0.1)
        xvel = yvel = 0;

    QRectF sceneRect = scene()->sceneRect();
    newPos = pos() + QPointF(xvel, yvel);
    newPos.setX(qMin(qMax(newPos.x(), sceneRect.left() + 10), sceneRect.right() - 10));
    newPos.setY(qMin(qMax(newPos.y(), sceneRect.top() + 10), sceneRect.bottom() - 10));
}



bool Node::advance()
{
    return false;
}

QVariant Node::itemChange(GraphicsItemChange change, const QVariant &value)
{
    switch (change) {
    case ItemPositionHasChanged:
        foreach(Edge *edge, edgeList)
            edge->adjust();
        break;
    default:
        break;
    };

    return QGraphicsTextItem::itemChange(change, value);
}

void Node::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    update();
    QGraphicsTextItem::mousePressEvent(event);
}

void Node::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    update();
    QGraphicsTextItem::mouseReleaseEvent(event);
}

void Node::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event)
{
    emit doubleClicked();
    QGraphicsTextItem::mouseDoubleClickEvent(event);
}

