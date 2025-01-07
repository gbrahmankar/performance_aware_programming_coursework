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
        m_accumulatedOnThisTest.reset(RepValueDefInitMin);
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
    m_testsStartedAt = PlatformMetrics::ReadCPUTimer();
}

bool RepetitionTester::isTesting()
{
    if(m_testMode == TestModeTesting)
    {
        u64 currentTime = PlatformMetrics::ReadCPUTimer();

        if(m_openBlockCount == 0)
        {
            return true;
        }

        if(m_openBlockCount != m_closeBlockCount)
        {
            reportError("Unbalanced BeginTime/EndTime");
        }

        if(m_accumulatedOnThisTest.E[RepValueByteCount] != m_targetProcessedByteCount)
        {
            reportError("Processed byte count mismatch");
        }

        if(m_testMode == TestModeTesting)
        {
            m_results.m_total.E[RepValueTestCount] = 1;
            for(u32 index = 0; index < RepValueCount; ++index)
            {
                m_results.m_total.E[index] += m_accumulatedOnThisTest.E[index];
            }

            if(m_results.m_max.E[RepValueCPUTimer] < m_accumulatedOnThisTest.E[RepValueCPUTimer])
            {
                m_results.m_max = m_accumulatedOnThisTest;
            }

            if(m_results.m_min.E[RepValueCPUTimer] > m_accumulatedOnThisTest.E[RepValueCPUTimer])
            {
                m_results.m_min = m_accumulatedOnThisTest;

                m_testsStartedAt = currentTime;

                if(m_printNewMinimums)
                {
                    printTime("min_time", m_results.m_min);
                }
            }

            m_openBlockCount = 0;
            m_closeBlockCount = 0;
            m_accumulatedOnThisTest.reset(RepValueDefInitMin);
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

    m_accumulatedOnThisTest.E[RepValueMemPageFaults] -= PlatformMetrics::readOSPageFaultCount();
    m_accumulatedOnThisTest.E[RepValueCPUTimer] -= PlatformMetrics::ReadCPUTimer();
}

void RepetitionTester::endTime()
{
    m_accumulatedOnThisTest.E[RepValueMemPageFaults] += PlatformMetrics::readOSPageFaultCount();
    m_accumulatedOnThisTest.E[RepValueCPUTimer] += PlatformMetrics::ReadCPUTimer();

    m_closeBlockCount++;
}

void RepetitionTester::countBytes(u64 bytesToAccumulate)
{
    m_accumulatedOnThisTest.E[RepValueByteCount] += bytesToAccumulate;
}

void RepetitionTester::printTime(const char* label, RepetitionValue value)
{
    u64 testCount = value.E[RepValueTestCount];
    f64 divisor = testCount ? (f64)testCount : 1;

    f64 E[RepValueCount];
    for(u32 index = 0; index < RepValueCount; ++index)
    {
        E[index] = (f64)value.E[index] / divisor;
    }

    std::cout << "label=" << label << " | cpu_clicks=" << E[RepValueCPUTimer];
    if(m_cpuTimerFreq)
    {
        f64 seconds = PlatformMetrics::secondsFromCPUTime(E[RepValueCPUTimer], m_cpuTimerFreq);
        std::cout << " | time=" << seconds * 1000 << "ms";

        if(E[RepValueByteCount])
        {
            f64 gigabyte = (1024.0f * 1024.0f * 1024.0f);
            f64 bestBandwidth = E[RepValueByteCount] / (gigabyte * seconds);
            std::cout << " | bandwidth=" << bestBandwidth << "gb/s";
        }
    }

    std::cout << " | page_faults=" << E[RepValueMemPageFaults] << " | bytes_per_fault=" << E[RepValueByteCount] / (E[RepValueMemPageFaults] * 1024.0) << "k/fault\n";
}

void RepetitionTester::printResults()
{
    std::cout << "------printing_final_results_starts------" << '\n';
    printTime("min_time", m_results.m_min);
    printTime("max_time", m_results.m_max);
    if(m_results.m_total.E[RepValueTestCount] > 0)
    {
        printTime("average", m_results.m_total);
    }
    std::cout << "-------printing_final_results_ends-------" << '\n';
}

void RepetitionTester::reportError(const char* errorMessage)
{
    m_testMode = TestModeError;
    std::cerr << "error_message=" << errorMessage << '\n';
}

}
