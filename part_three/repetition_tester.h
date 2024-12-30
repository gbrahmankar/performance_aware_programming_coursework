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
    u64 m_testCount;

    u64 m_maxTime;
    u64 m_minTime;
    u64 m_totalTime;
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

    u64 m_targetProcessedByteCount;
    u64 m_cpuTimerFreq;
    u64 m_tryForTime;
    u64 m_testsStartedAt;

    TestMode m_testMode = TestModeUninitialized;
    bool m_printNewMinimums;
    u32 m_openBlockCount;
    u32 m_closeBlockCount;
    u64 m_timeAccumulatedOnThisTest;
    u64 m_bytesAccumulatedOnThisTest;

    RepetitionTestResults m_results;
};

}
