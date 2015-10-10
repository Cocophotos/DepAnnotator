#ifndef LOGCONSOLE_H
#define LOGCONSOLE_H

#include <QString>
#include <QList>
#include <QPair>

/**
 * @brief The Console class
 * Singleton class to handle log console
 * during a session of the program.
 */
class LogConsole
{
public:
    enum Status{ Info = 0, Warning, Critical, Debug };
    /**
     * @brief console
     * @return An instance of the class
     */
    static LogConsole *console ()
    {
        if (NULL == _singleton)
        {
            _singleton = new LogConsole;
        }
        return _singleton;
    }

    /**
     * @brief kill
     * Destroy the instance
     */
    static void kill ()
    {
        if (NULL != _singleton)
        {
            delete _singleton;
            _singleton = NULL;
        }
    }

    void addLine(QString line, Status status);
    QString getEntireLog() const;


private:
    LogConsole(){}
    ~LogConsole(){}

private:
    static LogConsole *_singleton;
    QList<QPair<QString, Status> > mLog;
};

#endif // LOGCONSOLE_H
