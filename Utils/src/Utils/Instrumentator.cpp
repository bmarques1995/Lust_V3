#include "Instrumentator.hpp"

Lust::Instrumentator::Instrumentator()
	: m_CurrentSession(nullptr), m_ProfileCount(0)
{
}

void Lust::Instrumentator::BeginSession(const std::string& name, const std::string& filepath)
{
	m_OutputStream.open(filepath);
	WriteHeader();
	m_CurrentSession = new InstrumentationSession{ name };
}

void Lust::Instrumentator::EndSession()
{
	WriteFooter();
	m_OutputStream.close();
	delete m_CurrentSession;
	m_CurrentSession = nullptr;
	m_ProfileCount = 0;
}

void Lust::Instrumentator::WriteProfile(const ProfileResult& result)
{
    if (m_ProfileCount++ > 0)
        m_OutputStream << ",";
    
    std::string name = result.Name;
    std::replace(name.begin(), name.end(), '"', '\'');

    m_OutputStream << "{";
    m_OutputStream << "\"cat\":\"function\",";
    m_OutputStream << "\"dur\":" << (result.End - result.Start) << ',';
    m_OutputStream << "\"name\":\"" << name << "\",";
    m_OutputStream << "\"ph\":\"X\",";
    m_OutputStream << "\"pid\":0,";
    m_OutputStream << "\"tid\":" << result.ThreadID << ",";
    m_OutputStream << "\"ts\":" << result.Start;
    m_OutputStream << "}";

    m_OutputStream.flush();
}

void Lust::Instrumentator::WriteHeader()
{
    m_OutputStream << "{\"otherData\": {},\"traceEvents\":[";
    m_OutputStream.flush();
}

void Lust::Instrumentator::WriteFooter()
{
    m_OutputStream << "]}";
    m_OutputStream.flush();
}

[[nodiscard]]
Lust::Instrumentator* Lust::Instrumentator::Get()
{
	// TODO: inserir instrução return aqui
    static Instrumentator instance;
    return &instance;
}

Lust::InstrumentationTimer::InstrumentationTimer(const char* name)
    : m_Name(name), m_Stopped(false)
{
    m_StartTimepoint = std::chrono::high_resolution_clock::now();
}

Lust::InstrumentationTimer::~InstrumentationTimer()
{
    if (!m_Stopped)
        Stop();
}

void Lust::InstrumentationTimer::Stop()
{
    auto endTimepoint = std::chrono::high_resolution_clock::now();

    long long start = std::chrono::time_point_cast<std::chrono::microseconds>(m_StartTimepoint).time_since_epoch().count();
    long long end = std::chrono::time_point_cast<std::chrono::microseconds>(endTimepoint).time_since_epoch().count();

    uint32_t threadID = std::hash<std::thread::id>{}(std::this_thread::get_id());
    Instrumentator::Get()->WriteProfile({ m_Name, start, end, threadID });

    m_Stopped = true;
}
