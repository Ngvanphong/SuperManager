// Fill out your copyright notice in the Description page of Project Settings.


#include "AssetAction/QuickAssetAction.h"
#include "DebugHeader.h"
#include "EditorUtilityLibrary.h"
#include "EditorAssetLibrary.h"


void UQuickAssetAction::DuplicateAssets(int32 NumOfDuplicates)
{
	if (NumOfDuplicates <= 0)
	{
		Print("Please enter a Valid Number", FColor::Red);
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
}
