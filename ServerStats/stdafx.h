#pragma once

#ifdef _WIN32
	#define snprintf _snprintf

	#include <WinSDKVer.h>
	#include <SDKDDKVer.h>

	#include "io.h"
	#include "fcntl.h"
	//#include "windows.h"
#endif

#include <stdio.h>
#include <stdint.h>
#include <string>
#include <vector>
#include <map>
#include <fstream>
#include <exception>
#include <list>
#include <set>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <iterator>

//Qt
#include <QtWidgets/QApplication>
#include <QtWidgets/QDialog>
#include <QtWidgets/QWidget>
#include <QtWidgets/QMessageBox>
#include <QtWidgets/QTableWidgetItem>
#include <QtWidgets/QSystemTrayIcon>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QtNetwork>
#include <QTimer>
#include <QElapsedTimer>
#include <QClipboard>
#include <QtWidgets/QFileDialog>
#include <QIntValidator>
#include <QtPlugin>


//Boost
#include <boost/shared_ptr.hpp>
#include <boost/make_shared.hpp>
#include <boost/tuple/tuple.hpp>
#include <boost/thread.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/archive/xml_iarchive.hpp>
#include <boost/archive/xml_oarchive.hpp>
#include <boost/serialization/string.hpp>
#include <boost/serialization/map.hpp>
#include <boost/serialization/version.hpp>
#include <boost/serialization/access.hpp>
#include <boost/archive/archive_exception.hpp>
#include <boost/archive/xml_archive_exception.hpp>
#include <boost/random.hpp>
#include <boost/random/random_device.hpp>

//GUI
#include "ui_serveredit.h"
#include "ui_serverstats.h"
#include "ui_fileexportsettings.h"
#include "ui_ftpexportsettings.h"

#include "shared/blowfish.h"
#include "shared/stream_utility.h"
#include "shared/silkroad_security.h"
#include "Config/config.h"
#include "serverstats.h"
#include "serveredit.h"
#include "ftpexportsettings.h"
#include "fileexportsettings.h"