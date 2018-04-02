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

	EPieceState CurrentState;

private:

	UPROPERTY(Category = Block, VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
		class USceneComponent* DummyRoot;

	UPROPERTY(Category = Block, VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
		UStaticMeshComponent* PieceMesh;

	UPROPERTY()
		class UMaterial* BaseMaterial;

	UPROPERTY()
		class UMaterialInstance* ActiveMaterial;

	UPROPERTY()
		class UMaterialInstance* SelectMaterial;

public:

	APiece();

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

	UStaticMeshComponent* GetBody() { return PieceMesh; }

	void SetMeshAndMaterial(UStaticMesh* NewMesh, UMaterial* NewMaterial){ 
		PieceMesh->SetStaticMesh(NewMesh); 
		PieceMesh->SetMaterial(0, NewMaterial);
	}

	// Called every frame
	virtual void Tick(float DeltaTime) override;

protected:

	UFUNCTION()
		void ChangePieceState(EPieceState State);

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

};