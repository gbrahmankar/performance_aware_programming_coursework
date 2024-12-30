#pragma once

#include "../common_utils.h"

namespace PartThree
{

enum TestMode : u32
{
    TestModeUninitialized,
    TestModeTesting,
    TestModeCompleted,
    TestModeError,
};

struct RepetitionTestResults
{
    u64 m_testCount = 0;

    u64 m_maxTime = 0;
    u64 m_minTime = maxU64 - 1;
    u64 m_totalTime = 0;
};

struct RepetitionTester
{
    bool isTesting();
    void newTestWave(u64 targetProcessedByteCount, u64 cpuTimerFreq, u32 secondsToTry = 10);

    void beginTime();
    void endTime();

    void countBytes(u64 bytesToAccumulate);

    void printTime(const char* label, f64 cpuTime, u64 byteCount);
    void printResults();
    void reportError(const char* errorMessage);

    u64 m_targetProcessedByteCount = 0;
    u64 m_cpuTimerFreq = 0;
    u64 m_tryForTime = 0;
    u64 m_testsStartedAt = 0;

    TestMode m_testMode = TestModeUninitialized;
    bool m_printNewMinimums = true;
    u32 m_openBlockCount = 0;
    u32 m_closeBlockCount = 0;
    u64 m_timeAccumulatedOnThisTest = 0;
    u64 m_bytesAccumulatedOnThisTest = 0;

    RepetitionTestResults m_results;
};

}
