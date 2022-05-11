#include "LogManager.h"

//---------------------------------------------------------------------------
LogManager * LogManager::inst = NULL;
//---------------------------------------------------------------------------
LogManager * LogManager::instance()
{
	if(!inst)
	{
		inst = new LogManager();
	}
	return inst;
}
//---------------------------------------------------------------------------
void LogManager::destroy()
{
	if(inst)
	{
		delete inst;
		inst = NULL;
	}
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
LogManager::LogManager()
{ 
	QString AppDir = QApplication::applicationDirPath();
	QDir LogFolder(AppDir);
	if(!LogFolder.entryList().contains("logs"))
		LogFolder.mkdir("logs");
	FileName = "";
	FileName.append(AppDir).append("/logs/SystemMessage_")
		.append(QDateTime().currentDateTime().toString("yyyy_MM_dd")).append(".log");
/*
	memset(FileName, 0, 1024);
	sprintf(FileName, "%s/logs/SystemMessage_%s.log", AppDir.toLatin1().data(),
		QDateTime().currentDateTime().toString("yyyy_MM_dd"));*/

	fl = NULL;
	fl = fopen(FileName.toLatin1().data(), "at");
	if(fl)
	{
		fprintf(fl, "=======================================================\n");
		fclose(fl);
		fl = NULL;
	}

	/*fl = NULL;
	fl = fopen(FileName.toLatin1().data(), "wt");
	if(fl)
	{
		fclose(fl);
		fl = NULL;
	}	*/
	
}
//---------------------------------------------------------------------------
LogManager::~LogManager()
{
}
//---------------------------------------------------------------------------
void LogManager::AddLog(char* mess, int MessPriority)
{
	if(!inst) return;

	mutex.lock();
	QString qmess;	

	qmess.append(time.currentTime().toString("hh:mm:ss "));
	qmess.append(date.currentDate().toString("(dd.MM.yyyy)"));
	char mess_pr[32]; memset(mess_pr, 0, 32);
	switch(MessPriority)
	{
		case 0: sprintf(mess_pr, " ");		break;
		case 1: sprintf(mess_pr, " WARNING - ");		break;
		case 2: sprintf(mess_pr, " ERROR - ");	break;
	}	
	qmess.append(QString::fromLocal8Bit(mess_pr));
	qmess.append(QString::fromLocal8Bit(mess));
	
	QString AppDir = QApplication::applicationDirPath();
	QDir LogFolder(AppDir);
	if(!LogFolder.entryList().contains("logs"))
		LogFolder.mkdir("logs");
	FileName = "";
	FileName.append(AppDir).append("/logs/SystemMessage_")
		.append(QDateTime().currentDateTime().toString("yyyy_MM_dd")).append(".log");

	fl = NULL;
	fl = fopen(FileName.toLatin1().data(), "at");	
	if(fl) 
	{
		fprintf(fl, "%s\n", qmess.toLocal8Bit().data());
		fclose(fl);
		fl = NULL;
	}
	
	emit(SignalLogMessage(qmess, MessPriority));
	mutex.unlock();
}


