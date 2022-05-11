#include "ComPacketUtils.h"

ComPacketUtils::ComPacketUtils(QObject *parent)
	: QObject(parent)
{

}

ComPacketUtils::~ComPacketUtils()
{

}


int64 ComPacketUtils::CheckPacketCorrectness(QByteArray packet)
{
	if(packet.size() != ComPacketSize)
		return 0;

	if((packet.at(0) != PacketStartMark) || (packet.at(ComPacketSize-1) != PacketEndMark))
		if( (packet.at(0) != PacketReturnStartMark) || (packet.at(ComPacketSize-1) != PacketReturnEndMark))
		return 0;

	int64 PacksCount = 0;

	for(int i = 0; i < 8; i++)
	{
		if((packet.at(1+i) & 0xC0) != 0x40)
			return 0; //не является количеством данных

		PacksCount = PacksCount | ( (packet[1+i] & 0x3F) << ((7 - i)*6));
	}

	/*if(packet.at(0) == PacketStartMark)
		TotalSentPackets = PacksCount;
	else
		TotalAnsweredPackets = PacksCount;*/

	int Sum = 0;
	//получение содержимого
	for(int i = 0; i < 8; i++)
	{
		if(packet.at(9+i) < 0x40) //если больше, то верхние биты не 00
			Sum += packet.at(9+i);
		else
			return 0;
	}

	int sumSRC = ((packet[17] & 0x3F) << 6) | (packet[18] & 0x3F);
	if(sumSRC != Sum)
		return 0;

	return PacksCount;
}


void ComPacketUtils::GeneratePack(char dst[ComPacketSize], int64 Counter, bool isAnswer)
{
	ComPacket compack;
	compack.isAnswer = isAnswer;
	compack.TotalData = Counter;

	if(compack.isAnswer)
		dst[0] = PacketReturnStartMark;
	else
		dst[0] = PacketStartMark;

	dst[1] = (compack.TotalData & 0xFC0000000000) >> (6*7);
	dst[2] = (compack.TotalData & 0x3F000000000) >> (6*6);
	dst[3] = (compack.TotalData & 0xFC0000000) >> (6*5);
	dst[4] = (compack.TotalData & 0x3F000000) >> (6*4);
	dst[5] = (compack.TotalData & 0xFC0000) >> (6*3);
	dst[6] = (compack.TotalData & 0x3F000) >> (6*2);
	dst[7] = (compack.TotalData & 0xFC0) >> (6);
	dst[8] = (compack.TotalData & 0x3F);

	int64 Sum = 0;
	for(int i = 0; i < 8; i++)
	{
		compack.Data[i] = rand() % 64;
		Sum += compack.Data[i];
	}

	compack.sumSRC = Sum;

	for(int i = 0; i < 8; i++)
	{
		dst[1+i] = (dst[1+i] & 0x3F) | (0x01 << 6);
		dst[9+i] = compack.Data[i] & 0x3F;
	}

	dst[17] = 0x80 | ((compack.sumSRC & 0xFC0) >> 6);
	dst[18] = 0x80 | (compack.sumSRC & 0x3F);

	if(compack.isAnswer)
		dst[19] = PacketReturnEndMark;
	else
		dst[19] = PacketEndMark;
}
