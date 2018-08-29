// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Runtime/Engine/Classes/Components/BoxComponent.h"
#include "Runtime/Engine/Classes/Materials/MaterialInstanceDynamic.h"
//#include "Materials/MaterialInstanceDynamic.h"
#include "Runtime/Engine/Classes/Materials/MaterialParameterCollection.h"
//#include "Materials/MaterialParameterCollection.h"
#include "ProceduralMeshComponent.h"

#include "PieceInterface.h"
#include "Piece.generated.h"

UENUM(BlueprintType)
enum class EPieceState : uint8 {
	ENone,
	EPieceActive,
	EPieceSelect,
};

UENUM(BlueprintType)
enum class EPieceSide : uint8 {
	ELeft = 0,
	ETop,
	ERight,
	EBottom,
};

UCLASS()
class CUSTOMJIGSAWPUZZLE_API APiece : public AActor, public IPieceInterface
{
	GENERATED_BODY()

private:

	const FName TEXTURE_PARAM = "PuzzleTexture";
	const FName TEXTURE_UTILE_PARAM = "UTile";
	const FName TEXTURE_VTILE_PARAM = "VTile";
	const FName TEXTURE_UPOSITION_PARAM = "UPosition";
	const FName TEXTURE_VPOSITION_PARAM = "VPosition";
	const FName MPC_BASE_COLOR = "BaseEmissionColor";
	const FName MPC_ACTIVE_COLOR = "ActiveEmissionColor";
	const FName MPC_SELECT_COLOR = "SelectEmissionColor";
	const FName EMISSION_PARAM = "Emission";
	const float SLEEP_TIME = 2.0f;

	UPROPERTY()
		UMaterialParameterCollection* PieceColorParam;

	UPROPERTY(EditAnywhere)
		FLinearColor BaseEmissionColor;

	UPROPERTY(EditAnywhere)
		FLinearColor ActiveEmissionColor;

	UPROPERTY(EditAnywhere)
		FLinearColor SelectEmissionColor;
	
	UPROPERTY(Category = Block, VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
		class USceneComponent* DummyRoot;

	UPROPERTY(Category = Block, VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
		UProceduralMeshComponent* PieceMesh;

	UPROPERTY()
		UMaterialInstanceDynamic* PieceMaterial;

	int PieceMapPosX;
	int PieceMapPosY;

	bool IsActiveSleepTimer = false;
	float SleepTimer = 0.0f;

public:

	EPieceState CurrentState;

public:

	APiece();

	virtual FVector GetPosition_Implementation() override;

	virtual bool SetPosition_Implementation(FVector Position) override;

	virtual bool Select_Implementation() override;

	virtual bool UnSelect_Implementation() override;

	virtual bool SetActive_Implementation(bool Enable) override;

	UFUNCTION(BlueprintCallable)
		UProceduralMeshComponent* GetBody() { return PieceMesh; }

	UFUNCTION(BlueprintCallable)
		void GetPieceMapPosition(int &MapPosX, int &MapPosY) {
		MapPosX = PieceMapPosX;
		MapPosY = PieceMapPosY;
	}

	UFUNCTION(BlueprintCallable)
		void SetPieceMapPosition(int MapPosX, int &MapPosY) {
		PieceMapPosX = MapPosX;
		PieceMapPosY = MapPosY;
	}

	UFUNCTION(BlueprintCallable)
		void SetPieceScale(FVector NewScale) {
		SetActorScale3D(NewScale);
	}

	UFUNCTION(BlueprintCallable)
		void SetPiecePosition(FVector Position) { SetActorLocation(Position); }

	UFUNCTION(BlueprintCallable)
		void SetPuzzleTexture(UTexture2D* NewTexture, float UTile, float VTile, float UPosition, float VPosition) {
		PieceMaterial->SetTextureParameterValue(TEXTURE_PARAM, ((UTexture*)NewTexture));
		PieceMaterial->SetScalarParameterValue(TEXTURE_UTILE_PARAM, UTile);
		PieceMaterial->SetScalarParameterValue(TEXTURE_VTILE_PARAM, VTile);
		PieceMaterial->SetScalarParameterValue(TEXTURE_UPOSITION_PARAM, UPosition);
		PieceMaterial->SetScalarParameterValue(TEXTURE_VPOSITION_PARAM, VPosition);
	}

	UFUNCTION(BlueprintCallable)
		bool CheckConnection(APiece* OtherPiece, EPieceSide Side);

	UFUNCTION()
		void HandleMouseDown();

	UFUNCTION()
		void HandleMouseUp();

	UFUNCTION()
		void Highlight(bool bOn);

	UFUNCTION()
		/* ��]��߂� */
		void RollingDefault(float DeltaTime);

	// Called every frame
	virtual void Tick(float DeltaTime) override;

protected:

	UFUNCTION()
		void ChangePieceState(EPieceState State);

	UFUNCTION()
		void StartCollisionSleepTimer();

	UFUNCTION()
		void CancelCollisionSleepTimer();

	UFUNCTION()
		void OnHit();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

};