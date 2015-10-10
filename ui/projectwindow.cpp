#include "projectwindow.h"

#if QT_VERSION >= 0x050000
    #include <QtWidgets>
    #include <QtGui>
#else
    #include <QtGui>
#endif

ProjectWindow::ProjectWindow(QWidget *parent) :
    QDialog(parent)
{
    this->setMinimumSize(400, 300);
    /**
     * Form defintion
     */
    projectNameLE = new QLineEdit(this);
    //projectNameLE->setPlaceholderText(trUtf8("Enter the project name..."));

    savePathLE = new QLineEdit(this);
    //savePathLE->setPlaceholderText(trUtf8("Path where to save the project..."));

    browseForSave = new QPushButton("...", this);
    connect(browseForSave, SIGNAL(clicked()), this, SLOT(getSavePath()));
    browseForImport = new QPushButton("...", this);
    connect(browseForImport, SIGNAL(clicked()), this, SLOT(getImportFile()));

    QHBoxLayout *saveLayout = new QHBoxLayout();
    QHBoxLayout *importLayout = new QHBoxLayout();
    saveLayout->addWidget(savePathLE);
    saveLayout->addWidget(browseForSave);

    importLE = new QLineEdit(this);
    //importLE->setPlaceholderText(trUtf8("File to import..."));
    importLayout->addWidget(importLE);
    importLayout->addWidget(browseForImport);

    QFormLayout *form = new QFormLayout();
    form->addRow(trUtf8("Project's name"), projectNameLE);
    form->addRow(trUtf8("Save path"), saveLayout);
    form->addRow(trUtf8("Files to import"), importLayout);
    /**********/

    /**
     * Buttons box at the bottom
     */
    applyBtn = new QPushButton(trUtf8("Apply"), this);
    connect(applyBtn, SIGNAL(clicked()), this, SLOT(testAccept()));

    cancelBtn = new QPushButton(trUtf8("Cancel"), this);
    connect(cancelBtn, SIGNAL(clicked()), this, SLOT(reject()));

    QHBoxLayout *horizBtnBox = new QHBoxLayout();
    horizBtnBox->addWidget(cancelBtn);
    horizBtnBox->addWidget(applyBtn);
    /**********/

    QVBoxLayout *mainLayout = new QVBoxLayout();
    mainLayout->addLayout(form);
    mainLayout->addLayout(horizBtnBox);
    this->setLayout(mainLayout);
}

void ProjectWindow::getSavePath()
{
#if QT_VERSION >= 0x050000
    QString desktopLocation = QStandardPaths::writableLocation(QStandardPaths::DesktopLocation);
#else
    QString desktopLocation = QDesktopServices::storageLocation(QDesktopServices::DesktopLocation);
#endif
    QString path = QFileDialog::getExistingDirectory(
                this,
                trUtf8("Choose a directory to save the project"),
                desktopLocation
                );

    if(path.isEmpty())
        return;

    savePathLE->setText(path);
}

void ProjectWindow::getImportFile()
{
#if QT_VERSION >= 0x050000
    QString desktopLocation = QStandardPaths::writableLocation(QStandardPaths::DesktopLocation);
#else
    QString desktopLocation = QDesktopServices::storageLocation(QDesktopServices::DesktopLocation);
#endif
    QStringList filepaths = QFileDialog::getOpenFileNames(
                this, trUtf8("Import files"),
                desktopLocation, trUtf8("Corpus format (*.conll);;All files (*.*)")
                );

    if(filepaths.isEmpty())
        return;

    importLE->setText(filepaths.join(";"));
}

void ProjectWindow::testAccept()
{
    if(projectNameLE->text().isEmpty())
    {
        QMessageBox::warning(this, trUtf8("Missing information"),
                             trUtf8("Please provide a name to the project."));
        return;
    }

    if(savePathLE->text().isEmpty())
    {
        QMessageBox::warning(this, trUtf8("Missing information"),
                             trUtf8("Please provide a path to save the project."));
        return;
    }

    if(importLE->text().isEmpty())
    {
        QMessageBox::warning(this, trUtf8("Missing information"),
                             trUtf8("Please provide files to import."));
        return;
    }

    accept();
}
