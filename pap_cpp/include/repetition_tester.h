#pragma once

#include "include/common_utils.h"

namespace PartThree
{

enum TestMode : u32
{
    TestModeUninitialized,
    TestModeTesting,
    TestModeCompleted,
    TestModeError,
};

enum RepetitionValueType
{
    RepValueTestCount,

    RepValueCPUTimer,
    RepValueMemPageFaults,
    RepValueByteCount,

    RepValueCount,
};

enum RepValueDefaultInitType
{
    RepValueDefInitMin,
    RepValueDefInitMax,

    RepValueDefInitCount
};

struct RepetitionValue
{
    RepetitionValue(RepValueDefaultInitType defaultInit)
    {
        reset(defaultInit); 
    }

    void reset(RepValueDefaultInitType defaultInit)
    {
        for(u32 index = 0; index < RepValueCount; ++index)
        {
            if (defaultInit == RepValueDefInitMin)
            {
                E[index] = 0;
            }
            else
            {
                E[index] = (maxU64) - 1;
            }
        }
    }

    u64 E[RepValueCount];
};

struct RepetitionTestResults
{
    RepetitionValue m_min{ RepValueDefInitMax };
    RepetitionValue m_max{ RepValueDefInitMin };
    RepetitionValue m_total{ RepValueDefInitMin };
};

struct RepetitionTester
{
    bool isTesting();
    void newTestWave(u64 targetProcessedByteCount, u64 cpuTimerFreq, u32 secondsToTry = 10);

    void beginTime();
    void endTime();

    void countBytes(u64 bytesToAccumulate);

    void printTime(const char* label, RepetitionValue value);
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
    RepetitionValue m_accumulatedOnThisTest{ RepValueDefInitMin };

    RepetitionTestResults m_results;
};

}
