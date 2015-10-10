#include "mainwindow.h"

#if QT_VERSION >= 0x050000
    #include <QtWidgets>
    #include <QtGui>
#else
    #include <QtGui>
#endif

const QString TA_VERSION = "1.0";

DependenciesAnnotator::DependenciesAnnotator(QWidget *parent)
    :QMainWindow(parent)
{
    /*QString password = "toto";
    QString login    = "corentin";
    QString salted   = login+password;

    QByteArray base64_(salted.toUtf8());
    base64_ = base64_.toBase64();
    base64_ = base64_.toBase64();
    qDebug() << base64_;

    QByteArray decoded_;
    decoded_ = QByteArray::fromBase64(base64_);
    decoded_ = QByteArray::fromBase64(decoded_);

    qDebug() << decoded_;*/

    isSaved = true; //The project is saved
    /**
     * Creation of the main QGraphicsScene
     */
    currentParseView  = new QGraphicsView(this);
    commentBox        = new CommentPlainEdit(this);
    commentBtn        = new QPushButton(trUtf8("Comment"), this);
    commentBtn->setMaximumWidth(100);
    commentBtn->setCheckable(true);
    commentBox->setVisible(false);
    commentBox->setMaximumHeight(125);

    currentParseView->setDragMode(QGraphicsView::ScrollHandDrag);
    currentGroupBox = new QGroupBox("Annotation in progress", this);
    QVBoxLayout *mainGroupBoxLayout = new QVBoxLayout(this);
    mainGroupBoxLayout->setContentsMargins(2,2,2,2);
    mainGroupBoxLayout->addWidget(commentBtn);
    mainGroupBoxLayout->addWidget(commentBox);
    mainGroupBoxLayout->addWidget(currentParseView);
    currentGroupBox->setLayout(mainGroupBoxLayout);

    /**
     * Creation of the panes
     */
    annotationTimerPane  = new QDockWidget(trUtf8("Timer Panel"), this);
    annotationTimer      = new AnnotationTimer;
    annotationTimerPane->setWidget(annotationTimer);

    projectPane          = new QDockWidget(trUtf8("Project Panel"), this);
    projectManagerWidget = new ProjectManagerWidget();
    projectPane->setWidget(projectManagerWidget);

    originalParsePane   = new QDockWidget(trUtf8("Original Parse"), this);
    originalParseWidget = new OriginalParseWidget();
    originalParsePane->setWidget(originalParseWidget);
    originalParsePane->setHidden( !Config::config()->getViewOriginalParse() );

    relsPane = new QDockWidget(trUtf8("Relations panel"), this);
    relationsWidget = new RelationsWidget();
    relsPane->setWidget(relationsWidget);

    /**********/
    this->addDockWidget(Qt::LeftDockWidgetArea, annotationTimerPane);
    this->addDockWidget(Qt::LeftDockWidgetArea, projectPane);
    this->addDockWidget(Qt::RightDockWidgetArea, originalParsePane);

    this->addDockWidget(Qt::LeftDockWidgetArea, relsPane);
    relsPane->setHidden(true);


    /**
     * Revision bar
     */
    revisionsFrame = new QFrame(this);
    revisionsFrame->setVisible( Config::config()->getViewRevisions() );
    revisionsFrame->setEnabled(false);


    revisionsStatus = new QLabel("", revisionsFrame);
    revisionRollback = new QPushButton("Rollback", revisionsFrame);
    revisionsSpinBox = new QSpinBox(this);

    QHBoxLayout *mainLayoutFrame = new QHBoxLayout(revisionsFrame);

    mainLayoutFrame->addWidget(revisionsStatus);
    mainLayoutFrame->addWidget(revisionsSpinBox);
    mainLayoutFrame->addWidget(revisionRollback);
    revisionsFrame->setLayout(mainLayoutFrame);
    mainLayoutFrame->setContentsMargins(2,2,2,2);
    mainLayoutFrame->setSpacing(4);

    /**
     * Creation of the undo stack
     */

    undoStack = new QUndoStack(this);

    /**
     * Creation of scene creators
     */
    currentSceneCreator  = new SceneCreator(undoStack, commentBox, this);
    originalSceneCreator = new SceneCreator(undoStack, commentBox, this);

    projectManager = new ProjectManager(projectManagerWidget->getProjectTree(),
                                        currentSceneCreator, originalSceneCreator);

    connect(projectManager, SIGNAL(canEnable()), this, SLOT(enableActions()));

    connect(this->projectManagerWidget->getProjectTree(), SIGNAL(doubleClicked(QModelIndex)),
            this, SLOT(showGraph(QModelIndex)));


    /**
     * Creation of the menus
     */
    fileMenu    = new QMenu(trUtf8("&File"), this->menuBar());
    editionMenu = new QMenu(trUtf8("&Edit"), this->menuBar());
    viewMenu    = new QMenu(trUtf8("&View"), this->menuBar());
    helpMenu    = new QMenu(trUtf8("&Help"), this->menuBar());

    this->menuBar()->addMenu(fileMenu);
    this->menuBar()->addMenu(editionMenu);
    this->menuBar()->addMenu(viewMenu);
    this->menuBar()->addMenu(helpMenu);

    createFileActions();
    createEditionActions();
    createViewActions();
    createHelpActions();

    connect(viewRevisions, SIGNAL(toggled(bool)), revisionsFrame, SLOT(setVisible(bool)));

    this->setWindowTitle("DepAnnotator v."+TA_VERSION);
    this->setWindowIcon(QIcon(":/img/app-icon.png"));
    this->resize(800, 600);

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addWidget(revisionsFrame);
    mainLayout->addWidget(currentGroupBox);
    QWidget *centralWidget = new QWidget(this);
    centralWidget->setLayout(mainLayout);
    this->setCentralWidget(centralWidget);

    /**
     *Creation of the project manager
     */

    connect(projectManager, SIGNAL(scenesChanged()), this, SLOT(redrawScenes()));
    connect(revisionsSpinBox, SIGNAL(valueChanged(int)), projectManager, SLOT(changeRevision(int)));
    connect(revisionRollback, SIGNAL(clicked()), this, SLOT(rollback()));
    connect(projectManager, SIGNAL(revisionChanged(int,int,QString)), this, SLOT(updateRevision(int,int,QString)));
    connect(projectManager, SIGNAL(saved()), this, SLOT(enableSave()));
    connect(projectManager, SIGNAL(opened(bool)), this, SLOT(finishOpen(bool)));

    connect(relationsWidget->tree(), SIGNAL(itemDoubleClicked(QTreeWidgetItem*,int)),
            currentSceneCreator, SLOT(changeRelation(QTreeWidgetItem*,int)));

    connect(commentBtn, SIGNAL(clicked(bool)), commentBox, SLOT(setVisible(bool)));
    connect(commentBox, SIGNAL(loseFocus(QString,QString)),
            currentSceneCreator, SLOT(addComment(QString,QString)));

    //Load annotation scheme if file exists
    QString lastAnnotationScheme = Config::config()->getLastAnnotationScheme();
    if(!lastAnnotationScheme.isEmpty()){
        QFileInfo fileInfo(lastAnnotationScheme);
        if(fileInfo.exists())
            relationsWidget->populate(lastAnnotationScheme);
    }
}

