#include "scenecreator.h"

#if QT_VERSION >= 0x050000
    #include <QtWidgets>
    #include <QtGui>
#else
    #include <QtGui>
#endif

#include <QtDebug>
#include <cmath>

//////////////////////////////
// GraphicsScene reimplementation for contextmenu
//////////////////////////////
GraphScene::GraphScene(QObject *parent)
    :QGraphicsScene(parent)
{
    menu = new QMenu();
}

void GraphScene::contextMenuEvent(QGraphicsSceneContextMenuEvent *event)
{
    menu->exec(event->screenPos());
}

void GraphScene::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    if (event->button() != Qt::LeftButton) {
        event->accept();
        return;
    }

    QGraphicsScene::mousePressEvent(event);
}

void GraphScene::keyPressEvent(QKeyEvent *event)
{
    switch(event->key()){
    case Qt::Key_Left:
        prevItem();
        break;

    case Qt::Key_Right:
        nextItem();
        break;

    default:
        QGraphicsScene::keyPressEvent(event);
    }
}

void GraphScene::nextItem()
{
    QList<QGraphicsItem*> items = this->selectedItems();
    if(items.size() != 1)
        return;

    QGraphicsItem *item = items[0];
    qreal x = item->sceneBoundingRect().x();
    qreal y = item->sceneBoundingRect().y();
    qreal w = item->sceneBoundingRect().width();
    qreal h = item->sceneBoundingRect().height()/2.0;

    QPainterPath path;
    path.addRect(x+w+35, y+h, 5, 1);

    this->setSelectionArea(path);
}

void GraphScene::prevItem()
{
    QList<QGraphicsItem*> items = this->selectedItems();
    if(items.size() != 1)
        return;

    QGraphicsItem *item = items[0];
    qreal x = item->sceneBoundingRect().x();
    qreal y = item->sceneBoundingRect().y();
    qreal w = item->sceneBoundingRect().width();
    qreal h = item->sceneBoundingRect().height()/2.0;

    QPainterPath path;
    path.addRect(x-40, y+h, 5, 1);

    this->setSelectionArea(path);
}


//////////////////////////////

bool sortByLevel(const EdgeInfo &e1, const EdgeInfo &e2)
{
    return e1.level <= e2.level;
}

SceneCreator::SceneCreator(QUndoStack *stack,
                           CommentPlainEdit *commentBox,
                           QObject *parent) :
    QObject(parent), mStack(stack), mCommentBox(commentBox)
{
    mScene = new GraphScene(this);
    contextMenu = new QMenu();
    mGraph = NULL;
    edgeEditor = NULL;        
}


