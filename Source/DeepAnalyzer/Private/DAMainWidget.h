// Copyright (C) 2024 Loris De Marchi. All rights reserved.

#pragma once

#include "CoreMinimal.h"

#include "EditorUtilityWidget.h"

#include "DAMainWidget.generated.h"

UCLASS()
class UDAMainWidget : public UEditorUtilityWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintImplementableEvent, Category = "DAMainWidget")
	void OnAnalysisResult();
};
