// Copyright (C) 2024 Loris De Marchi. All rights reserved.

#pragma once

#include "CoreMinimal.h"

#include "Framework/Commands/Commands.h"

#include "DeepAnalyzerStyle.h"

class FDeepAnalyzerCommands : public TCommands<FDeepAnalyzerCommands>
{
public:
	FDeepAnalyzerCommands() :
	    TCommands<FDeepAnalyzerCommands>(TEXT("DeepAnalyzer"), NSLOCTEXT("Contexts", "DeepAnalyzer", "DeepAnalyzer Plugin"), NAME_None, FDeepAnalyzerStyle::GetStyleSetName())
	{
	}

	// TCommands<> interface
	virtual void RegisterCommands() override;

public:
	TSharedPtr<FUICommandInfo> OpenPluginWindow;
	TSharedPtr<FUICommandInfo> DeepAnalyze;
};
