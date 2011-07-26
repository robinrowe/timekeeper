
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

// TODO: add rich-text option
void timereport::setEntries(QList<timeentry*> &entries)
{
  QMap<QString, int> map;
  QMap<QString, QString> map2;
  QDateTime firstdatetime = QDateTime::currentDateTime();
  QDateTime lastdatetime  = QDateTime();
  bool  showStartStop = prefs::showStartStop();

  for (int i = 0; i < entries.size(); ++i)
  {
    timeentry * te = entries.at(i);
    if (te->_start < firstdatetime)
      firstdatetime = te->_start;

    if (te->_stop.isValid() && (lastdatetime > te->_stop || lastdatetime.isNull()))
      lastdatetime = te->_stop;
    else 
      lastdatetime = QDateTime::currentDateTime();

    int secs = te->_start.secsTo(te->_stop.isValid() ? te->_stop : QDateTime::currentDateTime());
    QTime interval(0, 0);
    interval = interval.addSecs(secs);

    if(map.contains(te->_task))
      secs += map.value(te->_task);
    map[te->_task] = secs;
    QString text;
    text = te->_start.toString("H:mm") + " - " + te->_stop.toString("H:mm") + "\t(" + interval.toString("H:mm") + ")";
    if(!te->_notesStart.isEmpty())
      text += (showStartStop ? "\nStart:\t" : "\n") + te->_notesStart + "\n";
    if(!te->_notesStop.isEmpty())
      text += (showStartStop ? "\nStop:\t" : "\n")  + te->_notesStop + "\n";

    if(!text.isEmpty())
    {
      if(map2.contains(te->_task))
        text = map2.value(te->_task) + "\n" + text;
      map2[te->_task] = text;
    }
  }

  QString report = firstdatetime.toString("'Timesheet for' MMM dd, yyyy\t") +
                   QDate::currentDate().toString("'Printed' MMM dd, yyyy'\n'");

  int totSecs = 0;
  QMapIterator<QString, int> it(map);
  while(it.hasNext())
  {
    it.next();
    int secs = it.value();
    totSecs += secs;
    QTime tm(0, 0);
    tm = tm.addSecs(secs);
    report += "\n======================================\n";
    report += it.key();
    report += "\t";
    report += tm.toString("H:mm");
    report += "  (" + QString::number(0.0 - tm.secsTo(QTime(0, 0)) / 60.0 / 60.0, 'g', 2) + " hr)";
    report += "\n======================================\n";
    report += map2.value(it.key());
    report += "\n";
  }

  QTime tm(0, 0);
  tm = tm.addSecs(totSecs);
  report += "\n======================================\n";
  report += firstdatetime.toString("MMM dd, yyyy, H:mm'-'");
  report += lastdatetime.toString("H:mm'\n'");
  report += tm.toString("'Total: ' H:mm");
  report += "  (" + QString::number(0.0 - tm.secsTo(QTime(0, 0)) / 60.0 / 60.0, 'g', 2) + " hr)";
  QTime breaks = QTime(0, 0).addSecs(lastdatetime.toTime_t() - firstdatetime.toTime_t() - totSecs);
  report += breaks.toString("'\tUnaccounted: ' H:mm'\t'");
  report += "  (" + QString::number(0.0 - breaks.secsTo(QTime(0, 0)) / 60.0 / 60.0, 'g', 2) + " hr)";

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
