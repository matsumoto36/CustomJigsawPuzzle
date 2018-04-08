// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

#include "Piece.h"

#include "ProceduralMeshComponent.h"
#include "Runtime/Engine/Classes/Components/SplineComponent.h"
#include "Runtime/Core/Public/Math/TransformNonVectorized.h"
#include "PieceGenerator.generated.h"

UCLASS()
class APieceGenerator : public AActor
{
	GENERATED_BODY()
	
private:

	class Offsets {

	public:
		const float xMin;
		const float xMax;
		const float yMin;
		const float yMax;

		Offsets(float xMin, float xMax, float yMin, float yMax) :
			xMin(xMin), xMax(xMax), yMin(yMin), yMax(yMax) {}
	};

	Offsets baseLineOffsets = Offsets(0.4, 0.45, -0.05, 0.15);
	Offsets upperOffsets = Offsets(0.3, 0.44, -0.25, -0.1);

public:

	// Sets default values for this actor's properties
	APieceGenerator();

	UFUNCTION(BlueprintCallable)
		APiece* SpawnPiece(FTransform SpawnTransform);
	
	UFUNCTION(BlueprintCallable)
		bool CreatePieceMesh(UProceduralMeshComponent* MeshComponent, TArray<FVector> PieceLinePoints);

	UFUNCTION(BlueprintCallable)
		const TArray<int32> ConvexPartitioning(TArray<FVector> RoundVertices, bool debugDraw = false);
		
	UFUNCTION(BlueprintCallable)
		const TArray<FVector> CreatePieceRoundVertices(TArray<USplineComponent*> SplineArray, int Partition);

	UFUNCTION(BlueprintCallable)
		USplineComponent* CreateSpline(TArray<FVector> Points);
	
	UFUNCTION(BlueprintCallable)
		const TArray<FVector> CreateJigsawSplinePoints();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	
};
