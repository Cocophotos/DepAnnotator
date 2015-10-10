#ifndef EDGEEDITOR_H
#define EDGEEDITOR_H

#include <QDialog>
#include <QLabel>
#include <QComboBox>
#include <QListWidget>
#include <QPushButton>
#include <QString>
#include <QStringList>
#include <QDialogButtonBox>


#include "project/scheme/schemereader.h"

class EdgeEditor : public QDialog
{
    Q_OBJECT
public:
    explicit EdgeEditor(QWidget *parent = 0);

    void setDefaultRelations(QString final, QString canonic);

    QString canonicRelation()const{return mCurrentCanonic;}
    QString finalRelation()const{return mCurrentFinal;}

public slots:
    void accept();
    int exec();

private slots:
    void setCanonicalAsFinal();
    void updateBoxFromList(QString newText);
    
private:
    void populateComboBox();
    void updateStacks();
    void viewStacksOnLists();
    

private:
    QLabel *finalLabel,
           *canonicalLabel,
           *recentFinalLabel,
           *recentCanonicalLabel;

    QComboBox *finalCBox,
              *canonicalCBox;

    QListWidget *recentFinalList,
                *recentCanonicalList;

    QPushButton *sameAsFinalBtn;

    QDialogButtonBox *buttonBox;

    SchemeReader *mReader;

    QStringList recentFinals, recentCanonics;

    const short unsigned historicSize;

    QString mCurrentFinal,
            mCurrentCanonic;
};

#endif // EDGEEDITOR_H
