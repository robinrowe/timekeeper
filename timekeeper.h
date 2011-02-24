#ifndef __TIMEKEEPER_H__
#define __TIMEKEEPER_H__

#include <QMainWindow>
#include <QDateTime>
#include <QTimer>
#include <QDir>
#include <QList>

#include "ui_timekeeper.h"

class QFile;
class timeentry;

class timekeeper : public QMainWindow, public Ui::timekeeper
{
  Q_OBJECT

  public:
    timekeeper();
    virtual ~timekeeper();

  public slots:
    void sTick();

    void sTask();
    void sReport();
    void sOpen();
    void sSave();
    void sAbout();

    void sStart();
    void sStop();


  protected:
    void closeEvent(QCloseEvent * event);
    QFile *getFile(QDate date);
    bool   parseFile(QFile * file, QList<timeentry*> &entries, QString &errmsg);

  private:
    QTimer _ticktock;
    QDateTime _taskStarted;
    QDateTime _lastSave;
    QDir _home;

    QList<timeentry*> _entries;
    timeentry * _currentEntry;
};

#endif // __TIMEKEEPER_H__
