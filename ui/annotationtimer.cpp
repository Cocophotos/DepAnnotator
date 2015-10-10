#include "annotationtimer.h"

#if QT_VERSION >= 0x050000
    #include <QtWidgets>
    #include <QtGui>
    #include<QtPrintSupport/QPrinter>
#else
    #include <QtGui>
#endif

#include <QDebug>

DigitalClock::DigitalClock(QWidget *parent)
    : QLCDNumber(parent)
{
    setSegmentStyle(Filled);

    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(showTime()));
    stop();
}

void DigitalClock::start()
{    
    m_start.start();
    m_start = m_start.addSecs(-m_elapsed);
    timer->start(249);
}

void DigitalClock::pause()
{
    timer->stop();
}

void DigitalClock::stop()
{
    timer->stop();
    m_elapsed = 0;
    display("00:00");
}

void DigitalClock::showTime()
{
    m_elapsed = m_start.elapsed() / 1000;
    QTime time;
    time = time.addSecs(m_elapsed);
    QString text = time.toString("mm:ss");

    if ((time.second() % 2) == 0)
        text[2] = ' ';
    display(text);
}

///////////////////////////////////////////////////////

AnnotationTimer::AnnotationTimer(QWidget *parent)
    :QWidget(parent)
{
    elapsedClock = new DigitalClock(parent);

    playBtn = new QPushButton("Play", this);
    playBtn->setIcon(QIcon(":/img/play-icon.png"));
    connect(playBtn, SIGNAL(clicked()), elapsedClock, SLOT(start()));
    pauseBtn = new QPushButton("Pause", this);
    pauseBtn->setIcon(QIcon(":/img/pause-icon.png"));
    connect(pauseBtn, SIGNAL(clicked()), elapsedClock, SLOT(pause()));
    stopBtn = new QPushButton("Stop", this);
    stopBtn->setIcon(QIcon(":/img/stop-icon.png"));
    connect(stopBtn, SIGNAL(clicked()), elapsedClock, SLOT(stop()));

    QVBoxLayout *layout = new QVBoxLayout;
    QHBoxLayout *buttonsLayout = new QHBoxLayout;

    buttonsLayout->addWidget(playBtn);
    buttonsLayout->addWidget(pauseBtn);
    buttonsLayout->addWidget(stopBtn);

    layout->addWidget(elapsedClock);
    layout->addLayout(buttonsLayout);
    this->setLayout(layout);
}

void AnnotationTimer::pause()
{
    elapsedClock->pause();
}

/*void AnnotationTimer::start()
{
    elapsedClock->start();
}*/

