// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Camera/CameraComponent.h"
#include "PlayerPawn.generated.h"

UCLASS()
class CUSTOMJIGSAWPUZZLE_API APlayerPawn : public APawn
{
	GENERATED_BODY()

private:
	
	const float PIECE_POSITION_Z = 10;

	FVector MoveVector = FVector();

protected:

	UPROPERTY(EditInstanceOnly, BlueprintReadWrite)
		float SelectedPieceHeight = 10.0f;

	UPROPERTY(EditInstanceOnly, BlueprintReadWrite)
		TScriptInterface<class IPieceInterface> CurrentPieceFocus;

	UPROPERTY(EditInstanceOnly, BlueprintReadWrite)
		FVector PieceMovePosition;

	UPROPERTY(EditInstanceOnly, BlueprintReadWrite)
		FVector2D MaxMovePieceArea = FVector2D(40.0f, 40.0f);

public:

	UPROPERTY(EditInstanceOnly, BlueprintReadWrite)
		bool bIsSelectPiece = false;

	UPROPERTY(EditInstanceOnly, BlueprintReadWrite)
		float MoveSpeed = 200.0f;

	UPROPERTY(EditInstanceOnly, BlueprintReadWrite)
		UCameraComponent* MainCamera;

	// Sets default values for this pawn's properties
	APlayerPawn();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual void CalcCamera(float DeltaTime, struct FMinimalViewInfo& OutResult) override;

protected:

	void TraceForBlock(const FVector& Start, const FVector& End, bool bDrawDebugHelpers);

	virtual void TriggerMouseDown();
	virtual void TriggerMouseUp();

	virtual void MoveHorizontal(float axis);
	virtual void MoveVertical(float axis);
	virtual void MoveForword(float axis);

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:

	bool CalcPieceLocation(FVector &PieceLocation);

	void UpdatePieceMove(float DeltaTime);

	void UpdatePlayerMove(float DeltaTime);
};