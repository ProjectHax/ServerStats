#include "stdafx.h"

extern ServerStatsConfig Config;

//Constructor
FTPExportSettings::FTPExportSettings(QWidget *parent) : QDialog(parent)
{
	ui.setupUi(this);

	QMessageBox::warning(this, "FTP", "Qt 5.1+ no longer contains the QFtp classes and this requires doing some hackery to get them back.\n\nFTP support has been disabled until they add a suitable replacement.");

	QIntValidator* validator = new QIntValidator(1, 65535, this);
	ui.Port->setValidator(validator);

	//Load the server info from the config
	ui.Hostname->setText(Config.stats_ftp_server.c_str());
	ui.Port->setText(QString("%0").arg(Config.stats_ftp_port));
	ui.Username->setText(Config.stats_ftp_username.c_str());
	ui.Password->setText(Config.stats_ftp_password.c_str());
	ui.Path->setText(Config.stats_ftp_path.c_str());
	ui.FileName->setText(Config.stats_ftp_file_name.c_str());

	if(Config.stats_ftp_transfer_mode == 0)
		ui.TransferMode->setCurrentIndex(0);
	else
		ui.TransferMode->setCurrentIndex(1);
}

//Destructor
FTPExportSettings::~FTPExportSettings()
{
}

//Saves the FTP server info
void FTPExportSettings::Save()
{
	//Copy the server info
	Config.stats_ftp_server = ui.Hostname->text().toLatin1().data();
	Config.stats_ftp_port = ui.Port->text().length() ? boost::lexical_cast<uint16_t>(ui.Port->text().toLatin1().data()) : 0;
	Config.stats_ftp_username = ui.Username->text().toLatin1().data();
	Config.stats_ftp_password = ui.Password->text().toLatin1().data();
	Config.stats_ftp_path = ui.Path->text().toLatin1().data();
	Config.stats_ftp_file_name = ui.FileName->text().toLatin1().data();

	if(ui.TransferMode->currentIndex() == 0)
		Config.stats_ftp_transfer_mode = 0;
	else
		Config.stats_ftp_transfer_mode = 1;

	if(!Config.stats_ftp_path.empty())
	{
		if(Config.stats_ftp_path.substr(Config.stats_ftp_path.length() - 1, 1) != "/")
		{
			Config.stats_ftp_path += "/";
			ui.Path->setText(Config.stats_ftp_path.c_str());
		}
	}

	//Save
	Config.Save(Config.config_path);

	QMessageBox::information(this, "Success", "FTP settings have been saved");
}

//Clears the window
void FTPExportSettings::Clear()
{
	ui.Hostname->clear();
	ui.Port->clear();
	ui.Username->clear();
	ui.Password->clear();
	ui.Path->clear();
	ui.FileName->clear();
}