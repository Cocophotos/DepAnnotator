#ifndef NODEEDITOR_H
#define NODEEDITOR_H

#include <QDialog>
#include <QLineEdit>
#include <QComboBox>
#include <QTableWidget>
#include <QDialogButtonBox>
#include <QPushButton>

#include "graph/conllgraph.h"
#include "project/scheme/schemereader.h"

class CellRemoveButton : public QPushButton
{
    Q_OBJECT

public:
    CellRemoveButton(const QIcon &icon, const QString &text, QTableWidgetItem *item, QWidget *parent = 0)
        :QPushButton(icon, text, parent), mItem(item){}

    QTableWidgetItem* item()const {return mItem;}
private:
    QTableWidgetItem* mItem;
};

class NodeEditor : public QDialog
{
    Q_OBJECT

public:
    NodeEditor(ConllNodeProperties node, QWidget *parent = 0);

    ConllNodeProperties node()const {return mNode;}

public slots:
    void accept();

private slots:
    void addFeature();
    void removeFeature();
    void populate();
    void updateFeatureValues(QString key);
private:
    QDialogButtonBox *buttonBox,
                     *buttonTableBox;

    QComboBox *keysCBox, *valuesCBox;
    QPushButton *addFeatureBtn;

    QLineEdit *tokenEdit,
              *lemmaEdit;
    QComboBox *cposBox, *posBox;
    QTableWidget *featuresTable;

    ConllNodeProperties mNode;
};

#endif // NODEEDITOR_H
