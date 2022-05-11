#ifndef DEFET_H
#define DEFET_H

#include <QtWidgets/QMainWindow>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QFormLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QFrame>
#include <QtWidgets/QTextEdit>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QPushButton.h>
#include <QtWidgets/QScrollBar>
#include <QtWidgets/QProgressBar>

#include <QTimer>

#include "config.h"
#include "LogManager.h"
#include "ComOperator.h"
#include "DataBuffer.h"

#include "ui_defet.h"

#define KEEP_TIME 20

class Defet : public QMainWindow
{
	Q_OBJECT

public:
	Defet(QWidget *parent = 0);
	~Defet();

private:
	Ui::DefetClass ui;

	Config			*cfg;
	DataBuffer		*dbf;
	ComOperator		*comOperator;

	QVBoxLayout		* ltLogMonitor;
	LogManager		* log;
	QTextEdit		* logMonitor;

	QFont HeaderFont;
	QString HeaderStyle;

	QGridLayout			*ltMain; //Главный
	
	QFormLayout			*ltOut; //Блок отправки > ltMainH 0
	QFormLayout			*ltIn; //Блок приёма > ltMainH 1
	QFrame				*frameIn;
	QFrame				*frameOut;

	QLabel		*labelInName;
	QLabel		*labelOutName;

	QLabel		*labelAccepted;
	QLabel		*labelCorrPacks;
	QLabel		*labelDataLoss;
	QLabel		*labelPacketLoss;
	QLabel		*labelPacketLossInPeriod;
	QLabel		*labelPacketSent;
	QLabel		*labelPacketAnswered;
	
	QLabel		*labelAcceptedField;
	QLabel		*labelCorrPacksField;
	QLabel		*labelDataLossField;
	QLabel		*labelPacketLossField;
	QLabel		*labelPacketLossInPeriodField;
	QLabel		*labelPacketSentField;
	QLabel		*labelPacketAnsweredField;

	QLabel		*labelTimeout;
	QLineEdit	*lineTimeout;
	QLabel		*labelCountPerTimeout;
	QLineEdit	*lineCount;

	QPushButton *butSaveSettings;
	QPushButton *butStartStopSending;
	QPushButton *butStartStopReceiving;

	QPushButton *butResetSender;
	QPushButton *butResetReceiver;
	
	QTimer		*timerRefresh;
	QTimer		*timerPartial;
	
	//QThread		*comReceiverThread;
	//QThread		*comAnswererThread; 
	QThread		*dbfThread;

	
	int64 * CorrectInPeriod;
	int64 * SentInPeriod;
	float lossInPeriod;
	QProgressBar * pbSignalLevel;

	void CreateInterface();
	void createLogWindow();
	void createInBlock();
	void createOutBlock();
	void buildMainGrid();

	QLabel * MakeLabel(	 const char *Text,
						QFont Font = QFont("Tahoma", 9, QFont::Normal),
						QString Style = "color: rgb(0, 0, 0)");


public slots:
	void slotLogMessage(QString qmess, int mess_priority);
	void slotRefreshTimerTimeout();
	void slotButtonSaveSettings();
	void slotButtonStartStopSending();
	void slotButtonResetSender();
	void slotButtonResetReceiver();
	void slotPartialTimerTimeout();


};

#endif // DEFET_H
