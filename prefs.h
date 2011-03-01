#include "ui_prefs.h"

#include <QPlainTextEdit>

class prefs : public QDialog, public Ui::prefs
{
  Q_OBJECT

  public:
    enum PrefsGroup { Display, Paper };

    prefs(QWidget *parent = 0, Qt::WindowFlags f = 0);

    static QString entryFontName();
    static int     entryFontSize();
    static QString reportFontName();
    static int     reportFontSize();
    static bool    showStartStop();
    static void    applyPrefs(QPlainTextEdit *widget, PrefsGroup group = Display);

  public slots:
    void sSave();
};
