//---------------------------------------------------------------------------
#ifndef LOGMANAGER_H
#define LOGMANAGER_H

//#include <QtCore>
#include <QApplication>
#include <QMutex>
#include <QTime>
#include <QTextCodec>
#include <QDir>
//#include "config.h"

//---------------------------------------------------------------------------
class LogManager : public QObject
{
	Q_OBJECT

private:
	static LogManager * inst;

	FILE* fl;
	//char FileName[1024];
	QString FileName;
	QMutex mutex;
	QTime  time;
	QDate	 date;

	
public :
	LogManager();
	~LogManager();
	static LogManager * instance();
	static void destroy();

	void AddLog(char* mess, int MessPriority);
	//void AddLog(QString qmess, int MessPriority);

signals:
	void SignalLogMessage(QString, int);		
};
//---------------------------------------------------------------------------
#endif //LOGMANAGER_H
