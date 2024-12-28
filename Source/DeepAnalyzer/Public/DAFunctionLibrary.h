// Copyright (C) 2024 Loris De Marchi. All rights reserved.

#pragma once

#include "CoreMinimal.h"

#include "Kismet/BlueprintFunctionLibrary.h"

#include "DAFunctionLibrary.generated.h"

UCLASS()
class DEEPANALYZER_API UDAFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	static void Analyze(const TArray<FName>& SelectedPackages);
	static void Analyze(const TArray<FAssetIdentifier>& SelectedIdentifiers);

	static void GetReferencedModulesFromClass(UObject* Object, UClass* Class);

	UFUNCTION(BlueprintCallable, Category = "DeepAnalyzer")
	static void GetReferencedModulesFromObject(UObject* Object);

	UFUNCTION(BlueprintCallable, Category = "DeepAnalyzer")
	static void GetReferencedModulesFromBlueprint(UBlueprint* Blueprint);

	static void GetReferencedModulesFromObjectProperty(UObject* Object, FProperty* Property, TArray<IModuleInterface*>& OutReferencedModules);

private:
	void RecursivelyCollectDependencies(const TArray<FAssetIdentifier>& Identifiers, TMap<FAssetIdentifier, FAssetData>& OutDependencies);
};
