#pragma once

#ifndef FTPEXPORTSETTINGS_H
#define FTPEXPORTSETTINGS_H

#include "stdafx.h"

class FTPExportSettings : public QDialog
{
	Q_OBJECT

private:

	//User interface
	Ui::FTPExportSettings ui;

private slots:

	//Saves the FTP server info
	void Save();

	//Clears the window
	void Clear();

public:

	//Constructor
	FTPExportSettings(QWidget *parent = 0);

	//Destructor
	~FTPExportSettings();
};

#endif