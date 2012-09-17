#include "serveredit.h"

#include <QIntValidator>
#include <QMessageBox>

extern ServerStatsConfig Config;

//Constructor
ServerEdit::ServerEdit(QWidget *parent) : QDialog(parent)
{
	//Sets up the user interface
	ui.setupUi(this);

	//Integer validators
	ui.Port->setValidator(new QIntValidator(1, 65536, this));
	ui.Locale->setValidator(new QIntValidator(1, 100, this));

	//Load servers
	for(std::map<std::string, ServerStatsInfo>::iterator itr = Config.servers.begin(); itr != Config.servers.end(); ++itr)
		ui.lstServers->addItem(itr->first.c_str());
}

//Destructor
ServerEdit::~ServerEdit()
{
}

//Adds a new server
void ServerEdit::Add()
{
	bool OK = false;

	QString name = ui.CustomName->text();
	QString hostname = ui.Hostname->text();
	bool connect = ui.ConnectStartup->isChecked();
	bool translate = ui.Translate->isChecked();

	uint16_t port = ui.Port->text().toUInt(&OK, 10);
	if(!OK)
	{
		QMessageBox::warning(this, "Error", "Could not convert the port to a decimal.");
		return;
	}

	uint8_t locale = ui.Locale->text().toUInt(&OK, 10);
	if(!OK)
	{
		QMessageBox::warning(this, "Error", "Could not convert the locale to a decimal.");
		return;
	}

	if(name.isEmpty())
	{
		QMessageBox::warning(this, "Error", "You must input a name for this server that you are trying to add.");
	}
	else if(hostname.isEmpty())
	{
		QMessageBox::warning(this, "Error", "Hostname/IP is missing.");
	}
	else if(port == 0)
	{
		QMessageBox::warning(this, "Error", "Port is missing.");
	}
	else if(locale == 0)
	{
		QMessageBox::warning(this, "Error", "Locale is missing.");
	}
	else
	{
		OK = false;

		//See if the server name exists already
		std::map<std::string, ServerStatsInfo>::iterator itr = Config.servers.find(name.toAscii().data());
		if(itr != Config.servers.end())
			OK = true;

		//No other servers will be able to connect on start up
		if(connect)
		{
			for(std::map<std::string, ServerStatsInfo>::iterator itr = Config.servers.begin(); itr != Config.servers.end(); ++itr)
			{
				itr->second.connect = false;
			}
		}

		//Add the new server to the map
		ServerStatsInfo info;
		info.locale = locale;
		info.hostname = hostname.toAscii().data();
		info.port = port;
		info.connect = connect;
		info.translate = translate;
		Config.servers[name.toAscii().data()] = info;

		if(OK)
		{
			QMessageBox::information(this, "Success", "The server has been overwritten!");
		}
		else
		{
			ui.lstServers->addItem(name);
			QMessageBox::information(this, "Success", "The server has been added!");
		}
	}
}

//Removes a server
void ServerEdit::Remove()
{
	//Get the selected item
	QListWidgetItem* item = ui.lstServers->currentItem();

	//Valid pointer check
	if(item)
	{
		//Remove server from map
		std::map<std::string, ServerStatsInfo>::iterator itr = Config.servers.find(item->text().toAscii().data());
		if(itr != Config.servers.end())
			Config.servers.erase(itr);

		//Remove item from the list widget
		delete item;
	}
}

//Locale changed
void ServerEdit::LocaleChanged(QString text)
{
	if(text == "iSRO")
	{
		ui.Locale->setText("18");
		ui.Hostname->setText("gwgt1.joymax.com");
	}
	else if(text == "SilkroadR")
	{
		ui.Locale->setText("65");
		ui.Hostname->setText("gateway.silkroad-r.joymax.com");
	}
	else if(text == "vSRO")
	{
		ui.Locale->setText("22");
		ui.Hostname->clear();
	}
	else if(text == "rSRO")
	{
		ui.Locale->setText("40");
		ui.Hostname->setText("212.24.57.34");
	}
	else if(text == "tSRO")
	{
		ui.Locale->setText("12");
		ui.Hostname->clear();
	}
	else if(text == "kSRO")
	{
		ui.Locale->setText("2");
		ui.Hostname->setText("gwgt1.silkroadonline.co.kr");
	}
	else if(text == "cSRO 1")
	{
		ui.Locale->setText("4");
		ui.Hostname->setText("shlogin1.srocn.com");
	}
	else if(text == "cSRO 2")
	{
		ui.Locale->setText("4");
		ui.Hostname->setText("bjlogin1.srocn.com");
	}
	else if(text == "jSRO")
	{
		ui.Locale->setText("15");
		ui.Hostname->clear();
	}
	else if(text == "thSRO")
	{
		ui.Locale->setText("38");
		ui.Hostname->clear();
	}
	else if(text == "Custom")
	{
		ui.Locale->clear();
		ui.Locale->setEnabled(true);
	}

	if(text != "Custom")
	{
		ui.Locale->setEnabled(false);
		ui.Port->setText("15779");
	}
}

//Server was clicked
void ServerEdit::ServerClicked(QListWidgetItem* item)
{
	if(item)
	{
		//Find the server in the map
		std::map<std::string, ServerStatsInfo>::iterator itr = Config.servers.find(item->text().toAscii().data());
		if(itr != Config.servers.end())
		{
			ui.CustomName->setText(item->text());
			ui.Locales->setCurrentIndex(ui.Locales->count() - 1);
			ui.Locale->setText(QString("%0").arg(static_cast<int>(itr->second.locale)));
			ui.Hostname->setText(itr->second.hostname.c_str());
			ui.Port->setText(QString("%0").arg(static_cast<int>(itr->second.port)));
			ui.ConnectStartup->setChecked(itr->second.connect);
			ui.Translate->setChecked(itr->second.translate);
		}
	}
}