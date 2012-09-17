#include "serverstats.h"

#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QFtp>
#include <QErrorMessage>

ServerStatsConfig Config;

//Constructor
ServerStats::ServerStats(QWidget *parent, Qt::WFlags flags) : QWidget(parent, flags), retry_5_seconds(false)
{
	//Sets up the user interface
	ui.setupUi(this);

	//Invalidate the timer so no packets will be sent before the first server stats packet is received
	stats_timer.invalidate();

	//Connect menu items
	connect(ui.actionExit, SIGNAL(triggered()), this, SLOT(close()));
	connect(ui.actionEdit, SIGNAL(triggered()), this, SLOT(Edit()));
	connect(ui.actionFTPSettings, SIGNAL(triggered()), this, SLOT(ExportFTPSettings()));
	connect(ui.actionFile_Settings, SIGNAL(triggered()), this, SLOT(ExportFileSettings()));
	
	//Resize tabs
	ui.tableServerStats->horizontalHeader()->setUpdatesEnabled(false);
	ui.tableServerStats->horizontalHeader()->resizeSection(0, 100);	//Server
    ui.tableServerStats->horizontalHeader()->resizeSection(1, 50);	//State
    ui.tableServerStats->horizontalHeader()->resizeSection(2, 50);	//Current
    ui.tableServerStats->horizontalHeader()->resizeSection(3, 50);	//Max
	ui.tableServerStats->horizontalHeader()->setUpdatesEnabled(true);
	ui.tableServerStats->verticalHeader()->setVisible(false);

	//New socket
	socket = new QTcpSocket(this);

	//Setup the connection slots
    connect(socket, SIGNAL(stateChanged(QAbstractSocket::SocketState)), this, SLOT(SocketState(QAbstractSocket::SocketState)));
	connect(socket, SIGNAL(readyRead()), this, SLOT(readyRead()));

	//Connect the timers
	packet_timer = new QTimer(this);
	connect(packet_timer, SIGNAL(timeout()), this, SLOT(ProcessPackets()));
	packet_timer->start(50);	//50 milliseconds

	connect_timeout = new QTimer(this);
	connect(connect_timeout, SIGNAL(timeout()), this, SLOT(ConnectTimeout()));

	connect_test = new QTimer(this);
	connect(connect_test, SIGNAL(timeout()), this, SLOT(ConnectTest()));

	//Load the config
	QString path = QCoreApplication::applicationFilePath();
	path = path.mid(0, path.lastIndexOf("/")) + "/config.xml";
	Config.Load(path.toAscii().data());

	//Find the server that will connect on start up
	for(std::map<std::string, ServerStatsInfo>::iterator itr = Config.servers.begin(); itr != Config.servers.end(); ++itr)
	{
		if(itr->second.connect)
		{
			//Copy the server info for later use
			current_server.second.hostname = itr->second.hostname;
			current_server.second.port = itr->second.port;
			current_server.second.locale = itr->second.locale;
			current_server.second.translate = itr->second.translate;

			Connect();
			break;
		}
	}

	//Load menu items
	ReloadServers();
}

//Destructor
ServerStats::~ServerStats()
{
	//Cleanup
	socket->close();
	packet_timer->stop();
	delete socket;
	delete packet_timer;
}

//Data received
void ServerStats::readyRead()
{
	//Read data
	std::vector<uint8_t> stream;
	stream.resize(socket->bytesAvailable());
	stream.resize(socket->read(reinterpret_cast<char*>(&stream[0]), socket->bytesAvailable()));

	//Receive bytes into the security api
	security->Recv(stream);
}

//Socket state changed
void ServerStats::SocketState(QAbstractSocket::SocketState state)
{
	if(state == QAbstractSocket::ConnectedState)
	{
		//Connected so stop the connect timeout timer
		connect_timeout->stop();

		//New security api
		security = boost::make_shared<SilkroadSecurity>();
	}
	else if(state == QAbstractSocket::ClosingState)
	{
		//Clear server stats table
		ui.tableServerStats->setRowCount(0);

		//Start the connect test timer
		connect_test->start(5000);
	}
}

//Connects to a server
void ServerStats::Connect()
{
	//Invalidate the timer so no packets will be sent before the first server stats packet is received
	stats_timer.invalidate();

	//Connect
	connect_timeout->start(5000);
	socket->connectToHost(current_server.second.hostname.c_str(), current_server.second.port);

	//Clear server stats table
	ui.tableServerStats->setRowCount(0);
}

