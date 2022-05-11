#include "ComOperator.h"

ComOperator::ComOperator(QString Name, QObject *parent)
	: QObject(parent)
{
	cfg = Config::instance();
	log			= LogManager::instance();
	dataBuffer	= DataBuffer::instance();
	comProcessor = NULL;
	hComPort = NULL;
	
	ComName = Name;
	rw_timeout = 10;
	speed = cfg->getComSpeed();
	//DataSent = 0;
	//DataReceived = 0;

	isInit = false;
	Timeout = 10;
	PacketsToSend = 1;

	bSending = false;
	bAnswering = false;
	bWorking = false;

	timerSender = new QTimer(this);
	timerSender->setInterval(Timeout);
	connect(timerSender, SIGNAL(timeout()), this, SLOT(SendStandartPacket()));

	timerAnswering = new QTimer(this);
	timerAnswering->setInterval(Timeout);
	connect(timerAnswering, SIGNAL(timeout()), this, SLOT(SendAnswerPacket()));

	//init();
	
}


ComOperator::~ComOperator()
{
	//comProcessor->stop();
	release();
}

bool ComOperator::init()
{
	if(!openPort()) return false;
	if(!initDCB()) return false;
	if(!setTimeouts()) return false;	

	if( !SetupComm(hComPort, IN_COM_BUFFER, OUT_COM_BUFFER) )
	{
		log->AddLog("Error setup comm", 2);
		return false;
	}

	log->AddLog("COM Port receiver is initialized and open successfully", 0);

	comProcessor = new ComProcessor(hComPort);
	comProcessorThread = new QThread();
	comProcessor->moveToThread(comProcessorThread);
	connect(comProcessorThread, SIGNAL(started()), comProcessor, SLOT(process()));
	connect(comProcessor, SIGNAL(finished()), comProcessorThread, SLOT(quit()));
	connect(comProcessor, SIGNAL(finished()), comProcessor, SLOT(deleteLater()));
	connect(comProcessorThread, SIGNAL(finished()), comProcessorThread, SLOT(deleteLater()));

	comProcessor->start();
	comProcessorThread->start();

	isInit = true;
	return true;
}

bool ComOperator::release()
{
	CloseHandle(hComPort);
	hComPort = NULL;
	isInit = false;

	return true;
}

void ComOperator::start()
{
	//mutex.lock();

	bWorking = true;

	if(bSending)
		timerSender->start();
	//Sleep(5);

	if(bAnswering)
		timerAnswering->start();
	
	//comProcessorThread->start();

	//mutex.unlock();
}

bool ComOperator::openPort()
{
	hComPort = CreateFile(ComName.toStdWString().c_str(),
		GENERIC_READ|GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL);
	
	if(hComPort==INVALID_HANDLE_VALUE)
	{
		LogManager::instance()->AddLog("Cannot open COM port", 2);
		return false;
	}

	if( !GetCommProperties(hComPort, &comprop) )
	{
		LogManager::instance()->AddLog("Error read port properties", 2);
		return false;
	}

	return true;

}

void ComOperator::setParams(int timeout, int packets)
{
	stop();
	mutex.lock();

	Timeout = timeout;
	PacketsToSend = packets;
	timerSender->setInterval(Timeout);
	timerAnswering->setInterval(Timeout);

	mutex.unlock();
}

