#pragma once

#ifndef SERVERSTATS_H
#define SERVERSTATS_H

#include <QtGui/QWidget>
#include "ui_serverstats.h"
#include "serveredit.h"

#include <stdint.h>
#include <string>
#include <vector>

#include <QtNetwork>
#include <QTimer>
#include <QElapsedTimer>
#include <QTableWidgetItem>
#include <QMessageBox>
#include <QClipboard>

#include <boost/shared_ptr.hpp>
#include <boost/make_shared.hpp>
#include <boost/tuple/tuple.hpp>

#include "shared/silkroad_security.h"
#include "Config/config.h"

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

	//Packet processing timer
	QTimer* packet_timer;

	//Timer for sending server stats request packet
	QElapsedTimer stats_timer;

	//Selected server locale
	uint8_t server_locale;

	//Packet injection functions
	void Inject(uint16_t opcode, StreamUtility & stream, bool encrypted) { if(security) security->Send(opcode, stream, encrypted); }
	void Inject(uint16_t opcode, bool encrypted) { if(security) security->Send(opcode, 0, 0, encrypted); }

	//Reloads server menu items
	void ReloadServers();

private slots:
		
	//Data received
	void readyRead();

	//Socket state changed
	void SocketState(QAbstractSocket::SocketState state);

	//Process packets
	void ProcessPackets();

	//Displays the server editor
	void Edit();

	//Menu item clicked
	void MenuBarClicked(QAction* action);

public:

	//Constructor
	ServerStats(QWidget *parent = 0, Qt::WFlags flags = 0);

	//Destructor
	~ServerStats();
};

#endif