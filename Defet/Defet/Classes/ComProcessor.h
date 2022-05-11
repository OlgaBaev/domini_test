#ifndef ComProcessor_H
#define ComProcessor_H

#include <QObject>
#include <QMutex>
#include <QtSerialPort/QSerialPort>

#include <Windows.h>
#include "DataBuffer.h"
#include "LogManager.h"
#include "config.h"
#include "stuff.h"


class ComProcessor : public QObject
{
	Q_OBJECT

public:
	ComProcessor( HANDLE ComPort, QObject *parent = 0);
	~ComProcessor();

	void start() { bRun = true; }
	void stop() { bRun = false; }

	int64 getDataRecieved() { return DataReceived; }
	int64 getDataSent() { return DataSent; }
	void reset() { DataReceived = 0; DataSent = 0; }

	void sendPacks(int count) { iNeedSendPacks = count; }
	void sendAnswer() { bNeedSendAnswer = true; }


private:
	Config		*cfg;
	LogManager	*log;
	DataBuffer * dataBuffer;

	QMutex mutex;

	HANDLE hComPort;
	bool bRun;

	int iNeedSendPacks;
	bool bNeedSendAnswer;

	int64 DataReceived;
	int64 DataSent;

	QTimer * timer;
	char Buffer[IN_COM_BUFFER];

public slots:
	void process();
};

#endif // ComProcessor_H