void DependenciesAnnotator::closeEvent(QCloseEvent *event){
    if(canExecute())
    {
        Config::config()->setViewRevisions(viewRevisions->isChecked());
        Config::config()->setViewOriginalParse(originalParsePane->isVisible());
        event->accept();
    }else
        event->ignore();
}

void DependenciesAnnotator::onAppFocusChanged()
{
    /*if( !QApplication::activeWindow() )
        annotationTimer->pause();*/
}

/*********************************************
 * FILE related SLOTS
 *********************************************/

bool DependenciesAnnotator::canExecute()
{
    if(!undoStack->isClean() || !isSaved){
         QMessageBox::StandardButton reply =
                 QMessageBox::question(this, trUtf8("Unsaved changes"),
                                       trUtf8("You didn't save your current work.\nAre you sure you want to do that?"),
                                       QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);

         if(reply == QMessageBox::Save)
         {
             saveProject();
             return true;
         }else if(reply == QMessageBox::Discard)
             return true;
         else
             return false;
    }
    return true;
}

void DependenciesAnnotator::newProject()
{
    if(!canExecute())
        return;

    ProjectWindow projectDlg;
    if(projectDlg.exec()){
        QString name = projectDlg.projectName();
        QString path = projectDlg.projectPath();
        QStringList files = projectDlg.projectFile().split(";", QString::SkipEmptyParts);

        Config::config()->setDefaultDirectory(path);

        waitForOpening = new QProgressDialog(trUtf8("Import in progress... Please wait."), trUtf8("Cancel"), 0, 0, this);
        waitForOpening->setWindowFlags(Qt::Window | Qt::CustomizeWindowHint |
                        Qt::WindowTitleHint|
                        Qt::WindowSystemMenuHint |
                        Qt::WindowMaximizeButtonHint | Qt::WindowMinimizeButtonHint);
        QPushButton *cancelBtn = new QPushButton(trUtf8("Cancel"));
        cancelBtn->setEnabled(false);
        waitForOpening->setCancelButton(cancelBtn);
        waitForOpening->setWindowModality(Qt::WindowModal);

        projectManager->addProject(name, path, files);
        waitForOpening->exec();
    }
}

