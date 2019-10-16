#pragma once

enum LogLevel
{
	Info = 0x1,
	Warning = 0x2,
	Error = 0x4,
	FixMe = 0x8
};

class Logger
{
	//nested classes
private:
	class BaseLogger
	{
	protected:
		std::wostream* m_os;
	public:
		BaseLogger(){};
		virtual ~BaseLogger(){};
		
		virtual void Log(const std::wstring& message)
		{
			(*m_os) << message;
			m_os->flush();
		}
	};

	class FileLogger : public BaseLogger
	{
		std::wstring m_filename;
	public:
		explicit FileLogger(const std::wstring& fileName)
		: m_filename(fileName)
		{
			m_os = new std::wofstream(m_filename.c_str());
		}
		~FileLogger()
		{
			if (m_os)
			{
				std::wofstream* of = static_cast<std::wofstream*>(m_os);
				of->close();
				delete m_os;
			}
		}
	};

	class ConsoleLogger : public BaseLogger
	{
	public:
		ConsoleLogger()
		{
			m_os = &std::wcout;    
		}
	};

public:
	static void Initialize();
	static void Release();
	static int StartPerformanceTimer();
	static double StopPerformanceTimer(int timerId);

	static void Log(LogLevel level, const std::wstring& msg, bool includeTimeStamp = false);
	static void LogFormat(LogLevel level, const wchar_t* format, ...);
	static void LogInfo(const std::wstring& msg, bool includeTimeStamp = false);
	static void LogWarning(const std::wstring& msg, bool includeTimeStamp = false);
	static void LogError(const std::wstring& msg, bool includeTimeStamp = false);
	static void LogFixMe(const std::wstring& source, bool includeTimeStamp = false);
	static bool LogHResult(HRESULT hr, const std::wstring& origin, bool includeTimeStamp = false);
	static void StartFileLogging(const std::wstring& fileName);
	static void StopFileLogging();

	static void BreakOnLog(LogLevel level, bool doBreak) { doBreak? m_BreakBitField |= level : m_BreakBitField &= ~level; }

private:
	Logger(void){};
	~Logger(void){};


	static void ReleaseLoggers()
	{
		SafeDelete(m_ConsoleLogger);
		SafeDelete(m_FileLogger);
	};
	//TIMER 
	static double m_PcFreq;
	static const int MAX_PERFORMANCE_TIMERS = 10;
	static __int64 m_PerformanceTimerArr[MAX_PERFORMANCE_TIMERS];

	static HANDLE m_ConsoleHandle;

	static ConsoleLogger* m_ConsoleLogger;
	static FileLogger* m_FileLogger;
	static char m_BreakBitField;

	static wchar_t* m_ConvertBuffer;
	static const size_t m_ConvertBufferSize = 1024;

	friend class Game;
private:
	// -------------------------
	// Disabling default copy constructor and default 
	// assignment operator.
	// -------------------------
	Logger( const Logger &obj);
	Logger& operator=( const Logger& obj);
};
