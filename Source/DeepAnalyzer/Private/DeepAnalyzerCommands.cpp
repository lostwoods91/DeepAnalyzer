// Copyright (C) 2024 Loris De Marchi. All rights reserved.

#include "DeepAnalyzerCommands.h"

#define LOCTEXT_NAMESPACE "FDeepAnalyzerModule"

void FDeepAnalyzerCommands::RegisterCommands()
{
	UI_COMMAND(OpenPluginWindow, "Open Window", "Bring up DeepAnalyzer window", EUserInterfaceActionType::Button, FInputChord());
	UI_COMMAND(DeepAnalyze, "Deep Analyze", "Deep analyze the selected assets", EUserInterfaceActionType::Button, FInputChord(EModifierKey::Shift | EModifierKey::Alt, EKeys::D));
}

#undef LOCTEXT_NAMESPACE