bool ComOperator::initDCB()
{
	dcb.DCBlength = sizeof(DCB);
	if(!GetCommState(hComPort, &dcb))   //��������� DCB ������� ���������� �����
	{
		LogManager::instance()->AddLog("Error read DCB", 2);
		return false;
	}
		//�������� ��������� ��������
	dcb.BaudRate = speed;                   //�������� �����
	dcb.fBinary = true;                     //��� ������� ����� ������
	dcb.fParity = false;                    //���. ����� ��������
	dcb.fOutxCtsFlow = false;               //���. ����� �������� �� �������� CTS
	dcb.fOutxDsrFlow = false;               //���. ����� �������� �� �������� DSR
	dcb.fDtrControl = DTR_CONTROL_DISABLE;  //����� ���������� DTR(���-�� ��� ������ �����)
	dcb.fDsrSensitivity = false;            //��������� ������ ������ ��� ������������ ������� DSR
	dcb.fTXContinueOnXoff = false;          //���������� �������� ��� ������������ ������ ������
	dcb.fOutX = false;                      //������������ XON/XOFF ��� ��������
	dcb.fInX = false;                       //������������ XON/XOFF ��� ������
	dcb.fErrorChar = false;                 //�������� ������� � ������� ��������
	dcb.fNull = false;                      //����������� ������� ����� ��� ��������
	dcb.fRtsControl = RTS_CONTROL_DISABLE;  //����� ���������� ��� RTS �������
	dcb.fAbortOnError = false;              //���������� �������� ��� ������
	dcb.fDummy2 = 0;                        //�� ������������
	dcb.XonLim = 4048;                      //����������� ����� �������� � �������� ������
	dcb.XoffLim = 514;                      //������������ ����� �������� � �������� ������ (maxAddBufLen - XoffLim)
	dcb.ByteSize = 0x08;                    //����� �������������� ��� � ������������ � ����������� ������ 0x08
	dcb.Parity = NOPARITY;                  //����� ����� �������� ��������
	dcb.StopBits = ONESTOPBIT;              //���������� �������� ���
	dcb.XonChar = 0x11;                     //������ ������ xon ��� ������/�������� 0x11
	dcb.XoffChar = 0x13;                    //������ ������ xoff ��� ������/�������� 0x13
	dcb.ErrorChar = 0x00;                   //������ ��� ������ �������� � ��������� ��������� 0x00
	dcb.EofChar = 0x1A;                     //������ ��� ������������ � ����� ������� 0x1A
	dcb.EvtChar = 0x1A;                     //������ ��� ������������ � ������� 0x1A
	dcb.wReserved1 = 0;                     //�� ������������



	if( !SetCommState(hComPort, &dcb) )
	{
		LogManager::instance()->AddLog("Error set DCB", 2);
		return false;
	}

	return true;
}

bool ComOperator::setTimeouts()
{
		//���������� ��������
	if( !GetCommTimeouts(hComPort, &timeouts) )
	{
		LogManager::instance()->AddLog("Error read timeout's", 2);
		
		return false;
	}

	timeouts.ReadIntervalTimeout = 0xFFFFFFFF;
	timeouts.ReadTotalTimeoutMultiplier = 0xFFFFFFFF;
	timeouts.ReadTotalTimeoutConstant = rw_timeout;
	timeouts.WriteTotalTimeoutMultiplier = 0;
	timeouts.WriteTotalTimeoutConstant = 0;

	if( !SetCommTimeouts(hComPort, &timeouts) )
	{
		LogManager::instance()->AddLog("Error set timeout's", 2);
		return false;
	}

	return true;
}

void ComOperator::SendAnswerPacket()
{
	if(comProcessor)
		comProcessor->sendAnswer();
	/*char response[ComPacketSize];
	ComPacketUtils::GeneratePack(response, dataBuffer->getTotalCorrectPackets(), true);
	if(!SendData(response, ComPacketSize))
	{
		log->AddLog(QString("Unable to send response (%2 to sent, %3 total)")
			.arg(ComPacketSize).arg(dataBuffer->getTotalCorrectPackets()).toLatin1().data(), 2);
		return false;
	}
	return true;*/
}


void ComOperator::SendStandartPacket()
{
	if(comProcessor)
		comProcessor->sendPacks(PacketsToSend);
	//char data[ComPacketSize];
	//unsigned long send_length = 0;
	//for(int i = 0; i < PacketsToSend; i++)
	//{
	//	ComPacketUtils::GeneratePack(data, DataSent);
	//	if(!SendData(data, ComPacketSize))
	//	//if(!WriteFile(hComPort, data, ComPacketSize, &send_length, NULL) )
	//	{
	//		log->AddLog(QString("Unable to send data (%2 to sent, %3 total)")
	//			.arg(ComPacketSize).arg(DataSent).toLatin1().data(), 2);
	//		return false;
	//	}
	//	else
	//	{
	//		//log->AddLog("Data sent", 0);
	//		DataSent++;
	//	}
	//}

	//return true;

}


bool ComOperator::SendData(char * data, size_t size)
{
	unsigned long send_length = 0;
	if(!WriteFile(hComPort, data, size, &send_length, NULL) )
		return false;
	return true;

}

