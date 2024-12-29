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
}

bool RepetitionTester::isTesting()
{
}

void RepetitionTester::beginTime()
{
}

void RepetitionTester::endTime()
{
}

void RepetitionTester::countBytes(u64 bytesToAccumulate)
{
}

void RepetitionTester::reportError(const char* errorMessage)
{
}

}
