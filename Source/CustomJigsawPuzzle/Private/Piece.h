// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
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
	
	// Sets default values for this actor's properties
	APiece();

	EPieceState CurrentState;

	UPROPERTY(Category = Block, VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
		class USceneComponent* DummyRoot;

	UPROPERTY(Category = Block, VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
		class UStaticMeshComponent* BlockMesh;

	UPROPERTY()
		class UMaterial* BaseMaterial;

	UPROPERTY()
		class UMaterialInstance* ActiveMaterial;

	UPROPERTY()
		class UMaterialInstance* SelectMaterial;

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

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION()
		void ChangePieceState(EPieceState State);

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};

