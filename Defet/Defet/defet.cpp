#include "defet.h"


Defet::Defet(QWidget *parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);

	

	cfg = Config::instance();
	log = LogManager::instance();
	dbf = DataBuffer::instance();

	comOperator = new ComOperator(cfg->getComPortName());
	if(cfg->isAnswering())
		comOperator->enableAnswering();
	if(cfg->isSending())
		comOperator->enableSending();
	
	CreateInterface();
	
	comOperator->init();

	


	//comReceiver->start();

	dbfThread = new QThread();

	dbf->moveToThread(dbfThread);
	connect(dbfThread, SIGNAL(started()), dbf, SLOT(process()));
	connect(dbf, SIGNAL(finished()), dbfThread, SLOT(quit()));
	connect(dbf, SIGNAL(finished()), dbf, SLOT(deleteLater()));
	connect(dbfThread, SIGNAL(finished()), dbfThread, SLOT(deleteLater()));
	dbfThread->start();

	timerRefresh = new QTimer(this);
	connect(timerRefresh, SIGNAL(timeout()), this, SLOT(slotRefreshTimerTimout()));
	timerRefresh->start(10);

	CorrectInPeriod = (int64*)malloc(KEEP_TIME * sizeof(int64));
	SentInPeriod = (int64*)malloc(KEEP_TIME * sizeof(int64));
	memset(CorrectInPeriod, 0, KEEP_TIME * sizeof(int64));
	memset(SentInPeriod, 0, KEEP_TIME * sizeof(int64));

	timerRefresh = new QTimer(this);
	connect(timerRefresh, SIGNAL(timeout()), this, SLOT(slotRefreshTimerTimeout()));
	timerRefresh->start(10);

	timerPartial = new QTimer(this);
	connect(timerPartial, SIGNAL(timeout()), this, SLOT(slotPartialTimerTimeout()));
	timerPartial->start(1000);

	//comOperator->start();
}


void clearThread(QThread * t)
{
	if(t)
	{ 
		t->exit();
		t->wait();
		//t->terminate();
		delete(t);
		t = NULL;
	}
}
	
Defet::~Defet()
{
	LogManager::destroy();
	Config::destroy();

	timerRefresh->stop();
	comOperator->stop();
	dbf->stop();

	clearThread(dbfThread);

	DataBuffer::destroy();
	comOperator->release();
	delete comOperator;
}


QLabel* Defet::MakeLabel(const char* Text, QFont Font, QString Style)
{
	QLabel * label = new QLabel(QString::fromLocal8Bit(Text));
	label->setFont(Font);
	label->setStyleSheet(Style);

	return label;
}


void Defet::CreateInterface()
{
	HeaderFont = QFont("Tahoma", 10, QFont::Bold, 0);
	HeaderStyle = "color: rgb(42, 0, 127)";

	createLogWindow();
	createInBlock();
	createOutBlock();
	buildMainGrid();

	ui.centralWidget->setLayout(ltMain);
}

void Defet::createLogWindow()
{
	ltLogMonitor = new QVBoxLayout;
	logMonitor = new QTextEdit();
	logMonitor->setReadOnly(true);
	ltLogMonitor->addWidget(logMonitor);

	connect(log,		SIGNAL(SignalLogMessage(QString, int)),
			this,		SLOT(slotLogMessage(QString, int)));
}



void Defet::createInBlock()
{
	ltIn = new QFormLayout;

	frameIn = new QFrame();
	frameIn->setFrameShape(QFrame::Box);
	frameIn->setFrameShadow(QFrame::Sunken);	
	frameIn->setLayout(ltIn);
	frameIn->setMinimumWidth(400);

	labelInName = MakeLabel("Приём", HeaderFont, HeaderStyle);
	ltIn->addWidget(labelInName);

	labelAccepted   = MakeLabel("Принято данных: ");
	labelCorrPacks  = MakeLabel("Корректных пакетов: ");
	labelDataLoss   = MakeLabel("Потери данных: ");
	labelPacketLoss = MakeLabel("Потери пакетов: ");
	labelPacketLossInPeriod = MakeLabel(QSTR("Потери за последние %1 сек: ").arg(KEEP_TIME).toLocal8Bit().data());

	labelAcceptedField   = MakeLabel("");
	labelCorrPacksField  = MakeLabel("");
	labelDataLossField   = MakeLabel("");
	labelPacketLossField = MakeLabel("");
	labelPacketLossInPeriodField = MakeLabel("");

	butResetReceiver = new QPushButton(QSTR("СБРОС"));
	connect(butResetReceiver, SIGNAL(released()), this, SLOT(slotButtonResetReceiver()));

	ltIn->addRow(labelAccepted, labelAcceptedField);
	ltIn->addRow(labelCorrPacks, labelCorrPacksField);
	//ltIn->addRow(labelDataLoss, labelDataLossField);
	ltIn->addRow(labelPacketLoss, labelPacketLossField);
	ltIn->addRow(labelPacketLossInPeriod, labelPacketLossInPeriodField);
	ltIn->addWidget(butResetReceiver);
}

