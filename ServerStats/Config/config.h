#pragma once

#ifndef CONFIG_H
#define CONFIG_H

#include <boost/archive/xml_iarchive.hpp>
#include <boost/archive/xml_oarchive.hpp>
#include <boost/serialization/string.hpp>
#include <boost/serialization/map.hpp>
#include <boost/serialization/version.hpp>
#include <boost/serialization/access.hpp>
#include <boost/archive/archive_exception.hpp>
#include <boost/archive/xml_archive_exception.hpp>

#include <stdint.h>
#include <string>
#include <map>
#include <fstream>

struct ServerStatsInfo
{
	friend class boost::serialization::access;

	template<class Archive>
	void serialize(Archive & ar, const uint32_t version = 1)
	{
		ar & BOOST_SERIALIZATION_NVP(locale);
		ar & BOOST_SERIALIZATION_NVP(hostname);
		ar & BOOST_SERIALIZATION_NVP(port);
		ar & BOOST_SERIALIZATION_NVP(connect);
		ar & BOOST_SERIALIZATION_NVP(translate);
    }

	uint8_t locale;
	std::string hostname;
	uint16_t port;
	bool connect;
	bool translate;
};

class ServerStatsConfig
{
private:

	friend class boost::serialization::access;

	template<class Archive>
	void serialize(Archive & ar, const uint32_t version = 1)
	{
		ar & BOOST_SERIALIZATION_NVP(servers);

		ar & BOOST_SERIALIZATION_NVP(stats_ftp_server);
		ar & BOOST_SERIALIZATION_NVP(stats_ftp_port);
		ar & BOOST_SERIALIZATION_NVP(stats_ftp_username);
		ar & BOOST_SERIALIZATION_NVP(stats_ftp_password);
		ar & BOOST_SERIALIZATION_NVP(stats_ftp_path);
		ar & BOOST_SERIALIZATION_NVP(stats_ftp_file_name);
		ar & BOOST_SERIALIZATION_NVP(stats_ftp_transfer_mode);

		ar & BOOST_SERIALIZATION_NVP(stats_file);
    }

public:

	//Constructor
	ServerStatsConfig();

	//Destructor
	~ServerStatsConfig();

	//Saves settings
	void Save(const std::string & path);

	//Loads settings
	void Load(const std::string & path);
	
	//Config path
	std::string config_path;

	//Servers
	std::map<std::string, ServerStatsInfo> servers;

	//FTP
	std::string stats_ftp_server;
	uint16_t stats_ftp_port;
	std::string stats_ftp_username;
	std::string stats_ftp_password;
	std::string stats_ftp_path;
	std::string stats_ftp_file_name;
	uint8_t stats_ftp_transfer_mode;

	//File
	std::string stats_file;
};

#endif