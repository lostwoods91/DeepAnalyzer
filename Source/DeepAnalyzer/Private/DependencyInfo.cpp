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
