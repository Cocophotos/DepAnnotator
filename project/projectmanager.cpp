#include "projectmanager.h"

#if QT_VERSION >= 0x050000
    #include <QtWidgets>
    #include <QtGui>
    #include <QtConcurrent/QtConcurrent>
#else
    #include <QtGui>
    #include <QtCore/QtConcurrentRun>
#endif

#include <QtDebug>

extern void saveProject(ProjectModel *model, Project *project)
{
    model->save(project);
}

extern bool watcher_openProject(Project *project)
{
    return project->openProject();
}

extern bool watcher_addProject(QStringList files, Project *project)
{
    bool success = true;
    foreach(QString file, files)
    {
        if(!success)
            return false;
        success = project->addFile(file);
    }
    return true;
}

ProjectManager::ProjectManager(QTreeView* _projectTree,
                               SceneCreator *currentSceneCreator,
                               SceneCreator *originalSceneCreator)
    :projectTree(_projectTree),
      mCurrentSceneCreator(currentSceneCreator),
      mOriginalSceneCreator(originalSceneCreator)
{
    committed = true;
    autocommit = true;
    //connect(projectTree, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(doubleClick(QModelIndex)));
}

void ProjectManager::addProject(QString name,
                                QString path,
                                QStringList files)
{
    project = new ConllProject(name, path);

    /*foreach(QString file, files)
    {
        project->addFile(file);
    }*/

    watcher = new QFutureWatcher<void>();
    connect(watcher, SIGNAL(finished()), this, SLOT(openCompleted()));
    QFuture<bool> future = QtConcurrent::run(watcher_addProject, files, project);
    watcher->setFuture(future);

    /*ProjectModel *model = new ProjectModel();
    model->addProject(project);
    projectTree->setModel(model);*/
}

void ProjectManager::openProject(QString filepath)
{
    project = new ConllProject(QFileInfo(filepath).baseName(), QFileInfo(filepath).absolutePath());
    watcher = new QFutureWatcher<void>();
    connect(watcher, SIGNAL(finished()), this, SLOT(openCompleted()));
    QFuture<bool> future = QtConcurrent::run(watcher_openProject, project);
    watcher->setFuture(future);

    /*project->openProject();
    ProjectModel *model = new ProjectModel();
    model->addProject(project);*/
}

void ProjectManager::exportProject(QString path)
{
    if(project){
        project->exportProject(path);
    }
}

void ProjectManager::importFile(QString path, QStringList files)
{
    addProject("Tmp", path, files);
}

void ProjectManager::showGraph(const QModelIndex& index)
{    
    QVariant var = projectTree->model()->data(index, Qt::UserRole);
    if( QString(var.typeName()) == "CompleteAnnotationItem*" )
    {
        currentIndex = index;
        CompleteAnnotationItem *annot = var.value<CompleteAnnotationItem*>();
        AnnotationItem* item = annot->getItem();

        mOriginalSceneCreator->draw(item->getOriginalParse());
        //Make a copy of the tree into an editableTree
        item->cloneFromCurrentToEditable();
        mCurrentSceneCreator->draw(item->getEditableParse(), true);


        emit revisionChanged(annot->getRevisionsCount()+1, annot->getRevisionsCount()+1,
                             trUtf8("Current version"));
        emit scenesChanged();

        committed = false;

        emit canEnable();
    }
}

void ProjectManager::changeRevision(int n)
{
    QVariant var = projectTree->model()->data(currentIndex, Qt::UserRole);
    CompleteAnnotationItem *annotation = var.value<CompleteAnnotationItem*>();
    QString status = "";
    if(n == annotation->getRevisionsCount()+1) //Show current version
    {
        autocommit = false;
        showGraph(currentIndex);
        autocommit = true;
        status = trUtf8("Current version");
    }
    else
    {
        Revision<AnnotationItem*> revision = annotation->getRevision(n-1);
        mCurrentSceneCreator->draw(revision.getValue()->getCurrentParse());
        status = trUtf8("Revision ") + revision.getTimestamp().toString("dd/MM/yyyy hh:mm:ss");
        emit revisionChanged(annotation->getRevisionsCount()+1, n, status);
    }   
}

void ProjectManager::rollbackToRevision(int n)
{
    ProjectModel *projectModel = static_cast<ProjectModel*>(projectTree->model());
    projectModel->rollback(currentIndex, n-1);
    committed = true;
    showGraph(currentIndex);
}

void ProjectManager::save(QString path)
{
    if(path.isEmpty())
    {
        watcher = new QFutureWatcher<void>();
        connect(watcher, SIGNAL(finished()), this, SLOT(saveCompleted()));
        QFuture<void> future = QtConcurrent::run(saveProject,
                                                 static_cast<ProjectModel*>(projectTree->model()), project);
        watcher->setFuture(future);
    }
    else
        QMessageBox::critical(0, trUtf8("Not implemented"), trUtf8("Sorry, not implemented yet"));
}

void ProjectManager::saveCompleted()
{
    emit saved();
}

void ProjectManager::openCompleted()
{
    ProjectModel *model = new ProjectModel();
    model->addProject(project);
    projectTree->setModel(model);
    emit opened(true);
}

void ProjectManager::commit()
{
    if(!committed)
    {
        ProjectModel *projectModel = static_cast<ProjectModel*>(projectTree->model());        
        projectModel->commit(currentIndex);
        showGraph(currentIndex);
    }
    committed = true;
}


void ProjectManager::generateWikiGraph()
{
    QVariant var = projectTree->model()->data(currentIndex, Qt::UserRole);
    CompleteAnnotationItem *annotation = var.value<CompleteAnnotationItem*>();

    wikiDlg = new WikiGraphDialog(annotation->getItem()->getEditableParse());
    wikiDlg->generate();
    wikiDlg->exec();
    delete wikiDlg;
}
