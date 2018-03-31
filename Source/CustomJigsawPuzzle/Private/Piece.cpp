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

	// Structure to hold one-time initialization
	struct FConstructorStatics {

		ConstructorHelpers::FObjectFinderOptional<UStaticMesh> PieceMesh;
		ConstructorHelpers::FObjectFinderOptional<UMaterial> BaseMaterial;
		ConstructorHelpers::FObjectFinderOptional<UMaterialInstance> ActiveMaterial;
		ConstructorHelpers::FObjectFinderOptional<UMaterialInstance> SelectMaterial;
		FConstructorStatics()
			: PieceMesh(TEXT("/Game/Meshes/SM_TestPiece.SM_TestPiece"))
			, BaseMaterial(TEXT("/Game/Materials/M_PieceBase.M_PieceBase"))
			, ActiveMaterial(TEXT("/Game/Materials/MI_PieceActive.MI_PieceActive"))
			, SelectMaterial(TEXT("/Game/Materials/MI_PieceSelect.MI_PieceSelect"))
		{}
	};

	static FConstructorStatics ConstructorStatics;

	// Create dummy root scene component
	DummyRoot = CreateDefaultSubobject<USceneComponent>(TEXT("Dummy0"));
	RootComponent = DummyRoot;

	// Create static mesh component
	BlockMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BlockMesh0"));
	BlockMesh->SetStaticMesh(ConstructorStatics.PieceMesh.Get());
	BlockMesh->SetMaterial(0, ConstructorStatics.BaseMaterial.Get());
	BlockMesh->SetupAttachment(DummyRoot);
	BlockMesh->SetSimulatePhysics(true);
	BlockMesh->SetEnableGravity(true);

	// Save a pointer to the orange material
	BaseMaterial = ConstructorStatics.BaseMaterial.Get();
	ActiveMaterial = ConstructorStatics.ActiveMaterial.Get();
	SelectMaterial = ConstructorStatics.SelectMaterial.Get();
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

	FRotator Rotation = BlockMesh->GetComponentRotation();
	float rotSpeed = 1000.0f * DeltaTime;
	
	Rotation.Pitch += fabsf(Rotation.Pitch) > rotSpeed ? Rotation.Pitch > 0 ? -rotSpeed : rotSpeed : -Rotation.Pitch;
	Rotation.Roll += fabsf(Rotation.Roll) > rotSpeed ? Rotation.Roll > 0 ? -rotSpeed : rotSpeed : -Rotation.Roll;
	Rotation.Yaw += fabsf(Rotation.Yaw) > rotSpeed ? Rotation.Yaw > 0 ? -rotSpeed : rotSpeed : -Rotation.Yaw;

	BlockMesh->SetWorldRotation(Rotation);
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
			BlockMesh->SetMaterial(0, ActiveMaterial);
			BlockMesh->SetSimulatePhysics(true);
			BlockMesh->SetEnableGravity(true);
			break;

		case EPieceState::EPieceSelect:
			BlockMesh->SetMaterial(0, SelectMaterial);
			BlockMesh->SetSimulatePhysics(false);
			BlockMesh->SetEnableGravity(false);
			break;

		case EPieceState::ENone:
		default:
			BlockMesh->SetMaterial(0, BaseMaterial);
			BlockMesh->SetSimulatePhysics(true);
			BlockMesh->SetEnableGravity(true);
			break;
	}

	CurrentState = State;

}

// Called when the game starts or when spawned
void APiece::BeginPlay() {
	Super::BeginPlay();

}