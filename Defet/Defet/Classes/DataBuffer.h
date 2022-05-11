#ifndef DATABUFFER_H
#define DATABUFFER_H

#include <QObject>
#include <QMutex>

#include "ComPacketUtils.h"
#include "LogManager.h"
#include "stuff.h"



class DataBuffer : public QObject
{
	Q_OBJECT

public:
	static DataBuffer *instance();
	static void destroy();

	DataBuffer(QObject *parent = 0);
	~DataBuffer();

	bool AddToBuffer(char * arr, int size);
	bool AddToBuffer(QByteArray arr, int size);

	void stop() { mutex.lock(); bRun = false; mutex.unlock(); }
	void reset() {mutex.lock(); TotalAccepted = 0; TotalProcessed = 0;
								TotalSentPackets = 0; TotalCorrectPackets = 0; mutex.unlock(); }

	int64 getTotalAccepted() { return TotalAccepted; }
	int64 getTotalProcessed() { return TotalProcessed; }
	int64 getTotalSentPackets() { return TotalSentPackets; }
	int64 getTotalCorrectPackets() { return TotalCorrectPackets; }
	int64 getTotalAnsweredPackets() { return TotalAnsweredPackets; }


public slots:
	void process();

private:
	static DataBuffer		*inst;
	LogManager				*log;
	QMutex mutex;

	QByteArray Buffer;

	bool bRun;

	int64 TotalAccepted;
	int64 TotalProcessed;
	int64 TotalSentPackets;
	int64 TotalCorrectPackets;
	int64 TotalAnsweredPackets;

	//bool CheckPacketCorrectness(QByteArray packet);

};

#endif // DATABUFFER_H
