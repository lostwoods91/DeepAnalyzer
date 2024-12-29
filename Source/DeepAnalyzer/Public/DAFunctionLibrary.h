// Copyright (C) 2024 Loris De Marchi. All rights reserved.

#pragma once

#include "CoreMinimal.h"

#include "Kismet/BlueprintFunctionLibrary.h"

#include "DAFunctionLibrary.generated.h"

struct FAssetDependency;
struct FAssetIdentifier;

struct FDependencyInfo;

UCLASS()
class DEEPANALYZER_API UDAFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	static void Analyze(const TArray<FName>& SelectedPackages, TArray<FDependencyInfo>& OutDependencyInfos);
	static void Analyze(const TArray<FAssetIdentifier>& SelectedIdentifiers, TArray<FDependencyInfo>& OutDependencyInfos);
	
	UFUNCTION(BlueprintPure, Category = "DependencyInfo")
	static FName GetPackageName(const FDependencyInfo& DependencyInfo);
	
	UFUNCTION(BlueprintPure, Category = "DependencyInfo")
	static FName GetModuleName(const FDependencyInfo& DependencyInfo);

private:
	static void RecursivelyCollectDependencies(const TArray<FAssetIdentifier>& Identifiers, TArray<FDependencyInfo>& OutDependencyInfos);
};
