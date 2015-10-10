#include "edgeeditor.h"

#if QT_VERSION >= 0x050000
    #include <QtWidgets>
    #include <QtGui>
#else
    #include <QtGui>
#endif

EdgeEditor::EdgeEditor(QWidget *parent) :
    QDialog(parent), mReader(SchemeReader::schemeReader()), historicSize(5)
{
    QGridLayout *mainLayout = new QGridLayout(this);

    buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok
                                     | QDialogButtonBox::Cancel, Qt::Horizontal, this);

    recentFinalLabel = new QLabel(trUtf8("Recent labels"), this);
    finalLabel  = new QLabel(trUtf8("Final relations"), this);
    finalCBox = new QComboBox(this);
    recentFinalList     = new QListWidget(this);

    mainLayout->addWidget(finalLabel, 0, 0);
    mainLayout->addWidget(finalCBox, 1, 0);
    mainLayout->addWidget(recentFinalLabel, 2, 0);
    mainLayout->addWidget(recentFinalList, 3, 0);

    canonicalLabel  = new QLabel(trUtf8("Canonic relations"), this);
    recentCanonicalLabel = new QLabel(trUtf8("Recent labels"), this);
    recentCanonicalList = new QListWidget(this);
    canonicalCBox = new QComboBox(this);
    sameAsFinalBtn = new QPushButton(trUtf8("Same as final"), this);

    if(mReader->initials().size() > 0){
        mainLayout->addWidget(canonicalLabel, 0, 1);
        mainLayout->addWidget(canonicalCBox, 1, 1);
        mainLayout->addWidget(sameAsFinalBtn, 1, 2);
        mainLayout->addWidget(recentCanonicalLabel, 2, 1);
        mainLayout->addWidget(recentCanonicalList, 3, 1);
        mainLayout->addWidget(buttonBox, 4, 2);
    }else{
        mainLayout->addWidget(buttonBox, 4, 0);
        canonicalLabel->setHidden(true);
        canonicalCBox->setHidden(true);
        sameAsFinalBtn->setHidden(true);
        recentCanonicalLabel->setHidden(true);
        recentCanonicalList->setHidden(true);
    }

    populateComboBox();

    this->setLayout(mainLayout);
    this->resize(200, 150 + historicSize*25);

    connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
    connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
    connect(recentFinalList, SIGNAL(currentTextChanged(QString)), this, SLOT(updateBoxFromList(QString)));

    connect(recentCanonicalList,
            SIGNAL(currentTextChanged(QString)),
            this, SLOT(updateBoxFromList(QString)));
    connect(sameAsFinalBtn, SIGNAL(released()), this, SLOT(setCanonicalAsFinal()));
}

void EdgeEditor::populateComboBox()
{
    QMap<QString, QString> fins = mReader->finals();
    QMap<QString, QString> cano = mReader->initials();

    canonicalCBox->clear();
    finalCBox->clear();

    if(cano.size() > 0)
        canonicalCBox->addItems(cano.keys());
    finalCBox->addItems(fins.keys());
}

void EdgeEditor::setCanonicalAsFinal()
{
    QString fin = finalCBox->currentText();
    int index = -1;

    if((index = canonicalCBox->findText(fin)) != -1)
        canonicalCBox->setCurrentIndex(index);
}

void EdgeEditor::setDefaultRelations(QString final, QString canonic)
{
    mCurrentFinal = final;
    mCurrentCanonic = canonic;

    int indexFinal = -1;
    if((indexFinal = finalCBox->findText(mCurrentFinal)) != -1)
        finalCBox->setCurrentIndex(indexFinal);

    int indexCanonic = -1;
    if((indexCanonic = canonicalCBox->findText(mCurrentCanonic)) != -1)
        canonicalCBox->setCurrentIndex(indexCanonic);

    viewStacksOnLists();
}

void EdgeEditor::updateStacks()
{
    recentFinals.prepend(mCurrentFinal);
    recentCanonics.prepend(mCurrentCanonic);

    recentFinals.removeDuplicates();
    recentCanonics.removeDuplicates();

    if(recentFinals.size() > historicSize)
        recentFinals.removeLast();

    if(recentCanonics.size() > historicSize)
        recentCanonics.removeLast();
}

void EdgeEditor::viewStacksOnLists()
{
    recentFinalList->clear();
    recentFinalList->addItems(recentFinals);

    recentCanonicalList->clear();
    recentCanonicalList->addItems(recentCanonics);

}

void EdgeEditor::updateBoxFromList(QString newText)
{
    if(sender() == recentFinalList)
    {
        int index = -1;
        if((index = finalCBox->findText(newText)) != -1)
            finalCBox->setCurrentIndex(index);
    }
    else
    {
        int index = -1;
        if((index = canonicalCBox->findText(newText)) != -1)
            canonicalCBox->setCurrentIndex(index);
    }
}

void EdgeEditor::accept()
{
    setDefaultRelations(finalCBox->currentText(), mReader->initials().isEmpty() ? finalCBox->currentText() : canonicalCBox->currentText());
    updateStacks();    
    QDialog::accept();
}

int EdgeEditor::exec()
{
    populateComboBox();
    setDefaultRelations(mCurrentFinal, mCurrentCanonic);
    return QDialog::exec();
}
