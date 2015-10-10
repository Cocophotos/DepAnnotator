#include "nodeeditor.h"

#if QT_VERSION >= 0x050000
    #include <QtWidgets>
    #include <QtGui>
#else
    #include <QtGui>
#endif


NodeEditor::NodeEditor(ConllNodeProperties node, QWidget *parent)
    :QDialog(parent), mNode(node)
{
    buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok
                                     | QDialogButtonBox::Cancel, Qt::Horizontal, this);

    connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
    connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));

    tokenEdit = new QLineEdit(this);
    lemmaEdit = new QLineEdit(this);
    cposBox   = new QComboBox(this);
    cposBox->addItems(SchemeReader::schemeReader()->coarsePos());
    posBox    = new QComboBox(this);
    posBox->addItems(SchemeReader::schemeReader()->pos());

    featuresTable = new QTableWidget(this);    
    featuresTable->setAlternatingRowColors(true);

    keysCBox = new QComboBox(this);
    keysCBox->addItems(SchemeReader::schemeReader()->featureKeys());
    valuesCBox = new QComboBox(this);
    addFeatureBtn = new QPushButton(QIcon(":/img/add-icon.png"), "", this);
    addFeatureBtn->setIconSize(QSize(16, 16));
    addFeatureBtn->setFlat(true);
    addFeatureBtn->resize(QSize(20, 20));
    updateFeatureValues(keysCBox->currentText());
    connect(keysCBox, SIGNAL(currentIndexChanged(QString)), this, SLOT(updateFeatureValues(QString)));
    connect(addFeatureBtn, SIGNAL(clicked()), this, SLOT(addFeature()));
    populate();

    QHBoxLayout *addFeatureLayout = new QHBoxLayout();
    addFeatureLayout->addWidget(keysCBox);
    addFeatureLayout->addWidget(new QLabel("="));
    addFeatureLayout->addWidget(valuesCBox);
    addFeatureLayout->addWidget(addFeatureBtn);

    QVBoxLayout *tableLayout = new QVBoxLayout();
    tableLayout->addWidget(featuresTable);
    tableLayout->addLayout(addFeatureLayout);

    QFormLayout *formLayout = new QFormLayout();
    formLayout->addRow("Token:", tokenEdit);
    formLayout->addRow("Lemma:", lemmaEdit);
    formLayout->addRow("Coarse POS:", cposBox);
    formLayout->addRow("POS:", posBox);

    QHBoxLayout *subLayout = new QHBoxLayout();
    subLayout->addLayout(formLayout);
    subLayout->addLayout(tableLayout);

    QVBoxLayout *mainLayout = new QVBoxLayout();
    mainLayout->addLayout(subLayout);
    mainLayout->addWidget(buttonBox);

    setLayout(mainLayout);
}

void NodeEditor::populate()
{
    tokenEdit->setText(mNode.token());
    lemmaEdit->setText(mNode.lemma());
    posBox->setCurrentIndex(posBox->findText(mNode.pos()));
    cposBox->setCurrentIndex(cposBox->findText(mNode.hyperpos()));

    featuresTable->setColumnCount(3);
    featuresTable->setHorizontalHeaderLabels(QStringList() << "." << "Name" << "Value");
    ConllNodeProperties::const_feature_iter fb, fe;
    for(boost::tie(fb, fe) = mNode.featuresIterator(); fb != fe; ++fb){
        featuresTable->insertRow(featuresTable->rowCount());

        int row = featuresTable->rowCount()-1;

        /* Make cells read only */
        QTableWidgetItem *keyItem = new QTableWidgetItem(fb.key());
        QTableWidgetItem *valueItem = new QTableWidgetItem(fb.value());
        keyItem->setFlags(keyItem->flags() ^ Qt::ItemIsEditable);
        valueItem->setFlags(valueItem->flags() ^ Qt::ItemIsEditable);

        CellRemoveButton *removeBtn = new CellRemoveButton(QIcon(":/img/remove-icon.png"), "", keyItem);
        removeBtn->setIconSize(QSize(12, 12));
        removeBtn->setFlat(true);
        connect(removeBtn, SIGNAL(clicked()), this, SLOT(removeFeature()));

        featuresTable->setCellWidget(row, 0, removeBtn);
        featuresTable->setItem(row,1, keyItem);
        featuresTable->setItem(row,2, valueItem);
    }
    featuresTable->resizeColumnToContents(0);
}

void NodeEditor::addFeature()
{
    featuresTable->insertRow(featuresTable->rowCount());
    int row = featuresTable->rowCount()-1;

    /* Make cells read only */
    QTableWidgetItem *keyItem = new QTableWidgetItem(keysCBox->currentText());
    QTableWidgetItem *valueItem = new QTableWidgetItem(valuesCBox->currentText());
    keyItem->setFlags(keyItem->flags() ^ Qt::ItemIsEditable);
    valueItem->setFlags(valueItem->flags() ^ Qt::ItemIsEditable);

    CellRemoveButton *removeBtn = new CellRemoveButton(QIcon(":/img/remove-icon.png"), "", keyItem);
    removeBtn->setIconSize(QSize(12, 12));
    removeBtn->setFlat(true);
    connect(removeBtn, SIGNAL(clicked()), this, SLOT(removeFeature()));

    featuresTable->setCellWidget(row, 0, removeBtn);
    featuresTable->setItem(row,1, keyItem);
    featuresTable->setItem(row,2, valueItem);
}

void NodeEditor::removeFeature()
{
    CellRemoveButton *btn = qobject_cast<CellRemoveButton*>(sender());
    if(btn)
    {
        featuresTable->removeRow(btn->item()->row());
    }
}

void NodeEditor::updateFeatureValues(QString key)
{
    valuesCBox->clear();
    valuesCBox->addItems(SchemeReader::schemeReader()->featureValues(key));
}

void NodeEditor::accept()
{
    mNode.setToken(tokenEdit->text().trimmed());
    mNode.setLemma(lemmaEdit->text().trimmed());
    mNode.setPos(posBox->currentText());
    mNode.setHyperpos(cposBox->currentText());

    mNode.clearAllFeatures();
    for(int i = 0; i < featuresTable->rowCount(); i++)
    {
        QTableWidgetItem * it0 = featuresTable->item(i, 1);
        QTableWidgetItem * it1 = featuresTable->item(i, 2);

        if(it0->text().isEmpty() || it1->text().isEmpty())
            continue;
        else
            mNode.setFeature(it0->text(), it1->text());
    }

    QDialog::accept();
}
