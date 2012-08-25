#include "fileexportsettings.h"

#include "Config/config.h"
#include <QMessageBox>
#include <QFileDialog>

extern ServerStatsConfig Config;

//Constructor
FileExportSettings::FileExportSettings(QWidget *parent) : QDialog(parent)
{
	ui.setupUi(this);
	ui.Path->setText(Config.stats_file.c_str());
}

//Destructor
FileExportSettings::~FileExportSettings()
{
}

//Saves the file info
void FileExportSettings::Save()
{
	//Copy the path
	Config.stats_file = ui.Path->text().toAscii().data();

	Config.Save(Config.config_path);
	QMessageBox::information(this, "Success", "File settings have been saved");
}

//Clears the window
void FileExportSettings::Clear()
{
	ui.Path->clear();
}

//Sets the file save path
void FileExportSettings::Open()
{
	ui.Path->setText(QFileDialog::getSaveFileName(this, "Server Stats Save Path", QDir::currentPath(), "HTML Document (*.html)"));
}