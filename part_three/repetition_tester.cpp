#include "repetition_tester.h"

namespace PartThree
{

void RepetitionTester::newTestWave(u64 targetProcessedByteCount, u64 cpuTimerFreq, u32 secondsToTry)
{
    if(m_testMode == TestModeUninitialized)
    {
        m_testMode = TestModeTesting;
        m_targetProcessedByteCount = targetProcessedByteCount;
        m_cpuTimerFreq = cpuTimerFreq;
        m_printNewMinimums = true;
        m_results.m_minTime = (maxU64)-1;
    }
    else if(m_testMode == TestModeCompleted)
    {
        m_testMode = TestModeTesting;

        if(m_targetProcessedByteCount != targetProcessedByteCount)
        {
            reportError("TargetProcessedByteCount changed");
        }

        if(m_cpuTimerFreq != cpuTimerFreq)
        {
            reportError("CPU frequency changed");
        }
    }

    m_tryForTime = secondsToTry * cpuTimerFreq;
    m_testsStartedAt = Profiler::ReadCPUTimer();
}

bool RepetitionTester::isTesting()
{
    if(m_testMode == TestModeTesting)
    {
        u64 currentTime = Profiler::ReadCPUTimer();

        if(m_openBlockCount == 0)
        {
            return true;
        }

        if(m_openBlockCount != m_closeBlockCount)
        {
            reportError("Unbalanced BeginTime/EndTime");
        }

        if(m_bytesAccumulatedOnThisTest != m_targetProcessedByteCount)
        {
            reportError("Processed byte count mismatch");
        }

        if(m_testMode == TestModeTesting)
        {
            u64 elapsedTime = m_timeAccumulatedOnThisTest;
            m_results.m_testCount += 1;
            m_results.m_totalTime += elapsedTime;
            if(m_results.m_maxTime < elapsedTime)
            {
                m_results.m_maxTime = elapsedTime;
            }

            if(m_results.m_minTime > elapsedTime)
            {
                m_results.m_minTime = elapsedTime;

                m_testsStartedAt = currentTime;

                if(m_printNewMinimums)
                {
                    printTime("min_time", m_results.m_minTime, m_bytesAccumulatedOnThisTest);
                }
            }

            m_openBlockCount = 0;
            m_closeBlockCount = 0;
            m_timeAccumulatedOnThisTest = 0;
            m_bytesAccumulatedOnThisTest = 0;
        }

        if((currentTime - m_testsStartedAt) > m_tryForTime)
        {
            m_testMode = TestModeCompleted;

            printResults();
        }
    }

    return m_testMode == TestModeTesting;
}

void RepetitionTester::beginTime()
{
    m_openBlockCount++;
    m_timeAccumulatedOnThisTest -= Profiler::ReadCPUTimer();
}

void RepetitionTester::endTime()
{
    m_closeBlockCount++;
    m_timeAccumulatedOnThisTest += Profiler::ReadCPUTimer();
}

void RepetitionTester::countBytes(u64 bytesToAccumulate)
{
    m_bytesAccumulatedOnThisTest += bytesToAccumulate;
}

void RepetitionTester::printTime(const char* label, f64 cpuTime, u64 byteCount)
{
    std::cout << "label=" << label << " | cpu_clicks=" << cpuTime;
    if(m_cpuTimerFreq)
    {
        f64 seconds = Profiler::secondsFromCPUTime(cpuTime, m_cpuTimerFreq);
        std::cout << " | time=" << seconds * 1000 << "ms";

        if(byteCount)
        {
            f64 gigabyte = (1024.0f * 1024.0f * 1024.0f);
            f64 bestBandwidth = byteCount / (gigabyte * seconds);
            std::cout << " | bandwidth=" << bestBandwidth << "gb/s\n";
        }
    }
}

void RepetitionTester::printResults()
{
    std::cout << "------printing_final_results_starts------" << '\n';
    printTime("min_time", (f64)m_results.m_minTime, m_targetProcessedByteCount);
    printTime("max_time", (f64)m_results.m_maxTime, m_targetProcessedByteCount);
    if(m_results.m_testCount)
    {
        printTime("average", (f64)m_results.m_totalTime / (f64)m_results.m_testCount, m_targetProcessedByteCount);
    }
    std::cout << "-------printing_final_results_ends-------" << '\n';
}

void RepetitionTester::reportError(const char* errorMessage)
{
    m_testMode = TestModeError;
    std::cerr << "error_message=" << errorMessage << '\n';
}

}
