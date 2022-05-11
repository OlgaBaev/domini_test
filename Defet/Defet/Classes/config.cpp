#include "config.h"

//==============================================================================
Config* Config::inst = NULL;
//==============================================================================

Config *Config::instance()
{
	if (!inst)
	{
		inst = new Config();
		inst->LoadConfig();
	}
	return inst;
}

//==============================================================================

void Config::destroy()
{
	if (inst)
	{
		inst->SaveConfig();
		delete inst;
		inst = NULL;
	}
}

//==============================================================================

Config::Config()
{
}

//==============================================================================

Config::~Config()
{

}
//==============================================================================

void Config::LoadConfig()
{
	QCoreApplication::setOrganizationName("RNIIRS");
	QCoreApplication::setApplicationName("Defet");	

	QString file_name = QCoreApplication::applicationDirPath();
	file_name.append("/Defet.ini");
	setting = new QSettings(file_name, QSettings::IniFormat);

	//Загрузка параметров из ini-файла

	ComPortName = setting->value("setting/COM_PORT_NAME", "").toString();
	if(ComPortName == "")
	{
		ComPortName = "\\\\.\\COM5";
		setting->setValue("setting/COM_PORT_NAME", ComPortName);
	}

	ComSpeed = setting->value("setting/COM_SPEED", -1).toInt();		
	if(ComSpeed == -1)
	{
		ComSpeed = 9600;
		setting->setValue("setting/COM_SPEED", ComSpeed);
	}

	AnsweringEnabled = setting->value("setting/ANSWERING_ENABLED", -1).toInt();		
	if(AnsweringEnabled == -1)
	{
		AnsweringEnabled = 1;
		setting->setValue("setting/ANSWERING_ENABLED", AnsweringEnabled);
	}

	SendingEnabled = setting->value("setting/SENDING_ENABLED", -1).toInt();		
	if(SendingEnabled == -1)
	{
		SendingEnabled = 0;
		setting->setValue("setting/SENDING_ENABLED", SendingEnabled);
	}
}


//==============================================================================

void Config::SaveConfig()
{
	setting->setValue("setting/COM_PORT_NAME_SENDER", ComPortName);
}

//==============================================================================




