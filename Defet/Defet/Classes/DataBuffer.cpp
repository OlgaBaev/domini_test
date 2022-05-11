#include "DataBuffer.h"

//==============================================================================
DataBuffer* DataBuffer::inst = NULL;
//==============================================================================

DataBuffer *DataBuffer::instance()
{
	if (!inst)
	{
		inst = new DataBuffer();
	}
	return inst;
}

//==============================================================================

void DataBuffer::destroy()
{
	if(inst)
	{
		delete inst;
		inst = NULL;
	}
}

DataBuffer::DataBuffer(QObject *parent)
	: QObject(parent)
{
	TotalAccepted = 0;
	TotalProcessed = 0;
	TotalSentPackets = 0;
	TotalCorrectPackets = 0;
	TotalAnsweredPackets = 0;

	Buffer.clear();
}

DataBuffer::~DataBuffer()
{
}

bool DataBuffer::AddToBuffer(char * arr, int size)
{
	if((Buffer.size() + size) > MAX_BUFFER_SIZE)
		log->AddLog(QString("Buffer extends it's maximum size! (%1 of %2)")
		.arg(Buffer.size() + size).arg(MAX_BUFFER_SIZE).toLatin1().data(), 1);

	mutex.lock();
	Buffer.append((const char*) arr, size);
	//TotalAccepted += size;
	mutex.unlock();

	return true;
}

bool DataBuffer::AddToBuffer(QByteArray arr, int size)
{
	if((Buffer.size() + size) > MAX_BUFFER_SIZE)
		log->AddLog(QString("Buffer extends it's maximum size! (%1 of %2)")
		.arg(Buffer.size() + size).arg(MAX_BUFFER_SIZE).toLatin1().data(), 1);

	mutex.lock();
	Buffer.append(arr, size);
	//TotalAccepted += size;
	mutex.unlock();

	return true;
}

void DataBuffer::process()
{
	bRun = true;

	while (bRun)
	{
		int pos = 0;
		//mutex.lock();
		if(Buffer.size() > ComPacketSize)
		{
			while((pos < ComPacketSize) && (pos+ComPacketSize <= Buffer.size()))
			{
				int64 counter = ComPacketUtils::CheckPacketCorrectness(Buffer.mid(pos, ComPacketSize));
				if(counter)
				{
					if(Buffer.at(pos) == PacketReturnStartMark)
					{
						TotalAnsweredPackets = counter;
						//TotalProcessed -= ComPacketSize;
					}
					else
					if(Buffer.at(pos) == PacketStartMark)
					{
						TotalSentPackets = counter;
						TotalCorrectPackets++;
						//TotalProcessed += ComPacketSize+pos;
					}
					//else 

					Buffer = Buffer.right(Buffer.length() - ComPacketSize - pos);
					if(Buffer.size() < ComPacketSize)
						break;
					pos = 0;
				}
				else
				{
					pos++;
					//TotalProcessed++;
				}
			}
			if(pos==ComPacketSize)
			{
				Buffer = Buffer.right(Buffer.length() - ComPacketSize);
			}
			
		}
		//else
			//_sleep(1);
		//mutex.unlock();
		//_sleep(1);
	}


}