void SceneCreator::computeEdgesPosition(ConllGraph *g,
                                           unsigned id,
                                           QMap<unsigned, EdgeInfo> &edges)
{
    //Edges which need to be under the sentence
    QSet<unsigned> underEdges;

    Node *graphicNode = reverseNodesModel.value(id);
    unsigned inEdgesNum  = g->nodeInDegree(id);
    unsigned outEdgesNum = g->nodeOutDegree(id);
    unsigned total       = inEdgesNum + outEdgesNum;

    //Compute edges which go/come from the left
    //and edges which go/come from the right
    std::multimap<int, ConllGraph::Edge> leftEdges, rightEdges;
    QMap<QPair<unsigned, unsigned>, int > multiEdges;

    ConllGraph::in_edge_iter ieb, iee;
    for(boost::tie(ieb, iee) = g->inEdges(id); ieb != iee; ++ieb)
    {
        ConllGraph::Edge e = *ieb;

        /*if(edges.find(g->edgeProperties(e).id) != edges.end())
            continue;*/

        unsigned srcId = g->nodeProperties(g->sourceNode(e)).id;
        Node *graphicSrc = reverseNodesModel.value( srcId );
        int level = qAbs(graphicNode->getLevel() - graphicSrc->getLevel());

        int c = multiEdges.value(qMakePair(srcId, id), 0);

        if( graphicSrc->pos().x() < graphicNode->pos().x() ){
            /*  --------------------
             * |                    |
             * |                   \|/
             * .graphicSrc       .graphicNode
             */
            if(!g->edgeProperties(e).original() || leftEdges.find(level) != leftEdges.end()
                    || g->edgeProperties(e).type() == ConllEdgeProperties::Deep)
                underEdges.insert(g->edgeProperties(e).id);

            if(c > 0)
                leftEdges.insert(make_pair(c+level, e ));
            else
                leftEdges.insert(make_pair(level, e ));
        }else{
            /*   --------------------
             *  |                    |
             * \|/                   |
             * .graphicNode       .graphicSrc
             */
            if(!g->edgeProperties(e).original() || rightEdges.find(level) != rightEdges.end()
                    || g->edgeProperties(e).type() == ConllEdgeProperties::Deep)
                underEdges.insert(g->edgeProperties(e).id);

            if(c > 0)
                rightEdges.insert(make_pair(c+level, e ));
            else
                rightEdges.insert(make_pair(level, e ));
        }        
        multiEdges.insert( qMakePair(srcId, id), multiEdges.value(qMakePair(srcId, id), 0)+1 );
    }

    ConllGraph::out_edge_iter oeb, oee;
    for(boost::tie(oeb, oee) = g->outEdges(id); oeb != oee; ++oeb)
    {
        ConllGraph::Edge e = *oeb;

        unsigned tarId = g->nodeProperties(g->targetNode(e)).id;
        Node* graphicTarget = reverseNodesModel.value( tarId );
        int level = qAbs(graphicTarget->getLevel() - graphicNode->getLevel());

        int c = multiEdges.value(qMakePair(id, tarId), 0);

        if( graphicNode->pos().x() < graphicTarget->pos().x() ){

            /*   --------------------
             *  |                    |
             *  |                   \|/
             * .graphicNode       .graphicTarget
             */

            if(!g->edgeProperties(e).original())//if(rightEdges.find(level) != rightEdges.end())
                underEdges.insert(g->edgeProperties(e).id);

            if(c > 0)
                rightEdges.insert(make_pair(c+level , e));
            else
                rightEdges.insert(make_pair(level , e));

            //if(g->nodeInDegree(id) >= 2 && !g->edgeProperties(e).original())
            //    underEdges.insert(g->edgeProperties(e).id);
        }else{

            /*    ----------------------
             *   |                      |
             *  \|/                     |
             * .graphicTarget       .graphicNode
             */

            if(!g->edgeProperties(e).original())
            //if(leftEdges.find(level) != leftEdges.end())
                underEdges.insert(g->edgeProperties(e).id);

            if(c > 0)
                leftEdges.insert(make_pair(c+level, e));
            else
                leftEdges.insert(make_pair(level, e));

            //if(g->nodeInDegree(id) >= 2 && !g->edgeProperties(e).original())
            //    underEdges.insert(g->edgeProperties(e).id);
        }
        multiEdges.insert( qMakePair(id, tarId), multiEdges.value(qMakePair(id, tarId), 0)+1 );
    }  

    //Compute the position of each edge on that specific node
    float step = (graphicNode->boundingRect().width() - 4) / (float)total;

    //Traverse in ascending order
    int count = 0;

    //Typedef for clarity
    typedef std::multimap<int, ConllGraph::Edge> EdgesMultiMap;
    typedef EdgesMultiMap::const_iterator EdgesMultiMapIter;
    typedef EdgesMultiMap::const_reverse_iterator EdgesMultiMapRIter;

    for(EdgesMultiMapIter it = leftEdges.begin(); it != leftEdges.end(); ++it)
    {
        ConllGraph::Edge e = it->second;
        unsigned eid = g->edgeProperties(e).id;
        Node *src  = reverseNodesModel.value( g->nodeProperties(g->sourceNode(e)).id )
                , *tar = reverseNodesModel.value( g->nodeProperties(g->targetNode(e)).id );

        int new_i = count+1;
        int level = it->first;//qAbs( src->getLevel() - tar->getLevel() );

        float relativeXTarToScene = tar->scenePos().x();
        float relativeXSrcToScene = src->scenePos().x();


        if(src == graphicNode)
        {
            if(edges.find(eid) != edges.end())
            {
                EdgeInfo e_info = edges.value(eid);
                e_info.x_src = count * step + relativeXSrcToScene;
                edges.insert(eid, e_info);
            }
            else
            {
                edges.insert(eid, EdgeInfo( src, tar, new_i * step + relativeXSrcToScene ,
                                            new_i * step + relativeXTarToScene,
                                            eid, g->edgeProperties(eid), level, underEdges.contains(eid) ) );
            }
        }
        else
        {
            if(edges.find(eid) != edges.end())
            {
                EdgeInfo e_info = edges.value(eid);
                e_info.x_tar = count * step + relativeXTarToScene;
                edges.insert(eid, e_info);
            }
            else{
                edges.insert( eid, EdgeInfo( src, tar,
                                             new_i * step + relativeXSrcToScene,
                                             new_i * step + relativeXTarToScene,
                                             eid, g->edgeProperties(eid), level, underEdges.contains(eid)) );
            }
        }
        count++;
    }

    //Traverse in descending order
    count = 0;
    for(EdgesMultiMapRIter it = rightEdges.rbegin(); it != rightEdges.rend(); ++it)
    {
        ConllGraph::Edge e = it->second;
        unsigned eid = g->edgeProperties(e).id;
        Node *src  = reverseNodesModel.value( g->nodeProperties(g->sourceNode(e)).id )
                , *tar = reverseNodesModel.value( g->nodeProperties(g->targetNode(e)).id );

        int new_i = count + leftEdges.size() + 1;
        int level = it->first;//qAbs( src->getLevel() - tar->getLevel() );
        count++;

        float relativeXSrcToScene = src->scenePos().x();
        float relativeXTarToScene = tar->scenePos().x();

        if(src == graphicNode)
        {

            if(edges.find(eid) != edges.end())
            {
                EdgeInfo e_info = edges.value(eid);
                e_info.x_src = new_i * step + relativeXSrcToScene;
                edges.insert(eid, e_info);
            }
            else{
                edges.insert(eid, EdgeInfo( src, tar, new_i * step + relativeXSrcToScene,
                                            new_i * step + relativeXTarToScene,
                                            eid, g->edgeProperties(eid), level, underEdges.contains(eid) ) );
            }
        }
        else
        {
            if(edges.find(eid) != edges.end())
            {
                EdgeInfo e_info = edges.value(eid);
                e_info.x_tar = new_i * step + relativeXTarToScene;
                edges.insert(eid, e_info);
            }
            else
            {
                edges.insert( eid, EdgeInfo( src, tar, new_i * step + relativeXSrcToScene,
                                             new_i * step + relativeXTarToScene,
                                             eid, g->edgeProperties(eid), level, underEdges.contains(eid)) );
            }
        }
    }
}

