#ifndef COMMANDS_H
#define COMMANDS_H

#include <QUndoCommand>
#include <QMap>

#include "graph/conllgraph.h"
#include "scene/scenecreator.h"
#include "scene/graphicsitem.h"

#include "ui/commentplainedit.h"

class SceneCreator;

/**
 * @brief The AddEdgeCommand class
 *
 * UndoCommand when we add an edge
 */
class AddEdgeCommand : public QUndoCommand
{
public:
    /**
     * @brief AddEdgeCommand
     * Constructor
     * @param src : source node
     * @param tar : target node
     * @param prop : new edge properties
     * @param graph : instance of the graph
     * @param creator : instance of the scene
     * @param parent : UndoCommand parent (always 0)
     */
    AddEdgeCommand(unsigned src, unsigned tar, ConllEdgeProperties prop,
                   ConllGraph *graph, SceneCreator *creator, QUndoCommand *parent = 0);

    /**
     * @brief undo
     * Reset transformation
     */
    void undo();

    /**
     * @brief redo
     * Add the new edge
     */
    void redo();   

private:
    unsigned mSrc, mTar;
    ConllEdgeProperties mProp;
    SceneCreator *mCreator;
    ConllGraph *mGraph;
};

/**
 * @brief The RemoveEdgeCommand class
 * UndoCommand for removing an edge
 */
class RemoveEdgeCommand : public QUndoCommand
{
public:
    /**
     * @brief RemoveEdgeCommand
     * Constructor
     * @param eids : List of edge ids to be removed
     * @param graph : Graph instance to modify
     * @param creator : Scene instance
     * @param parent : UndoCommand parent (always 0)
     */
    RemoveEdgeCommand(QList<unsigned> eids, ConllGraph *graph, SceneCreator *creator, QUndoCommand *parent = 0);

    /**
     * @brief undo
     * Reset transformations(s)
     */
    void undo();

    /**
     * @brief redo
     * Remove the edge(s)
     */
    void redo();

private:
    QList<unsigned> mEids;
    ConllGraph *mGraph;
    SceneCreator *mCreator;

    //Internal
    QMap<unsigned, QPair<unsigned, unsigned> > id2nodes;
    QMap<unsigned, ConllEdgeProperties> id2properties;
};

/**
 * @brief The InvertEdgeCommand class
 *
 * UndoCommand for inverting an edge (switch source and target)
 */
class InvertEdgeCommand : public QUndoCommand
{
public:
    /**
     * @brief InvertEdgeCommand
     * Constructor
     * @param eids : List of edge ids to invert
     * @param graph : Graph instance
     * @param creator : Creator instance
     * @param parent : UndoCommand parent (always 0)
     */
    InvertEdgeCommand(QList<unsigned> eids, ConllGraph *graph, SceneCreator *creator, QUndoCommand *parent = 0);

    /**
     * @brief undo
     * Reset transformation(s)
     */
    void undo();

    /**
     * @brief redo
     * Invert list of edges
     */
    void redo();

private:
    QList<unsigned> mEids;
    ConllGraph *mGraph;
    SceneCreator *mCreator;
};

/**
 * @brief The ChangeLabelCommand class
 *
 * UndoCommand for changing a label
 */
class ChangeLabelCommand : public QUndoCommand
{
public:
    /**
     * @brief ChangeLabelCommand
     * Constructor
     * @param ids : List of edge ids
     * @param graph : Graph instance
     * @param creator : Scene instance
     * @param parent : UndoCommand parent (always 0)
     */
    ChangeLabelCommand(QList<QPair<unsigned, QString> > ids,
                       ConllGraph *graph, SceneCreator *creator, QUndoCommand *parent = 0);

    /**
     * @brief undo
     * Reset transformation(s)
     */
    void undo();

    /**
     * @brief redo
     * Change edges' label
     */
    void redo();

private:
    QList<QPair<unsigned, QString> > mIds;
    ConllGraph *mGraph;
    SceneCreator *mCreator;

    //Internal
    QList<QPair<unsigned, QString> > mIdsBefore;
};

/**
 * @brief The ChangeEdgeTypeCommand class
 * Change edges' type command
 */
