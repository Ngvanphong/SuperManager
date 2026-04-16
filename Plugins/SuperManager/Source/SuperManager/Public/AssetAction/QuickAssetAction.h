// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AssetActionUtility.h"
#include "AssetActionUtility.h"
#include "EditorUtilityBlueprint.h"
#include "QuickAssetAction.generated.h"

/**
 * 
 */
UCLASS()
class SUPERMANAGER_API UQuickAssetAction : public UAssetActionUtility
{
	GENERATED_BODY()
	
public:
	UFUNCTION(CallInEditor)
	void DuplicateAssets(int32 NumOfDuplicates);

	UFUNCTION(CallInEditor)
	void AddPrefix();

	UFUNCTION(CallInEditor)
	void RemoveUnuseAssets();
	
	

private:
	TMap<UClass*, FString> PrefixMap=
		{
			{UBlueprint::StaticClass(),TEXT("BP_")},
			{UQuickAssetAction::StaticClass(),TEXT("BP_")},
			{UEditorUtilityBlueprint::StaticClass(),TEXT("BP_")},

		};
	
	void FixUpRedirector();
	
};
