#ifndef _STUFF_H_
#define _STUFF_H_

#include <memory>

#define QSTR(str) \
	QString::fromLocal8Bit(str)

#define IN_COM_BUFFER  40
#define OUT_COM_BUFFER 40

#define MAX_BUFFER_SIZE (9600*10)

typedef long long int64;
//typedef unsigned char byte;

//background: QLinearGradient( x1: 0, y1: 0, x2: 1, y2: 0,stop: 0 #FF0350,stop: 0.4999 #FF0020,stop: 0.5 #FF0019,stop: 1 #FF0000 );
const QString perfect = "QProgressBar::chunk { background-color: #00bfff;}";
const QString good = "QProgressBar::chunk { background-color: #25b01e;}";
const QString satis = "QProgressBar::chunk { background-color: #e7ed7b;}";
const QString weak = "QProgressBar::chunk { background-color: #de7d40;}";
const QString absent = "QProgressBar::chunk { background-color: #5e0903;}";


#endif