void DependenciesAnnotator::openProject()
{

    if(!canExecute())
        return;

    QString location = Config::config()->getDefaultDirectory();

    if(location.isEmpty()){
#if QT_VERSION >= 0x050000
        location = QStandardPaths::writableLocation(QStandardPaths::DesktopLocation);
#else
        location = QDesktopServices::storageLocation(QDesktopServices::DesktopLocation);
#endif
    }

    QString filepath = QFileDialog::getOpenFileName(this, trUtf8("Open a project"), location,
                                                    "DependenciesAnnotator Project (*.dap)");

    if(filepath.isEmpty())
        return;

    Config::config()->setDefaultDirectory(QFileInfo(filepath).absolutePath());

    waitForOpening = new QProgressDialog(trUtf8("Open in progress... Please wait."), trUtf8("Cancel"), 0, 0, this);
    waitForOpening->setWindowFlags(Qt::Window | Qt::CustomizeWindowHint |
                    Qt::WindowTitleHint|
                    Qt::WindowSystemMenuHint |
                    Qt::WindowMaximizeButtonHint | Qt::WindowMinimizeButtonHint);
    QPushButton *cancelBtn = new QPushButton(trUtf8("Cancel"));
    cancelBtn->setEnabled(false);
    waitForOpening->setCancelButton(cancelBtn);
    waitForOpening->setWindowModality(Qt::WindowModal);

    projectManager->openProject(filepath);
    waitForOpening->exec();


    /*projectManager->openProject(filepath);
    projectPane->setWindowTitle("Project Panel | "+QString::number(projectManager->getProject()->total())+" annotation units");*/
}

void DependenciesAnnotator::importFile()
{
    if(!canExecute())
        return;

    QString location = Config::config()->getDefaultDirectory();

    qDebug() << location;

    if(location.isEmpty()){
#if QT_VERSION >= 0x050000
        location = QStandardPaths::writableLocation(QStandardPaths::DesktopLocation);
#else
        location = QDesktopServices::storageLocation(QDesktopServices::DesktopLocation);
#endif
    }

    QStringList filepaths = QFileDialog::getOpenFileNames(
                this, trUtf8("Import files"),
                location, trUtf8("Corpus format (*.conll);;All files (*.*)")
                );

    if(filepaths.isEmpty())
        return;

    Config::config()->setDefaultDirectory(QFileInfo(filepaths[0]).absolutePath());

    waitForOpening = new QProgressDialog(trUtf8("Import in progress... Please wait."), trUtf8("Cancel"), 0, 0, this);
    waitForOpening->setWindowFlags(Qt::Window | Qt::CustomizeWindowHint |
                    Qt::WindowTitleHint|
                    Qt::WindowSystemMenuHint |
                    Qt::WindowMaximizeButtonHint | Qt::WindowMinimizeButtonHint);
    QPushButton *cancelBtn = new QPushButton(trUtf8("Cancel"));
    cancelBtn->setEnabled(false);
    waitForOpening->setCancelButton(cancelBtn);
    waitForOpening->setWindowModality(Qt::WindowModal);

    projectManager->importFile(QFileInfo(filepaths[0]).absolutePath(), filepaths);
    waitForOpening->exec();
}

