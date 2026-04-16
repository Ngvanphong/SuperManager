// Fill out your copyright notice in the Description page of Project Settings.


#include "AssetAction/QuickAssetAction.h"

#include "AssetToolsModule.h"
#include "DebugHeader.h"
#include "EditorUtilityLibrary.h"
#include "EditorAssetLibrary.h"
#include "ObjectTools.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "Framework/Notifications/NotificationManager.h"
#include "Widgets/Notifications/SNotificationList.h"

void UQuickAssetAction::DuplicateAssets(int32 NumOfDuplicates)
{
	if (NumOfDuplicates <= 0)
	{
		//Print("Please enter a Valid Number", FColor::Red);
		FMessageDialog::Open(EAppMsgType::Ok, FText::FromString(TEXT("Please enter a Valid number")));
		return;
	};
	TArray<FAssetData> SelectedAssetsData = UEditorUtilityLibrary::GetSelectedAssetData();
	int32 counter = 0;
	for (const FAssetData& SelectedAssetData : SelectedAssetsData)
	{
		while (true)
		{
			if (counter > NumOfDuplicates) break;
			const FString SourceAssetPath = SelectedAssetData.GetObjectPathString();
			const FString NewAssetName = FString::Printf(TEXT("%s_%d"), *SelectedAssetData.AssetName.ToString(), counter);
			const FString NewPathName = FPaths::Combine(SelectedAssetData.PackagePath.ToString(), NewAssetName);
			if (UEditorAssetLibrary::DuplicateAsset(SourceAssetPath, NewPathName))
			{
				UEditorAssetLibrary::SaveAsset(NewAssetName, false);
			}
			counter++;
		}
	}

	FNotificationInfo NotifyInfo(FText::FromString(FString::Printf(TEXT("%d"), counter)));
	NotifyInfo.bUseLargeFont = true;
	NotifyInfo.FadeOutDuration = 7.f;
	FSlateNotificationManager::Get().AddNotification(NotifyInfo);
}

void UQuickAssetAction::AddPrefix()
{
	TArray<UObject*> SelectedAssets = UEditorUtilityLibrary::GetSelectedAssets();
	for (UObject* Item : SelectedAssets)
	{
		if (!Item) continue;
		FString* PrefixFound = PrefixMap.Find(Item->GetClass());
		if (!PrefixFound || PrefixFound->IsEmpty())
		{
			Print(TEXT("Fail to find prefix for class"), FColor::Red);
			continue;
		}
		if (!Item->GetName().StartsWith(*PrefixFound))
		{
			UEditorUtilityLibrary::RenameAsset(Item, *PrefixFound + Item->GetName());
		}
	}
}

void UQuickAssetAction::RemoveUnuseAssets()
{
	FixUpRedirector();
	TArray<FAssetData> SelectedAssetDatas = UEditorUtilityLibrary::GetSelectedAssetData();
	TArray<UObject*> UnuseAssetDatas;
	for (const FAssetData& SelectedAssetData : SelectedAssetDatas)
	{
		TArray<FString> ReferencersForAsset = UEditorAssetLibrary::FindPackageReferencersForAsset(SelectedAssetData.GetObjectPathString());
		if (ReferencersForAsset.Num() == 0) UnuseAssetDatas.Add(SelectedAssetData.GetAsset());
	}
	if (UnuseAssetDatas.Num() > 0)
	{
		ObjectTools::DeleteObjects(UnuseAssetDatas, true);
	}
}

void UQuickAssetAction::FixUpRedirector()
{
	TArray<UObjectRedirector*> RedirectorsToFixArray;
	FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>(TEXT("AssetRegistry"));
	FARFilter Filter;
	Filter.bRecursivePaths = true;
	Filter.PackagePaths.Emplace("/Game");
	Filter.ClassPaths.Emplace("ObjectRedirector");
	TArray<FAssetData> OutRedirectors;
	AssetRegistryModule.Get().GetAssets(Filter, OutRedirectors);
	for (const FAssetData& RedirectorData : OutRedirectors)
	{
		if (UObjectRedirector* RedirectorToFix = Cast<UObjectRedirector>(RedirectorData.GetAsset()))
		{
			RedirectorsToFixArray.Add(RedirectorToFix);
		}
	}
	FAssetToolsModule& AssetToolsModule = FModuleManager::LoadModuleChecked<FAssetToolsModule>(TEXT("AssetTools"));
	AssetToolsModule.Get().FixupReferencers(RedirectorsToFixArray);
}