void SceneCreator::draw(ConllGraph *graph, bool edit/* = false*/, bool clearStack)
{
    /*qDebug() << "----------------";
    qDebug() << "IN DRAW";
    qDebug() << "----------------";*/
    editable = edit;

    //Draw same graph once
    if(mGraph == graph && clearStack){
        return;
    }

    nodesModel.clear();
    reverseNodesModel.clear();
    edgesModel.clear();
    reverseEdgesModel.clear();
    mScene->clear();

    mGraph = graph;

    if(clearStack)
        mStack->clear();

    mNode_x_pos   = 0;
    mNode_y_pos   = 0;
    mScene_height = 0;
    mLevel        = 0;

    drawGraph(graph);

    mScene->setSceneRect(QRectF(0, mNode_y_pos,
                         mNode_x_pos, mScene_height*2).adjusted(-30, -30, 40, 40));
    mScene->update(mScene->sceneRect());
}

void SceneCreator::drawGraph(ConllGraph *g)
{
    int level = 1;

    //Put the nodes in the right order
    bool first = true;
    ConllGraph::ordered_node_iter nb, ne;
    for(boost::tie(nb, ne) = g->orderedNodes(); nb != ne; ++nb)
    {
        if(first)
            mCommentBox->setPlainText(mGraph->nodeProperties(nb->first).comment());
        first = false;

        int number_edges = g->nodeDegree(nb->first);

        Node *n = drawNode(level++, nb->first, number_edges);
        reverseNodesModel.insert(nb->first, n);
        nodesModel.insert(n, nb->first);

        if(!editable){
            n->setFlags(0x00);
        }
    }

    //Keep track of visited nodes
    unordered_set<unsigned> nodes;

    //Dictionary of edges
    // Basic Idea :
    // We compute for each edge the source position and the target position
    // We keep the label, the ID and a pointer to the virtual graph representation
    //When we draw each edges, we know exactly its position and we end with a beautiful graph
    QMap<unsigned, EdgeInfo> edges;
    for(boost::tie(nb, ne) = g->orderedNodes(); nb != ne; ++nb)
    {
        unsigned id_src = nb->first;
        computeEdgesPosition(g, id_src, edges);
        nodes.insert( id_src );

        ConllGraph::out_edge_iter eb, ee;
        for(boost::tie(eb, ee) = g->outEdges(nb->first); eb != ee; ++eb)
        {
            ConllGraph::Edge e = *eb;
            unsigned id_tar = g->nodeProperties(g->targetNode(e)).id;
            if( nodes.find(id_tar) != nodes.end() )
                continue;

            computeEdgesPosition(g, id_tar, edges);
            nodes.insert(id_tar);
        }
    }

    //Draw edges
    if(edges.size() > 0)
    {
        //Get a list of edges and sort it by level
        QList<EdgeInfo> edges_list = edges.values();
        qSort(edges_list.begin(), edges_list.end(), sortByLevel);
        int real_level = 1;
        int current_level = -1;

        for(int i = 0; i < edges_list.size(); i++)
        {
            EdgeInfo e_info = edges_list.at(i);
            if(real_level < e_info.level && current_level < e_info.level)
            {
                real_level++;
                current_level = e_info.level;
            }

            //Draw Edge
            ConllEdgeProperties prop = e_info.edgeProperties;

            QColor edgeColor;
            switch((int)prop.type()){
            case (int)ConllEdgeProperties::Both:
                edgeColor.setRgb(0, 0, 0);
                break;
            case (int)ConllEdgeProperties::Surfacic:
                edgeColor.setRgb(255, 0, 0);
                break;
            case (int)ConllEdgeProperties::Deep:
                edgeColor.setRgb(0, 0, 255);
                break;
            case (int)ConllEdgeProperties::None:
                edgeColor.setRgb(122, 122, 122);
                break;
            default:
                edgeColor.setRgb(0,0,0);
            }

            Edge *e = new Edge(e_info.src, e_info.tar, e_info.eid,
                                       real_level, e_info.x_src, e_info.x_tar, e_info.under, edgeColor);

            connect(e, SIGNAL(doubleClicked()), this, SLOT(showEdgeEditor()));


            mScene->addItem(e);


            //Update y and x max position
            mNode_y_pos = min(mNode_y_pos, e->sceneBoundingRect().y());
            mScene_height = max(mScene_height, e->sceneBoundingRect().height());

            //Insert into models
            edgesModel.insert(e, e_info.eid);
            reverseEdgesModel.insert(e_info.eid, e);

            //Draw edge label
            QString label = "";
            if(prop.relFinal() == prop.relInit())
            {
                if(SchemeReader::schemeReader()->doubledRels().contains(prop.relFinal()))
                    label = prop.relFinal() + ":" + prop.relInit();
                else
                    label = prop.relFinal();
            }
            else
                label = prop.relFinal() + ":" + prop.relInit();

            EdgeLabel* e_label = drawEdgeLabel(e, label, edgeColor);
            //mScene->addItem(e_label);

            //If not editable, remove all interactions
            if(!editable)
            {
                e->setFlags(0x00);
                e_label->setFlags(0x00);
                e_label->setTextInteractionFlags(Qt::NoTextInteraction);
            }
        }

        mLevel = real_level;
    }
}

