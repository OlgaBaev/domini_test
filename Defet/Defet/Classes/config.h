#ifndef CONFIG
#define CONFIG
#include <QtCore>
#include <QMutex>

class Config : QObject
{
	Q_OBJECT
public:
	static Config *instance();
	static void destroy();
	
	Config();
	~Config();

	void LoadConfig();
	void SaveConfig();


	QString		getComPortName()	{ return ComPortName; }
	void		setComPortName(QString value) { ComPortName = value; }
	int			getComSpeed() { return ComSpeed; }
	void		setComSpeed(int value) { ComSpeed = value; }
	int			isAnswering() { return AnsweringEnabled; }
	int			isSending()   { return SendingEnabled; }

private:
	static Config*  inst;
	QSettings *setting;
	QMutex mutex;

	QString ComPortName;

	int ComSpeed;
	int AnsweringEnabled;
	int SendingEnabled;

public slots:
};


#endif //CONFIG
