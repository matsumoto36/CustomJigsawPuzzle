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
		ConstructorHelpers::FObjectFinderOptional<UMaterialInstance> PieceMaterial;
		ConstructorHelpers::FObjectFinderOptional<UMaterialParameterCollection> PieceColorParam;
		FConstructorStatics()
			: PieceMaterial(TEXT("/Game/Materials/MI_PieceBase.MI_PieceBase")),
			PieceColorParam(TEXT("/Game/Materials/MPC_Piece.MPC_Piece")) {}
	};
	static FConstructorStatics ConstructorStatics;

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

	//色情報を読み込む
	PieceColorParam = ConstructorStatics.PieceColorParam.Get();
	BaseEmissionColor = PieceColorParam->GetVectorParameterByName(MPC_BASE_COLOR)->DefaultValue;
	ActiveEmissionColor = PieceColorParam->GetVectorParameterByName(MPC_ACTIVE_COLOR)->DefaultValue;
	SelectEmissionColor = PieceColorParam->GetVectorParameterByName(MPC_SELECT_COLOR)->DefaultValue;

	//マテリアルを生成
	PieceMaterial =
		UMaterialInstanceDynamic::Create(ConstructorStatics.PieceMaterial.Get(), NULL);
	PieceMaterial->SetVectorParameterValue(EMISSION_PARAM, ActiveEmissionColor);
	
	PieceMesh->SetMaterial(0, PieceMaterial);

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
			PieceMaterial->SetVectorParameterValue(EMISSION_PARAM, ActiveEmissionColor);
			PieceCollision->SetSimulatePhysics(true);
			PieceCollision->SetEnableGravity(true);
			PieceCollision->SetCollisionProfileName("BlockAll");
			break;

		case EPieceState::EPieceSelect:
			PieceMaterial->SetVectorParameterValue(EMISSION_PARAM, SelectEmissionColor);
			PieceCollision->SetSimulatePhysics(false);
			PieceCollision->SetEnableGravity(false);
			PieceCollision->SetCollisionProfileName("NoCollision");
			break;

		case EPieceState::ENone:
		default:
			PieceMaterial->SetVectorParameterValue(EMISSION_PARAM, BaseEmissionColor);
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