//Connection test when the socket is closed
void ServerStats::ConnectTest()
{
	//Stop the timer
	connect_test->stop();

	if(socket->state() == QAbstractSocket::UnconnectedState)
	{
		//Reconnect
		Connect();
	}
}

void ServerStats::ProcessPackets()
{
	if(socket && security)
	{
		//Receiving data
		while(security->HasPacketToRecv())
		{
			PacketContainer container = security->GetPacketToRecv();

			//Packet processing
			switch(container.opcode)
			{
				//Server requests locale/client version
				case SERVER_VERSION:
				{
					StreamUtility w;
					w.Write<uint8_t>(current_server.second.locale);	//Locale
					w.Write<uint16_t>(9);							//Length of 'SR_Client'
					w.Write_Ascii("SR_Client");
					w.Write<uint32_t>(123);							//Version

					Inject(0x6100, w, true);
				}break;
				//Update info
				case SERVER_UPDATE_INFO:
				{
					Inject(CLIENT_SERVER_STATS, true);
				}break;
				//Server stats
				case SERVER_SERVER_STATS:
				{
					StreamUtility & r = container.data;
					const uint8_t & locale = current_server.second.locale;

					size_t num_count = 0;
					std::vector<boost::tuple<std::string, QString, int, int> > servers;

					uint8_t entry = r.Read<uint8_t>();
					while(entry == 1)
					{
						r.SeekRead(1, Seek_Forward);

						//Name
						r.SeekRead(r.Read<uint16_t>(), Seek_Forward);

						//Next
						entry = r.Read<uint8_t>();
					}

					entry = r.Read<uint8_t>();
					while(entry)
					{
						//Server ID
						uint16_t id = r.Read<uint16_t>();

						//Server name
						std::string name = r.Read_Ascii(r.Read<uint16_t>());

						//Country flag check (if each server has a number in front of it then it's iSRO/SilkroadR)
						if(name.find_first_of("0123456789.") == 0)
							num_count++;
						
						int current = 0;
						int max = 0;

						//iSRO / SilkroadR
						if(locale == 18 || locale == 65)
						{
							float ratio = r.Read<float>();
							current = static_cast<int>(3500.0f * ratio);
							max = 3500;
						}
						//Everything else
						else
						{
							current = static_cast<int>(r.Read<uint16_t>());
							max = static_cast<int>(r.Read<uint16_t>());
						}

						//Server state (open or closed)
						QString state = (r.Read<uint8_t>() == 1 ? "Open" : "Closed");

						if(locale == 4 || locale == 22)
							r.Read<uint8_t>();

						//Add server to list
						servers.push_back(boost::tuple<std::string, QString, int, int>(name, state, current, max));

						//Next
						entry = r.Read<uint8_t>();
					}

					//Set the number of rows
					ui.tableServerStats->setRowCount(servers.size());

					//Disable sorting so the rows don't get messed up when updating the servers
					ui.tableServerStats->setSortingEnabled(false);

					QTextCodec* codec = 0;

					if(locale == 2)			//kSRO
						codec = QTextCodec::codecForName("EUC-KR");
					else if(locale == 4)	//cSRO
						codec = QTextCodec::codecForName("GB18030");
					else if(locale == 15)	//jSRO
						codec = QTextCodec::codecForName("EUC-JP");
					else if(locale == 40)	//rSRO
						codec = QTextCodec::codecForName("Windows-1251");

					//HTML server stats
					QString html = "<table border=\"0\">\n";
					html += "<tr>\n";
						html += "<td style=\"border-bottom-style:solid; border-bottom-width:1px;\"><b>Name</b></td>\n";
						html += "<td style=\"border-bottom-style:solid; border-bottom-width:1px;\"><b>Current</b></td>\n";
						html += "<td style=\"border-bottom-style:solid; border-bottom-width:1px;\"><b>Max</b></td>\n";
						html += "<td style=\"border-bottom-style:solid; border-bottom-width:1px;\"><b>Status</b></td>\n";
					html += "</tr>\n";

					for(int x = 0; x < static_cast<int>(servers.size()); ++x)
					{
						QString name;

						//See if the codec was found
						if(codec)
							name = codec->toUnicode(servers[x].get<0>().c_str());
						else
							name = servers[x].get<0>().c_str();

						if(num_count == servers.size())
							name = name.mid(1);

						if(current_server.second.translate)
							name = GoogleTranslate(name);

						html += "<tr>\n";
							html += QString("<td>%0</td>\n").arg(name);
							html += QString("<td>%0</td>\n").arg(servers[x].get<2>());
							html += QString("<td>%0</td>\n").arg(servers[x].get<3>());
							html += QString("<td>%0</td>\n").arg(servers[x].get<1>());
						html += "</tr>\n";

						//Server name
						ui.tableServerStats->setItem(x, 0, new QTableWidgetItem(name));

						//Server state
						ui.tableServerStats->setItem(x, 1, new QTableWidgetItem(servers[x].get<1>()));

						//Current
						QTableWidgetItem* item = new QTableWidgetItem;
						item->setData(Qt::DisplayRole, servers[x].get<2>());
						ui.tableServerStats->setItem(x, 2, item);

						//Max
						item = new QTableWidgetItem;
						item->setData(Qt::DisplayRole, servers[x].get<3>());
						ui.tableServerStats->setItem(x, 3, item);
					}

					html += "</table>";

					//Enable sorting
					ui.tableServerStats->setSortingEnabled(true);

					//Upload stats via FTP
					ExportFTP(QByteArray(html.toAscii().data()));

					//Export the stats via file
					ExportFile(html);

					//Restart the stats timer
					stats_timer.restart();
				}break;
				default:
				{
				}
			}
		}

		//Make sure the timer is valid and has expired before injecting another server stats request
		if(stats_timer.isValid() && stats_timer.hasExpired(5000))
		{
			//Request servers stats
			Inject(CLIENT_SERVER_STATS, true);

			//Restart the stats timer
			stats_timer.restart();
		}

		//Sending data
		while(security->HasPacketToSend())
		{
			//Send packet
			std::vector<uint8_t> packet = security->GetPacketToSend();
			socket->write(reinterpret_cast<char*>(&packet[0]), packet.size());
		}
	}
}