void Defet::createOutBlock()
{
	ltOut = new QFormLayout;

	frameOut = new QFrame();
	frameOut->setFrameShape(QFrame::Box);
	frameOut->setFrameShadow(QFrame::Sunken);	
	frameOut->setLayout(ltOut);
	frameOut->setMinimumWidth(400);

	labelOutName = MakeLabel("Передача", HeaderFont, HeaderStyle);
	ltOut->addWidget(labelOutName);

	labelPacketSent			= MakeLabel("Отправлено пакетов: ");
	labelPacketAnswered		= MakeLabel("Принято источником пакетов: ");

	labelPacketSentField	 = MakeLabel("");
	labelPacketAnsweredField = MakeLabel("");

	labelTimeout = MakeLabel("Отправлять пакеты каждые (мсек)");
	lineTimeout = new QLineEdit();
	lineTimeout->setPlaceholderText(QString("%1").arg(comOperator->getTimeout()));
	lineTimeout->setValidator(new QIntValidator(1, 4000, this));

	labelCountPerTimeout = MakeLabel("Количество пакетов за раз");
	lineCount = new QLineEdit();
	lineCount->setPlaceholderText(QString("%1").arg(comOperator->getPacketsToSend()));
	lineCount->setValidator(new QIntValidator(1, 480, this));

	butSaveSettings = new QPushButton(QSTR("Настроить"));
	butStartStopSending = new QPushButton(QSTR("СТАРТ"));
	//butStartStopSending = new QPushButton(QSTR("СТОП"));
	butResetSender = new QPushButton(QSTR("СБРОС"));

	connect(butStartStopSending, SIGNAL(released()), this, SLOT(slotButtonStartStopSending()));
	connect(butSaveSettings, SIGNAL(released()), this, SLOT(slotButtonSaveSettings()));
	connect(butResetSender, SIGNAL(released()), this, SLOT(slotButtonResetSender()));

	ltOut->addRow(labelPacketSent, labelPacketSentField);
	ltOut->addRow(labelPacketAnswered, labelPacketAnsweredField);
	ltOut->addRow(labelTimeout, lineTimeout);
	ltOut->addRow(labelCountPerTimeout, lineCount);
	ltOut->addRow(butStartStopSending, butSaveSettings);
	ltOut->addWidget(butResetSender);
}


void Defet::buildMainGrid()
{
	pbSignalLevel = new QProgressBar();
	pbSignalLevel->setOrientation(Qt::Vertical);
	pbSignalLevel->setMinimum(0);
	pbSignalLevel->setMaximum(100);
	pbSignalLevel->setTextVisible(false);

	ltMain = new QGridLayout;
	ltMain->addWidget(frameIn, 0, 0);
	ltMain->addWidget(pbSignalLevel, 0, 1);
	ltMain->addWidget(frameOut, 0, 2);
	
	ltMain->addLayout(ltLogMonitor, 1, 0, 1, 2);
}


void Defet::slotLogMessage(QString qmess, int mess_priority)
{
	int maxsz = 60000;
	static int counter = 0;
	int sz = qmess.size();
	counter += sz;
	QTextCursor cursor = logMonitor->textCursor();
	if(counter > maxsz)
	{
		cursor.setPosition(0);
		cursor.setPosition(sz+1, QTextCursor::KeepAnchor);
		cursor.removeSelectedText();
		cursor.clearSelection();
		counter -= (sz+1);
	}
	logMonitor->insertPlainText(qmess + "\n");
	cursor.movePosition(QTextCursor::End);
	logMonitor->setTextCursor(cursor);
}


