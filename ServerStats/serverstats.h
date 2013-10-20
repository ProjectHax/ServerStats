#pragma once

#ifndef SERVERSTATS_H
#define SERVERSTATS_H

#include "stdafx.h"

enum Server_Opcodes : uint16_t
{
	SERVER_VERSION		=		0x2001,		//Server requests Silkroad locale/version from the client
	SERVER_UPDATE_INFO	=		0x6005,		//Silkroad update info
	SERVER_SERVER_STATS	=		0xA101		//Silkroad server stats info
};

enum Client_Opcodes : uint16_t
{
	CLIENT_VERSION		=		0x6100,		//Silkroad locale/version info
	CLIENT_SERVER_STATS	=		0x6101		//Server stats request
};

//Silkroad Locales
enum SilkroadLocales : uint8_t
{
	Locale_iSRO = 18,				//International Silkroad
	Locale_SilkroadR = 65,			//SilkroadR
	Locale_kSRO = 2,				//Korean Silkroad
	Locale_rSRO = 40,				//Russian Silkroad
	Locale_cSRO = 4,				//Chinese Silkroad
	Locale_vSRO = 22,				//Vietnamese Silkroad
	Locale_jSRO = 15				//Japanese Silkroad
};

class ServerStats : public QWidget
{
	Q_OBJECT

private:

	//User interface
	Ui::ServerStatsClass ui;

private:

	//TCP socket
	QTcpSocket* socket;

	//Silkroad security
	boost::shared_ptr<SilkroadSecurity> security;

	//Tray icon
	QSystemTrayIcon* tray;

	//Packet processing timer
	QTimer* packet_timer;

	//Connect timeout timer
	QTimer* connect_timeout;

	//Connect test
	QTimer* connect_test;

	//Timer for sending server stats request packet
	QElapsedTimer stats_timer;

	//Packet injection functions
	void Inject(uint16_t opcode, StreamUtility & stream, bool encrypted) { if(security) security->Send(opcode, stream, encrypted); }
	void Inject(uint16_t opcode, bool encrypted) { if(security) security->Send(opcode, 0, 0, encrypted); }

	//Reloads server menu items
	void ReloadServers();

	//Uses Google to translate text
	std::map<QString, std::pair<QString, QString> > translations;
	QString GoogleTranslate(QString text, QString to = "en");

	//Current server
	std::pair<std::string, ServerStatsInfo> current_server;

	//Connects to a server
	void Connect();

	//Connect retry
	bool retry_5_seconds;

private slots:

	//Data received
	void readyRead();

	//Socket state changed
	void SocketState(QAbstractSocket::SocketState state);

	//Process packets
	void ProcessPackets();

	//Connection test when the socket is closed
	void ConnectTest();

	//Displays the server editor
	void Edit();

	//Menu item clicked
	void MenuBarClicked(QAction* action);

	//Connect timeout
	void ConnectTimeout();

	//Displays the FTP export GUI
	void ExportFTPSettings();

	//Displays the file export GUI
	void ExportFileSettings();

	//Uploads the server stats via FTP
	void ExportFTP(const QByteArray & data);

	//Exports the server stats via file
	void ExportFile(const QString & data);

	void iconActivated(QSystemTrayIcon::ActivationReason reason);

protected:

	void changeEvent(QEvent* event);

public:

	//Constructor
	ServerStats(QWidget *parent = 0);

	//Destructor
	~ServerStats();
};

#endif