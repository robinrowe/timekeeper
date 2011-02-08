
#include "timereport.h"

#include <QPrinter>
#include <QPrintDialog>

timereport::timereport(QWidget * parent, Qt::WindowFlags f)
  : QDialog(parent, f)
{
  setupUi(this);
  connect(_print, SIGNAL(clicked()), this, SLOT(sPrint()));
}

timereport::~timereport()
{
}

void timereport::sPrint()
{
  QPrinter printer;
  QPrintDialog pd(&printer);
  if (pd.exec() == QDialog::Accepted)
    _report->print(&printer);
}
