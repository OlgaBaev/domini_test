#include "ComProcessor.h"

ComProcessor::ComProcessor(HANDLE ComPort, QObject *parent)
	: QObject(parent)
{
	cfg = Config::instance();
	log			= LogManager::instance();
	dataBuffer	= DataBuffer::instance();

	DataReceived = 0;
	DataSent = 0;

	iNeedSendPacks = 0;
	bNeedSendAnswer = false;

	hComPort = ComPort;

	bRun = false;
}

ComProcessor::~ComProcessor()
{
}


void ComProcessor::process()
{
	bRun = true;
	unsigned long read_length = 0;
	while(bRun)
	{
		mutex.lock();

		memset(Buffer, 0, IN_COM_BUFFER*sizeof(char));
		read_length = 0;

		if(bNeedSendAnswer)
		{
			char response[ComPacketSize];
			unsigned long send_length = 0;
			ComPacketUtils::GeneratePack(response, dataBuffer->getTotalCorrectPackets(), true);
			if(!WriteFile(hComPort, response, ComPacketSize, &send_length, NULL) )
			{
				log->AddLog(QString("Unable to send response (%2 to sent, %3 total)")
					.arg(ComPacketSize).arg(dataBuffer->getTotalCorrectPackets()).toLatin1().data(), 2);
			}

			bNeedSendAnswer = false;
		}

		if(iNeedSendPacks)
		{
			char data[ComPacketSize];
			unsigned long send_length = 0;
			for(int i = 0; i < iNeedSendPacks; i++)
			{
				DataSent++;
				ComPacketUtils::GeneratePack(data, DataSent+1);
				if(!WriteFile(hComPort, data, ComPacketSize, &send_length, NULL) )
				{
					log->AddLog(QString("Unable to send data (%2 to sent, %3 total)")
						.arg(ComPacketSize).arg(DataSent).toLatin1().data(), 2);
				}
				else
				{
					//log->AddLog("Data sent", 0);
					//DataSent++;
				}
			}

			iNeedSendPacks = 0;
		}

		if( !ReadFile(hComPort, Buffer, IN_COM_BUFFER, &read_length, NULL) )
		{
			LogManager::instance()->AddLog("Error receiving data. Releasing receiver.", 2);
			stop();
		}
		else
		{
			//log->AddLog(QString("Received %1 bytes").arg(read_length).toLatin1().data(), 0);
			if(read_length)
			{
				DataReceived += read_length;
				DataBuffer::instance()->AddToBuffer(Buffer, read_length);
			}
		}

		mutex.unlock();

		//Sleep(1);
		//msleep(1);

	}
}