void DependenciesAnnotator::saveProject()
{
    waitForSaving = new QProgressDialog(trUtf8("Save in progress... Please wait."), trUtf8("Cancel"), 0, 0, this);
    waitForSaving->setWindowFlags(Qt::Window | Qt::CustomizeWindowHint |
                    Qt::WindowTitleHint|
                    Qt::WindowSystemMenuHint |
                    Qt::WindowMaximizeButtonHint | Qt::WindowMinimizeButtonHint);
    QPushButton *cancelBtn = new QPushButton(trUtf8("Cancel"));
    cancelBtn->setEnabled(false);
    waitForSaving->setCancelButton(cancelBtn);
    waitForSaving->setWindowModality(Qt::WindowModal);

    commit();
    saveProjectAction->setEnabled(false);
    commitAction->setEnabled(false);
    projectManager->save();
    waitForSaving->exec();
}

void DependenciesAnnotator::enableSave()
{
    saveProjectAction->setEnabled(true);
    commitAction->setEnabled(true);
    waitForSaving->close();
    isSaved = true;
    setWindowModified(!isSaved);
    statusBar()->showMessage(trUtf8("Save completed!"), 5000);
}

void DependenciesAnnotator::finishOpen(bool success)
{
    Q_UNUSED(success);
    waitForOpening->close();
    setWindowTitle(tr("%1[*] - %2").arg(projectManager->getProject()->projectName()).arg(trUtf8("DependenciesAnnotator v")+TA_VERSION));
    statusBar()->showMessage(trUtf8("Open completed!"), 5000);
    projectPane->setWindowTitle("Project Panel | "+QString::number(projectManager->getProject()->total())+" annotation units");

    currentSceneCreator->scene()->clear();
    originalSceneCreator->scene()->clear();
}

void DependenciesAnnotator::saveProjectAs()
{
    QString location = Config::config()->getDefaultDirectory();

    if(location.isEmpty()){
#if QT_VERSION >= 0x050000
        location = QStandardPaths::writableLocation(QStandardPaths::DesktopLocation);
#else
        location = QDesktopServices::storageLocation(QDesktopServices::DesktopLocation);
#endif
    }
    QString path = QFileDialog::getExistingDirectory(
                this, trUtf8("Choose a directory for the project"), location);

    if(path.isEmpty())
        return;

    Config::config()->setDefaultDirectory(path);
    projectManager->save(path);
}

void DependenciesAnnotator::exportProject()
{
    QString location = Config::config()->getDefaultDirectory();

    if(location.isEmpty()){
#if QT_VERSION >= 0x050000
        location = QStandardPaths::writableLocation(QStandardPaths::DesktopLocation);
#else
        location = QDesktopServices::storageLocation(QDesktopServices::DesktopLocation);
#endif
    }

    QString path = QFileDialog::getExistingDirectory(
                this, trUtf8("Choose a directory to export the project"), location);

    if(path.isEmpty())
        return;

    commit();
    projectManager->exportProject(path);
    QMessageBox::information(this, trUtf8("Export completed."), trUtf8("Export completed."));
}

void DependenciesAnnotator::exportInto()
{
    QString location = Config::config()->getDefaultDirectory();

    if(location.isEmpty()){
#if QT_VERSION >= 0x050000
        location = QStandardPaths::writableLocation(QStandardPaths::DesktopLocation);
#else
        location = QDesktopServices::storageLocation(QDesktopServices::DesktopLocation);
#endif
    }

    QString filepath = QFileDialog::getSaveFileName(
                this, trUtf8("Export current graph into..."),
                location, trUtf8("PDF (*.pdf);;SVG (*.svg);;PNG (*.png)")
                );

    if(filepath.isEmpty())
        return;

    Exporter *exporter = 0;
    if(QFileInfo(filepath).completeSuffix().toLower() == "svg")
        exporter = new SVGExporter();
    else if(QFileInfo(filepath).completeSuffix().toLower() == "pdf")
        exporter = new PDFExporter();
    else if(QFileInfo(filepath).completeSuffix().toLower() == "png")
        exporter = new PNGExporter();

    if(exporter)
        exporter->exporting(currentParseView, filepath);
}

void DependenciesAnnotator::loadAnnotationScheme()
{
    QString location = Config::config()->getDefaultDirectory();

    if(location.isEmpty()){
#if QT_VERSION >= 0x050000
        location = QStandardPaths::writableLocation(QStandardPaths::DesktopLocation);
#else
        location = QDesktopServices::storageLocation(QDesktopServices::DesktopLocation);
#endif
    }

    QString filepath = QFileDialog::getOpenFileName(
                this, trUtf8("Choose a directory for the project"), location, "DGA XML (*.xml)");

    if(filepath.isEmpty())
        return;

    Config::config()->setLastAnnotationScheme(filepath);
    relationsWidget->populate(filepath);
}

