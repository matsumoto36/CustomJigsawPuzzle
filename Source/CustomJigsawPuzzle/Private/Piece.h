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

public:

	APiece();

	UFUNCTION(BlueprintCallable)
		UProceduralMeshComponent* GetBody() { return PieceMesh; }

	UFUNCTION(BlueprintCallable)
		void SetPieceScale(FVector NewScale) {
		SetActorScale3D(NewScale);
		//‚±‚ê‚ð“ü‚ê‚È‚¢‚ÆƒRƒŠƒWƒ‡ƒ“‚ª•sˆÀ’è‚É‚È‚é
		UpdateComponentTransforms();
	}

	UFUNCTION(BlueprintCallable)
		void SetPuzzleTexture(UTexture2D* NewTexture) { PieceMaterial->SetTextureParameterValue(TEXTURE_PARAM, NewTexture); }

	UFUNCTION()
		void HandleMouseDown();

	UFUNCTION()
		void HandleMouseUp();

	UFUNCTION()
		void MouseCursorOverBigin(UPrimitiveComponent* TouchedComponent);

	UFUNCTION()
		void MouseCursorOverEnd(UPrimitiveComponent* TouchedComponent);

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

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

};