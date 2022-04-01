#ifndef __TIMEREPORT_H__
#define __TIMEREPORT_H__

#include <QList>
#include <QDialog>

#include "ui_timereport.h"

#include "timeentry.h"

class timereport : public QDialog, public Ui::timereport
{
  Q_OBJECT

  public:
    timereport(QWidget * parent = 0, Qt::WindowFlags f = Qt::Widget);
    virtual ~timereport();

  public slots:
    void setEntries(QList<timeentry*> &entries);
    void sPrint();
};

#endif // __TIMEREPORT_H__
