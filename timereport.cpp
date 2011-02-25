
#include "timereport.h"

#include <QPrinter>
#include <QPrintDialog>

#include "prefs.h"

timereport::timereport(QWidget * parent, Qt::WindowFlags f)
  : QDialog(parent, f)
{
  setupUi(this);
  connect(_print, SIGNAL(clicked()), this, SLOT(sPrint()));

  prefs::applyPrefs(_report, prefs::Display);
}

timereport::~timereport()
{
}

void timereport::setEntries(QList<timeentry*> &entries)
{
  QMap<QString, int> map;
  QMap<QString, QString> map2;
  QDate firstdate = QDate::currentDate();
  for (int i = 0; i < entries.size(); ++i)
  {
    timeentry * te = entries.at(i);
    if (te->_start.date() < firstdate)
      firstdate = te->_start.date();
    int secs = te->_start.secsTo(te->_stop.isValid() ? te->_stop : QDateTime::currentDateTime());
    QTime interval(0, 0);
    interval = interval.addSecs(secs);

    if(map.contains(te->_task))
      secs += map.value(te->_task);
    map[te->_task] = secs;
    QString text;
    text = te->_start.toString("H:mm") + " - " + te->_stop.toString("H:mm") + "\t(" + interval.toString("H:mm") + ")\n\n";
    if(!te->_notesStart.isEmpty())
      text += "Start:\t" + te->_notesStart + "\n";
    if(!te->_notesStop.isEmpty())
      text += "Stop:\t" + te->_notesStop + "\n";

    if(!text.isEmpty())
    {
      if(map2.contains(te->_task))
        text = map2.value(te->_task) + "\n\n" + text;
      map2[te->_task] = text;
    }
  }

  QString report = firstdate.toString("'Timesheet for' MMM dd, yyyy\t") +
                   QDate::currentDate().toString("'Printed' MMM dd, yyyy");
  report += "\n======================================\n";

  int totSecs = 0;
  QMapIterator<QString, int> it(map);
  while(it.hasNext())
  {
    it.next();
    int secs = it.value();
    totSecs += secs;
    QTime tm(0, 0);
    tm = tm.addSecs(secs);
    report += it.key();
    report += "\t";
    report += tm.toString("H:mm");
    report += "  (" + QString::number(0.0 - tm.secsTo(QTime(0, 0)) / 60.0 / 60.0, 'g', 2) + " hr)";
    report += "\n======================================\n";
    report += map2.value(it.key());
    report += "\n\n";
  }

  QTime tm(0, 0);
  tm = tm.addSecs(totSecs);
  report += "Total\t";
  report += tm.toString("H:mm");
  report += "  (" + QString::number(0.0 - tm.secsTo(QTime(0, 0)) / 60.0 / 60.0, 'g', 2) + " hr)";
  report += "\n======================================\n";

  _report->setPlainText(report);
}

void timereport::sPrint()
{
  prefs::applyPrefs(_report, prefs::Paper);
  QPrinter printer;
  QPrintDialog pd(&printer);
  if (pd.exec() == QDialog::Accepted)
    _report->print(&printer);
  prefs::applyPrefs(_report, prefs::Display);
}
