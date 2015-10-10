#include "projectmodel.h"

#include <QtGui>

#include <QtDebug>
#include <iostream>

ProjectTreeItem::ProjectTreeItem(Project *project, QVariant columnValue, ProjectTreeItem *parent)
    :mProject(project), mColumnValue(columnValue), mParent(parent)
{}

ProjectTreeItem::~ProjectTreeItem()
{
    qDeleteAll(childItems);
}

void ProjectTreeItem::appendChild(ProjectTreeItem *item)
{
    childItems.append(item);
}

ProjectTreeItem *ProjectTreeItem::child(int row)
{
    return childItems.value(row);
}

int ProjectTreeItem::childCount() const
{
    return childItems.count();
}

int ProjectTreeItem::columnCount() const
{
    return 1;
}

QVariant ProjectTreeItem::data(int column, int role) const
{
    if(column == 0)
    {
        if( QString(mColumnValue.typeName()) == "CompleteAnnotationItem*" && role != Qt::UserRole)
        {
            CompleteAnnotationItem *annot = mColumnValue.value<CompleteAnnotationItem*>();

            //If sentid exists in the first non-virtual node (not root node so)
            //Put that id, otherwise keeps a normal id
            ConllGraph *currentGraph = annot->getItem()->getCurrentParse();
            ConllGraph::ordered_node_iter onb = currentGraph->orderedNodes().first;
            //Get first node without counting the root node
            //(which is virtual and does not contain sentid)
            onb++;
            QString id = currentGraph->nodeProperties(onb->second).feature("sentid");
            if(id.isEmpty())
                id = QString::number(annot->getItem()->getId());

            return "(" + id + ") " + annot->getItem()->toString();
        }
        return mColumnValue;
    }
    return 0;
}

void ProjectTreeItem::setData(QVariant value)
{
    mColumnValue = value;
}

ProjectTreeItem *ProjectTreeItem::parent()
{
    return mParent;
}

int ProjectTreeItem::row() const
{
    if (mParent)
        return mParent->childItems.indexOf(const_cast<ProjectTreeItem*>(this));
    return 0;
}

/*************************************************/

ProjectModel::ProjectModel(QObject *parent) :
    QAbstractItemModel(parent)
{
    rootItem = new ProjectTreeItem(NULL, "Project");
}

ProjectModel::~ProjectModel()
{
    delete rootItem;
}

void ProjectModel::addProject(Project *project)
{
    //Add project and its name.
    ProjectTreeItem *childProject = new ProjectTreeItem(project, project->projectName(), rootItem);

    //Add each file in the project with its name
    for(int i = 0; i < project->filesCount(); i++)
    {
        StringAndAnnotations saa = project->file(i);
        ProjectTreeItem *childFile = new ProjectTreeItem(project, saa.first, childProject);

        //For each file add the annotations (one per row in the model)
        for(int j = 0; j < saa.second.size(); j++)
        {
            QVariant var;
            var.setValue(saa.second.at(j));

            ProjectTreeItem * childAnnotation = new ProjectTreeItem(project, var, childFile);
            childFile->appendChild(childAnnotation);
        }

        childProject->appendChild(childFile);
    }

    rootItem->appendChild(childProject);
}

void ProjectModel::rollback(QModelIndex &index, int n)
{
    ProjectTreeItem *item = static_cast<ProjectTreeItem*>(index.internalPointer());
    QVariant var = item->data(0, Qt::UserRole);

    CompleteAnnotationItem *currentAnnotation = var.value<CompleteAnnotationItem*>();
    Revision<AnnotationItem*> revision = currentAnnotation->getRevision(n);

    RevisionAnnotationList revisions;

    for(int i = 0; i < n; i++){
        revisions << currentAnnotation->getRevision(i);
    }
    CompleteAnnotationItem *newAnnotation =
            new CompleteAnnotationItem(revision.getValue()->getId(),
                                       revision.getValue(), revisions);

    var.setValue<CompleteAnnotationItem*>(newAnnotation);
    item->setData(var);
}

void ProjectModel::commit(QModelIndex &index)
{
    ProjectTreeItem *item = static_cast<ProjectTreeItem*>(index.internalPointer());
    QVariant var = item->data(0, Qt::UserRole);

    CompleteAnnotationItem *oldAnnot = var.value<CompleteAnnotationItem*>();

    AnnotationItem* oldItem = oldAnnot->getItem();

    AnnotationItem* newItem = new AnnotationItem(oldItem->getId(),
                                                 new ConllGraph(*oldItem->getOriginalParse()),
                                                 new ConllGraph(*oldItem->getEditableParse()));

    RevisionAnnotationList revisions;
    for(int i = 0; i < oldAnnot->getRevisionsCount(); i++)
        revisions << oldAnnot->getRevision(i);
    revisions << Revision<AnnotationItem*>(oldItem);

    CompleteAnnotationItem *newAnnot = new CompleteAnnotationItem(oldAnnot->id(), newItem, revisions);

    item->setData(QVariant::fromValue(newAnnot));

    //Which file the item belongs ?
    QString filename = item->parent()->data(0).toString();
    //Get the annotation and update
    int indexFile = item->project()->findFile(filename);
    item->project()->changeAnnotation( indexFile , index.row(), newAnnot );
    //item->project()->addModified( indexFile, index.row() );

}


void ProjectModel::save(Project *project)
{
    project->saveProject();
}

int ProjectModel::columnCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return static_cast<ProjectTreeItem*>(parent.internalPointer())->columnCount();
    else
        return rootItem->columnCount();
}

QVariant ProjectModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    if (role != Qt::DisplayRole && role != Qt::UserRole)
        return QVariant();

    ProjectTreeItem *item = static_cast<ProjectTreeItem*>(index.internalPointer());

    return item->data(index.column(), role);
}

bool ProjectModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if(!index.isValid())
        return false;

    if(role == Qt::EditRole)
    {
        ProjectTreeItem *item = static_cast<ProjectTreeItem*>(index.internalPointer());
        item->setData(value);
        return true;
    }

    return false;
}

Qt::ItemFlags ProjectModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return 0;

    return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
}

QVariant ProjectModel::headerData(int section, Qt::Orientation orientation,
                               int role) const
{
    Q_UNUSED(section);

    if (orientation == Qt::Horizontal && role == Qt::DisplayRole)
        return "Project";

    return QVariant();
}

QModelIndex ProjectModel::index(int row, int column, const QModelIndex &parent)
            const
{
    if (!hasIndex(row, column, parent))
        return QModelIndex();

    ProjectTreeItem *parentItem;

    if (!parent.isValid())
        parentItem = rootItem;
    else
        parentItem = static_cast<ProjectTreeItem*>(parent.internalPointer());

    ProjectTreeItem *childItem = parentItem->child(row);
    if (childItem)
        return createIndex(row, column, childItem);
    else
        return QModelIndex();
}

QModelIndex ProjectModel::parent(const QModelIndex &index) const
{
    if (!index.isValid())
        return QModelIndex();

    ProjectTreeItem *childItem = static_cast<ProjectTreeItem*>(index.internalPointer());
    ProjectTreeItem *parentItem = childItem->parent();

    if (parentItem == rootItem)
        return QModelIndex();

    return createIndex(parentItem->row(), 0, parentItem);
}

int ProjectModel::rowCount(const QModelIndex &parent) const
{
    ProjectTreeItem *parentItem;
    if (parent.column() > 0)
        return 0;

    if (!parent.isValid())
        parentItem = rootItem;
    else
        parentItem = static_cast<ProjectTreeItem*>(parent.internalPointer());

    return parentItem->childCount();
}