class ChangeEdgeTypeCommand : public QUndoCommand
{

public:
    /**
     * @brief ChangeEdgeTypeCommand
     * @param ids : List of edge ids
     * @param type : New edge type
     * @param graph : Graph instance
     * @param creator : Scene instance
     * @param parent : UndoCommand (always 0)
     */
    ChangeEdgeTypeCommand(QList<unsigned > ids, ConllEdgeProperties::DependencyType type,
                          ConllGraph *graph,
                          SceneCreator *creator,
                          QUndoCommand *parent = 0);

    /**
     * @brief undo
     * Reset transformation
     */
    void undo();

    /**
     * @brief redo
     * Change edges' type (deep, surfacic, both)
     */
    void redo();

private:
    QList<unsigned> mIds;
    ConllEdgeProperties::DependencyType mType;
    ConllGraph *mGraph;
    SceneCreator *mCreator;

    //Internal
    QList<QPair<unsigned, ConllEdgeProperties::DependencyType> > mTypesBefore;
};

//////////////////////////////////////

/**
 * @brief The AddNodeCommand class
 * Add new (elliptic) node
 */
class AddNodeCommand : public QUndoCommand
{
public:
    /**
     * @brief AddNodeCommand
     * @param nid : new node id
     * @param graph : Graph instance
     * @param creator : Scene instance
     * @param parent : Undocommand' parent (always 0)
     */
    AddNodeCommand(unsigned nid, ConllGraph *graph,
                   SceneCreator *creator, QUndoCommand *parent = 0);

    /**
     * @brief undo
     * Reset transformation
     */
    void undo();

    /**
     * @brief redo
     * Add new node
     */
    void redo();

private:
    unsigned mNid;
    ConllGraph *mGraph;
    SceneCreator *mCreator;
};

/**
 * @brief The CoindexNodesCommand class
 *
 * Command to coindex two nodes (one elliptic node and
 * one normal node)
 */
class CoindexNodesCommand : public QUndoCommand
{
public:
    /**
     * @brief CoindexNodesCommand
     * @param nid1 : first node id to coindex
     * @param nid2 : second node id to coindex
     * @param graph : Graph instance
     * @param creator : Scene instance
     * @param parent : Undocommand parent (always 0)
     */
    CoindexNodesCommand(unsigned nid1, unsigned nid2, ConllGraph *graph,
                   SceneCreator *creator, QUndoCommand *parent = 0);

    /**
     * @brief undo
     * Reset transformation
     */
    void undo();

    /**
     * @brief redo
     * Coindex nodes
     */
    void redo();

private:
    unsigned mNid1, mNid2;
    ConllGraph *mGraph;
    SceneCreator *mCreator;

    //Internal
    unsigned old_coindexation;
};

/**
 * @brief The ChangeNodePropertiesCommand class
 * Command to change node properties
 */
class ChangeNodePropertiesCommand : public QUndoCommand
{
public:
    /**
     * @brief ChangeNodePropertiesCommand
     * @param node : Node to change
     * @param prop : New properties
     * @param graph : Graph instance
     * @param creator : Scene instance
     * @param parent : Undocommand parent (always 0)
     */
    ChangeNodePropertiesCommand(Node *node, ConllNodeProperties prop,
                                ConllGraph *graph,
                                SceneCreator *creator,
                                QUndoCommand *parent = 0);

    /**
     * @brief undo
     * Reset transformation
     */
    void undo();

    /**
     * @brief redo
     * Change node properties
     */
    void redo();

private:
    Node *mNode;
    ConllNodeProperties mProp;
    ConllGraph *mGraph;
    SceneCreator *mCreator;

    //Internal
    ConllNodeProperties oldProp;
};

/**
 * @brief The AddCommentCommand class
 * Add new comment for the sentence
 */
class AddCommentCommand : public QUndoCommand
{
public:
    /**
     * @brief AddCommentCommand
     * @param oldc : Old comment (might be empty)
     * @param newc : New comment
     * @param commentBox : Comment Box Widget to update
     * @param graph : Graph instance
     * @param creator : Scene instance
     * @param parent : Undocommand parent (always 0)
     */
    AddCommentCommand(QString oldc, QString newc,
                      CommentPlainEdit *commentBox,
                      ConllGraph *graph,
                      SceneCreator *creator, QUndoCommand *parent = 0);

    /**
     * @brief undo
     * Reset transformation
     */
    void undo();

    /**
     * @brief redo
     * Add new comment
     */
    void redo();

private:
    QString mOldComment, mNewComment;
    CommentPlainEdit *mCommentBox;
    ConllGraph *mGraph;
    SceneCreator *mCreator;
};

#endif // COMMANDS_H
