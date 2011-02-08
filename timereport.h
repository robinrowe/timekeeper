#ifndef __TIMEREPORT_H__
#define __TIMEREPORT_H__

#include <QDialog>

#include "ui_timereport.h"

class timereport : public QDialog, public Ui::timereport
{
  Q_OBJECT

  public:
    timereport(QWidget * parent = 0, Qt::WindowFlags f = 0);
    virtual ~timereport();

  public slots:
    void sPrint();
};

#endif // __TIMEREPORT_H__
