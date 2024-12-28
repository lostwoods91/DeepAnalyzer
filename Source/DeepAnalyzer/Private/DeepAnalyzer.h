// Copyright (C) 2024 Loris De Marchi. All rights reserved.

#pragma once

#include "CoreMinimal.h"

#include "ContentBrowserDelegates.h"

#include "Modules/ModuleManager.h"

class FToolBarBuilder;
class FMenuBuilder;
class IAssetRegistry;

class UDAMainWidget;

class FDeepAnalyzerModule : public IModuleInterface
{
private:
	FName WidgetTabID;
	FDelegateHandle ContentBrowserCommandExtenderDelegateHandle;
	IAssetRegistry* AssetRegistry;

public:
	//~ Begin IModuleInterface Interface
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
	//~ End IModuleInterface Interface

private:
	void RegisterMenus();
	void CreateAssetContextMenu(FToolMenuSection& InSection);
	void OnExtendContentBrowserCommands(TSharedRef<FUICommandList> CommandList, FOnContentBrowserGetSelection GetSelectionDelegate);
	TArray<FName> GetContentBrowserSelectedAssetPackages(FOnContentBrowserGetSelection GetSelectionDelegate);
	void GetAssetDataInPaths(const TArray<FString>& Paths, TArray<FAssetData>& OutAssetData);

	UDAMainWidget* FindOrSpawnWidgetTab();
	void CloseWidgetTab() const;
	void ToggleWidgetTab();

	TSharedPtr<class FUICommandList> PluginCommands;
};
