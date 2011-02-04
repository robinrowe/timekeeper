
#include "timekeeper.h"
#include "tasklist.h"
#include "timeentry.h"
#include "timereport.h"

#include <QCloseEvent>
#include <QMessageBox>
#include <QSettings>
#include <QFile>
#include <QTextStream>
#include <QDomDocument>
#include <QDebug>

timekeeper::timekeeper()
  : QMainWindow()
{
  setupUi(this);

  _currentEntry = 0;

  connect(&_ticktock, SIGNAL(timeout()), this, SLOT(sTick()));
  connect(_taskAction, SIGNAL(triggered()), this, SLOT(sTask()));
  connect(_reportAction, SIGNAL(triggered()), this, SLOT(sReport()));
  connect(_aboutAction, SIGNAL(triggered()), this, SLOT(sAbout()));
  connect(_start, SIGNAL(clicked()), this, SLOT(sStart()));
  connect(_stop, SIGNAL(clicked()), this, SLOT(sStop()));

  // Figure out where we are going to store our files
  _home = QDir::home();
  if(!_home.exists("timekeeper"))
    _home.mkdir("timekeeper");
  if(!_home.cd("timekeeper"))
  {
    QMessageBox::critical(this, tr("Initialization Error"), tr("Could not access the configuration files."));
    close();
  }

  // load the task list
  QFile tf(_home.absolutePath() + "/tasks.txt");
  if(tf.exists())
  {
    if(tf.open(QIODevice::ReadOnly | QIODevice::Text))
    {
      QTextStream ts(&tf);
      while(!ts.atEnd())
        _task->addItem(ts.readLine());
      tf.close();
    }
    else
      QMessageBox::critical(this, tr("Initialization Error"), tr("Could not access the configuration files."));
  }
  if(_task->count() == 0)
    _task->addItem("--None Specified--");

  // load any file that exists already for today
  QString dn = QDate::currentDate().toString("yyyyMMMdd");
  QFile f(_home.absolutePath() + "/" + dn + ".xml");
  if(f.exists())
  {
    QDomDocument doc = QDomDocument();
    QString errMsg;
    int errLine, errCol;
    if(doc.setContent(&f, &errMsg, &errLine, &errCol))
    {
      QDomElement root = doc.documentElement();
      if(root.tagName() == "TimeKeeper")
      {
        QDomNodeList nlist = root.childNodes();
        QDomNode it;
        for(int i = 0; i < nlist.count(); i++)
        {
          it = nlist.at(i);
          if(it.isElement() && it.nodeName() == "entry")
          {
            timeentry * te = new timeentry();

            QDomNodeList nl = it.childNodes();
            for(int n = 0; n < nl.count(); n++)
            {
              QDomElement elemThis = nl.item(n).toElement();
              if(elemThis.tagName() == "task")
                te->_task = elemThis.text();
              else if(elemThis.tagName() == "notesStart")
                te->_notesStart = elemThis.text();
              else if(elemThis.tagName() == "notesStop")
                te->_notesStop = elemThis.text();
              else if(elemThis.tagName() == "start")
                te->_start = QDateTime::fromString(elemThis.text());
              else if(elemThis.tagName() == "stop")
                te->_stop = QDateTime::fromString(elemThis.text());
            }
            _entries.append(te);
          }
        }
      }
      else
        QMessageBox::critical(this, tr("Initialization Error"),
          tr("Existing daily log does not appear to be a valid TimeKeeper log."));
    }
    else
      QMessageBox::critical(this, tr("Initialization Error"),
        tr("Encountered an error while parsing %1\n\n%2 (Line %3 Column %4)")
        .arg(dn + ".xml")
        .arg(errMsg)
        .arg(errLine)
        .arg(errCol));
  }

  sTick();
  _ticktock.start(60000);
}

timekeeper::~timekeeper()
{
}

void timekeeper::sTick()
{
  _time->setText(QDateTime::currentDateTime().toString(Qt::SystemLocaleShortDate));

  if(_lastSave.secsTo(QDateTime::currentDateTime()) > 600)
    sSave();
}

