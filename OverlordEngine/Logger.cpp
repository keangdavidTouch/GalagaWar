
#include "stdafx.h"

#include "Logger.h"
#include <ctime>
#include <io.h>
#include <fcntl.h>


__int64 Logger::m_PerformanceTimerArr[] = {0};
double Logger::m_PcFreq = 0.0;
HANDLE Logger::m_ConsoleHandle = nullptr;

Logger::ConsoleLogger* Logger::m_ConsoleLogger = nullptr;
Logger::FileLogger* Logger::m_FileLogger = nullptr;
char Logger::m_BreakBitField = LogLevel::Error;
wchar_t* Logger::m_ConvertBuffer = new wchar_t[m_ConvertBufferSize];

void Logger::Initialize()
{
	for(int i = 0; i<MAX_PERFORMANCE_TIMERS; ++i)
		m_PerformanceTimerArr[i] = -1;

	LARGE_INTEGER li;
	if(QueryPerformanceFrequency(&li))
	{
		m_PcFreq = double(li.QuadPart)/1000.0;
		//LOG ERROR
	}

#if defined(DEBUG) | defined(_DEBUG)
	if (AllocConsole())
	{
		// Redirect the CRT standard input, output, and error handles to the console
		FILE* pCout;
		freopen_s(&pCout, "CONIN$", "r", stdin);
		freopen_s(&pCout, "CONOUT$", "w", stdout);
		freopen_s(&pCout, "CONOUT$", "w", stderr);

		//Clear the error state for each of the C++ standard stream objects. We need to do this, as
		//attempts to access the standard streams before they refer to a valid target will cause the
		//iostream objects to enter an error state. In versions of Visual Studio after 2005, this seems
		//to always occur during startup regardless of whether anything has been read from or written to
		//the console or not.
		std::wcout.clear();
		std::cout.clear();
		std::wcerr.clear();
		std::cerr.clear();
		std::wcin.clear();
		std::cin.clear();
		std::cin.clear();

		//Set ConsoleHandle
		m_ConsoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);
		m_ConsoleLogger = new ConsoleLogger();

		//Disable Close-Button
		HWND hwnd = GetConsoleWindow();
		if (hwnd != NULL)
		{
			HMENU hMenu = GetSystemMenu(hwnd, FALSE);
			if (hMenu != NULL) DeleteMenu(hMenu, SC_CLOSE, MF_BYCOMMAND);
		}
	}
#endif
}

void Logger::Release()
{
	ReleaseLoggers();

	delete[] m_ConvertBuffer;
	m_ConvertBuffer = nullptr;
}

int Logger::StartPerformanceTimer()
{
	int counter = 0;
	while(m_PerformanceTimerArr[counter] != -1)
	{
		++counter;
		if(counter == MAX_PERFORMANCE_TIMERS)
		{
			counter = -1;
			break;
		}
	}

	if(counter>=0)
	{
		LARGE_INTEGER li;
		QueryPerformanceCounter(&li);
		m_PerformanceTimerArr[counter] = li.QuadPart;
	}

	return counter;
}

double Logger::StopPerformanceTimer(int timerId)
{
	LARGE_INTEGER li;
	QueryPerformanceCounter(&li);
	double diff = double(li.QuadPart - m_PerformanceTimerArr[timerId])/m_PcFreq;

	m_PerformanceTimerArr[timerId] = -1;

	return diff;
}

void Logger::StartFileLogging(const std::wstring& fileName)
{
	SafeDelete(m_FileLogger);

	m_FileLogger = new FileLogger(fileName);
}

void Logger::StopFileLogging()
{
	SafeDelete(m_FileLogger);
}

void Logger::LogInfo(const std::wstring& msg, bool includeTimeStamp)
{
	Log(LogLevel::Info, msg, includeTimeStamp);
}

void Logger::LogWarning(const std::wstring& msg, bool includeTimeStamp)
{
	Log(LogLevel::Warning, msg, includeTimeStamp);
}

