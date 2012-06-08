#ifndef SERVERSTATS_H
#define SERVERSTATS_H

#include <QtGui/QWidget>
#include "ui_serverstats.h"

class ServerStats : public QWidget
{
	Q_OBJECT

private:

	//User interface
	Ui::ServerStatsClass ui;

public:

	//Constructor
	ServerStats(QWidget *parent = 0, Qt::WFlags flags = 0);

	//Destructor
	~ServerStats();

};

#endif