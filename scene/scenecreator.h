#ifndef SCENECREATOR_H
#define SCENECREATOR_H

#include <QObject>
#include <QGraphicsScene>
#include <QGraphicsTextItem>
#include <QHash>
#include <QTreeWidgetItem>
#include <QUndoStack>
#include <QMenu>


#include "scene/commands.h"
#include "scene/graphicsitem.h"
#include "graph/conllgraph.h"
#include "ui/nodeeditor.h"
#include "ui/edgeeditor.h"
#include "ui/commentplainedit.h"
#include "project/scheme/schemereader.h"

class GraphScene : public QGraphicsScene
{
    Q_OBJECT

public:
    GraphScene(QObject * parent = 0);

    void addToContextMenu(QList<QAction*> actions){menu->addActions(actions);}
    void addSepToContextMenu(){menu->addSeparator();}

public slots:

protected:
    void contextMenuEvent(QGraphicsSceneContextMenuEvent *event);
    void mousePressEvent(QGraphicsSceneMouseEvent *event);
    void keyPressEvent(QKeyEvent *event);

private:
    void nextItem();
    void prevItem();
private:
    QMenu *menu;

};

class SceneCreator : public QObject
{
    Q_OBJECT
public:
    explicit SceneCreator(QUndoStack *stack,
                          CommentPlainEdit *commentBox, QObject *parent = 0);

    GraphScene* scene() const{return mScene;}
    void draw(ConllGraph *graph, bool edit = false, bool clearStack = true);

    ConllGraph *graph()const {return mGraph;}

    void drawTextNode(Node *node, unsigned id);

    Edge *id2Edge(unsigned id){return reverseEdgesModel.value(id, NULL);}

public slots:
    void addEdge();
    void invertEdge();
    void removeEdge();
    void changeRelation(QTreeWidgetItem *relationItem, int c);

    void setSurfacicEdge();
    void setDeepEdge();
    void setBothEdge();
    void setNoneEdge();

    void addUnrealisedNode();
    void coindexNodes();
    void showFeatures();
    void showEdgeEditor();
    void addComment(QString oldComment, QString newComment);

private slots:
    void p_addEdge(Node *src, Node *tar, QString init, QString final);

private:
    bool isEdgeIsUnder(const QList<ConllGraph::Edge> edges);

    void computeEdgesPosition(ConllGraph *g,
                              unsigned id,
                              QMap<unsigned, EdgeInfo> &edges);
    void drawGraph(ConllGraph *t);
    Node* drawNode(int level, unsigned id, unsigned number_edges);
    EdgeLabel* drawEdgeLabel(Edge *e, QString rel, QColor color);

private:
    QUndoStack *mStack;
    //SchemeReader *mReader;
    CommentPlainEdit *mCommentBox;

    GraphScene *mScene;
    QHash<Node*, unsigned> nodesModel;
    QMap<unsigned, Node*> reverseNodesModel;
    QHash<Edge*, unsigned> edgesModel;
    QMap<unsigned, Edge*> reverseEdgesModel;

    qreal mNode_x_pos;
    qreal mNode_y_pos;
    qreal mScene_height;
    unsigned mLevel;
    bool editable;

    ConllGraph *mGraph;

    QMenu *contextMenu;

    EdgeEditor *edgeEditor;
};

#endif // SCENECREATOR_H
