#include <QtGui>
#include "logconsolewidget.h"

LogConsoleWidget::LogConsoleWidget(QDialog *parent)
    :QDialog(parent)
{

    te = new QPlainTextEdit(this);
    mainLayout = new QHBoxLayout(this);
    mainLayout->addWidget(te);
    this->setLayout(mainLayout);
    this->setMinimumSize(600, 400);
    this->setWindowIcon(QIcon(":/img/app-icon.png"));
    this->setWindowTitle("Log Console");

    te->setPlainText(LogConsole::console()->getEntireLog());
}



