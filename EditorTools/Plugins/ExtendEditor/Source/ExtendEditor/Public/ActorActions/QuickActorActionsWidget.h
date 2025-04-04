// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EditorUtilityWidget.h"
#include "QuickActorActionsWidget.generated.h"

UENUM(BlueprintType)
enum class E_DuplicationAxis : uint8
{
	EDA_XAxis UMETA (DisplayName = "X Axis"),
	EDA_YAxis UMETA (DisplayName = "Y Axis"),
	EDA_ZAxis UMETA (DisplayName = "Z Axis"),
	EDA_MAX UMETA (DisplayName = "Default Max")
};

USTRUCT(BlueprintType)
struct FRandomActorRotation
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite) bool bRandomizeRotYaw = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (EditCondition="bRandomizeRotYaw")) float RotYawMin = -45.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (EditCondition="bRandomizeRotYaw")) float RotYawMax = 45.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite) bool bRandomizeRotPitch = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (EditCondition="bRandomizeRotPitch")) float RotPitchMin = -45.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (EditCondition="bRandomizeRotPitch")) float RotPitchMax = 45.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite) bool bRandomizeRotRoll = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (EditCondition="bRandomizeRotRoll")) float RotRollMin = -45.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (EditCondition="bRandomizeRotRoll")) float RotRollMax = 45.f;
	
};

USTRUCT(BlueprintType)
struct FRandomActorPosition
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite) bool bRandomizeX = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (EditCondition="bRandomizeX")) float XMin = -100.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (EditCondition="bRandomizeX")) float XMax = 100.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite) bool bRandomizeY = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (EditCondition="bRandomizeY")) float YMin = -100.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (EditCondition="bRandomizeY")) float YMax = 100.f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite) bool bRandomizeZ = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (EditCondition="bRandomizeZ")) float ZMin = -100.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (EditCondition="bRandomizeZ")) float ZMax = 100.f;
};

USTRUCT(BlueprintType)
struct FRandomActorSize
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite) bool bRandomizeSize = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (EditCondition="bRandomizeSize")) float SizeMin = 0.5f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (EditCondition="bRandomizeSize")) float SizeMax = 2.f;
};

UCLASS()
class EXTENDEDITOR_API UQuickActorActionsWidget : public UEditorUtilityWidget
{
	GENERATED_BODY()

public:
	//Actor selection
	UFUNCTION(BlueprintCallable) void SelectAllActorsWithSimilarName();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="ActorBatchSelection")
	TEnumAsByte<ESearchCase::Type> SearchCase = ESearchCase::IgnoreCase;

	//Actor duplication
	UFUNCTION(BlueprintCallable) void DuplicateActors();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="ActorBatchDuplication")
	E_DuplicationAxis DuplicationAxis = E_DuplicationAxis::EDA_XAxis;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="ActorBatchDuplication")
	int32 NumberOfDuplicates = 5;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="ActorBatchDuplication")
	float OffsetDist = 300.f;

	//Actor randomization
	UFUNCTION(BlueprintCallable) void RandomizeActorTransform();
	UFUNCTION(BlueprintCallable) void RandomizeActorPosition(AActor* SelectedActor);
	UFUNCTION(BlueprintCallable) void RandomizeActorRotation(AActor* SelectedActor);
	UFUNCTION(BlueprintCallable) void RandomizeActorSize(AActor* SelectedActor);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="RandomizeActorTransform")
	FRandomActorPosition PosConfig;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="RandomizeActorTransform")
	FRandomActorRotation RotConfig;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="RandomizeActorTransform")
	FRandomActorSize SizeConfig;
	
private:
	UPROPERTY()
	class UEditorActorSubsystem* EditorActorSubsystem = nullptr;

	bool GetEditorActorSubsystem();
	
};