/**************************************
 * Edit SLOTS
 **************************************/

void DependenciesAnnotator::commit()
{
    if(undoStack->isClean()){
        statusBar()->showMessage(trUtf8("Commit completed!"), 5000);
        return;
    }

    projectManager->commit();
    statusBar()->showMessage(trUtf8("Commit completed!"), 5000);
    undoStack->clear();
    isSaved = false;
    setWindowModified(!isSaved);
}

void DependenciesAnnotator::rollback()
{
    projectManager->rollbackToRevision(revisionsSpinBox->value());
    statusBar()->showMessage(trUtf8("Rollback completed!"), 5000);
}

void DependenciesAnnotator::changeSentence()
{
    QModelIndex index = projectManagerWidget->getProjectTree()->currentIndex();
    if(sender() == prevSentenceAction){
        index = projectManagerWidget->getProjectTree()->model()->index(index.row()-1, index.column(), index.parent());
    }else{
        index = projectManagerWidget->getProjectTree()->model()->index(index.row()+1, index.column(), index.parent());
    }
    commit();
    showGraph(index);
    projectManagerWidget->getProjectTree()->setCurrentIndex(index);
}


/**************************************
 * Drawing stuffs
 **************************************/
void DependenciesAnnotator::showGraph(const QModelIndex &index)
{
    commit();
    projectManager->showGraph(index);
}

void DependenciesAnnotator::updateRevision(int count, int i, QString status)
{
    revisionsFrame->setEnabled(true);
    revisionsSpinBox->setMaximum(count);
    revisionsSpinBox->setMinimum(1);
    revisionsStatus->setText(status);
    revisionsSpinBox->setValue(i);

    revisionRollback->setEnabled(i != count);
}

void DependenciesAnnotator::changeRevision()
{
    projectManager->changeRevision(revisionsSpinBox->value());
}

void DependenciesAnnotator::redrawScenes()
{
    currentParseView->setScene(currentSceneCreator->scene());
    originalParseWidget->view()->setScene( originalSceneCreator->scene() );
}

void DependenciesAnnotator::zoomIn()
{
    currentParseView->scale(1.2, 1.2);
}

void DependenciesAnnotator::zoomOut()
{
    currentParseView->scale(0.8, 0.8);
}

void DependenciesAnnotator::fitToView()
{
    currentParseView->fitInView(currentSceneCreator->scene()->sceneRect(), Qt::KeepAspectRatio);
}

void DependenciesAnnotator::viewLogConsole()
{
    LogConsoleWidget *w = new LogConsoleWidget();
    w->exec();
}

void DependenciesAnnotator::enableActions()
{
    QList<QAction*> actions;
    actions << invertEdgeAction << addEdgeAction << removeEdgeAction
            << bothEdgeAction << deepEdgeAction << surfacicEdgeAction << noneEdgeAction
            << coindexNodesAction << generateDepLoriaAction << commitAction << addNodeAction;

    foreach(QAction *a, actions){
        a->setEnabled(true);
    }
}

/**************************************
 * Helping stuffs
 **************************************/
void DependenciesAnnotator::about()
{
    QString about = trUtf8("DependenciesAnnotator has been developed by Corentin Ribeyre.\n"
                           "The software is under LGPL license and delivered as is, "
                           "without any guarantee or warranty.\n\n"
                           "www.cocophotos.eu for more information about the software's developer.\n\n"
                           "Thanks to Djamé Seddah for his suggestions to improve the software.\n\n"
                           "Compilation date: ")+QString::fromStdString(__TIMESTAMP__);
    QMessageBox::about(this, trUtf8("About DependenciesAnnotator"), about);
}

void DependenciesAnnotator::showOptions()
{
    PreferencesDlg dlg;
    dlg.exec();
}


/**************************************
 * Menu stuffs
 **************************************/


