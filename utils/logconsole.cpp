#include "logconsole.h"

LogConsole *LogConsole::_singleton = NULL;

void LogConsole::addLine(QString line, Status status)
{
    mLog << qMakePair(line, status);
}

QString LogConsole::getEntireLog() const
{
    QString concat = "";
    typedef QPair<QString, Status> LogPair;
    foreach(LogPair pair, mLog)
    {
        QString status;
        switch(pair.second){
        case Info:
            status = "Info:";
            break;
        case Warning:
            status = "Warning:";
            break;
        case Critical:
            status = "Critical:";
            break;
        case Debug:
            status = "Debug:";
            break;
        }

        concat += status + " " + pair.first + "\n";
    }

    return concat;
}
