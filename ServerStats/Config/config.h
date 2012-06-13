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
    }

	uint8_t locale;
	std::string hostname;
	uint16_t port;
	bool connect;
};

class ServerStatsConfig
{
private:

	friend class boost::serialization::access;

	template<class Archive>
	void serialize(Archive & ar, const uint32_t version = 1)
	{
		ar & BOOST_SERIALIZATION_NVP(servers);
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
	std::string configpath;

	//Servers
	std::map<std::string, ServerStatsInfo> servers;
};

#endif