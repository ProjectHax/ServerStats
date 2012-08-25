#include "config.h"

//Constructor
ServerStatsConfig::ServerStatsConfig()
{
	stats_ftp_port = 21;
	stats_ftp_username = "anonymous";
	stats_ftp_path = "/";
	stats_ftp_transfer_mode = 0;
}

//Destructor
ServerStatsConfig::~ServerStatsConfig()
{
}

//Saves settings
void ServerStatsConfig::Save(const std::string & path)
{
	std::ofstream ofs(path.c_str(), std::ios::out | std::ios::binary);
	config_path = path;

	if(ofs.is_open())
	{
		try
		{
			boost::archive::xml_oarchive oa(ofs);
			oa << boost::serialization::make_nvp("ServerStats", *this);
		}
		catch(boost::archive::archive_exception & e)
		{
			std::cout << e.what() << std::endl;
		}

		//Close the file
		ofs.close();
	}
}

//Loads settings
void ServerStatsConfig::Load(const std::string & path)
{
	//Open the XML file
	std::ifstream ifs(path.c_str(), std::ios::in | std::ios::binary);
	config_path = path;

	//Open check
	if(ifs.is_open())
	{
		try
		{
			//Load the settings
			boost::archive::xml_iarchive ia(ifs);
			ia >> boost::serialization::make_nvp("ServerStats", *this);
		}
		catch(boost::archive::archive_exception & e)
		{
			std::cout << e.what() << std::endl;
		}

		//Close the file
		ifs.close();
	}
	else
	{
		//Problem opening the file so create it
		Save(path);
	}
}