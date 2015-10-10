#ifndef REVISION_H
#define REVISION_H

#include <QDateTime>

/**
 * Generic template describing a revision
 *
 * A revision is two simple things :
 * 1. An item
 * 2. A timestamp
 */
template<typename T> class Revision
{
public:
    /**
     * @brief Revision
     * @param val is the item to be put in a revision
     * @param time is the timestamp of the revision
     */
    Revision(T val, QDateTime time = QDateTime::currentDateTime())
        :value(val), timestamp(time){}

    /**
     * @brief getValue
     * @return the item
     */
    T getValue() const {return value;}

    /**
     * @brief getTimestamp
     * @return the timestamp
     */
    QDateTime getTimestamp() const {return timestamp;}

private:
    T value;
    QDateTime timestamp;
};

#endif // REVISION_H