//////////////////////////
//Primitives drawing
//////////////////////////
void SceneCreator::drawTextNode(Node *node, unsigned id)
{
    ConllNodeProperties prop = mGraph->nodeProperties(id);

    QString html = "<html><head>"
                    +QString("<style>body{text-align:center;}i{color:black;}u{color:black;}.pos{color:black;}.void{color:red}</style>")
                    +QString("</head><body>");

    if(prop.type() == ConllNodeProperties::Unrealised)
        html += " <br />";
    else
        html += QString::number(id/10) + "<br />";

    if(!prop.feature("void").isEmpty())
        html += QString("<b class='void'>"+prop.token()+"</b><br />");
    else
        html += QString("<b>"+prop.token()+"</b><br />");
    html += QString("<i>"+prop.lemma()+"</i><br />");
    html += QString("<span class='hyperpos'>"+prop.hyperpos()+"</span><br />");
    html += QString("<span class='pos'>"+prop.pos()+"</span><br />");
    html += QString("</body></html>");
    node->setHtml(html);
}

Node* SceneCreator::drawNode(int level,
                             unsigned id, unsigned number_edges)
{
    Node* node = new Node(level, id, number_edges);   
    connect(node, SIGNAL(doubleClicked()), this, SLOT(showFeatures()));

    drawTextNode(node, id);
    node->setPos(mNode_x_pos, 0);
    mScene->addItem(node);
    mNode_x_pos += node->boundingRect().width() + 35;
    return node;
}


