// Copyright (C) 2024 Loris De Marchi. All rights reserved.

#include "DADeveloperSettings.h"

UDADeveloperSettings::UDADeveloperSettings() :
    Super()
{
}

FName UDADeveloperSettings::GetCategoryName() const
{
	return TEXT("Plugins");
}

FText UDADeveloperSettings::GetSectionText() const
{
	return NSLOCTEXT("DeepAnalyzer", "DeepAnalyzerSection", "Deep Analyzer");
}
