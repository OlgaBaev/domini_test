#include "defet.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	Defet w;
	w.show();
	return a.exec();
}