EdgeLabel* SceneCreator::drawEdgeLabel(Edge *e, QString rel, QColor color)
{
    EdgeLabel *edgeLabel = new EdgeLabel(e, rel, color);
    qreal labelWidth = edgeLabel->sceneBoundingRect().width();
    qreal edgeWidth = e->sceneBoundingRect().width();
    qreal edgeX = e->sceneBoundingRect().x();
    qreal edgeY = e->isUnder() ?
              e->sceneBoundingRect().y() + e->sceneBoundingRect().height() - 20
              : e->sceneBoundingRect().y() - 5;

    edgeLabel->setPos(edgeX + (edgeWidth/2.0 - labelWidth/2.0), edgeY);

    connect(edgeLabel, SIGNAL(doubleClicked()), this, SLOT(showEdgeEditor()));
    return edgeLabel;
}

/////////////////////////////
// Basic operations on graph
/////////////////////////////
void SceneCreator::addEdge()
{
    QList<QGraphicsItem*> selection = mScene->selectedItems();

    if(selection.size() < 2)return;

    Node *src = NULL, *tar = NULL;
    foreach(QGraphicsItem *item, selection){
        Node* node = qgraphicsitem_cast<Node*>(item);
        if(node){
            if(src)
            {
                tar = node;
                break;
            }
            else
                src = node;
        }
    }
    p_addEdge(src, tar, "_", "_");
}

void SceneCreator::p_addEdge(Node *src, Node *tar, QString init, QString final)
{
    if(src && tar){
        unsigned src_id = nodesModel.value(src), tar_id = nodesModel.value(tar);
        mStack->push(new AddEdgeCommand(src_id, tar_id,
                                        ConllEdgeProperties(mGraph->counter(),
                                                            init, final), mGraph, this));
    }
}

void SceneCreator::invertEdge()
{
    QSet<unsigned> ids;
    QList<QGraphicsItem*> selection = mScene->selectedItems();
    foreach(QGraphicsItem *item, selection){
        Edge* edge = qgraphicsitem_cast<Edge*>(item);
        EdgeLabel *edgeLabel = qgraphicsitem_cast<EdgeLabel*>(item);
        unsigned eid;
        bool found = false;
        if(edge)
        {
            found = true;
            eid = edgesModel.value(edge);
        }
        else if(edgeLabel)
        {
            found = true;
            eid = edgeLabel->id();
        }

        if(found)
            ids << eid;
        //mGraph->invertEdge(eid);
    }

    if(!ids.empty())
        mStack->push(new InvertEdgeCommand(ids.toList(), mGraph, this));
    //draw(mGraph, true);
}

void SceneCreator::removeEdge()
{
    QList<unsigned> ids;
    QList<QGraphicsItem*> selection = mScene->selectedItems();
    foreach(QGraphicsItem *item, selection){
        Edge* edge = qgraphicsitem_cast<Edge*>(item);
        EdgeLabel *edgeLabel = qgraphicsitem_cast<EdgeLabel*>(item);
        unsigned eid;
        bool found = false;
        if(edge){
            eid = edgesModel.value(edge);
            found = true;
        }else if(edgeLabel){
            eid = edgeLabel->id();
            found = true;
        }

        if(found)
            ids << eid;
    }

    if(!ids.empty())
        mStack->push(new RemoveEdgeCommand(ids, mGraph, this));
    //draw(mGraph, true);
}

