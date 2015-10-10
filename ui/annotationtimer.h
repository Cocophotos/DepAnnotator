#ifndef ANNOTATIONTIMER_H
#define ANNOTATIONTIMER_H

#include <QPushButton>
#include <QLCDNumber>
#include <QTime>
#include <QTimer>

class DigitalClock: public QLCDNumber
{
public:
    Q_OBJECT

public:
    DigitalClock(QWidget *parent = 0);   

    int elapsed()const{return m_elapsed;}

public slots:
    void pause();
    void stop();
    void start();

private slots:
    void showTime();

private:

    //! Timer to display elapsed time
    QTimer *timer;

    //! Start time
    QTime m_start;

    //! Elapsed time (in ms)
    int m_elapsed;
};


class AnnotationTimer: public QWidget
{
    Q_OBJECT

public:
    AnnotationTimer(QWidget *parent = 0);

    /**
     * @brief elapsedTime
     * @return number of seconds elapsed
     */
    int elapsedSeconds()const{return elapsedClock->elapsed();}

public slots:
    void pause();
    //void start();

private:
    //! DigitalClock to view the elasped time
    DigitalClock *elapsedClock;

    //! Push button to pause the timer
    QPushButton *pauseBtn;

    //! Push button to (re)start the timer
    QPushButton *playBtn;

    //! Push button to reset the timer
    QPushButton *stopBtn;
};

#endif // ANNOTATIONTIMER_H
