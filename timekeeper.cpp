
#include "prefs.h"
#include "timekeeper.h"
#include "tasklist.h"
#include "timeentry.h"
#include "timereport.h"

#include <QCloseEvent>
#include <QMessageBox>
#include <QSettings>
#include <QFile>
#include <QFileDialog>
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
  connect(_openAction,   SIGNAL(triggered()), this, SLOT(sOpen()));
  connect(_prefsAction,  SIGNAL(triggered()), this, SLOT(sPrefs()));
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

  prefs::applyPrefs(_notes, prefs::Display);

  // load any file that exists already for today
  QFile *f = getFile(QDate::currentDate());
  QString errmsg;
  if (! parseFile(f, _entries, errmsg) && ! errmsg.isEmpty())
    QMessageBox::critical(this, tr("Initialization Error"), errmsg);

  sTick();
  _ticktock.start(60000);
}

timekeeper::~timekeeper()
{
}

bool timekeeper::parseFile(QFile *file, QList<timeentry*> &entries, QString &errmsg)
{
  if(file && file->exists())
  {
    QDomDocument doc = QDomDocument();
    QString errMsg;
    int errLine, errCol;
    if(doc.setContent(file, &errMsg, &errLine, &errCol))
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
            entries.append(te);
          }
        }
        return true;
      }
      else
        errmsg =
          tr("Existing daily log does not appear to be a valid TimeKeeper log.");
    }
    else
      errmsg = 
        tr("Encountered an error while parsing %1\n\n%2 (Line %3 Column %4)")
        .arg(file->fileName())
        .arg(errMsg)
        .arg(errLine)
        .arg(errCol);
  }

  return false;
}

QFile *timekeeper::getFile(QDate date)
{
  // get the file for the given date. if date.isNull then let the user choose.
  QFile *thefile = 0;
  if (date.isNull())
    QMessageBox::critical(this, tr("Not Implemented"),
                          tr("Don't know how to open files for other dates"));
  else
    thefile = new QFile(_home.absolutePath() + QDir::separator() +
                        date.toString("yyyyMMMdd") + ".xml");

  return thefile;
}

void timekeeper::sOpen()
{
  QString filename = QFileDialog::getOpenFileName(this,
                                                  tr("Select a TimeKeeper File"),
                                                  _home.absolutePath(),
                                                  tr("TimeKeeper files (*.xml)"),
                                                  0,
                                                  QFileDialog::ReadOnly);
  QFile file(filename);
  if (file.open(QIODevice::ReadOnly))
  {
    QList<timeentry*> tmpentries;
    QString errmsg;
    if (parseFile(&file, tmpentries, errmsg))
    {
      timereport dlg(this);
      dlg.setEntries(tmpentries);
      dlg.exec();
    }
  }
}

void timekeeper::sTick()
{   QLocale::setDefault(QLocale::C);
    QLocale locale; // Constructs a default QLocale
    QString s = locale.toString(QDateTime::currentDateTime());
    _time->setText(s);
//    _time->setText(QDateTime::currentDateTime().toString(Qt::SystemLocaleShortDate));

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
        ts << ci->text() << Qt::endl;
      }
    }
    tf.close();
  }
  else
    QMessageBox::critical(this, tr("Initialization Error"), tr("Could not access the configuration files."));
}

void timekeeper::sPrefs()
{
  prefs dlg(this);
  if (dlg.exec() == QDialog::Accepted)
    prefs::applyPrefs(_notes, prefs::Display);
}

void timekeeper::sReport()
{
  timereport dlg(this);
  dlg.setEntries(_entries);
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

  QFile *tf = getFile(QDate::currentDate());
  if(tf->open(QIODevice::WriteOnly | QIODevice::Truncate | QIODevice::Text))
  {
    QTextStream ts(tf);
    ts << doc.toString();
    tf->close();
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
  _currentEntry = 0;

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