void SceneCreator::changeRelation(QTreeWidgetItem *relationItem, int c)
{
    bool found = false;
    Q_UNUSED(c);

    QList<QPair<unsigned, QString> > ids;
    QList<QGraphicsItem*> selection = mScene->selectedItems();
    foreach(QGraphicsItem *item, selection){
        EdgeLabel* label = qgraphicsitem_cast<EdgeLabel*>(item);
        Edge* edge = qgraphicsitem_cast<Edge*>(item);
        if(label || edge){
            found = true;
            unsigned id = label ? label->id() : edgesModel.value(edge);
            if( ! relationItem->parent() && relationItem->text(0) != "Canonique"
                    && relationItem->text(0) != "Final" ){
                ids << qMakePair(id, relationItem->text(0));
            }else if(relationItem->parent()){
                QString rel;
                if(relationItem->parent()->text(0) == "Canonique")
                    rel = mGraph->edgeProperties(id).relFinal() + ":" + relationItem->text(0);
                else if(relationItem->parent()->text(0) == "Final")
                    rel = relationItem->text(0) + ":" + mGraph->edgeProperties(id).relInit();
                ids << qMakePair(id, rel);
            }
        }
    }

    if(found)
    {
        mStack->push(new ChangeLabelCommand(ids, mGraph, this));
        return;
    }

    //Test for two nodes, if it's the case and create the edge with the right label
    if(selection.size() < 2)return;

    Node *src = NULL, *tar = NULL;
    foreach(QGraphicsItem *item, selection){
        Node* node = qgraphicsitem_cast<Node*>(item);
        if(node){
            if(src)
            {
                tar = node;
                break;
            }
            else
                src = node;
        }
    }

    QString init = "_", final = "_";
    if( ! relationItem->parent() && relationItem->text(0) != "Canonique"
            && relationItem->text(0) != "Final" ){
        init = final = relationItem->text(0);
    }else if(relationItem->parent()){
        if(relationItem->parent()->text(0) == "Canonique")
            init = relationItem->text(0);
        else if(relationItem->parent()->text(0) == "Final")
            final = relationItem->text(0);
    }
    p_addEdge(src, tar, init, final);
}

void SceneCreator::setDeepEdge(){
    QList<unsigned> ids;
    QList<QGraphicsItem*> selection = mScene->selectedItems();
    foreach(QGraphicsItem *item, selection){
        Edge* edge = qgraphicsitem_cast<Edge*>(item);
        EdgeLabel *edgeLabel = qgraphicsitem_cast<EdgeLabel*>(item);
        unsigned eid;
        if(edge)
            eid = edgesModel.value(edge);
        else if(edgeLabel)
            eid = edgeLabel->id();        

        ids << eid;
    }
    mStack->push(new ChangeEdgeTypeCommand(ids, ConllEdgeProperties::Deep, mGraph, this));
    //draw(mGraph, true);
}

void SceneCreator::setSurfacicEdge(){
    QList<unsigned> ids;
    QList<QGraphicsItem*> selection = mScene->selectedItems();
    foreach(QGraphicsItem *item, selection){
        Edge* edge = qgraphicsitem_cast<Edge*>(item);
        EdgeLabel *edgeLabel = qgraphicsitem_cast<EdgeLabel*>(item);
        unsigned eid;
        if(edge)
            eid = edgesModel.value(edge);
        else if(edgeLabel)
            eid = edgeLabel->id();

        ids << eid;
    }

    mStack->push(new ChangeEdgeTypeCommand(ids, ConllEdgeProperties::Surfacic, mGraph, this));
    //draw(mGraph, true);
}

