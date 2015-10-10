#ifndef PROJECTMANAGERWIDGET_H
#define PROJECTMANAGERWIDGET_H

#include <QTreeView>
#include <QWidget>

class ProjectManagerWidget: public QWidget
{
    Q_OBJECT
public:
    ProjectManagerWidget(QWidget *parent = 0);
    QTreeView *getProjectTree() const {return projectTree;}

private:
    QTreeView *projectTree;

};

#endif // PROJECTMANAGERWIDGET_H
