// Fill out your copyright notice in the Description page of Project Settings.


#include "MaterialCreationWidget.h"

#include "AssetToolsModule.h"
#include "DebugHelper.h"
#include "EditorAssetLibrary.h"
#include "EditorUtilityLibrary.h"
#include "Factories/MaterialFactoryNew.h"
#include "Factories/MaterialInstanceConstantFactoryNew.h"
#include "Materials/MaterialExpressionTextureSample.h"
#include "Materials/MaterialInstanceConstant.h"

void UMaterialCreationWidget::CreateMaterialFromSelectedTextures()
{
	if(bCustomMaterialName)
	{
		if(MaterialName.IsEmpty() || MaterialName.Equals(TEXT("M_")))
		{
			ShowMsg(EAppMsgType::Ok, "Please enter a valid name");
			return;
		}
	}

	TArray<FAssetData> selectedAD = UEditorUtilityLibrary::GetSelectedAssetData();
	TArray<UTexture2D*> selectedTex;
	FString selectedFolderPath;

	if(!ProcessSelectedData(selectedAD, selectedTex, selectedFolderPath))
	{
		MaterialName = TEXT("M_");
		return;
	}
	
	if(CheckIsNameUsed(selectedFolderPath, MaterialName))
	{
		MaterialName = TEXT("M_");
		return;
	}
	
	UMaterial* newMaterial = CreateMaterialAsset(MaterialName, selectedFolderPath);

	if(!newMaterial)
	{
		ShowMsg(EAppMsgType::Ok, TEXT("Failed to create material"));
		return;
	}
	uint32 count = 0;
	for(UTexture2D* texture : selectedTex)
	{
		if(!texture)
			continue;

		switch (ChannelPackingType)
		{
		case E_ChannelPacking::ECPT_NoChannelPacking:
			CreateMatNodes(newMaterial, texture, count);
			break;
		case E_ChannelPacking::ECPT_ORM:
			ORM_CreateMatNodes(newMaterial, texture, count);
			break;
		case E_ChannelPacking::ECPT_MAX:
			break;
		}
	}

	if(count>0)
		ShowNotify(TEXT("Successfully conected ") +  FString::FromInt(count) + TEXT(" pins"));

	if(bCreateMaterialInstance)
	{
		CreateMaterialInstanceAsset(newMaterial, MaterialName, selectedFolderPath);
	}

	MaterialName = TEXT("M_");
}

#pragma region CreateMaterialNodes

#define TRY_CONNECT_NODE(CheckFunction, ConnectFunction) \
if (!Material->CheckFunction()) \
{ \
if (ConnectFunction(textureSampleNode, Texture, Material)) \
{ \
PinsConnectedCount++; \
return; \
} \
}

void UMaterialCreationWidget::CreateMatNodes(UMaterial* Material, UTexture2D* Texture, uint32& PinsConnectedCount)
{
	auto* textureSampleNode = NewObject<UMaterialExpressionTextureSample>(Material);

	if(!textureSampleNode)
		return;
	
	TRY_CONNECT_NODE(HasBaseColorConnected, TryConnectBaseColor);
	TRY_CONNECT_NODE(HasMetallicConnected, TryConnectMetallic);
	TRY_CONNECT_NODE(HasRoughnessConnected, TryConnectRoughness);
	TRY_CONNECT_NODE(HasNormalConnected, TryConnectNormal);
	TRY_CONNECT_NODE(HasAmbientOcclusionConnected, TryConnectAmbientOcclusion);
}

void UMaterialCreationWidget::ORM_CreateMatNodes(UMaterial* Material, UTexture2D* Texture, uint32& PinsConnectedCount)
{
	auto* textureSampleNode = NewObject<UMaterialExpressionTextureSample>(Material);

	if(!textureSampleNode)
		return;

	TRY_CONNECT_NODE(HasBaseColorConnected, TryConnectBaseColor);
	TRY_CONNECT_NODE(HasNormalConnected, TryConnectNormal);
	TRY_CONNECT_NODE(HasRoughnessConnected, TryConnectORM);

}

