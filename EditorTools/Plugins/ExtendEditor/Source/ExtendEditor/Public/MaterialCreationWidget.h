// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EditorUtilityWidget.h"
#include "MaterialCreationWidget.generated.h"

class UMaterialExpressionTextureSample;

UENUM(BlueprintType)
enum class E_ChannelPacking : uint8
{
	ECPT_NoChannelPacking UMETA (DisplayName = "No channel packing"),
	ECPT_ORM UMETA (DisplayName = "OcclusionRoughnessMetallic"),
	ECPT_MAX UMETA (DisplayName = "DefaultMax")
};

UCLASS()
class EXTENDEDITOR_API UMaterialCreationWidget : public UEditorUtilityWidget
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable) 
	void CreateMaterialFromSelectedTextures();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="MaterialCreation")
	E_ChannelPacking ChannelPackingType = E_ChannelPacking::ECPT_NoChannelPacking;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="MaterialCreation")
	bool bCustomMaterialName = true;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="MaterialCreation", meta = (EditCondition="bCustomMaterialName"))
	FString MaterialName = TEXT("M_");

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="MaterialCreation")
	bool bCreateMaterialInstance = false;

#pragma region SuportedTextureNames

	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category="TextureNames");
	TArray<FString> BaseColorArray={
		TEXT("_BaseColor"),
		TEXT("_Albedo"),
		TEXT("_Diffuse"),
		TEXT("_diff"),
	};

	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category = "Supported Texture Names")
	TArray<FString> MetallicArray = {
		TEXT("_Metallic"),
		TEXT("_metal")
		};

	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category = "Supported Texture Names")
	TArray<FString> RoughnessArray = {
		TEXT("_Roughness"),
		TEXT("_RoughnessMap"),
		TEXT("_rough")
		};
	
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category = "Supported Texture Names")
	TArray<FString> NormalArray = {
		TEXT("_Normal"),
		TEXT("_NormalMap"),
		TEXT("_nor")
		};
	              
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category = "Supported Texture Names")
	TArray<FString> AmbientOcclusionArray = {
		TEXT("_AmbientOcclusion"),
		TEXT("_AmbientOcclusionMap"),
		TEXT("_AO")
		};

	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category = "Supported Texture Names")
	TArray<FString> ORMArray = {
		TEXT("_arm"),
		TEXT("_OcclusionRoughnessMetallic"),
		TEXT("_ORM")
		};
	
#pragma endregion
	
private:
	bool ProcessSelectedData(const TArray<FAssetData>& DataToProcess, TArray<UTexture2D*>& OutTexArray, FString& OutTexPath);
	bool CheckIsNameUsed(const FString& FolderPathToCheck, const FString& MaterialNameToCheck);
	UMaterial* CreateMaterialAsset(const FString& Name, const FString& FolderPath);
	
	void CreateMatNodes(UMaterial* Material, UTexture2D* Texture, uint32& PinsConnectedCount);
	void ORM_CreateMatNodes(UMaterial* Material, UTexture2D* Texture, uint32& PinsConnectedCount);

	bool TryConnectBaseColor(UMaterialExpressionTextureSample* TextureSampleNode, UTexture2D* Texture, UMaterial* Material);
	bool TryConnectMetallic(UMaterialExpressionTextureSample* TextureSampleNode, UTexture2D* Texture, UMaterial* Material);
	bool TryConnectRoughness(UMaterialExpressionTextureSample* TextureSampleNode, UTexture2D* Texture, UMaterial* Material);
	bool TryConnectNormal(UMaterialExpressionTextureSample* TextureSampleNode, UTexture2D* Texture, UMaterial* Material);
	bool TryConnectAmbientOcclusion(UMaterialExpressionTextureSample* TextureSampleNode, UTexture2D* Texture, UMaterial* Material);
	bool TryConnectORM(UMaterialExpressionTextureSample* TextureSampleNode, UTexture2D* Texture, UMaterial* Material);

	class UMaterialInstanceConstant* CreateMaterialInstanceAsset(UMaterial* Material, FString Name, const FString& FolderPath);
};
