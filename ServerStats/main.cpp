#include "serverstats.h"
#include <QtGui/QApplication>

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	ServerStats w;
	w.show();
	return a.exec();
}