#pragma once

#ifndef SERVEREDIT_H
#define SERVEREDIT_H

#include <QDialog>
#include "ui_serveredit.h"

#include "Config/config.h"

class ServerEdit : public QDialog
{
	Q_OBJECT

private:

	//User interface
	Ui::ServerEdit ui;

private slots:

	//Adds a new server
	void Add();

	//Removes a server
	void Remove();

	//Locale changed
	void LocaleChanged(QString text);

	//Server was clicked
	void ServerClicked(QListWidgetItem* item);

public:

	//Constructor
	ServerEdit(QWidget *parent = 0);

	//Destructor
	~ServerEdit();
};

#endif