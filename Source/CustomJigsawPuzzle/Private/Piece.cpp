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

	//メッシュを生成
	PieceMesh = CreateDefaultSubobject<UProceduralMeshComponent>(TEXT("PieceMesh0"));
	PieceMesh->SetupAttachment(RootComponent);
	PieceMesh->SetCollisionProfileName("PhysicsActor");
	
	//コリジョン設定
	SetActorEnableCollision(true);
	PieceMesh->bUseComplexAsSimpleCollision = false;
	PieceMesh->SetAllUseCCD(true);
	PieceMesh->SetSimulatePhysics(true);
	PieceMesh->SetEnableGravity(true);
	PieceMesh->bGenerateOverlapEvents = false;
	PieceMesh->bAlwaysCreatePhysicsState = true;
	PieceMesh->GetBodyInstance()->SleepFamily = ESleepFamily::Custom;
	PieceMesh->GetBodyInstance()->CustomSleepThresholdMultiplier = 0;

	//色情報を読み込む
	PieceColorParam = ConstructorStatics.PieceColorParam.Get();
	BaseEmissionColor = PieceColorParam->GetVectorParameterByName(MPC_BASE_COLOR)->DefaultValue;
	ActiveEmissionColor = PieceColorParam->GetVectorParameterByName(MPC_ACTIVE_COLOR)->DefaultValue;
	SelectEmissionColor = PieceColorParam->GetVectorParameterByName(MPC_SELECT_COLOR)->DefaultValue;

	//マテリアルを生成
	PieceMaterial =
		UMaterialInstanceDynamic::Create(ConstructorStatics.PieceMaterial.Get(), NULL);
	PieceMaterial->SetVectorParameterValue(EMISSION_PARAM, BaseEmissionColor);
	
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
			PieceMesh->SetSimulatePhysics(true);
			PieceMesh->SetEnableGravity(true);
			PieceMesh->SetCollisionProfileName("PhysicsActor");
			break;

		case EPieceState::EPieceSelect:
			PieceMaterial->SetVectorParameterValue(EMISSION_PARAM, SelectEmissionColor);
			PieceMesh->SetSimulatePhysics(false);
			PieceMesh->SetEnableGravity(false);
			PieceMesh->SetCollisionProfileName("NoCollision");
			break;

		case EPieceState::ENone:
		default:
			PieceMaterial->SetVectorParameterValue(EMISSION_PARAM, BaseEmissionColor);
			PieceMesh->SetSimulatePhysics(true);
			PieceMesh->SetEnableGravity(true);
			PieceMesh->SetCollisionProfileName("PhysicsActor");
			break;
	}

	CurrentState = State;

}

// Called when the game starts or when spawned
void APiece::BeginPlay() {
	Super::BeginPlay();
	PieceMesh->WakeAllRigidBodies();
}