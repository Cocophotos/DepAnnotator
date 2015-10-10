#include "preferencesdlg.h"

#if QT_VERSION >= 0x050000
    #include <QtWidgets>
    #include <QtGui>
#else
    #include <QtGui>
#endif

//----------------------------------------
//              General Widget
//----------------------------------------

GeneralPreferencesWidget::GeneralPreferencesWidget(QWidget *parent)
    :QWidget(parent)
{
    loginLE = new QLineEdit(this);
    passwordLE = new QLineEdit(this);
    passwordLE->setEchoMode(QLineEdit::Password);

    wikiConnectionInfoGBox = new QGroupBox("Wiki Connection Informations", this);

    QFormLayout *wikiConnectionLayout = new QFormLayout(wikiConnectionInfoGBox);
    wikiConnectionLayout->addRow("Login", loginLE);
    wikiConnectionLayout->addRow("Password", passwordLE);

    QVBoxLayout *main = new QVBoxLayout(this);
    main->addWidget(wikiConnectionInfoGBox);
    this->setLayout(main);
}

//----------------------------------------

//----------------------------------------
//            Commands Widget
//----------------------------------------
CommandsPreferencesWidget::CommandsPreferencesWidget(QWidget *parent)
    :QWidget(parent)
{
    commandsTable = new QTableWidget(this);
    QHBoxLayout *mainLayout = new QHBoxLayout;
    mainLayout->addWidget(commandsTable);

    initTable();

    this->setLayout(mainLayout);
}

void CommandsPreferencesWidget::initTable()
{
    commandsTable->setColumnCount(2);
    commandsTable->setHorizontalHeaderLabels( QStringList() << trUtf8("Command")
                                              << trUtf8("Shortcut") );
}

//----------------------------------------

PreferencesDlg::PreferencesDlg(QWidget *parent) :
    QDialog(parent)
{
    optionsList = new QListWidget(this);
    optionsList->setViewMode(QListView::IconMode);
    optionsList->setIconSize(QSize(64, 64));
    optionsList->setMovement(QListView::Static);
    optionsList->setMaximumWidth(optionsList->iconSize().width()*2.0 - 20);
    optionsList->setMaximumHeight(600);
    optionsList->setSpacing(12);

    buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok
                                          | QDialogButtonBox::Cancel);


    generalWidget = new GeneralPreferencesWidget();
    commandsWidget = new CommandsPreferencesWidget;
    stackLayout = new QStackedLayout();
    stackLayout->addWidget(generalWidget);
    stackLayout->addWidget(commandsWidget);

    QHBoxLayout *hlayout = new QHBoxLayout();
    hlayout->addWidget(optionsList);
    hlayout->addLayout(stackLayout);

    mainLayout = new QVBoxLayout(this);
    mainLayout->addLayout(hlayout);
    mainLayout->addWidget(buttonBox);

    connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
    connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
    connect(optionsList, SIGNAL(currentRowChanged(int)), this, SLOT(selectOption(int)));

    populateList();

    optionsList->setCurrentRow(0);
    selectOption(0);

    this->setWindowTitle(trUtf8("Preferences"));
    this->setWindowIcon(QIcon(":/img/app-icon.png"));
    this->setMinimumSize(QSize(400, 450));
    this->setLayout(mainLayout);
}

void PreferencesDlg::selectOption(int index)
{
    if(index >= stackLayout->count())
        return;

    if(index < 0)
        return;

    stackLayout->setCurrentIndex(index);
}

void PreferencesDlg::populateList()
{
    optionsList->addItem( new QListWidgetItem( QIcon(":/img/general-icon.png"), trUtf8("General") ) );
    optionsList->addItem( new QListWidgetItem( QIcon(":/img/commands-icon.png"), trUtf8("Commands") ) );
    optionsList->addItem( new QListWidgetItem( QIcon(":/img/view-icon.png"), trUtf8("View") ) );
}
