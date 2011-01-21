
#include <QApplication>

#include "timekeeper.h"

int main(int argc, char *argv[])
{
  Q_INIT_RESOURCE(timekeeper);

  QApplication app(argc, argv);

#ifndef Q_WS_MACX
  QApplication::setWindowIcon(QIcon(":/images/icon32x32.png"));
#endif

  timekeeper mainwin;
  mainwin.show();
  return app.exec();
}

