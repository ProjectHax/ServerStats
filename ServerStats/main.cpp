#include "stdafx.h"

#ifdef _WIN32
	Q_IMPORT_PLUGIN(QWindowsIntegrationPlugin);
#endif

#if _WIN32 && _DEBUG
void CreateConsole(const char *winTitle)
{
	//http://www.gamedev.net/community/forums/viewreply.asp?ID=1958358
	int hConHandle = 0;
	HANDLE lStdHandle = 0;
	FILE *fp = 0 ;

	AllocConsole();
	if(winTitle)
		SetConsoleTitleA(winTitle);

	// redirect unbuffered STDOUT to the console
	lStdHandle = GetStdHandle(STD_OUTPUT_HANDLE);
	hConHandle = _open_osfhandle(PtrToUlong(lStdHandle), _O_TEXT);
	fp = _fdopen(hConHandle, "w");
	*stdout = *fp;
	setvbuf(stdout, NULL, _IONBF, 0);

	// redirect unbuffered STDIN to the console
	lStdHandle = GetStdHandle(STD_INPUT_HANDLE);
	hConHandle = _open_osfhandle(PtrToUlong(lStdHandle), _O_TEXT);
	fp = _fdopen(hConHandle, "r");
	*stdin = *fp;
	setvbuf(stdin, NULL, _IONBF, 0);

	// redirect unbuffered STDERR to the console
	lStdHandle = GetStdHandle(STD_ERROR_HANDLE);
	hConHandle = _open_osfhandle(PtrToUlong(lStdHandle), _O_TEXT);
	fp = _fdopen(hConHandle, "w");
	*stderr = *fp;
	setvbuf(stderr, NULL, _IONBF, 0);
}
#endif

int main(int argc, char *argv[])
{
#if _WIN32 && _DEBUG
	CreateConsole(0);
#endif

	QApplication a(argc, argv);
	ServerStats w;
	w.show();
	return a.exec();
}