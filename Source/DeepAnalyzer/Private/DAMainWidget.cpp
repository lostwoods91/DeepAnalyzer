// Copyright (C) 2024 Loris De Marchi. All rights reserved.

#include "DAMainWidget.h"

#include "DAFunctionLibrary.h"
#include "DependencyInfo.h"

#include "DeepAnalyzerLog.h"

void UDAMainWidget::Analyze(const TArray<FName> PackageNames)
{
	for (const FName& PackageName : PackageNames)
	{
		TArray<FDependencyInfo> DependencyInfos;
		UDAFunctionLibrary::Analyze(TArray<FName>{ PackageName }, DependencyInfos);
		OnAnalysisResult(DependencyInfos);

		for (const FDependencyInfo& DependencyInfo : DependencyInfos)
		{
			UE_LOG(LogDeepAnalyzer, Warning, TEXT("%s -> %s"), *DependencyInfo.GetPackageName().ToString(), *DependencyInfo.GetModuleName().ToString());
		}
	}
}
