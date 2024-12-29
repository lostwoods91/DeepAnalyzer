// Copyright (C) 2024 Loris De Marchi. All rights reserved.

#include "DeepAnalyzer.h"

#include "AssetRegistry/AssetRegistryModule.h"
#include "AssetRegistry/IAssetRegistry.h"

#include "EditorUtilitySubsystem.h"
#include "EditorUtilityWidgetBlueprint.h"

#include "ContentBrowserMenuContexts.h"
#include "ContentBrowserModule.h"

#include "ContentBrowserDataSubsystem.h"
#include "IContentBrowserDataModule.h"

#include "LevelEditor.h"

#include "Widgets/Docking/SDockTab.h"
#include "Widgets/Layout/SBox.h"
#include "Widgets/Text/STextBlock.h"

#include "ToolMenus.h"

#include "DADeveloperSettings.h"
#include "DAFunctionLibrary.h"
#include "DAMainWidget.h"
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

	FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>(TEXT("AssetRegistry"));
	AssetRegistry = &AssetRegistryModule.Get();

	PluginCommands = MakeShareable(new FUICommandList);

	PluginCommands->MapAction(
	    FDeepAnalyzerCommands::Get().OpenPluginWindow,
	    FUIAction(FExecuteAction::CreateRaw(this, &FDeepAnalyzerModule::ToggleWidgetTab)));

	UToolMenus::RegisterStartupCallback(FSimpleMulticastDelegate::FDelegate::CreateRaw(this, &FDeepAnalyzerModule::RegisterMenus));

	// Register content browser hook
	FContentBrowserModule& ContentBrowserModule = FModuleManager::LoadModuleChecked<FContentBrowserModule>(TEXT("ContentBrowser"));

	TArray<FContentBrowserCommandExtender>& CBCommandExtenderDelegates = ContentBrowserModule.GetAllContentBrowserCommandExtenders();
	CBCommandExtenderDelegates.Add(FContentBrowserCommandExtender::CreateRaw(this, &FDeepAnalyzerModule::OnExtendContentBrowserCommands));
	ContentBrowserCommandExtenderDelegateHandle = CBCommandExtenderDelegates.Last().GetHandle();
}

void FDeepAnalyzerModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.

	FContentBrowserModule& ContentBrowserModule = FModuleManager::LoadModuleChecked<FContentBrowserModule>(TEXT("ContentBrowser"));
	TArray<FContentBrowserCommandExtender>& CBCommandExtenderDelegates = ContentBrowserModule.GetAllContentBrowserCommandExtenders();
	CBCommandExtenderDelegates.RemoveAll([this](const FContentBrowserCommandExtender& Delegate)
	    {
		    return Delegate.GetHandle() == ContentBrowserCommandExtenderDelegateHandle;
	    });

	UToolMenus::UnRegisterStartupCallback(this);

	UToolMenus::UnregisterOwner(this);

	FDeepAnalyzerStyle::Shutdown();

	FDeepAnalyzerCommands::Unregister();
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

	FSlateIcon Icon = FSlateIcon(FAppStyle::GetAppStyleSetName(), "ContentBrowser.Migrate");

	{
		UToolMenu* Menu = UToolMenus::Get()->ExtendMenu("ContentBrowser.FolderContextMenu");
		FToolMenuSection& Section = Menu->AddSection("DeepAnalyzer", LOCTEXT("DeepAnalyzerSection", "Deep Analyzer"));
		CreateAssetContextMenu(Section);
	}

	{
		UToolMenu* Menu = UToolMenus::Get()->ExtendMenu("ContentBrowser.AssetContextMenu");
		FToolMenuSection& Section = Menu->AddSection("DeepAnalyzer", LOCTEXT("DeepAnalyzerSection", "Deep Analyzer"));
		CreateAssetContextMenu(Section);
	}
}

void FDeepAnalyzerModule::CreateAssetContextMenu(FToolMenuSection& InSection)
{
	InSection.AddMenuEntry(
	    FDeepAnalyzerCommands::Get().DeepAnalyze,
	    TAttribute<FText>(), // Use command Label
	    TAttribute<FText>(), // Use command tooltip
	    FSlateIcon(FAppStyle::GetAppStyleSetName(), "ContentBrowser.ReferenceViewer"));
}

