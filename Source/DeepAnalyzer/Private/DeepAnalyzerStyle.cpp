// Copyright (C) 2024 Loris De Marchi. All rights reserved.

#include "DeepAnalyzerStyle.h"
#include "Styling/SlateStyleRegistry.h"
#include "Framework/Application/SlateApplication.h"
#include "Slate/SlateGameResources.h"
#include "Interfaces/IPluginManager.h"
#include "Styling/SlateStyleMacros.h"

#define RootToContentDir Style->RootToContentDir

TSharedPtr<FSlateStyleSet> FDeepAnalyzerStyle::StyleInstance = nullptr;

void FDeepAnalyzerStyle::Initialize()
{
	if (!StyleInstance.IsValid())
	{
		StyleInstance = Create();
		FSlateStyleRegistry::RegisterSlateStyle(*StyleInstance);
	}
}

void FDeepAnalyzerStyle::Shutdown()
{
	FSlateStyleRegistry::UnRegisterSlateStyle(*StyleInstance);
	ensure(StyleInstance.IsUnique());
	StyleInstance.Reset();
}

FName FDeepAnalyzerStyle::GetStyleSetName()
{
	static FName StyleSetName(TEXT("DeepAnalyzerStyle"));
	return StyleSetName;
}

const FVector2D Icon16x16(16.0f, 16.0f);
const FVector2D Icon20x20(20.0f, 20.0f);

TSharedRef< FSlateStyleSet > FDeepAnalyzerStyle::Create()
{
	TSharedRef< FSlateStyleSet > Style = MakeShareable(new FSlateStyleSet("DeepAnalyzerStyle"));
	Style->SetContentRoot(IPluginManager::Get().FindPlugin("DeepAnalyzer")->GetBaseDir() / TEXT("Resources"));

	Style->Set("DeepAnalyzer.OpenPluginWindow", new IMAGE_BRUSH_SVG(TEXT("PlaceholderButtonIcon"), Icon20x20));

	return Style;
}

void FDeepAnalyzerStyle::ReloadTextures()
{
	if (FSlateApplication::IsInitialized())
	{
		FSlateApplication::Get().GetRenderer()->ReloadTextureResources();
	}
}

const ISlateStyle& FDeepAnalyzerStyle::Get()
{
	return *StyleInstance;
}
