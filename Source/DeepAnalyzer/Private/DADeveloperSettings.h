// Copyright (C) 2024 Loris De Marchi. All rights reserved.

#pragma once

#include "CoreMinimal.h"

#include "Engine/DeveloperSettings.h"

#include "DADeveloperSettings.generated.h"

UCLASS(config = DeepAnalyzer)
class UDADeveloperSettings : public UDeveloperSettings
{
	GENERATED_BODY()

public:
	UDADeveloperSettings();

	UPROPERTY(config, EditAnywhere, Category = "DeepAnalyzer")
	TSoftObjectPtr<class UEditorUtilityWidgetBlueprint> MainWidget;

	//~ Begin UDeveloperSettings Interface
	virtual FName GetCategoryName() const override;
#if WITH_EDITOR
	virtual FText GetSectionText() const override;
#endif
	//~ End UDeveloperSettings Interface
};
