#ifndef __TASKLIST_H__
#define __TASKLIST_H__

#include <QDialog>

#include <ui_tasklist.h>

class tasklist : public QDialog, public Ui::tasklist
{
  Q_OBJECT

  public:
    tasklist(QWidget * parent = 0, Qt::WindowFlags f = 0);
    virtual ~tasklist();

  public slots:
    void sNew();
    void sEdit();
    void sDelete();
};

#endif // __TASKLIST_H__