void SceneCreator::setBothEdge(){
    QList<unsigned> ids;
    QList<QGraphicsItem*> selection = mScene->selectedItems();
    foreach(QGraphicsItem *item, selection){
        Edge* edge = qgraphicsitem_cast<Edge*>(item);
        EdgeLabel *edgeLabel = qgraphicsitem_cast<EdgeLabel*>(item);
        unsigned eid;
        if(edge)
            eid = edgesModel.value(edge);
        else if(edgeLabel)
            eid = edgeLabel->id();

        ids << eid;
    }
    mStack->push(new ChangeEdgeTypeCommand(ids, ConllEdgeProperties::Both, mGraph, this));
    //draw(mGraph, true);
}


void SceneCreator::setNoneEdge(){
    QList<unsigned> ids;
    QList<QGraphicsItem*> selection = mScene->selectedItems();
    foreach(QGraphicsItem *item, selection){
        Edge* edge = qgraphicsitem_cast<Edge*>(item);
        EdgeLabel *edgeLabel = qgraphicsitem_cast<EdgeLabel*>(item);
        unsigned eid;
        if(edge)
            eid = edgesModel.value(edge);
        else if(edgeLabel)
            eid = edgeLabel->id();

        ids << eid;
    }
    mStack->push(new ChangeEdgeTypeCommand(ids, ConllEdgeProperties::None, mGraph, this));
    //draw(mGraph, true);
}

void SceneCreator::addUnrealisedNode(){
    QList<QGraphicsItem*> selection = mScene->selectedItems();
    if(selection.size() != 1)
        return;

    Node *node = qgraphicsitem_cast<Node*>(selection[0]);
    if(node){

        unsigned step  = 10;
        unsigned id = node->id() + step;
        bool next = true;
        while(next){
            if(step == 1)return;
            step = (int)ceil(step/2.0);
            id = node->id() + step;
            next = mGraph->existsNode(id);
        }

        if(!mGraph->existsNode(id)){
            mStack->push(new AddNodeCommand(id, mGraph, this));
        }
    }
}

void SceneCreator::coindexNodes()
{
    QList<QGraphicsItem*> selection = mScene->selectedItems();
    if(selection.size() != 2)
        return;

    Node *n1 = qgraphicsitem_cast<Node*>(selection[0]),
            *n2 = qgraphicsitem_cast<Node*>(selection[1]);

    if(n1 && n2){
        if(mGraph->nodeProperties(n1->id()).type() == ConllNodeProperties::Unrealised
                || mGraph->nodeProperties(n2->id()).type() == ConllNodeProperties::Unrealised){
            mStack->push(new CoindexNodesCommand(n1->id(), n2->id(), mGraph, this));
        }
    }
}


void SceneCreator::showFeatures()
{
    Node *node = qobject_cast<Node*>(sender());

    NodeEditor *editor = new NodeEditor(mGraph->nodeProperties(node->id()));
    if(editor->exec() == QDialog::Accepted)    
        mStack->push(new ChangeNodePropertiesCommand(node, editor->node(), mGraph, this));
}

void SceneCreator::showEdgeEditor()
{
    if(!edgeEditor)
        edgeEditor = new EdgeEditor();

    Edge *edge = qobject_cast<Edge*>(sender());
    EdgeLabel *label = qobject_cast<EdgeLabel*>(sender());

    if(edge || label){

        unsigned eid = 0;
        if(label)
            eid = label->id();
        else if(edge)
            eid = edgesModel.value(edge);

        ConllEdgeProperties &edgeProp = mGraph->edgeProperties(eid);
        edgeEditor->setDefaultRelations(edgeProp.relFinal(), edgeProp.relInit());
        if(edgeEditor->exec() == QDialog::Accepted){
            QList<QPair<unsigned, QString> > ids;
            QString rel = edgeEditor->finalRelation() + ":" + edgeEditor->canonicRelation();
            ids << qMakePair(eid, rel);

            if(edge)
                mScene->removeItem(edge);
            else
                mScene->removeItem(label);

            mStack->push(new ChangeLabelCommand(ids, mGraph, this));
        }
    }
}

void SceneCreator::addComment(QString oldComment, QString newComment)
{
    mStack->push( new AddCommentCommand(oldComment, newComment,
                                       qobject_cast<CommentPlainEdit*>(sender()),
                                       mGraph, this) );
}