void DependenciesAnnotator::createFileActions()
{
    quitAction = new QAction(trUtf8("Quit"), fileMenu);
    quitAction->setMenuRole(QAction::QuitRole);
    quitAction->setShortcut(QKeySequence::Quit);
    connect(quitAction, SIGNAL(triggered()), this, SLOT(close()));

    newProjectAction = new QAction(trUtf8("New"), fileMenu);
    newProjectAction->setShortcut(QKeySequence::New);
    newProjectAction->setIcon(QIcon(":/img/add-project-icon.png"));
    connect(newProjectAction, SIGNAL(triggered()), this, SLOT(newProject()));

    openProjectAction = new QAction(trUtf8("Open..."), fileMenu);
    openProjectAction->setShortcut(QKeySequence::Open);
    openProjectAction->setIcon(QIcon(":/img/open-icon.png"));
    connect(openProjectAction, SIGNAL(triggered()), this, SLOT(openProject()));

    importAction = new QAction(trUtf8("Import..."), fileMenu);
    importAction->setShortcut(QKeySequence("Ctrl+I"));
    connect(importAction, SIGNAL(triggered()), this, SLOT(importFile()));

    saveProjectAction = new QAction(trUtf8("Save project"), fileMenu);
    saveProjectAction->setShortcut(QKeySequence::Save);
    saveProjectAction->setIcon(QIcon(":/img/save-icon.png"));
    connect(saveProjectAction, SIGNAL(triggered()), this, SLOT(saveProject()));

    saveProjectAsAction = new QAction(trUtf8("Save project as..."), fileMenu);
    saveProjectAsAction->setShortcut(QKeySequence::SaveAs);
    connect(saveProjectAsAction, SIGNAL(triggered()), this, SLOT(saveProjectAs()));

    saveConllAction = new QAction(trUtf8("Save into .conll"), fileMenu);
    saveConllAction->setShortcut(QKeySequence("Ctrl+E"));
    connect(saveConllAction, SIGNAL(triggered()), this, SLOT(exportProject()));

    exportAction = new QAction(trUtf8("Export into..."), fileMenu);
    exportAction->setShortcut(QKeySequence("Ctrl+Shift+E"));
    connect(exportAction, SIGNAL(triggered()), this, SLOT(exportInto()));

    loadAnnotationSchemeAction = new QAction(trUtf8("Load annotation scheme"), fileMenu);
    loadAnnotationSchemeAction->setShortcut(QKeySequence("Ctrl+Shift+A"));
    connect(loadAnnotationSchemeAction, SIGNAL(triggered()), this, SLOT(loadAnnotationScheme()));

    QList<QAction*> actions;
    actions << newProjectAction << openProjectAction
            << saveProjectAction;

    fileMenu->addActions(actions);
    fileMenu->addSeparator();
    fileMenu->addAction(importAction);
    fileMenu->addAction(saveConllAction);
    fileMenu->addAction(exportAction);
    fileMenu->addSeparator();
    fileMenu->addAction(loadAnnotationSchemeAction);
    fileMenu->addAction(quitAction);

    QToolBar *fileToolbar = addToolBar(trUtf8("File"));
    fileToolbar->addAction(newProjectAction);
    fileToolbar->addAction(openProjectAction);
    fileToolbar->addAction(saveProjectAction);
}

