// Fill out your copyright notice in the Description page of Project Settings.

#include "Piece.h"
#include "UObject/ConstructorHelpers.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/StaticMesh.h"
#include "Materials/MaterialInstance.h"

#include "EngineGlobals.h"
#include "Runtime/Engine/Classes/Engine/Engine.h"

// Sets default values
APiece::APiece()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	CurrentState = EPieceState::ENone;

	// Create dummy root scene component
	DummyRoot = CreateDefaultSubobject<USceneComponent>(TEXT("Dummy0"));
	RootComponent = DummyRoot;

	//コリジョンを生成
	PieceCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("Box Collision"));
	PieceCollision->SetupAttachment(DummyRoot);
	PieceCollision->SetCollisionProfileName("BlockAll");
	PieceCollision->SetSimulatePhysics(true);
	PieceCollision->SetEnableGravity(true);
	PieceCollision->SetBoxExtent(FVector(10, 10, 1));

	//メッシュを生成
	PieceMesh = CreateDefaultSubobject<UProceduralMeshComponent>(TEXT("PieceMesh0"));
	PieceMesh->SetCollisionProfileName("NoCollision");
	PieceMesh->SetupAttachment(PieceCollision);
}

void APiece::HandleMouseDown() {
	ChangePieceState(EPieceState::EPieceSelect);
}

void APiece::HandleMouseUp() {
	ChangePieceState(EPieceState::EPieceActive);
}

void APiece::MouseCursorOverBigin(UPrimitiveComponent* TouchedComponent) {
	Highlight(true);
}

void APiece::MouseCursorOverEnd(UPrimitiveComponent* TouchedComponent) {
	Highlight(false);
}

void APiece::Highlight(bool bOn) {

	if (bOn) {
		ChangePieceState(EPieceState::EPieceActive);
	}
	else {
		ChangePieceState(EPieceState::ENone);
	}
}

void APiece::RollingDefault(float DeltaTime) {

	FRotator Rotation = PieceMesh->GetComponentRotation();
	float rotSpeed = 1000.0f * DeltaTime;
	
	Rotation.Pitch += fabsf(Rotation.Pitch) > rotSpeed ? Rotation.Pitch > 0 ? -rotSpeed : rotSpeed : -Rotation.Pitch;
	Rotation.Roll += fabsf(Rotation.Roll) > rotSpeed ? Rotation.Roll > 0 ? -rotSpeed : rotSpeed : -Rotation.Roll;
	Rotation.Yaw += fabsf(Rotation.Yaw) > rotSpeed ? Rotation.Yaw > 0 ? -rotSpeed : rotSpeed : -Rotation.Yaw;

	PieceMesh->SetWorldRotation(Rotation);
}

// Called every frame
void APiece::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);

	switch (CurrentState) {
		case EPieceState::EPieceActive:
			break;
		case EPieceState::EPieceSelect:
			RollingDefault(DeltaTime);
			break;
		case EPieceState::ENone:
		default:
			break;
	}
}

void APiece::ChangePieceState(EPieceState State) {

	if (State == CurrentState) return;

	switch (State) {
		case EPieceState::EPieceActive:
			PieceMesh->SetMaterial(0, ActiveMaterial);
			PieceCollision->SetSimulatePhysics(true);
			PieceCollision->SetEnableGravity(true);
			PieceCollision->SetCollisionProfileName("BlockAll");
			break;

		case EPieceState::EPieceSelect:
			PieceMesh->SetMaterial(0, SelectMaterial);
			PieceCollision->SetSimulatePhysics(false);
			PieceCollision->SetEnableGravity(false);
			PieceCollision->SetCollisionProfileName("NoCollision");
			break;

		case EPieceState::ENone:
		default:
			PieceMesh->SetMaterial(0, BaseMaterial);
			PieceCollision->SetSimulatePhysics(true);
			PieceCollision->SetEnableGravity(true);
			PieceCollision->SetCollisionProfileName("BlockAll");
			break;
	}

	CurrentState = State;

}

// Called when the game starts or when spawned
void APiece::BeginPlay() {
	Super::BeginPlay();

}