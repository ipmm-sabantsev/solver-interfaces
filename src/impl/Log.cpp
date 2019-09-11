#include "ILog.h"

#include <QFile>
#include <QTime>
#include <QDebug>

static QFile logFile;

int ILog::report(const char *msg)
{
  if (!msg)
    return ERR_WRITE_TO_ILogImpl;

  if(!logFile.isOpen() || !logFile.isWritable()) {
    qWarning() << "logFile is not open: " << logFile.errorString();
    return ERR_WRITE_TO_ILogImpl;
  }

  const QString timeString =
    "[" + QTime::currentTime().toString("hh:mm:ss.zzz") + "] ";

  logFile.write(timeString.toUtf8().data());
  logFile.write(msg);
  logFile.write("\n");

  logFile.flush();

  return ERR_OK;
}

int ILog::init(const char* fileName)
{
  if(!fileName)
    return ERR_WRONG_ARG;

  if(logFile.isOpen())
    return ERR_OPEN_ILogImpl;

  logFile.setFileName(QString(fileName));

  if (!logFile.open(QIODevice::WriteOnly | QIODevice::Append)) {
    qWarning() << "Cannot open logFile: " << logFile.errorString();
    return ERR_OPEN_ILogImpl;
  }

  return report("log begin");
}

void ILog::destroy()
{
  report("log end\n");
  logFile.close();
}