void DependenciesAnnotator::createEditionActions()
{

    undoAction = undoStack->createUndoAction(this, trUtf8("&Undo"));
    undoAction->setShortcut(QKeySequence::Undo);
    undoAction->setIcon(QIcon(":/img/undo-icon.png"));

    redoAction = undoStack->createRedoAction(this, trUtf8("&Redo"));
    redoAction->setShortcut(QKeySequence::Redo);
    redoAction->setIcon(QIcon(":/img/redo-icon.png"));

    prevSentenceAction = new QAction(trUtf8("Previous sentence"), editionMenu);
    prevSentenceAction->setShortcut(QKeySequence("Alt+Shift+Tab"));
    connect(prevSentenceAction, SIGNAL(triggered()), this, SLOT(changeSentence()));

    nextSentenceAction = new QAction(trUtf8("Next sentence"), editionMenu);
    nextSentenceAction->setShortcut(QKeySequence("Alt+Tab"));
    connect(nextSentenceAction, SIGNAL(triggered()), this, SLOT(changeSentence()));

    prefAction = new QAction(trUtf8("Options"), editionMenu);
    prefAction->setShortcut(QKeySequence::Preferences);
    prefAction->setMenuRole(QAction::PreferencesRole);
    connect(prefAction, SIGNAL(triggered()), this, SLOT(showOptions()));

    commitAction = new QAction(trUtf8("Commit"), editionMenu);
    commitAction->setShortcut(QKeySequence("Alt+S"));
    connect(commitAction, SIGNAL(triggered()), this, SLOT(commit()));

    addEdgeAction = new QAction(trUtf8("Add an edge"), editionMenu);
    addEdgeAction->setShortcut(QKeySequence("A"));
    addEdgeAction->setIcon(QIcon(":/img/add-icon.png"));
    connect(addEdgeAction, SIGNAL(triggered()), currentSceneCreator, SLOT(addEdge()));

    removeEdgeAction = new QAction(trUtf8("Remove edge(s)"), editionMenu);
    removeEdgeAction->setShortcut(QKeySequence("Backspace"));
    removeEdgeAction->setIcon(QIcon(":/img/remove-icon.png"));
    connect(removeEdgeAction, SIGNAL(triggered()), currentSceneCreator, SLOT(removeEdge()));

    addNodeAction = new QAction(QIcon(":/img/epsilon-icon.png"), trUtf8("Add ε node"), editionMenu);
    addNodeAction->setShortcut(QKeySequence("E"));
    connect(addNodeAction, SIGNAL(triggered()), currentSceneCreator, SLOT(addUnrealisedNode()));

    coindexNodesAction = new QAction(QIcon(":/img/coindex-icon.png"), trUtf8("Coindex nodes"), editionMenu);
    coindexNodesAction->setShortcut(QKeySequence("C"));
    connect(coindexNodesAction, SIGNAL(triggered()), currentSceneCreator, SLOT(coindexNodes()));

    QPixmap pix(24, 24);
    pix.fill(Qt::red);
    surfacicEdgeAction = new QAction(pix, trUtf8("Surfacic edge(s)"), editionMenu);
    surfacicEdgeAction->setShortcut(QKeySequence("S"));
    connect(surfacicEdgeAction, SIGNAL(triggered()), currentSceneCreator, SLOT(setSurfacicEdge()));

    pix.fill(Qt::blue);
    deepEdgeAction = new QAction(pix, trUtf8("Deep edge(s)"), editionMenu);
    deepEdgeAction->setShortcut(QKeySequence("D"));
    connect(deepEdgeAction, SIGNAL(triggered()), currentSceneCreator, SLOT(setDeepEdge()));

    pix.fill(Qt::black);
    bothEdgeAction = new QAction(pix, trUtf8("Surfacic and deep edge(s)"), editionMenu);
    bothEdgeAction->setShortcut(QKeySequence("B"));
    connect(bothEdgeAction, SIGNAL(triggered()), currentSceneCreator, SLOT(setBothEdge()));

    pix.fill(Qt::gray);
    noneEdgeAction = new QAction(pix, trUtf8("Intermediary edge(s)"), editionMenu);
    noneEdgeAction->setShortcut(QKeySequence("N"));
    connect(noneEdgeAction, SIGNAL(triggered()), currentSceneCreator, SLOT(setNoneEdge()));

    invertEdgeAction = new QAction(QIcon(":/img/invert-icon.png"), trUtf8("Invert edge(s)"), editionMenu);
    invertEdgeAction->setShortcut(QKeySequence("I"));
    connect(invertEdgeAction, SIGNAL(triggered()), currentSceneCreator, SLOT(invertEdge()));

    generateDepLoriaAction = new QAction(trUtf8("Generate wiki graph"), editionMenu);
    generateDepLoriaAction->setShortcut(QKeySequence("Ctrl+Alt+W"));
    connect(generateDepLoriaAction, SIGNAL(triggered()), projectManager, SLOT(generateWikiGraph()));

    editionMenu->addAction(undoAction);
    editionMenu->addAction(redoAction);
    editionMenu->addSeparator();

    editionMenu->addAction(addEdgeAction);
    editionMenu->addAction(removeEdgeAction);
    editionMenu->addAction(invertEdgeAction);
    editionMenu->addAction(surfacicEdgeAction);
    editionMenu->addAction(deepEdgeAction);
    editionMenu->addAction(bothEdgeAction);
    editionMenu->addAction(noneEdgeAction);
    editionMenu->addAction(addNodeAction);
    editionMenu->addAction(coindexNodesAction);
    editionMenu->addSeparator();

    editionMenu->addAction(prevSentenceAction);
    editionMenu->addAction(nextSentenceAction);
    editionMenu->addSeparator();

    editionMenu->addAction(commitAction);
    editionMenu->addAction(generateDepLoriaAction);
    editionMenu->addSeparator();

    editionMenu->addAction(prefAction);

    QToolBar *editionToolbar = addToolBar(trUtf8("Edit"));
    editionToolbar->setIconSize(QSize(24, 24));
    editionToolbar->addAction(undoAction);
    editionToolbar->addAction(redoAction);
    editionToolbar->addSeparator();
    editionToolbar->addAction(addEdgeAction);
    editionToolbar->addAction(invertEdgeAction);
    editionToolbar->addAction(removeEdgeAction);
    editionToolbar->addAction(addNodeAction);
    editionToolbar->addAction(coindexNodesAction);
    editionToolbar->addSeparator();
    editionToolbar->addAction(bothEdgeAction);
    editionToolbar->addAction(surfacicEdgeAction);
    editionToolbar->addAction(deepEdgeAction);
    editionToolbar->addAction(noneEdgeAction);

    QList<QAction*> actions;
    actions << addEdgeAction << removeEdgeAction
            << invertEdgeAction << addNodeAction
            << coindexNodesAction << bothEdgeAction
            << deepEdgeAction << surfacicEdgeAction
            << noneEdgeAction;

    currentSceneCreator->scene()->addToContextMenu(QList<QAction*>() << undoAction << redoAction);
    currentSceneCreator->scene()->addSepToContextMenu();
    //Create the context menu for the scene.
    currentSceneCreator->scene()->addToContextMenu(actions);

    actions << generateDepLoriaAction << commitAction;

    foreach(QAction *a, actions)
        a->setEnabled(false);
}