bool UMaterialCreationWidget::TryConnectBaseColor(UMaterialExpressionTextureSample* TextureSampleNode,
                                                  UTexture2D* Texture, UMaterial* Material)
{
	for(const FString& baseColorName : BaseColorArray)
	{
		if(Texture->GetName().Contains(baseColorName))
		{
			TextureSampleNode->Texture = Texture;
			Material->GetExpressionCollection().AddExpression(TextureSampleNode);
			Material->GetExpressionInputForProperty(MP_BaseColor)->Connect(0,TextureSampleNode);
			Material->PostEditChange();

			TextureSampleNode->MaterialExpressionEditorX -= 600;
			return true;
		}
	}
	return false;
}

bool UMaterialCreationWidget::TryConnectMetallic(UMaterialExpressionTextureSample* TextureSampleNode,
	UTexture2D* Texture, UMaterial* Material)
{
	for(const FString& metallicName : MetallicArray)
	{
		if(Texture->GetName().Contains(metallicName))
		{
			Texture->CompressionSettings = TC_Default;
			Texture->SRGB = false;
			Texture->PostEditChange();
			
			TextureSampleNode->Texture = Texture;
			TextureSampleNode->SamplerType = SAMPLERTYPE_LinearColor;
			
			Material->GetExpressionCollection().AddExpression(TextureSampleNode);
			Material->GetExpressionInputForProperty(MP_Metallic)->Connect(0,TextureSampleNode);
			Material->PostEditChange();

			TextureSampleNode->MaterialExpressionEditorX -= 600;
			TextureSampleNode->MaterialExpressionEditorY += 240;
			return true;
		}
	}
	return false;
}

bool UMaterialCreationWidget::TryConnectRoughness(UMaterialExpressionTextureSample* TextureSampleNode,
	UTexture2D* Texture, UMaterial* Material)
{
	for(const FString& roughnessName : RoughnessArray)
	{
		if(Texture->GetName().Contains(roughnessName))
		{
			Texture->CompressionSettings = TC_Default;
			Texture->SRGB = false;
			Texture->PostEditChange();
			
			TextureSampleNode->Texture = Texture;
			TextureSampleNode->SamplerType = SAMPLERTYPE_LinearColor;
			
			Material->GetExpressionCollection().AddExpression(TextureSampleNode);
			Material->GetExpressionInputForProperty(MP_Roughness)->Connect(0,TextureSampleNode);
			Material->PostEditChange();

			TextureSampleNode->MaterialExpressionEditorX -= 600;
			TextureSampleNode->MaterialExpressionEditorY += 480;
			return true;
		}
	}
	return false;
}

bool UMaterialCreationWidget::TryConnectNormal(UMaterialExpressionTextureSample* TextureSampleNode, UTexture2D* Texture,
	UMaterial* Material)
{
	for(const FString& normalName : NormalArray)
	{
		if(Texture->GetName().Contains(normalName))
		{
			TextureSampleNode->Texture = Texture;
			TextureSampleNode->SamplerType = SAMPLERTYPE_Normal;
			
			Material->GetExpressionCollection().AddExpression(TextureSampleNode);
			Material->GetExpressionInputForProperty(MP_Normal)->Connect(0,TextureSampleNode);
			Material->PostEditChange();

			TextureSampleNode->MaterialExpressionEditorX -= 600;
			TextureSampleNode->MaterialExpressionEditorY += 720;
			return true;
		}
	}
	return false;
}

bool UMaterialCreationWidget::TryConnectAmbientOcclusion(UMaterialExpressionTextureSample* TextureSampleNode,
	UTexture2D* Texture, UMaterial* Material)
{
	for(const FString& ambientOcclusionName : AmbientOcclusionArray)
	{
		if(Texture->GetName().Contains(ambientOcclusionName))
		{
			Texture->CompressionSettings = TC_Default;
			Texture->SRGB = false;
			Texture->PostEditChange();
			
			TextureSampleNode->Texture = Texture;
			TextureSampleNode->SamplerType = SAMPLERTYPE_LinearColor;

			Material->GetExpressionCollection().AddExpression(TextureSampleNode);
			Material->GetExpressionInputForProperty(MP_AmbientOcclusion)->Connect(0,TextureSampleNode);
			Material->PostEditChange();

			TextureSampleNode->MaterialExpressionEditorX -= 600;
			TextureSampleNode->MaterialExpressionEditorY += 960;
			return true;
		}
	}
	return false;
}

