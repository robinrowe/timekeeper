#ifndef __TIMEENTRY_H__
#define __TIMEENTRY_H__

#include <QString>
#include <QDateTime>

class timeentry
{
  public:
    timeentry() {};
    virtual ~timeentry() {};

    QString _task;
    QString _notesStart;
    QString _notesStop;
    QDateTime _start;
    QDateTime _stop;
};

#endif // __TIMEENTRY_H__