void DependenciesAnnotator::createViewActions()
{
    viewRevisions = new QAction(trUtf8("View revisions"), viewMenu);
    viewRevisions->setShortcut(QKeySequence("Ctrl+R"));
    viewRevisions->setIcon(QIcon(":/img/view-icon.png"));
    viewRevisions->setCheckable(true);
    viewRevisions->setChecked(revisionsFrame->isVisible());

    zoomInAction = new QAction(trUtf8("Zoom in"), viewMenu);
    zoomInAction->setShortcut(QKeySequence::ZoomIn);
    zoomInAction->setIcon(QIcon(":/img/zoom-in-icon.png"));
    connect(zoomInAction, SIGNAL(triggered()), this, SLOT(zoomIn()));

    zoomOutAction = new QAction(trUtf8("Zoom out"), viewMenu);
    zoomOutAction->setShortcut(QKeySequence::ZoomOut);
    zoomOutAction->setIcon(QIcon(":/img/zoom-out-icon.png"));
    connect(zoomOutAction, SIGNAL(triggered()), this, SLOT(zoomOut()));

    fitToViewAction = new QAction(trUtf8("Fit to view"), viewMenu);
    fitToViewAction->setShortcut(QKeySequence("Ctrl+Shift+F"));
    fitToViewAction->setIcon(QIcon(":/img/fitview-icon.png"));
    connect(fitToViewAction, SIGNAL(triggered()), this, SLOT(fitToView()));

    viewLogConsoleAction = new QAction(trUtf8("Log Console"), viewMenu);
    connect(viewLogConsoleAction, SIGNAL(triggered()), this, SLOT(viewLogConsole()));

    QList<QAction*> actions;
    actions << projectPane->toggleViewAction()
            << originalParsePane->toggleViewAction()
            << relsPane->toggleViewAction()
            << annotationTimerPane->toggleViewAction()
            << viewRevisions;

    viewMenu->addActions(actions);
    viewMenu->addSeparator();
    viewMenu->addAction(zoomInAction);
    viewMenu->addAction(zoomOutAction);
    viewMenu->addAction(fitToViewAction);
    viewMenu->addSeparator();
    viewMenu->addAction(viewLogConsoleAction);

    QToolBar *viewToolbar= addToolBar(trUtf8("View"));
    viewToolbar->setIconSize(QSize(24,24));
    viewToolbar->addAction(viewRevisions);
    viewToolbar->addAction(zoomInAction);
    viewToolbar->addAction(zoomOutAction);
    viewToolbar->addAction(fitToViewAction);
}

void DependenciesAnnotator::createHelpActions()
{
    aboutAction = new QAction(trUtf8("About"), helpMenu);
    aboutAction->setMenuRole(QAction::AboutRole);
    connect(aboutAction, SIGNAL(triggered()), this, SLOT(about()));

    helpMenu->addAction(aboutAction);
}
