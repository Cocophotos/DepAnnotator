#ifndef LOGCONSOLEWIDGET_H
#define LOGCONSOLEWIDGET_H

#include <QPlainTextEdit>
#include <QHBoxLayout>
#include <QDialog>

#include "utils/logconsole.h"

class LogConsoleWidget: public QDialog
{
    Q_OBJECT
public:
    LogConsoleWidget(QDialog *parent = 0);

private:
    QPlainTextEdit *te;
    QHBoxLayout *mainLayout;
};

#endif // LOGCONSOLEWIDGET_H
