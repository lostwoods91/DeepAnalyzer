// Copyright (C) 2024 Loris De Marchi. All rights reserved.

#include "DependencyInfo.h"

#include "Modules/ModuleInterface.h"
#include "Modules/ModuleManager.h"

#include "Misc/PackageName.h"

#include "DeepAnalyzerLog.h"

FName FDependencyInfo::GetPackageName() const
{
	return Identifier.PackageName;
}

FName FDependencyInfo::GetModuleName() const
{
	FString PackageNameString = GetPackageName().ToString();
	FStringView PackageNameView(PackageNameString);
	FString ScriptString = TEXT("/Script");
	if (PackageNameView.StartsWith(ScriptString))
	{
		return *FString(FPathViews::GetMountPointNameFromPath(PackageNameView.GetData() + ScriptString.Len()));
	}
	else
	{
		return *FString(FPathViews::GetMountPointNameFromPath(PackageNameView));
	}
}

IModuleInterface* FDependencyInfo::GetModule() const
{
	FStringView PackageName = Identifier.PackageName.ToString();

	FPackageName::IsScriptPackage(PackageName);

	if (FPackageName::IsValidLongPackageName(PackageName))
	{
	}
	else
	{
		// #todo test missing script modules
	}

	FString Root;
	FString Path;
	FString Name;
	FPackageName::SplitLongPackageName(Identifier.PackageName.ToString(), Root, Path, Name);
	FText Reason;
	FPackageName::IsValidObjectPath(Identifier.PackageName.ToString(), &Reason);
	FPackageName::IsValidLongPackageName(Identifier.PackageName.ToString(), true);

	FStringView ModuleName;
	if (FPackageName::TryConvertScriptPackageNameToModuleName(Identifier.PackageName.ToString(), ModuleName))
	{
		if (IModuleInterface* ModuleInterface = FModuleManager::Get().GetModule(FName(ModuleName)))
		{
			UE_LOG(LogDeepAnalyzer, Warning, TEXT("%s -> %s"), *Identifier.PackageName.ToString(), *FName(ModuleName).ToString());
			return ModuleInterface;
		}
		else
		{
			UE_LOG(LogDeepAnalyzer, Warning, TEXT("Can't find module for identifier: %s"), *Identifier.PackageName.ToString());
		}
	}

	return nullptr;
}
