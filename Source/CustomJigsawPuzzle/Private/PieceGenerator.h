// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PieceGenerator.generated.h"

UCLASS()
class APieceGenerator : public AActor
{
	GENERATED_BODY()
	
public:

	// Sets default values for this actor's properties
	APieceGenerator();

	void SpawnPiece(int width, int height);

	// Called every frame
	virtual void Tick(float DeltaTime) override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	
};
