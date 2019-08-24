#include "ILog.h"

#include <QFile>
#include <QTime>

static QFile logFile;

int ILog::report(const char *msg)
{
  if (!msg)
    return ERR_WRITE_TO_ILogImpl;

  if(!logFile.isOpen())
    return ERR_WRITE_TO_ILogImpl;

  if(!logFile.isWritable())
    return ERR_WRITE_TO_ILogImpl;

  const QString timeString =
    "[" + QTime::currentTime().toString("hh:mm:ss.zzz") + "] ";

  logFile.write(timeString.toUtf8().data());
  logFile.write(msg);
  logFile.write("\n");

  return ERR_OK;
}

int ILog::init(const char* fileName)
{
  if(!fileName)
    return ERR_WRONG_ARG;

  logFile.setFileName(QString(fileName));

  if (!logFile.exists())
    return ERR_OPEN_ILogImpl;

  if (!logFile.open(QIODevice::WriteOnly | QIODevice::Append))
    return ERR_OPEN_ILogImpl;

  return ERR_OK;
}

void ILog::destroy()
{
  logFile.close();
}
