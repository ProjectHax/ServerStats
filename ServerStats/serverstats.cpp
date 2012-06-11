#include "serverstats.h"

#include <QTableWidgetItem>

#include <vector>
#include <string>

#include <boost/tuple/tuple.hpp>

//Constructor
ServerStats::ServerStats(QWidget *parent, Qt::WFlags flags) : QWidget(parent, flags)
{
	//Sets up the user interface
	ui.setupUi(this);

	//Invalidate the timer so no packets will be sent before the first server stats packet is received
	stats_timer.invalidate();

	//Connect menu items
	connect(ui.actionExit, SIGNAL(triggered()), this, SLOT(close()));

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

	//Connect timer
	packet_timer = new QTimer(this);
	connect(packet_timer, SIGNAL(timeout()), this, SLOT(ProcessPackets()));
	packet_timer->start(50);	//50 milliseconds

	//Connect
	socket->connectToHost("gwgt1.joymax.com", 15779);
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
		//New security api
		security = boost::make_shared<SilkroadSecurity>();
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
			StreamUtility & r = container.data;

			//Packet processing
			switch(container.opcode)
			{
				//Server requests locale/client version
				case SERVER_VERSION:
				{
					StreamUtility w;
					w.Write<uint8_t>(18);			//Locale
					w.Write<uint16_t>(9);			//Length of 'SR_Client'
					w.Write_Ascii("SR_Client");
					w.Write<uint32_t>(123);			//Version
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
					size_t num_count = 0;
					std::vector<boost::tuple<QString, QString, int, int> > servers;

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

						//Server ratio or current/max
						float ratio = r.Read<float>();
						int current = static_cast<int>(3500.0f * ratio);
						int max = 3500;

						//Server state (open or closed)
						QString state = (r.Read<uint8_t>() == 1 ? "Open" : "Closed");

						//Add server to list
						servers.push_back(boost::tuple<QString, QString, int, int>(name.c_str(), state, current, max));

						//Next
						entry = r.Read<uint8_t>();
					}

					ui.tableServerStats->setRowCount(servers.size());
					for(size_t x = 0; x < servers.size(); ++x)
					{
						//Add server to table
						ui.tableServerStats->setItem(static_cast<int>(x), 0, new QTableWidgetItem(num_count == servers.size() ? servers[x].get<0>().mid(1) : servers[x].get<0>()));
						ui.tableServerStats->setItem(static_cast<int>(x), 1, new QTableWidgetItem(servers[x].get<1>()));
						ui.tableServerStats->setItem(static_cast<int>(x), 2, new QTableWidgetItem(QString("%0").arg(servers[x].get<2>())));
						ui.tableServerStats->setItem(static_cast<int>(x), 3, new QTableWidgetItem(QString("%0").arg(servers[x].get<3>())));
					}

					//Restart timer
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

			//Restart timer
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