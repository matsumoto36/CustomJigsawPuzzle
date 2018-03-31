// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "PlayerPawn.generated.h"

UCLASS()
class CUSTOMJIGSAWPUZZLE_API APlayerPawn : public APawn
{
	GENERATED_BODY()

protected:

	UPROPERTY(EditInstanceOnly, BlueprintReadWrite)
		float SelectedPieceHeight = 10.0f;

	UPROPERTY(EditInstanceOnly, BlueprintReadWrite)
		class APiece* CurrentPieceFocus;

	UPROPERTY(EditInstanceOnly, BlueprintReadWrite)
		FVector PieceMovePosition;

	UPROPERTY(EditInstanceOnly, BlueprintReadWrite)
		FVector2D MaxMoveArea = FVector2D(40.0f, 40.0f);

public:

	bool bIsSelectPiece = false;

	// Sets default values for this pawn's properties
	APlayerPawn();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual void CalcCamera(float DeltaTime, struct FMinimalViewInfo& OutResult) override;

protected:

	void TraceForBlock(const FVector& Start, const FVector& End, bool bDrawDebugHelpers);

	void TriggerMouseDown();

	void TriggerMouseUp();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:

	bool CalcPieceLocation(FVector &PieceLocation);
};