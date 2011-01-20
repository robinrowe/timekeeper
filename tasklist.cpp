
#include "tasklist.h"

#include <QInputDialog>

tasklist::tasklist(QWidget * parent, Qt::WindowFlags f)
  : QDialog(parent, f)
{
  setupUi(this);

  connect(_new, SIGNAL(clicked()), this, SLOT(sNew()));
  connect(_edit, SIGNAL(clicked()), this, SLOT(sEdit()));
  connect(_delete, SIGNAL(clicked()), this, SLOT(sDelete()));
}

tasklist::~tasklist()
{
}

void tasklist::sNew()
{
  bool ok = false;
  QString ret = QInputDialog::getText(this, tr("Edit Task Name"), tr("Name"), QLineEdit::Normal, "", &ok);
  if(ok)
  {
    _tasks->addItem(ret);
    _tasks->sortItems();
  }
}

void tasklist::sEdit()
{
  QListWidgetItem * ci = _tasks->currentItem();
  if(!ci)
    return;

  bool ok = false;
  QString ret = QInputDialog::getText(this, tr("Edit Task Name"), tr("Name"), QLineEdit::Normal, ci->text(), &ok);
  if(ok)
  {
    ci->setText(ret);
    _tasks->sortItems();
  }
}

void tasklist::sDelete()
{
  QListWidgetItem * ci = _tasks->takeItem(_tasks->currentRow());
  if(ci)
    delete ci;
}

