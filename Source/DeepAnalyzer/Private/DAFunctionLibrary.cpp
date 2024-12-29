// Copyright (C) 2024 Loris De Marchi. All rights reserved.

#include "DAFunctionLibrary.h"

#include "AssetRegistry/AssetRegistryModule.h"
#include "AssetRegistry/IAssetRegistry.h"

#include "Modules/ModuleManager.h"

#include "UObject/Class.h"
#include "UObject/UObjectGlobals.h"
#include "UObject/UnrealTypePrivate.h"
#include "UObject/WeakObjectPtr.h"

#include "Engine/Blueprint.h"

#include "Interfaces/IPluginManager.h"
#include "Interfaces/IProjectManager.h"
#include "ProjectDescriptor.h"

#include "DependencyInfo.h"

void UDAFunctionLibrary::Analyze(const TArray<FName>& SelectedPackages, TArray<FDependencyInfo>& OutDependencyInfos)
{
	TArray<FAssetIdentifier> Identifiers;
	for (FName Name : SelectedPackages)
	{
		Identifiers.Add(FAssetIdentifier(Name));
	}

	Analyze(Identifiers, OutDependencyInfos);
}

void UDAFunctionLibrary::Analyze(const TArray<FAssetIdentifier>& SelectedIdentifiers, TArray<FDependencyInfo>& OutDependencyInfos)
{
	RecursivelyCollectDependencies(SelectedIdentifiers, OutDependencyInfos);
}

FName UDAFunctionLibrary::GetPackageName(const FDependencyInfo& DependencyInfo)
{
	return DependencyInfo.GetPackageName();
}

FName UDAFunctionLibrary::GetModuleName(const FDependencyInfo& DependencyInfo)
{
	return DependencyInfo.GetModuleName();
}

void UDAFunctionLibrary::RecursivelyCollectDependencies(const TArray<FAssetIdentifier>& Identifiers, TArray<FDependencyInfo>& OutDependencyInfos)
{
	IAssetRegistry& AssetRegistry = FModuleManager::LoadModuleChecked<FAssetRegistryModule>("AssetRegistry").Get();
	TArray<FAssetIdentifier> TempDependencies;
	for (const FAssetIdentifier& Identifier : Identifiers)
	{
		OutDependencyInfos.AddUnique(FDependencyInfo(Identifier));
		AssetRegistry.GetDependencies(Identifier, TempDependencies, UE::AssetRegistry::EDependencyCategory::All, UE::AssetRegistry::EDependencyQuery::NoRequirements);
	}

	for (const FAssetIdentifier& Dependency : TempDependencies)
	{
		RecursivelyCollectDependencies({ Dependency }, OutDependencyInfos);
	}
}
