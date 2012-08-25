#pragma once

#ifndef FILEEXPORTSETTINGS_H
#define FILEEXPORTSETTINGS_H

#include <QDialog>
#include "ui_fileexportsettings.h"

class FileExportSettings : public QDialog
{
	Q_OBJECT

private:

	//User interface
	Ui::FileExportSettings ui;

private slots:

	//Saves the file info
	void Save();

	//Clears the window
	void Clear();

	//Sets the file save path
	void Open();

public:

	//Constructor
	FileExportSettings(QWidget *parent = 0);

	//Destructor
	~FileExportSettings();
};

#endif