void FDeepAnalyzerModule::OnExtendContentBrowserCommands(TSharedRef<FUICommandList> CommandList, FOnContentBrowserGetSelection GetSelectionDelegate)
{
	CommandList->MapAction(FDeepAnalyzerCommands::Get().DeepAnalyze,
	    FExecuteAction::CreateLambda([this, GetSelectionDelegate]
	        {
		        TArray<FName> SelectedPackages = GetContentBrowserSelectedAssetPackages(GetSelectionDelegate);
		        if (SelectedPackages.Num() > 0)
		        {
			        if (UDAMainWidget* DAMainWidget = FindOrSpawnWidgetTab())
			        {
				        DAMainWidget->Analyze(SelectedPackages);
			        }
		        }
	        }));
}

TArray<FName> FDeepAnalyzerModule::GetContentBrowserSelectedAssetPackages(FOnContentBrowserGetSelection GetSelectionDelegate)
{
	TArray<FName> OutAssetPackages;
	TArray<FAssetData> SelectedAssets;
	TArray<FString> SelectedPaths;

	if (GetSelectionDelegate.IsBound())
	{
		TArray<FString> SelectedVirtualPaths;
		GetSelectionDelegate.Execute(SelectedAssets, SelectedVirtualPaths);

		for (const FString& VirtualPath : SelectedVirtualPaths)
		{
			FString InvariantPath;
			if (IContentBrowserDataModule::Get().GetSubsystem()->TryConvertVirtualPath(VirtualPath, InvariantPath) == EContentBrowserPathType::Internal)
			{
				SelectedPaths.Add(InvariantPath);
			}
		}
	}

	GetAssetDataInPaths(SelectedPaths, SelectedAssets);

	TArray<FName> PackageNames;
	for (const FAssetData& AssetData : SelectedAssets)
	{
		OutAssetPackages.AddUnique(AssetData.PackageName);
	}

	return OutAssetPackages;
}

void FDeepAnalyzerModule::GetAssetDataInPaths(const TArray<FString>& Paths, TArray<FAssetData>& OutAssetData)
{
	// Form a filter from the paths
	FARFilter Filter;
	Filter.bRecursivePaths = true;
	for (const FString& Path : Paths)
	{
		new (Filter.PackagePaths) FName(*Path);
	}

	// Query for a list of assets in the selected paths
	AssetRegistry->GetAssets(Filter, OutAssetData);
}

UDAMainWidget* FDeepAnalyzerModule::FindOrSpawnWidgetTab()
{
	UEditorUtilitySubsystem* EditorUtilitySubsystem = GEditor->GetEditorSubsystem<UEditorUtilitySubsystem>();
	const UDADeveloperSettings* DADeveloperSettings = GetDefault<UDADeveloperSettings>();
	if (UEditorUtilityWidgetBlueprint* MainWidget = DADeveloperSettings->MainWidget.LoadSynchronous())
	{
		if (WidgetTabID.IsNone())
		{
			EditorUtilitySubsystem->RegisterTabAndGetID(MainWidget, WidgetTabID);
		}

		if (!EditorUtilitySubsystem->DoesTabExist(WidgetTabID))
		{
			EditorUtilitySubsystem->SpawnRegisteredTabByID(WidgetTabID);
		}

		if (UDAMainWidget* DAMainWidget = Cast<UDAMainWidget>(EditorUtilitySubsystem->FindUtilityWidgetFromBlueprint(MainWidget)))
		{
			return DAMainWidget;
		}
		else
		{
			UE_LOG(LogDeepAnalyzer, Warning, TEXT("Main widget class must derive from UDAMainWidget. Please check project settings."));
		}
	}
	else
	{
		UE_LOG(LogDeepAnalyzer, Warning, TEXT("Main widget class is missing. Please check project settings."));
	}

	return nullptr;
}

void FDeepAnalyzerModule::CloseWidgetTab() const
{
	UEditorUtilitySubsystem* EditorUtilitySubsystem = GEditor->GetEditorSubsystem<UEditorUtilitySubsystem>();
	EditorUtilitySubsystem->CloseTabByID(WidgetTabID);
}

void FDeepAnalyzerModule::ToggleWidgetTab()
{
	UEditorUtilitySubsystem* EditorUtilitySubsystem = GEditor->GetEditorSubsystem<UEditorUtilitySubsystem>();
	if (!EditorUtilitySubsystem->DoesTabExist(WidgetTabID))
	{
		FindOrSpawnWidgetTab();
	}
	else
	{
		CloseWidgetTab();
	}
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FDeepAnalyzerModule, DeepAnalyzer)
