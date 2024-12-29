// Copyright (C) 2024 Loris De Marchi. All rights reserved.

#pragma once

#include "CoreMinimal.h"

#include "DependencyInfo.generated.h"

class IModuleInterface;

struct FAssetIdentifier;

USTRUCT(BlueprintType)
struct FDependencyInfo
{
	GENERATED_BODY()

private:
	FAssetIdentifier Identifier;

public:
	FDependencyInfo()
	{}

	FDependencyInfo(const FAssetIdentifier& Identifier) :
	    Identifier(Identifier)
	{
		check(Identifier.IsPackage());
	}

	bool operator==(const FDependencyInfo& Other) const
	{
		return Identifier == Other.Identifier;
	}

	FName GetPackageName() const;
	FName GetModuleName() const;
};
