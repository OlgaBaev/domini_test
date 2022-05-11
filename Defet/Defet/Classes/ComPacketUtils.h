#ifndef COMPACKETUTILS_H
#define COMPACKETUTILS_H

#include <QObject>
#include "stuff.h"

const char PacketStartMark = 0xD5;
const char PacketEndMark = 0xC3;

const char PacketReturnStartMark = 0xEC;
const char PacketReturnEndMark = 0xFA;

const int ComPacketSize = 20;
struct ComPacket
{
	bool isAnswer;
	int64 TotalData; //количество пакетов
	char Data[8];
	int sumSRC;

	ComPacket()
	{
		memset(Data, 0, 8);
		TotalData = 0;
		sumSRC = 0;
	}
};



class ComPacketUtils : public QObject
{
	Q_OBJECT

public:
	ComPacketUtils(QObject *parent);
	~ComPacketUtils();

	static void GeneratePack(char dst[ComPacketSize], int64 Counter, bool isAnswer = 0);
	static int64 CheckPacketCorrectness(QByteArray packet);
};

#endif // COMPACKETUTILS_H
