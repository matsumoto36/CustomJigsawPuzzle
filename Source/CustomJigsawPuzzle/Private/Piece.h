// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Runtime/Engine/Classes/Components/BoxComponent.h"
#include "Runtime/Engine/Classes/Materials/MaterialInstanceDynamic.h"
#include "Runtime/Engine/Classes/Materials/MaterialParameterCollection.h"
#include "ProceduralMeshComponent.h"
#include "Piece.generated.h"

UENUM(BlueprintType)
enum class EPieceState : uint8 {
	ENone,
	EPieceActive,
	EPieceSelect,
};

UCLASS()
class APiece : public AActor
{
	GENERATED_BODY()
	
public:

	EPieceState CurrentState;

private:

	const FName TEXTURE_PARAM = "PuzzleTexture";
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

	bool IsActiveSleepTimer = false;
	float SleepTimer = 0.0f;

public:

	APiece();

	UFUNCTION(BlueprintCallable)
		UProceduralMeshComponent* GetBody() { return PieceMesh; }

	UFUNCTION(BlueprintCallable)
		void SetPieceScale(FVector NewScale) {
		SetActorScale3D(NewScale);
	}

	UFUNCTION(BlueprintCallable)
		void SetPuzzleTexture(UTexture2D* NewTexture) { PieceMaterial->SetTextureParameterValue(TEXTURE_PARAM, NewTexture); }

	UFUNCTION()
		void HandleMouseDown();

	UFUNCTION()
		void HandleMouseUp();

	UFUNCTION()
		void Highlight(bool bOn);

	UFUNCTION()
		/* ‰ñ“]‚ð–ß‚· */
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