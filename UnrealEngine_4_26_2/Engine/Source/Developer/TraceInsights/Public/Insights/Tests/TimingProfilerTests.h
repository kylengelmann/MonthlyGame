// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Logging/LogMacros.h"

DECLARE_LOG_CATEGORY_EXTERN(TimingProfilerTests, Log, All);

////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * A class containing code for parametric tests for Insight functionality
 * Intended to be called from automatic or user triggered tests
 */
class TRACEINSIGHTS_API FTimingProfilerTests
{
public:
	struct FCheckValues
	{
		double TotalEventDuration = 0.0;
		uint64 EventCount = 0;
		uint32 SumDepth = 0;
		uint32 SumTimerIndex = 0;
		double SessionDuration = 0;

		double EnumerationDuration = 0;
	};

	struct FEnumerateTestParams
	{
		double Interval = 0.01;
		int32 NumEnumerations = 10000;
	};

	static void RunEnumerateBenchmark(const FEnumerateTestParams& InParams, FCheckValues& OutCheckValues);
};