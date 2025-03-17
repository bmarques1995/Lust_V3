#include "Instrumentator.hpp"

Lust::Instrumentator::Instrumentator()
	: m_CurrentSession(nullptr), m_ProfileCount(0)
{
}

void Lust::Instrumentator::BeginSession(const std::string& name, const std::string& filepath)
{
	m_OutputFilename = filepath;
	WriteHeader();
	m_CurrentSession = new InstrumentationSession{ name };
}

void Lust::Instrumentator::EndSession()
{
    Json::StreamWriterBuilder builder;
    const std::unique_ptr<Json::StreamWriter> writer(builder.newStreamWriter());
    std::ofstream file(m_OutputFilename);
	writer->write(m_Root, &file);
	file.close();
    m_Root.clear();

    delete m_CurrentSession;
	m_CurrentSession = nullptr;
	m_ProfileCount = 0;
}

void Lust::Instrumentator::WriteProfile(const ProfileResult& result)
{
    Json::Value element;

    element["cat"] = "function";
	element["dur"] = result.End - result.Start;
	element["name"] = result.Name;
	element["ph"] = "X";
	element["pid"] = 0;
	element["tid"] = result.ThreadID;
	element["ts"] = result.Start;

    m_Root["traceEvents "].append(element);
}

void Lust::Instrumentator::WriteHeader()
{
    m_Root["otherData"] = Json::objectValue;
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
