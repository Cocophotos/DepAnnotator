#ifndef PROJECTWINDOW_H
#define PROJECTWINDOW_H

#include <QDialog>
#include <QLabel>
#include <QPushButton>
#include <QLineEdit>

class ProjectWindow : public QDialog
{
    Q_OBJECT
public:
    explicit ProjectWindow(QWidget *parent = 0);

    QString projectName() const {return projectNameLE->text();}
    QString projectPath() const {return savePathLE->text();}
    QString projectFile() const {return importLE->text();}
        
public slots:
    void getSavePath();
    void getImportFile();
    void testAccept();
    
private:
    QLineEdit *projectNameLE,
              *savePathLE,
              *importLE;
    QPushButton *applyBtn,
                *cancelBtn,
                *browseForSave,
                *browseForImport;
};

#endif // PROJECTWINDOW_H
