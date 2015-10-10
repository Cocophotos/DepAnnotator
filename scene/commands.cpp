#include "scene/commands.h"

AddEdgeCommand::AddEdgeCommand(unsigned src, unsigned tar, ConllEdgeProperties prop,
                               ConllGraph *graph, SceneCreator *creator, QUndoCommand *parent)
    :QUndoCommand(parent)
{
    mSrc = src, mTar = tar;
    mProp = prop;
    mCreator = creator;
    mGraph = graph;
}

void AddEdgeCommand::redo()
{
    mGraph->addEdge(mSrc, mTar, mProp);
    mCreator->draw(mGraph, true, false);

    if(mCreator->id2Edge(mProp.id))
        mCreator->id2Edge(mProp.id)->setSelected(true);
}

void AddEdgeCommand::undo()
{
    mGraph->removeEdge(mProp.id);
    mCreator->draw(mGraph, true, false);
}

//////////////////////////////

InvertEdgeCommand::InvertEdgeCommand(QList<unsigned> eids,
                                     ConllGraph *graph,
                                     SceneCreator *creator,
                                     QUndoCommand *parent)
    :QUndoCommand(parent)
{
    mEids = eids;
    mGraph = graph;
    mCreator = creator;
}

void InvertEdgeCommand::undo()
{
    foreach(unsigned eid, mEids){
        mGraph->invertEdge(eid);
    }
    mCreator->draw(mGraph, true, false);
}

void InvertEdgeCommand::redo()
{
    foreach(unsigned eid, mEids){
        mGraph->invertEdge(eid);        
    }
    mCreator->draw(mGraph, true, false);

    foreach(unsigned eid, mEids){
        if(mCreator->id2Edge(eid))
            mCreator->id2Edge(eid)->setSelected(true);
    }
}

//////////////////////////////

RemoveEdgeCommand::RemoveEdgeCommand(QList<unsigned> eids,
                                     ConllGraph *graph,
                                     SceneCreator *creator,
                                     QUndoCommand *parent)
    :QUndoCommand(parent)
{
    mEids = eids;
    mGraph = graph;
    mCreator = creator;
}

void RemoveEdgeCommand::undo()
{
    foreach(unsigned eid, mEids){
        QPair<unsigned, unsigned> nodes = id2nodes.value(eid);
        mGraph->addEdge(nodes.first, nodes.second, id2properties.value(eid));
    }

    id2properties.clear();
    id2nodes.clear();
    mCreator->draw(mGraph, true, false);
}

void RemoveEdgeCommand::redo()
{
    foreach(unsigned eid, mEids){
        try{
            unsigned src = mGraph->nodeProperties(mGraph->sourceNode(eid)).id;
            unsigned tar = mGraph->nodeProperties(mGraph->targetNode(eid)).id;
            id2nodes.insert(eid, qMakePair(src, tar));
            id2properties.insert(eid, mGraph->edgeProperties(eid));
            mGraph->removeEdge(eid);
        }catch(const std::out_of_range &e){(void)e;}
    }
    mCreator->draw(mGraph, true, false);
}

//////////////////////////////

ChangeLabelCommand::ChangeLabelCommand(QList<QPair<unsigned, QString> > ids,
                                       ConllGraph *graph,
                                       SceneCreator *creator,
                                       QUndoCommand *parent)
    :QUndoCommand(parent)
{
    mIds = ids;
    mGraph = graph;
    mCreator = creator;
}

void ChangeLabelCommand::undo()
{
    typedef QPair<unsigned, QString> IdToRels;
    foreach(IdToRels p, mIdsBefore){
        QStringList rels = p.second.split(":");
        mGraph->edgeProperties(p.first).setRelFinal(rels[0]);
        mGraph->edgeProperties(p.first).setRelInit(rels[1]);
    }

    mIdsBefore.clear();
    mCreator->draw(mGraph, true, false);
}

void ChangeLabelCommand::redo()
{
    typedef QPair<unsigned, QString> IdToRels;
    foreach(IdToRels p, mIds){
        QString rel = mGraph->edgeProperties(p.first).relFinal()
                + ":" + mGraph->edgeProperties(p.first).relInit();
        mIdsBefore << qMakePair(p.first, rel);

        QStringList rels = p.second.split(":");
        mGraph->edgeProperties(p.first).setRelFinal(rels[0]);
        if(rels.size() == 1)
            mGraph->edgeProperties(p.first).setRelInit(rels[0]);
        else
            mGraph->edgeProperties(p.first).setRelInit(rels[1]);
    }

    mCreator->draw(mGraph, true, false);
}

//////////////////////////////

ChangeEdgeTypeCommand::ChangeEdgeTypeCommand(QList<unsigned> ids,
                                             ConllEdgeProperties::DependencyType type,
                                             ConllGraph *graph, SceneCreator *creator,
                                             QUndoCommand *parent)
    :QUndoCommand(parent)
{
    mIds = ids;
    mType = type;
    mGraph = graph;
    mCreator = creator;
}

