#include "prefs.h"

#include <QFont>
#include <QSettings>
#include <QTextEdit>

prefs::prefs(QWidget *parent, Qt::WindowFlags f)
  : QDialog(parent, f)
{
  setupUi(this);

  connect(_buttonBox, SIGNAL(accepted()), this, SLOT(sSave()));

  QSettings settings("xtuple.com", "timekeeper");

  settings.beginGroup("entry");
  if (settings.contains("FontName"))
    _entryFont->setCurrentFont(QFont(settings.value("FontName").toString()));
  if (settings.contains("FontSize"))
    _entrySize->setValue(settings.value("FontSize").toInt());
  settings.endGroup();

  settings.beginGroup("report");
  if (settings.contains("FontName"))
    _reportFont->setCurrentFont(QFont(settings.value("FontName").toString()));
  if (settings.contains("FontSize"))
    _reportSize->setValue(settings.value("FontSize").toInt());
  settings.endGroup();
}

void prefs::applyPrefs(QPlainTextEdit *widget, PrefsGroup group)
{
  if (! widget)
    return;

  int     size   = widget->document()->defaultFont().pointSize();
  QString family = widget->document()->defaultFont().family();

  if (Display == group)
  {
    size   = prefs::entryFontSize();
    family = prefs::entryFontName();
  }
  else if (Paper == group)
  {
    size   = prefs::reportFontSize();
    family = prefs::reportFontName();
  }
  widget->setStyleSheet(QString("* { font-size: %1pt;"
                                "    font-family: \"%2\";"
                                "}")
                         .arg(size).arg(family));
}

void prefs::sSave()
{
  QTextEdit textedit;
  QSettings settings("xtuple.com", "timekeeper");

  settings.beginGroup("entry");
  if (_entryFont->currentFont().family() == textedit.fontFamily())
    settings.remove("FontName");
  else
    settings.setValue("FontName", _entryFont->currentFont().family());
  if (_entrySize->value() <= 0)
    settings.remove("FontSize");
  else
    settings.setValue("FontSize", _entrySize->value());
  settings.endGroup();

  settings.beginGroup("report");
  if (_reportFont->currentFont().family() == textedit.fontFamily())
    settings.remove("FontName");
  else
    settings.setValue("FontName", _reportFont->currentFont().family());
  if (_reportSize->value() <= 0)
    settings.remove("FontSize");
  else
    settings.setValue("FontSize", _reportSize->value());
  settings.endGroup();

  accept();
}

QString prefs::entryFontName()
{
  QSettings settings("xtuple.com", "timekeeper");
  settings.beginGroup("entry");
  return settings.value("FontName", "").toString();
}

int prefs::entryFontSize()
{
  QSettings settings("xtuple.com", "timekeeper");
  settings.beginGroup("entry");
  return settings.value("FontSize", -1).toInt();
}

QString prefs::reportFontName()
{
  QSettings settings("xtuple.com", "timekeeper");
  settings.beginGroup("report");
  return settings.value("FontName", "").toString();
}

int prefs::reportFontSize()
{
  QSettings settings("xtuple.com", "timekeeper");
  settings.beginGroup("report");
  return settings.value("FontSize", -1).toInt();
}
