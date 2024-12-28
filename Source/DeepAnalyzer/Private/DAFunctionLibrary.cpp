// Copyright (C) 2024 Loris De Marchi. All rights reserved.

#include "DAFunctionLibrary.h"

#include "AssetRegistry/AssetRegistryModule.h"
#include "AssetRegistry/IAssetRegistry.h"

#include "Modules/ModuleManager.h"

#include "UObject/Class.h"
#include "UObject/UObjectGlobals.h"
#include "UObject/UnrealTypePrivate.h"
#include "UObject/WeakObjectPtr.h"

#include "Engine/Blueprint.h"

#include "Interfaces/IPluginManager.h"
#include "Interfaces/IProjectManager.h"
#include "ProjectDescriptor.h"

#include "DeepAnalyzerLog.h"

void UDAFunctionLibrary::Analyze(const TArray<FName>& SelectedPackages)
{
	TArray<FAssetIdentifier> Identifiers;
	for (FName Name : SelectedPackages)
	{
		Identifiers.Add(FAssetIdentifier(Name));
	}

	Analyze(Identifiers);
}

void UDAFunctionLibrary::Analyze(const TArray<FAssetIdentifier>& SelectedIdentifiers)
{
	IAssetRegistry& AssetRegistry = FModuleManager::LoadModuleChecked<FAssetRegistryModule>("AssetRegistry").Get();

	TArray<FAssetIdentifier> Dependencies;
	for (FAssetIdentifier AssetIdentifier : SelectedIdentifiers)
	{
		AssetRegistry.GetDependencies(AssetIdentifier, Dependencies, UE::AssetRegistry::EDependencyCategory::All, UE::AssetRegistry::EDependencyQuery::NoRequirements);
	}
}

void UDAFunctionLibrary::RecursivelyCollectDependencies(const TArray<FAssetIdentifier>& Identifiers, TMap<FAssetIdentifier, FAssetData>& Dependencies)
{
}

void UDAFunctionLibrary::GetReferencedModulesFromClass(UObject* Object, UClass* Class)
{
	if (Class == nullptr)
	{
		return;
	}

	TArray<FModuleStatus> ModuleStatuses;
	FModuleManager::Get().QueryModules(ModuleStatuses);

	TArray<FProperty*> Dependencies;
	TArray<IModuleInterface*> ReferencedModules;
	for (TFieldIterator<FProperty> PropIt(Class); PropIt; ++PropIt)
	{
		FProperty* Property = *PropIt;
		{
			Dependencies.Add(Property);
			GetReferencedModulesFromObjectProperty(Object, Property, ReferencedModules);
		}
	}
}

void UDAFunctionLibrary::GetReferencedModulesFromObject(UObject* Object)
{
	if (Object)
	{
		for (TFieldIterator<FObjectPropertyBase> It(Object->GetClass()); It; ++It)
		{
			FObjectPropertyBase* Property = *It;
			auto asd = Property->ContainerPtrToValuePtr<void>(Object, 0);
			if (UObject* Value = Property->GetObjectPropertyValue(asd))
			{
				UE_LOG(LogDeepAnalyzer, Warning, TEXT("%s"), *Value->GetName());
			}
		}

		GetReferencedModulesFromClass(Object, Object->GetClass());
	}
}

void UDAFunctionLibrary::GetReferencedModulesFromBlueprint(UBlueprint* Blueprint)
{
	if (Blueprint)
	{
		GetReferencedModulesFromClass(Blueprint->GeneratedClass->GetDefaultObject(), Blueprint->GeneratedClass);
	}
}

void UDAFunctionLibrary::GetReferencedModulesFromObjectProperty(UObject* Object, FProperty* Property, TArray<IModuleInterface*>& OutReferencedModules)
{
	if (FArrayProperty* ArrayProperty = CastField<FArrayProperty>(Property))
	{
		//FScriptArrayHelper ArrayHelper(ArrayProperty, ArrayProperty->ContainerPtrToValuePtr<void>(Object));
		GetReferencedModulesFromObjectProperty(Object, ArrayProperty->Inner, OutReferencedModules);
	}
	else if (FSetProperty* SetProperty = CastField<FSetProperty>(Property))
	{
		GetReferencedModulesFromObjectProperty(Object, SetProperty->ElementProp, OutReferencedModules);
	}
	else if (FMapProperty* MapProperty = CastField<FMapProperty>(Property))
	{
		GetReferencedModulesFromObjectProperty(Object, MapProperty->KeyProp, OutReferencedModules);
		GetReferencedModulesFromObjectProperty(Object, MapProperty->ValueProp, OutReferencedModules);
	}
	else
	{
		UObject* PropertyObject = nullptr;

		if (FObjectPropertyBase* ObjectPropertyBase = CastField<FObjectPropertyBase>(Property))
		{
			PropertyObject = Cast<UObject>(ObjectPropertyBase->PropertyClass);
			//void* PropertyValue = Property->ContainerPtrToValuePtr<void>(Object);
			//UObject* Value = ObjectPropertyBase->GetObjectPropertyValue(PropertyValue);
		}
		else if (FStructProperty* StructProperty = CastField<FStructProperty>(Property))
		{
			PropertyObject = Cast<UObject>(StructProperty->Struct);
		}
		else if (FEnumProperty* EnumProperty = CastField<FEnumProperty>(Property))
		{
			PropertyObject = Cast<UObject>(EnumProperty->GetEnum());
		}

		if (PropertyObject)
		{
			if (UPackage* Package = PropertyObject->GetPackage())
			{
				FStringView ModuleName;
				if (FPackageName::TryConvertScriptPackageNameToModuleName(Package->GetName(), ModuleName))
				{
					if (IModuleInterface* ModuleInterface = FModuleManager::Get().GetModule(FName(ModuleName)))
					{
						OutReferencedModules.Add(ModuleInterface);
						UE_LOG(LogDeepAnalyzer, Warning, TEXT("%s -> %s"), *Property->GetName(), *FName(ModuleName).ToString());
					}
				}
			}
		}
	}
}
