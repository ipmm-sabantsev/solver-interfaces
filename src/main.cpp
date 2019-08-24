#include <QCoreApplication>
#include <QDebug>
#include <logging.h>
#include <IVector.h>

#define array_size(array) (sizeof(array)/sizeof(*array))

int main(int argc, char *argv[])
{
  //QCoreApplication a(argc, argv);

  int logInit = ILog::init("logging");
  if (logInit != ERR_OK)
    return -1;

  double vals[5] = { 0.1, 0.2, 0.3, 0.4, 0.5 };
  IVector* vector = IVector::createVector(array_size(vals), vals);
  for (unsigned int i = 0; i < vector->getDim(); ++i) {
    double element;
    vector->getCoord(i, element);
    LOG(QString::number(element).toStdString().c_str());
  }

  ILog::destroy();
  return 0;
  //return a.exec();
}