void Defet::slotRefreshTimerTimeout()
{
	int64 accepted = comOperator->getDataReceived(); //dbf->getTotalAccepted();
	int64 sentFrom = dbf->getTotalSentPackets(); //dbf->getTotalSentPackets();
	int64 sentTo = comOperator->getDataSent();
	int64 corrpack = dbf->getTotalCorrectPackets();
	int64 answpack = dbf->getTotalAnsweredPackets();

	labelAcceptedField->setText(QSTR("%1 байт").arg(accepted));
	labelCorrPacksField->setText(QSTR("%1 из %2").arg(corrpack).arg(sentFrom));

	double loss = 0;
	/*if(sent)
		loss = 100 - accepted*100/sent/ComPacketSize;
	labelDataLossField->setText(QString("%1%").arg(loss));*/

	if(sentFrom)
		loss = 100. - corrpack*100./sentFrom;
	labelPacketLossField->setText(QString("%1 %").arg(loss));

	labelPacketSentField->setText(QSTR("%1 (%2 байт)").arg(sentTo).arg(sentTo*ComPacketSize));
	labelPacketAnsweredField->setText(QString("%1").arg(answpack));
}

void Defet::slotButtonStartStopSending()
{
	if(comOperator->isWorking())
	{
		comOperator->stop();
		butStartStopSending->setText(QSTR("СТАРТ"));
	}
	else
	{
		comOperator->start();
		butStartStopSending->setText(QSTR("СТОП"));
	}
}


void Defet::slotButtonSaveSettings()
{
	int timeout = 0;
	int packets = 0;

	timeout = lineTimeout->text().toInt();
	packets = lineCount->text().toInt();

	if(!timeout || !packets)
	{
		lineTimeout->clear();
		lineCount->clear();
		
		lineTimeout->setPlaceholderText(QString("%1").arg(comOperator->getTimeout()));
		lineCount->setPlaceholderText(QString("%1").arg(comOperator->getPacketsToSend()));
		return;
	}

	if(comOperator->isWorking())
	{
		comOperator->stop();
		butStartStopSending->setText(QSTR("СТАРТ"));
	}

	comOperator->setParams(timeout, packets);

	lineTimeout->clear();
	lineCount->clear();
		
	lineTimeout->setPlaceholderText(QString("%1").arg(comOperator->getTimeout()));
	lineCount->setPlaceholderText(QString("%1").arg(comOperator->getPacketsToSend()));
}

void Defet::slotButtonResetSender()
{
	memset(CorrectInPeriod, 0, KEEP_TIME * sizeof(int64));
	memset(SentInPeriod, 0, KEEP_TIME * sizeof(int64));
	comOperator->reset();
	dbf->reset();
}

void Defet::slotButtonResetReceiver()
{
	memset(CorrectInPeriod, 0, KEEP_TIME * sizeof(int64));
	memset(SentInPeriod, 0, KEEP_TIME * sizeof(int64));
	comOperator->reset();
	dbf->reset();
}

void Defet::slotPartialTimerTimeout()
{
	memcpy(CorrectInPeriod, CorrectInPeriod+1, (KEEP_TIME-1)*sizeof(int64));
	memcpy(SentInPeriod, SentInPeriod+1, (KEEP_TIME-1)*sizeof(int64));

	CorrectInPeriod[KEEP_TIME-1] = dbf->getTotalCorrectPackets();
	SentInPeriod[KEEP_TIME-1] = dbf->getTotalSentPackets();

	lossInPeriod = 0;
	if(SentInPeriod[KEEP_TIME-1])
	{
		lossInPeriod = 100. - (CorrectInPeriod[KEEP_TIME-1] - CorrectInPeriod[0]) * 100. / (SentInPeriod[KEEP_TIME-1] - SentInPeriod[0]);
		labelPacketLossInPeriodField->setText(QString("%1 %").arg(lossInPeriod));

		pbSignalLevel->setValue(100-lossInPeriod);

		if(lossInPeriod <=0)
			pbSignalLevel->setStyleSheet(perfect);
		if(lossInPeriod < 20 && lossInPeriod > 0)
			pbSignalLevel->setStyleSheet(good);
		if(lossInPeriod < 50 && lossInPeriod > 20)
			pbSignalLevel->setStyleSheet(satis);
		if(lossInPeriod < 80 && lossInPeriod > 50)
			pbSignalLevel->setStyleSheet(weak);
		if(lossInPeriod >= 80)
			pbSignalLevel->setStyleSheet(absent);
	}

}