bool UMaterialCreationWidget::TryConnectORM(UMaterialExpressionTextureSample* TextureSampleNode, UTexture2D* Texture,
	UMaterial* Material)
{
	for(const FString& ormName : ORMArray)
	{
		if(Texture->GetName().Contains(ormName))
		{
			Texture->CompressionSettings = TC_Masks;
			Texture->SRGB = false;
			Texture->PostEditChange();
			
			TextureSampleNode->Texture = Texture;
			TextureSampleNode->SamplerType = SAMPLERTYPE_Masks;

			Material->GetExpressionCollection().AddExpression(TextureSampleNode);
			Material->GetExpressionInputForProperty(MP_AmbientOcclusion)->Connect(1,TextureSampleNode);
			Material->GetExpressionInputForProperty(MP_Roughness)->Connect(2,TextureSampleNode);
			Material->GetExpressionInputForProperty(MP_Metallic)->Connect(3,TextureSampleNode);
			Material->PostEditChange();

			TextureSampleNode->MaterialExpressionEditorX -= 600;
			TextureSampleNode->MaterialExpressionEditorY += 240;
			return true;
		}
	}
	
	return false;
}

#pragma endregion


//Filter out textures and return false if non-texture selected
bool UMaterialCreationWidget::ProcessSelectedData(const TArray<FAssetData>& DataToProcess,
	TArray<UTexture2D*>& OutTexArray, FString& OutTexPath)
{
	if(!DataToProcess.Num())
	{
		ShowMsg(EAppMsgType::Ok, TEXT("No texture selected"));
		return false;
	}

	bool bMaterialNameSet = false;
	
	for(const FAssetData& data : DataToProcess)
	{
		UObject* asset = data.GetAsset();
		if(!asset)
			continue;

		UTexture2D* texture = Cast<UTexture2D>(asset);
		if(!texture)
		{
			ShowMsg(EAppMsgType::Ok, TEXT("Please select only textures\n" + asset->GetName()+ TEXT(" is not a texture")));
			return false;
		}

		OutTexArray.Add(texture);
		if(OutTexPath.IsEmpty())
			OutTexPath = data.PackagePath.ToString();

		if(!bCustomMaterialName && !bMaterialNameSet)
		{
			MaterialName = asset->GetName();
			MaterialName.RemoveFromStart(TEXT("T_"));
			MaterialName.InsertAt(0, TEXT("M_"));

			bMaterialNameSet = true;
		}
	}
	return true;
}

bool UMaterialCreationWidget::CheckIsNameUsed(const FString& FolderPathToCheck, const FString& MaterialNameToCheck)
{
	auto paths = UEditorAssetLibrary::ListAssets(FolderPathToCheck, false);
	for(const FString& path : paths)
	{
		const FString assetName = FPaths::GetBaseFilename(path);
		if(assetName.Equals(MaterialNameToCheck))
		{
			ShowMsg(EAppMsgType::Ok, MaterialNameToCheck + TEXT(" is already used"));
			return true;
		}
	}
	return false;
}

UMaterial* UMaterialCreationWidget::CreateMaterialAsset(const FString& Name, const FString& FolderPath)
{
	auto& assetToolsModule = FModuleManager::LoadModuleChecked<FAssetToolsModule>(TEXT("AssetTools"));
	auto* factory = NewObject<UMaterialFactoryNew>();
	UObject* newObject = assetToolsModule.Get().CreateAsset(Name, FolderPath, UMaterial::StaticClass(), factory);

	return Cast<UMaterial>(newObject);
}

UMaterialInstanceConstant* UMaterialCreationWidget::CreateMaterialInstanceAsset(UMaterial* Material,
	FString Name, const FString& FolderPath)
{
	Name.RemoveFromStart(TEXT("M_"));
	Name.InsertAt(0, TEXT("MI_"));

	auto& assetToolsModule = FModuleManager::LoadModuleChecked<FAssetToolsModule>(TEXT("AssetTools"));
	auto* factory = NewObject<UMaterialInstanceConstantFactoryNew>();
	UObject* newObject = assetToolsModule.Get().CreateAsset(Name, FolderPath, UMaterialInstanceConstant::StaticClass(), factory);

	if(auto* newMI = Cast<UMaterialInstanceConstant>(newObject))
	{
		newMI->SetParentEditorOnly(Material);
		newMI->PostEditChange();
		Material->PostEditChange();

		return newMI;
	}

	return nullptr;
}