//Displays the server editor
void ServerStats::Edit()
{
	//Display the server editor window
	ServerEdit edit(this);
	edit.exec();

	//Reload menu items
	ReloadServers();

	//Save the changes
	Config.Save(Config.config_path);
}

//Menu item clicked
void ServerStats::MenuBarClicked(QAction* action)
{
	QString text = action->text();
	if(text != "File" && text != "Servers" && text != "Exit" && text != "Edit" && text != "Export" && text != "FTP Settings" && text != "File Settings")
	{
		//Locate server
		std::map<std::string, ServerStatsInfo>::iterator itr = Config.servers.find(text.toAscii().data());

		//Not found
		if(itr == Config.servers.end())
		{
			QMessageBox::critical(this, "Error", QString("Could not load the server settings for '%0'").arg(text));
		}
		else
		{
			//Disconnect first if there is an active connection
			if(socket->state() != QAbstractSocket::UnconnectedState)
				socket->close();

			//Copy the server info for later use
			current_server.second.hostname = itr->second.hostname;
			current_server.second.port = itr->second.port;
			current_server.second.locale = itr->second.locale;
			current_server.second.translate = itr->second.translate;

			Connect();
		}
	}
}

//Reloads server menu items
void ServerStats::ReloadServers()
{
	//Remove menu options
	QList<QAction*> actions = ui.menuServers->actions();
	for(int x = 0; x < actions.size(); ++x)
	{
		if(actions[x]->text() != "Edit")
			delete actions[x];
	}

	ui.menuServers->addSeparator();

	//Reload servers
	for(std::map<std::string, ServerStatsInfo>::iterator itr = Config.servers.begin(); itr != Config.servers.end(); ++itr)
	{
		QAction* action = new QAction(ui.menuServers);
		action->setText(itr->first.c_str());
		ui.menuServers->addAction(action);
	}
}

//Uses Google to translate text
QString ServerStats::GoogleTranslate(QString text, QString to)
{
	//Locate the translation in the map
	std::map<QString, std::pair<QString, QString> >::iterator itr = translations.find(text);
	if(itr != translations.end())
	{
		if(itr->second.first == to)
			return itr->second.second;
	}

	//Translate URL
	QString url = QString("http://translate.google.com/translate_a/t?client=t&text=%0&hl=%1&sl=auto&tl=%1&multires=1&prev=enter&oc=2&ssel=0&tsel=0&uptl=%1&sc=1").arg(text).arg(to);

	QNetworkAccessManager manager;
	QNetworkRequest request(url);
	QNetworkReply* reply = manager.get(request);

	//Get reply from Google
	do
	{
		QCoreApplication::processEvents();
	} while (!reply->isFinished());

	//Convert to string
	QString translation(reply->readAll());
	reply->close();

	//Free memory
	delete reply;

	//Remove [[[" from the beginning
	translation = translation.replace("[[[\"", "");

	//Extract final translated string
	translation = translation.mid(0, translation.indexOf(",\"") - 1);

	//Add the translation to the map so we don't need to make another web request for a translation
	translations[text] = std::pair<QString, QString>(to, translation);

	return translation;
}

