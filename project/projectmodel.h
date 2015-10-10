#ifndef PROJECTMODEL_H
#define PROJECTMODEL_H

#include <QAbstractItemModel>

#include "project/project.h"

class ProjectTreeItem{

public:
    ProjectTreeItem(Project *project, QVariant columnValue = 0, ProjectTreeItem *parent = 0);
    ~ProjectTreeItem();

    void appendChild(ProjectTreeItem *child);

    ProjectTreeItem *child(int row);
    int childCount() const;
    int columnCount() const;
    QVariant data(int column, int role = Qt::DisplayRole) const;
    void setData(QVariant value);

    int row() const;
    ProjectTreeItem *parent();
    Project *project()const {return mProject;}

private:
    QList<ProjectTreeItem*> childItems;
    Project *mProject;
    QVariant mColumnValue;
    ProjectTreeItem *mParent;
};

class ProjectModel : public QAbstractItemModel
{
    Q_OBJECT    

public:
    ProjectModel(QObject *parent = 0);
    ~ProjectModel();

    void addProject(Project *project);

    QVariant data(const QModelIndex &index, int role) const;
    bool setData(const QModelIndex &index, const QVariant &value, int role);

    Qt::ItemFlags flags(const QModelIndex &index) const;
    QVariant headerData(int section, Qt::Orientation orientation,
                        int role = Qt::DisplayRole) const;
    QModelIndex index(int row, int column,
                      const QModelIndex &parent = QModelIndex()) const;
    QModelIndex parent(const QModelIndex &child) const;
    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    int columnCount(const QModelIndex &parent = QModelIndex()) const;

    void commit(QModelIndex& index);
    void rollback(QModelIndex& index, int n);

    void save(Project *project);

private:
    ProjectTreeItem *rootItem;

};

#endif // PROJECTMODEL_H
