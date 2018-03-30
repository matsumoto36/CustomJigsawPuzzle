// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "PlayerPawn.generated.h"

UCLASS()
class CUSTOMJIGSAWPUZZLE_API APlayerPawn : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	APlayerPawn();

protected:

	UPROPERTY(EditInstanceOnly, BlueprintReadWrite)
		class APiece* CurrentPieceFocus;

	void TraceForBlock(const FVector& Start, const FVector& End, bool bDrawDebugHelpers);

	void TriggerMouseDown();

	void TriggerMouseUp();

public:


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual void CalcCamera(float DeltaTime, struct FMinimalViewInfo& OutResult) override;
};
