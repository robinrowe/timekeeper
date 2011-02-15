
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
  // TODO: make font and size configurable, both for display and print
  _report->setStyleSheet("* { font-size: 10pt; }");
  QPrinter printer;
  QPrintDialog pd(&printer);
  if (pd.exec() == QDialog::Accepted)
    _report->print(&printer);
  _report->setStyleSheet("");
}
