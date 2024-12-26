// Copyright (C) 2024 Loris De Marchi. All rights reserved.

#include "DeepAnalyzerCommands.h"

#define LOCTEXT_NAMESPACE "FDeepAnalyzerModule"

void FDeepAnalyzerCommands::RegisterCommands()
{
	UI_COMMAND(OpenPluginWindow, "DeepAnalyzer", "Bring up DeepAnalyzer window", EUserInterfaceActionType::Button, FInputChord());
}

#undef LOCTEXT_NAMESPACE