void Logger::LogError(const std::wstring& msg, bool includeTimeStamp)
{
	Log(LogLevel::Error, msg, includeTimeStamp);
}

void Logger::LogFixMe(const std::wstring& source, bool includeTimeStamp)
{
	Log(LogLevel::FixMe, source, includeTimeStamp);
}

bool Logger::LogHResult(HRESULT hr, const std::wstring& origin, bool includeTimeStamp)
{
	if(FAILED(hr))
	{
		if(FACILITY_WINDOWS == HRESULT_FACILITY(hr)) 
			hr = HRESULT_CODE(hr); 

		std::wstringstream ss;
		if(origin.size() != 0)
		{
			ss<<">ORIGIN:\n";
			ss<<origin;
			ss<<"\n\n";
		}
		ss<<"ERROR:\n";

		TCHAR* errorMsg; 
		if(FormatMessage( 
			FORMAT_MESSAGE_ALLOCATE_BUFFER|FORMAT_MESSAGE_FROM_SYSTEM, 
			NULL, hr, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), 
			(LPTSTR)&errorMsg, 0, NULL) != 0) 
		{ 
			ss<<errorMsg;
		} 
		else
		{
			ss<<"Could not find a description for error ";
			ss<<hr;
			ss<<".";		 
		}

		Log(LogLevel::Error, ss.str(), includeTimeStamp);
		return true;
	}

	return false;
}

void Logger::LogFormat(LogLevel level, const wchar_t* format, ...)
{
	va_list ap;

	va_start(ap, format);
	_vsnwprintf_s(&m_ConvertBuffer[0], m_ConvertBufferSize, m_ConvertBufferSize, format, ap);
	va_end(ap);
	Log(level, std::wstring(&m_ConvertBuffer[0]));
}

void Logger::Log(LogLevel level, const std::wstring& msg, bool includeTimeStamp)
{
	std::wstringstream stream;

	if(includeTimeStamp)
	{
		SYSTEMTIME st;
        GetSystemTime(&st);
		stream << L"[" << st.wYear << L"-" << st.wMonth << L"-" << st.wDay << L" - ";
		stream << st.wHour << L":" << st.wMinute << L":" << st.wSecond << L":" << st.wMilliseconds << L"]";
	}

	switch(level)
	{
	case LogLevel::Info:
		SetConsoleTextAttribute(m_ConsoleHandle, FOREGROUND_INTENSITY|FOREGROUND_RED|FOREGROUND_GREEN|FOREGROUND_BLUE);
		stream<<L"[INFO]    ";
		break;
	case LogLevel::Warning:			
		SetConsoleTextAttribute(m_ConsoleHandle, FOREGROUND_INTENSITY|FOREGROUND_RED|FOREGROUND_GREEN);
		stream<<L"[WARNING] ";
		break;
	case LogLevel::Error:
		SetConsoleTextAttribute(m_ConsoleHandle, FOREGROUND_INTENSITY|FOREGROUND_RED);
		stream<<L"[ERROR]   ";
		break;
	case LogLevel::FixMe:
		SetConsoleTextAttribute(m_ConsoleHandle, FOREGROUND_INTENSITY|FOREGROUND_RED|FOREGROUND_BLUE);
		stream<<L"[FIX-ME]   ";
		break;
	}

	stream<<msg;
	stream<<L"\n";

	//Use specific loggers to log
	if(m_ConsoleLogger)
		m_ConsoleLogger->Log(stream.str());
	if(m_FileLogger)
		m_FileLogger->Log(stream.str());

	//if error, break
	if(level == LogLevel::Error)
	{
		MessageBox(0, msg.c_str(), L"ERROR" ,0);
	}

#if _DEBUG
	if((m_BreakBitField&level) == level) __asm { int 3 };
#else
	if((m_BreakBitField&level) == level) exit(-1);
#endif

}