void timekeeper::sTask()
{
  tasklist dlg(this);
  for(int c = 0; c < _task->count(); c++)
  {
    dlg._tasks->addItem(_task->itemText(c));
  }
  dlg.exec();

  _task->clear();
  QFile tf(_home.absolutePath() + "/tasks.txt");
  if(tf.open(QIODevice::WriteOnly | QIODevice::Truncate | QIODevice::Text))
  {
    QTextStream ts(&tf);
    for(int i = 0; i < dlg._tasks->count(); i++)
    {
      QListWidgetItem * ci = dlg._tasks->item(i);
      if(ci)
      {
        _task->addItem(ci->text());
        ts << ci->text() << endl;
      }
    }
    tf.close();
  }
  else
    QMessageBox::critical(this, tr("Initialization Error"), tr("Could not access the configuration files."));
}

void timekeeper::sReport()
{
  QMap<QString, int> map;
  QMap<QString, QString> map2;
  for (int i = 0; i < _entries.size(); ++i)
  {
    timeentry * te = _entries.at(i);
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

  QString report;
  QMapIterator<QString, int> it(map);
  while(it.hasNext())
  {
    it.next();
    int secs = it.value();
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

  timereport dlg(this);
  dlg._report->setPlainText(report);
  dlg.exec();
}

void timekeeper::sSave()
{
  QDomDocument doc("TimeKeeperDef");
  QDomElement root = doc.createElement("TimeKeeper");
  doc.appendChild(root);

  for (int i = 0; i < _entries.size(); ++i)
  {
    timeentry * te = _entries.at(i);

    QDomElement entry = doc.createElement("entry");
    QDomElement text;
    text = doc.createElement("task");
    text.appendChild(doc.createTextNode(te->_task));
    entry.appendChild(text);
    text = doc.createElement("notesStart");
    text.appendChild(doc.createTextNode(te->_notesStart));
    entry.appendChild(text);
    text = doc.createElement("notesStop");
    text.appendChild(doc.createTextNode(te->_notesStop));
    entry.appendChild(text);
    text = doc.createElement("start");
    text.appendChild(doc.createTextNode(te->_start.toString()));
    entry.appendChild(text);
    text = doc.createElement("stop");
    if(te->_stop.isValid())
      text.appendChild(doc.createTextNode(te->_stop.toString()));
    else
      text.appendChild(doc.createTextNode(QDateTime::currentDateTime().toString()));
    entry.appendChild(text);
    root.appendChild(entry);
  }

  QString dn = QDate::currentDate().toString("yyyyMMMdd");
  QFile tf(_home.absolutePath() + "/" + dn + ".xml");
  if(tf.open(QIODevice::WriteOnly | QIODevice::Truncate | QIODevice::Text))
  {
    QTextStream ts(&tf);
    ts << doc.toString();
    tf.close();
  }
  else
    QMessageBox::critical(this, tr("Save Error"), tr("Could not access the configuration files."));
}

void timekeeper::sStart()
{
  _taskStarted = QDateTime::currentDateTime();
  _currentEntry = new timeentry();
  _currentEntry->_task = _task->currentText();
  _currentEntry->_notesStart = _notes->toPlainText();
  _currentEntry->_start = _taskStarted;
  _entries.append(_currentEntry);

  _stop->setEnabled(true);
  _start->setEnabled(false);
  _task->setEnabled(false);
  _taskAction->setEnabled(false);
  _notes->clear();
}

void timekeeper::sAbout()
{
  QMessageBox::about(this, tr("About Time Keeper"), tr("Time Keeper 0.0.2"));
}

void timekeeper::sStop()
{
  if(_currentEntry)
  {
    _currentEntry->_notesStop = _notes->toPlainText();
    _currentEntry->_stop = QDateTime::currentDateTime();
  }
  _currentEntry = false;

  _taskStarted = QDateTime(); // set to null date-time
  _stop->setEnabled(false);
  _start->setEnabled(true);
  _task->setEnabled(true);
  _taskAction->setEnabled(true);
  _notes->clear();
}

void timekeeper::closeEvent(QCloseEvent *event)
{
  if(_currentEntry)
  {
    QString notes = _notes->toPlainText();
    if(!notes.isEmpty())
      notes += "\n\n";
    notes += "Task Stopped on application exit.";
    _notes->setPlainText(notes);
    sStop();
  }

  sSave();

  QSettings settings("xTuple", "TimeKeeper");
  settings.setValue("geometry", saveGeometry());
  settings.setValue("windowState", saveState());
  QMainWindow::closeEvent(event);
}

