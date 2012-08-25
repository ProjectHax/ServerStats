#include "ftpexportsettings.h"
#include "Config/config.h"

#include <boost/lexical_cast.hpp>
#include <QMessageBox>
#include <QFtp>
#include <QIntValidator>

extern ServerStatsConfig Config;

//Constructor
FTPExportSettings::FTPExportSettings(QWidget *parent) : QDialog(parent)
{
	ui.setupUi(this);

	QIntValidator* validator = new QIntValidator(1, 65535, this);
	ui.Port->setValidator(validator);

	//Load the server info from the config
	ui.Hostname->setText(Config.stats_ftp_server.c_str());
	ui.Port->setText(QString("%0").arg(Config.stats_ftp_port));
	ui.Username->setText(Config.stats_ftp_username.c_str());
	ui.Password->setText(Config.stats_ftp_password.c_str());
	ui.Path->setText(Config.stats_ftp_path.c_str());
	ui.FileName->setText(Config.stats_ftp_file_name.c_str());

	if(Config.stats_ftp_transfer_mode == QFtp::Passive)
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
	Config.stats_ftp_server = ui.Hostname->text().toAscii().data();
	Config.stats_ftp_port = ui.Port->text().length() ? boost::lexical_cast<uint16_t>(ui.Port->text().toAscii().data()) : 0;
	Config.stats_ftp_username = ui.Username->text().toAscii().data();
	Config.stats_ftp_password = ui.Password->text().toAscii().data();
	Config.stats_ftp_path = ui.Path->text().toAscii().data();
	Config.stats_ftp_file_name = ui.FileName->text().toAscii().data();

	if(ui.TransferMode->currentIndex() == 0)
		Config.stats_ftp_transfer_mode = QFtp::Passive;
	else
		Config.stats_ftp_transfer_mode = QFtp::Active;

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