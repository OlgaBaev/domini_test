#ifndef COMOPERATOR_H
#define COMOPERATOR_H

#include <QObject>
#include <QMutex>

#include <Windows.h>

#include "ComProcessor.h"
#include "DataBuffer.h"
#include "LogManager.h"
#include "config.h"
#include "stuff.h"


class ComOperator : public QObject
{
	Q_OBJECT

public:
	ComOperator(QString ComName, QObject *parent = 0);
	~ComOperator();

	bool init();
	bool release();
	void reset() { if(comProcessor) comProcessor->reset(); }
	void start();
	void stop() { mutex.lock(); timerSender->stop(); timerAnswering->stop(); bWorking = false; mutex.unlock(); }

	bool isAnswering() { return bAnswering; }
	void disableAnswering() { bAnswering = false; stop(); }
	void enableAnswering() { bAnswering = true; }

	bool isSending() { return bSending; }
	void disableSending() { bSending = false; stop(); }
	void enableSending() { bSending = true; }

	void setParams(int timeout, int packets);
	int getTimeout() { return Timeout; }
	int getPacketsToSend() { return PacketsToSend; }

	bool isWorking() { return bWorking; }

	int64 getDataSent() { if(comProcessor) return comProcessor->getDataSent(); else return 0;}
	int64 getDataReceived() { if(comProcessor)  return comProcessor->getDataRecieved(); else return 0;}

private:
	Config		*cfg;
	LogManager	*log;
	DataBuffer * dataBuffer;
	ComProcessor * comProcessor;
	QThread * comProcessorThread;

	bool bSending;
	bool bAnswering;
	bool bWorking;

	int PacketsToSend;
	int Timeout;

	bool isInit;
	QMutex mutex;


	QTimer * timerSender;
	QTimer * timerAnswering;

	

	QString ComName;
	HANDLE hComPort;
	DCB dcb;
	COMMTIMEOUTS timeouts; //Структура для настройки таймаутов
	COMMPROP comprop; //Структура возможностей устройства

	//int64 DataSent;
	

	int rw_timeout;
	int speed;

	bool openPort();
	bool initDCB();
	bool setTimeouts();


	

	bool SendData(char * data, size_t size);

public slots:
	//void process();
	void SendStandartPacket();
	void SendAnswerPacket();
	
};

#endif // COMOPERATOR_H