void ChangeEdgeTypeCommand::redo()
{
    foreach(unsigned eid, mIds)
    {
        mTypesBefore << qMakePair(eid, mGraph->edgeProperties(eid).type());
        mGraph->edgeProperties(eid).setDependencyType(mType);
    }
    mCreator->draw(mGraph, true, false);

    foreach(unsigned eid, mIds){
        if(mCreator->id2Edge(eid))
            mCreator->id2Edge(eid)->setSelected(true);
    }
}

void ChangeEdgeTypeCommand::undo()
{
    typedef QPair<unsigned, ConllEdgeProperties::DependencyType> IdToType;
    foreach(IdToType p, mTypesBefore){
        mGraph->edgeProperties(p.first).setDependencyType(p.second);
    }

    mTypesBefore.clear();
    mCreator->draw(mGraph, true, false);

    foreach(unsigned eid, mIds){
        if(mCreator->id2Edge(eid))
            mCreator->id2Edge(eid)->setSelected(true);
    }
}

//////////////////////////////

AddNodeCommand::AddNodeCommand(unsigned nid, ConllGraph *graph,
                               SceneCreator *creator, QUndoCommand *parent)
    :QUndoCommand(parent)
{
    mNid = nid;
    mGraph = graph;
    mCreator = creator;
}

void AddNodeCommand::undo(){
    mGraph->removeNode(mNid);
    mCreator->draw(mGraph, true, false);
}

void AddNodeCommand::redo(){
    mGraph->addNode(mNid,
                    ConllNodeProperties(mNid, "_e_", "_e_", "X", "X",
                                        QMap<QString, QString>(),
                                        ConllNodeProperties::Unrealised));
    mCreator->draw(mGraph, true, false);
}

//////////////////////////////

CoindexNodesCommand::CoindexNodesCommand(unsigned nid1, unsigned nid2,
                                         ConllGraph *graph, SceneCreator *creator,
                                         QUndoCommand *parent)
    :QUndoCommand(parent)
{
    mNid1 = nid1, mNid2 = nid2;
    mGraph = graph;
    mCreator = creator;
}

void CoindexNodesCommand::redo()
{
    if(mGraph->nodeProperties(mNid1).type() == ConllNodeProperties::Unrealised){
        old_coindexation = mGraph->nodeProperties(mNid1).coindex();
        mGraph->nodeProperties(mNid1).setCoindex(mNid2);
    }else{
        old_coindexation = mGraph->nodeProperties(mNid2).coindex();
        mGraph->nodeProperties(mNid2).setCoindex(mNid1);
    }

    mCreator->draw(mGraph, true, false);
}

void CoindexNodesCommand::undo()
{
    if(mGraph->nodeProperties(mNid1).type() == ConllNodeProperties::Unrealised)
        mGraph->nodeProperties(mNid1).setCoindex(old_coindexation);
    else
        mGraph->nodeProperties(mNid2).setCoindex(old_coindexation);

    mCreator->draw(mGraph, true, false);
}

//////////////////////////////

ChangeNodePropertiesCommand::ChangeNodePropertiesCommand(Node *node, ConllNodeProperties prop,
                                                         ConllGraph *graph, SceneCreator *creator,
                                                         QUndoCommand *parent)
    :QUndoCommand(parent), mProp(prop), oldProp(prop)
{
    mNode = node;
    //mProp = prop;
    mGraph = graph;
    mCreator = creator;
}

void ChangeNodePropertiesCommand::redo()
{
    oldProp = ConllNodeProperties(mGraph->nodeProperties(mNode->id()));
    mGraph->setNodeProperties(mNode->id(), mProp);
    mCreator->drawTextNode(mNode, mNode->id());
}

void ChangeNodePropertiesCommand::undo()
{
    mGraph->setNodeProperties(mNode->id(), oldProp);
    mCreator->drawTextNode(mNode, mNode->id());
}

//////////////////////////////

AddCommentCommand::AddCommentCommand(QString oldc, QString newc,
                                     CommentPlainEdit *commentBox,
                                     ConllGraph *graph, SceneCreator *creator,
                                     QUndoCommand *parent)
    :QUndoCommand(parent)
{
    mOldComment = oldc;
    mNewComment = newc;
    mCommentBox = commentBox;
    mGraph = graph;
    mCreator = creator;
}

void AddCommentCommand::redo()
{
    ConllGraph::ordered_node_range_t ordered = mGraph->orderedNodes();
    ConllGraph::ordered_node_iter nb = ordered.first;
    mGraph->nodeProperties(nb->first).setComment(mNewComment);
    mCommentBox->setPlainText(mNewComment);
}

void AddCommentCommand::undo()
{
    ConllGraph::ordered_node_range_t ordered = mGraph->orderedNodes();
    ConllGraph::ordered_node_iter nb = ordered.first;
    mGraph->nodeProperties(nb->first).setComment(mOldComment);
    mCommentBox->setPlainText(mOldComment);
}
