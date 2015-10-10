#ifndef PREFERENCESDLG_H
#define PREFERENCESDLG_H

#include <QDialog>
#include <QStackedLayout>
#include <QLineEdit>
#include <QPushButton>
#include <QGroupBox>
#include <QListWidget>
#include <QTableWidget>
#include <QDialogButtonBox>

class GeneralPreferencesWidget : public QWidget
{
public:
    explicit GeneralPreferencesWidget(QWidget *parent = 0);

private:
    QLineEdit *loginLE,
              *passwordLE;

    QGroupBox *wikiConnectionInfoGBox;
};

class CommandsPreferencesWidget : public QWidget
{
public:
    explicit CommandsPreferencesWidget(QWidget *parent = 0);

private:
    void initTable();

private:
    QTableWidget *commandsTable;
};


class PreferencesDlg : public QDialog
{
    Q_OBJECT
public:
    explicit PreferencesDlg(QWidget *parent = 0);
    
private slots:
    void selectOption(int index);
private:
    void populateList();
private:
    QListWidget *optionsList;
    QStackedLayout *stackLayout;
    QVBoxLayout *mainLayout;
    QDialogButtonBox *buttonBox;

    GeneralPreferencesWidget *generalWidget;
    CommandsPreferencesWidget *commandsWidget;
};

#endif // PREFERENCESDLG_H
