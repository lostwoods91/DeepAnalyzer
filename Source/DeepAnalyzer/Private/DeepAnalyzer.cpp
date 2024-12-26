// Copyright (C) 2024 Loris De Marchi. All rights reserved.

#include "DeepAnalyzer.h"

#include "EditorUtilitySubsystem.h"
#include "EditorUtilityWidgetBlueprint.h"

#include "LevelEditor.h"

#include "Widgets/Docking/SDockTab.h"
#include "Widgets/Layout/SBox.h"
#include "Widgets/Text/STextBlock.h"

#include "ToolMenus.h"

#include "DADeveloperSettings.h"
#include "DeepAnalyzerCommands.h"
#include "DeepAnalyzerLog.h"
#include "DeepAnalyzerStyle.h"

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
}

void FDeepAnalyzerModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.

	UToolMenus::UnRegisterStartupCallback(this);

	UToolMenus::UnregisterOwner(this);

	FDeepAnalyzerStyle::Shutdown();

	FDeepAnalyzerCommands::Unregister();
}

void FDeepAnalyzerModule::PluginButtonClicked()
{
	const UDADeveloperSettings* DADeveloperSettings = GetDefault<UDADeveloperSettings>();
	UEditorUtilitySubsystem* EditorUtilitySubsystem = GEditor->GetEditorSubsystem<UEditorUtilitySubsystem>();

	FName MainWidgetTabId;
	if (UEditorUtilityWidgetBlueprint* MainWidget = DADeveloperSettings->MainWidget.LoadSynchronous())
	{
		EditorUtilitySubsystem->RegisterTabAndGetID(MainWidget, MainWidgetTabId);
	}
	else
	{
		UE_LOG(LogDeepAnalyzer, Warning, TEXT("Main widget class is missing. Please check project settings."));
	}

	bool bIsUIShown = EditorUtilitySubsystem->DoesTabExist(MainWidgetTabId);

	if (!bIsUIShown)
	{
		EditorUtilitySubsystem->SpawnRegisteredTabByID(MainWidgetTabId);
	}
	else
	{
		EditorUtilitySubsystem->CloseTabByID(MainWidgetTabId);
	}
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
