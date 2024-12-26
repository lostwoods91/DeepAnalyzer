// Copyright (C) 2024 Loris De Marchi. All rights reserved.

#include "DeepAnalyzer.h"
#include "DeepAnalyzerStyle.h"
#include "DeepAnalyzerCommands.h"
#include "LevelEditor.h"
#include "Widgets/Docking/SDockTab.h"
#include "Widgets/Layout/SBox.h"
#include "Widgets/Text/STextBlock.h"
#include "ToolMenus.h"

static const FName DeepAnalyzerTabName("DeepAnalyzer");

#define LOCTEXT_NAMESPACE "FDeepAnalyzerModule"

void FDeepAnalyzerModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
	
	FDeepAnalyzerStyle::Initialize();
	FDeepAnalyzerStyle::ReloadTextures();

	FDeepAnalyzerCommands::Register();
	
	PluginCommands = MakeShareable(new FUICommandList);

	PluginCommands->MapAction(
		FDeepAnalyzerCommands::Get().OpenPluginWindow,
		FExecuteAction::CreateRaw(this, &FDeepAnalyzerModule::PluginButtonClicked),
		FCanExecuteAction());

	UToolMenus::RegisterStartupCallback(FSimpleMulticastDelegate::FDelegate::CreateRaw(this, &FDeepAnalyzerModule::RegisterMenus));
	
	FGlobalTabmanager::Get()->RegisterNomadTabSpawner(DeepAnalyzerTabName, FOnSpawnTab::CreateRaw(this, &FDeepAnalyzerModule::OnSpawnPluginTab))
		.SetDisplayName(LOCTEXT("FDeepAnalyzerTabTitle", "DeepAnalyzer"))
		.SetMenuType(ETabSpawnerMenuType::Hidden);
}

void FDeepAnalyzerModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.

	UToolMenus::UnRegisterStartupCallback(this);

	UToolMenus::UnregisterOwner(this);

	FDeepAnalyzerStyle::Shutdown();

	FDeepAnalyzerCommands::Unregister();

	FGlobalTabmanager::Get()->UnregisterNomadTabSpawner(DeepAnalyzerTabName);
}

TSharedRef<SDockTab> FDeepAnalyzerModule::OnSpawnPluginTab(const FSpawnTabArgs& SpawnTabArgs)
{
	FText WidgetText = FText::Format(
		LOCTEXT("WindowWidgetText", "Add code to {0} in {1} to override this window's contents"),
		FText::FromString(TEXT("FDeepAnalyzerModule::OnSpawnPluginTab")),
		FText::FromString(TEXT("DeepAnalyzer.cpp"))
		);

	return SNew(SDockTab)
		.TabRole(ETabRole::NomadTab)
		[
			// Put your tab content here!
			SNew(SBox)
			.HAlign(HAlign_Center)
			.VAlign(VAlign_Center)
			[
				SNew(STextBlock)
				.Text(WidgetText)
			]
		];
}

void FDeepAnalyzerModule::PluginButtonClicked()
{
	FGlobalTabmanager::Get()->TryInvokeTab(DeepAnalyzerTabName);
}

void FDeepAnalyzerModule::RegisterMenus()
{
	// Owner will be used for cleanup in call to UToolMenus::UnregisterOwner
	FToolMenuOwnerScoped OwnerScoped(this);

	{
		UToolMenu* Menu = UToolMenus::Get()->ExtendMenu("LevelEditor.MainMenu.Window");
		{
			FToolMenuSection& Section = Menu->FindOrAddSection("WindowLayout");
			Section.AddMenuEntryWithCommandList(FDeepAnalyzerCommands::Get().OpenPluginWindow, PluginCommands);
		}
	}

	{
		UToolMenu* ToolbarMenu = UToolMenus::Get()->ExtendMenu("LevelEditor.LevelEditorToolBar.PlayToolBar");
		{
			FToolMenuSection& Section = ToolbarMenu->FindOrAddSection("PluginTools");
			{
				FToolMenuEntry& Entry = Section.AddEntry(FToolMenuEntry::InitToolBarButton(FDeepAnalyzerCommands::Get().OpenPluginWindow));
				Entry.SetCommandList(PluginCommands);
			}
		}
	}
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FDeepAnalyzerModule, DeepAnalyzer)