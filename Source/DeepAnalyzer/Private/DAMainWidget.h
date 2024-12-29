// Copyright (C) 2024 Loris De Marchi. All rights reserved.

#pragma once

#include "CoreMinimal.h"

#include "EditorUtilityWidget.h"

#include "DAMainWidget.generated.h"

struct FDependencyInfo;

UCLASS()
class UDAMainWidget : public UEditorUtilityWidget
{
	GENERATED_BODY()

public:
	void Analyze(const TArray<FName> PackageNames);

	UFUNCTION(BlueprintImplementableEvent, Category = "DAMainWidget")
	void OnAnalysisResult(const TArray<FDependencyInfo>& DependencyInfos);
};