//Connect timeout
void ServerStats::ConnectTimeout()
{
	//Stop the timer
	connect_timeout->stop();

	if(socket->state() != QAbstractSocket::ConnectedState)
	{
		//Display server is offline message
		QString offline = "<b>Server is offline</b>";
		ExportFTP(offline.toAscii().data());
		ExportFile(offline);

		//Disconnect first if there is an active connection
		if(socket->state() != QAbstractSocket::UnconnectedState)
			socket->close();

		//Don't display an error message if an FTP server or file path has been configured
		if((!retry_5_seconds) && (Config.stats_ftp_server.empty() || Config.stats_ftp_port != 0 || Config.stats_ftp_username.empty() || Config.stats_ftp_path.empty()) && (Config.stats_file.empty()))
		{
			if(QMessageBox::warning(this, "Connect Error", QString("The Silkroad server [%0:%1] appears to be offline.\nDo you wish to retry connecting every 5 seconds?").arg(current_server.second.hostname.c_str()).arg(current_server.second.port), QMessageBox::Yes, QMessageBox::No) == QMessageBox::Yes)
			{
				retry_5_seconds = true;
				Connect();
			}
		}
		else
			Connect();
	}
}

//Displays the FTP export GUI
void ServerStats::ExportFTPSettings()
{
	FTPExportSettings ftpsettings(this);
	ftpsettings.exec();
}

//Displays the file export GUI
void ServerStats::ExportFileSettings()
{
	FileExportSettings filesettings(this);
	filesettings.exec();
}

//Uploads the server stats via FTP
void ServerStats::ExportFTP(const QByteArray & data)
{
	//Make sure the FTP settings are set correctly before attempting to upload the stats info
	if(!Config.stats_ftp_server.empty() && Config.stats_ftp_port != 0 && !Config.stats_ftp_username.empty() && !Config.stats_ftp_path.empty())
	{
		QFtp ftp(this);
		ftp.setTransferMode((QFtp::TransferMode)Config.stats_ftp_transfer_mode);
		ftp.connectToHost(Config.stats_ftp_server.c_str(), Config.stats_ftp_port);
		ftp.login(Config.stats_ftp_username.c_str(), Config.stats_ftp_password.c_str());

		if(ftp.error() == QFtp::NoError)
		{
			//Delete the file
			/*ftp.remove(QString("%0%1").arg(Config.stats_ftp_path.c_str()).arg(Config.stats_ftp_file_name.c_str()));
			while((ftp.hasPendingCommands() || ftp.currentCommand() != QFtp::None) && ftp.error() == QFtp::NoError)
			{
				//Process FTP events
				QApplication::processEvents();

				//Prevent high CPU usage
				boost::this_thread::sleep(boost::posix_time::milliseconds(1));
			}*/

			//Upload the server stats
			ftp.put(data, QString("%0%1").arg(Config.stats_ftp_path.c_str()).arg(Config.stats_ftp_file_name.c_str()), QFtp::Binary);
			while((ftp.hasPendingCommands() || ftp.currentCommand() != QFtp::None) && (ftp.error() == QFtp::NoError || ftp.error() == QFtp::UnknownError))
			{
				//Process FTP events
				QApplication::processEvents();

				//Prevent high CPU usage
				boost::this_thread::sleep(boost::posix_time::milliseconds(1));
			}

			if(ftp.error() != QFtp::NoError && ftp.error() != QFtp::UnknownError)
				QMessageBox::critical(this, QString("FTP Error - %0:%1").arg(Config.stats_ftp_server.c_str()).arg(Config.stats_ftp_port), ftp.errorString());
		}

		//Close the FTP connection
		ftp.close();
	}
}

//Exports the server stats via file
void ServerStats::ExportFile(const QString & data)
{
	if(!Config.stats_file.empty())
	{
		//Open the file for writing
		QFile file(Config.stats_file.c_str());
		if(file.open(QIODevice::WriteOnly | QIODevice::Text))
		{
			//Write the HTML
			QTextStream out(&file);
			out << data;

			//Close
			out.flush();
			file.close();
		}
	}
}