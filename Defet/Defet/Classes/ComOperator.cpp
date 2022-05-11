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
	if(!GetCommState(hComPort, &dcb))   //Заполняем DCB текущим состоянием порта
	{
		LogManager::instance()->AddLog("Error read DCB", 2);
		return false;
	}
		//Изменяем некоторые значения
	dcb.BaudRate = speed;                   //Скорость порта
	dcb.fBinary = true;                     //Вкл двичный режим обмена
	dcb.fParity = false;                    //вкл. режим четности
	dcb.fOutxCtsFlow = false;               //вкл. режим слежения за сигналом CTS
	dcb.fOutxDsrFlow = false;               //вкл. режим слежения за сигналом DSR
	dcb.fDtrControl = DTR_CONTROL_DISABLE;  //режим управления DTR(исп-ся при потере связи)
	dcb.fDsrSensitivity = false;            //Принимать данные только при установленом сигнале DSR
	dcb.fTXContinueOnXoff = false;          //Продолжать передачу при переполненом буфере приема
	dcb.fOutX = false;                      //использовать XON/XOFF при передаче
	dcb.fInX = false;                       //использовать XON/XOFF при приеме
	dcb.fErrorChar = false;                 //заменять символы с ошибкой четности
	dcb.fNull = false;                      //отбрасывать нулевые байты при передаче
	dcb.fRtsControl = RTS_CONTROL_DISABLE;  //режим управления для RTS сигнала
	dcb.fAbortOnError = false;              //прекращает работать при ошибке
	dcb.fDummy2 = 0;                        //не используется
	dcb.XonLim = 4048;                      //минимальное число символов в приемном буфере
	dcb.XoffLim = 514;                      //максимальное число символов в приемном буфере (maxAddBufLen - XoffLim)
	dcb.ByteSize = 0x08;                    //число информационных бит в передаваемых и принимаемых данных 0x08
	dcb.Parity = NOPARITY;                  //выбор схемы контроля четности
	dcb.StopBits = ONESTOPBIT;              //количество стоповых бит
	dcb.XonChar = 0x11;                     //задает символ xon для приема/передачи 0x11
	dcb.XoffChar = 0x13;                    //задает символ xoff для приема/передачи 0x13
	dcb.ErrorChar = 0x00;                   //символ для замены символов с ошибочной четностью 0x00
	dcb.EofChar = 0x1A;                     //символ для сигнализации о конце даннных 0x1A
	dcb.EvtChar = 0x1A;                     //символ для сигнализации о событии 0x1A
	dcb.wReserved1 = 0;                     //не используется



	if( !SetCommState(hComPort, &dcb) )
	{
		LogManager::instance()->AddLog("Error set DCB", 2);
		return false;
	}

	return true;
}

bool ComOperator::setTimeouts()
{
		//Установить таймауты
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

