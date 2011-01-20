
#include <QApplication>

#include "timekeeper.h"

int main(int argc, char *argv[])
{
  QApplication app(argc, argv);
  timekeeper mainwin;
  mainwin.show();
  return app.exec();
}

