// Fill out your copyright notice in the Description page of Project Settings.

#include "Piece.h"
#include "PuzzleGameMode.h"
#include "UObject/ConstructorHelpers.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/StaticMesh.h"
#include "Materials/MaterialInstance.h"
#include "Runtime/Engine/Classes/PhysicsEngine/PhysicsConstraintActor.h"
#include "Runtime/Engine/Classes/PhysicsEngine/PhysicsConstraintComponent.h"

#include "EngineGlobals.h"
#include "Runtime/Engine/Classes/Engine/Engine.h"

// Sets default values
APiece::APiece() {

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
	RootComponent = PieceMesh;
	PieceMesh->SetCollisionProfileName("PhysicsActor");
	PieceMesh->ComponentTags.Add(TEXT("PieceMesh"));
	//PieceMesh->bEnableAutoLODGeneration = true; //パッケージに失敗する*

	//コリジョン設定
	SetActorEnableCollision(true);
	PieceMesh->SetNotifyRigidBodyCollision(true);
	PieceMesh->bUseComplexAsSimpleCollision = false;
	PieceMesh->SetAllUseCCD(true);
	PieceMesh->SetSimulatePhysics(true);
	PieceMesh->SetEnableGravity(true);
	PieceMesh->SetGenerateOverlapEvents(false);
	PieceMesh->bAlwaysCreatePhysicsState = true;

	//イベントの設定
	auto hitMethod = FScriptDelegate();
	hitMethod.BindUFunction(this, "OnHit");
	PieceMesh->OnComponentHit.Add(hitMethod);

	//接続のコリジョンを設定
	TArray<FVector> cyclePoints = {
		FVector(0, -0.5, 0),
		FVector(0.5,0, 0),
		FVector(0, 0.5, 0),
		FVector(-0.5, 0, 0),
	};

	for (int i = 0; i < 4; i++) {
		auto num = FString::FromInt(i);
		auto name = "PieceCollision" + num;
		auto collision = CreateDefaultSubobject<UBoxComponent>(FName(*name));
		collision->SetGenerateOverlapEvents(true);
		collision->SetupAttachment(RootComponent);
		collision->SetWorldScale3D(SIDE_COLLISION_SIZE);
		collision->SetWorldLocationAndRotation(cyclePoints[i], FRotator(0, i * 90, 0));
		collision->SetCollisionProfileName(TEXT("OverlapAll"));
		//方向を識別するために番号を入れておく(EPieceSideと同じ)
		collision->ComponentTags.Add(FName(*num));
		collision->ComponentTags.Add(SIDE_COLLISION_TAGNAME);

		//イベントの設定
		auto overlapEvent = FScriptDelegate();
		overlapEvent.BindUFunction(this, "OnOverlapBegin");
		collision->OnComponentBeginOverlap.Add(overlapEvent);
		overlapEvent = FScriptDelegate();
		overlapEvent.BindUFunction(this, "OnOverlapEnd");
		collision->OnComponentEndOverlap.Add(overlapEvent);

		auto info = FPieceSideCollisionInfo();
		info.Collision = collision;
		info.IsHitting = false;
		info.HitOtherPiece = nullptr;
		PieceSideCollisionInfo.Emplace(info);
	}

	//SetActiveSideCollision(false);

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

APiece::~APiece() {

}

FVector APiece::GetPosition_Implementation() {
	return PieceMesh->GetComponentLocation() - mouseOffset;
}

bool APiece::SetPosition_Implementation(FVector Position) {
	//SetActorLocation(Position);
	PieceMesh->SetWorldLocation(Position + mouseOffset);
	return true;
}

bool APiece::Select_Implementation(FVector ClickPos) {
	PieceMesh->SetSimulatePhysics(false);
	mouseOffset = PieceMesh->GetComponentLocation() - ClickPos;
	mouseOffset.Z = 0;
	CancelCollisionSleepTimer();
	ChangePieceState(EPieceState::EPieceSelect);
	return true;
}

bool APiece::UnSelect_Implementation() {

	//離した瞬間に接続判定
	for (int i = 0; i < 4;i++) {
		if (!PieceSideCollisionInfo[i].IsHitting) continue;
		CheckConnection(PieceSideCollisionInfo[i].HitOtherPiece, static_cast<EPieceSide>(i));
	}

	PieceMesh->SetSimulatePhysics(true);
	StartCollisionSleepTimer();
	ChangePieceState(EPieceState::EPieceActive);

	return true;
}

bool APiece::SetActive_Implementation(bool Enable) {
	if (Enable) {
		ChangePieceState(EPieceState::EPieceActive);
	}
	else {
		ChangePieceState(EPieceState::ENone);
	}
	return true;
}

bool APiece::CheckConnection(APiece* OtherPiece, EPieceSide Side) {
	
	//合ってない場合はリターン
	int x, y;
	OtherPiece->GetPieceMapPosition(x, y);
	auto mapPos = FVector2D(PieceMapPosX, PieceMapPosY);
	auto otherMapPos = FVector2D(x, y);
	auto sideVector = GetVector2D(Side);
	if (mapPos + sideVector != otherMapPos) return false;

	auto otherSide = static_cast<EPieceSide>((static_cast<int>(Side) + 6) % 4);

	//角度差が少ないときのみ通す
	auto vec1 = GetPieceDirection(Side);
	auto vec2 = -OtherPiece->GetPieceDirection(otherSide);
	auto angle = FMath::RadiansToDegrees(FMath::Acos(FVector::DotProduct(vec1, vec2)));

	UE_LOG(LogTemp, Log, TEXT("angle = %f"), angle);

	if (angle > 20) return false;
	
	UE_LOG(LogTemp, Log, TEXT("Connection Start"));

	//くっつける
	// todo 接続の判定を切る
	PieceSideCollisionInfo[static_cast<int>(Side)].Collision->SetGenerateOverlapEvents(false);
	OtherPiece
		->PieceSideCollisionInfo[static_cast<int>(otherSide)]
		.Collision
		->SetGenerateOverlapEvents(false);

	auto scale = PieceMesh->GetComponentScale();

	//論理的なグループを設定
	if (!pieceOwner) {
		auto group = NewObject<class UPieceGroup>();
		auto offset = 
			GetPieceDirection(EPieceSide::ERight) * PieceMapPosX * scale.X +
			GetPieceDirection(EPieceSide::EBottom) * PieceMapPosY * scale.Y;

		group->SetGroupPosition(IPieceInterface::Execute_GetPosition(this) - offset);
		group->SetMouseOffset(-offset);
		group->AddGroup(this);
		auto ipiece = TScriptInterface<IPieceInterface>();
		ipiece.SetObject(group);
		ipiece.SetInterface(Cast<IPieceInterface>(group));
		SetPieceOwner(ipiece);
	}

	auto otherOwner = OtherPiece->pieceOwner;
	auto myGroup = Cast<UPieceGroup>(pieceOwner.GetObject());

	if (!myGroup) {
		UE_LOG(LogTemp, Error, TEXT("CAST myGroup is null"));
	}

	//物理を固定
	auto position = PieceMesh->GetComponentLocation();

	//XYのスケールが同じことを前提
	auto otherPos = position + GetPieceDirection(Side) * scale.X;

	UE_LOG(LogTemp, Log, TEXT("%d, %d"), x, y);
	OtherPiece->PieceMesh->SetWorldLocationAndRotation(otherPos, PieceMesh->GetComponentRotation());
	//OtherPiece->PieceMesh->UpdateComponentToWorld();

	auto constraint = GetWorld()
		->SpawnActor<APhysicsConstraintActor>()
		->GetConstraintComp();

	//制約をセットする
	constraint->SetDisableCollision(true);

	constraint->SetLinearXLimit(LCM_Locked, 0);
	constraint->SetLinearYLimit(LCM_Locked, 0);
	constraint->SetLinearZLimit(LCM_Locked, 0);
	constraint->SetLinearPositionDrive(false, false, false);
	constraint->SetLinearDriveParams(0, 0, 0);

	constraint->SetAngularSwing1Limit(ACM_Locked, 0);
	constraint->SetAngularSwing2Limit(ACM_Locked, 0);
	constraint->SetAngularTwistLimit(ACM_Locked, 0);
	constraint->SetAngularOrientationDrive(false, false);
	constraint->SetAngularDriveParams(0, 0, 0);
	//constraint->ConstraintInstance.AngularDriveMode = EAngularDriveMode::SLERP;
	constraint->SetConstrainedComponents(PieceMesh, FName(), OtherPiece->PieceMesh, FName());

	//すでに追加されているピースであればここで終了
	for (auto p : myGroup->linkedPieceArray) {
		if (p == OtherPiece) return false;
	}

	if (otherOwner) {
		if (volatile auto g = Cast<UPieceGroup>(otherOwner.GetObject())) {

			if (g == myGroup) {
				UE_LOG(LogTemp, Error, TEXT("SameGroup"));
				return false;
			}

			//相手のグループのピースをすべて追加
			for (auto piece : g->linkedPieceArray) {
				piece->SetPieceOwner(pieceOwner);
				myGroup->AddGroup(piece);
			}
		}
	}
	else {
		myGroup->AddGroup(OtherPiece);
		IPieceInterface::Execute_SetActive(OtherPiece, true);
		OtherPiece->SetPieceOwner(pieceOwner);
	}

	UE_LOG(LogTemp, Log, TEXT("Connection"));

	//移動の更新
	IPieceInterface::Execute_SetPosition(pieceOwner.GetObject(), IPieceInterface::Execute_GetPosition(pieceOwner.GetObject()));

	//ゲームクリア判定
	((APuzzleGameMode*)GetWorld()->GetAuthGameMode())->CheckGameClear();

	return true;
}

void APiece::RollingDefault(float DeltaTime) {

	FRotator Rotation = PieceMesh->GetComponentRotation();
	float rotSpeed = 1000.0f * DeltaTime;
	
	Rotation.Pitch += fabsf(Rotation.Pitch) > rotSpeed ? Rotation.Pitch > 0 ? -rotSpeed : rotSpeed : -Rotation.Pitch;
	Rotation.Roll += fabsf(Rotation.Roll) > rotSpeed ? Rotation.Roll > 0 ? -rotSpeed : rotSpeed : -Rotation.Roll;
	Rotation.Yaw += fabsf(Rotation.Yaw) > rotSpeed ? Rotation.Yaw > 0 ? -rotSpeed : rotSpeed : -Rotation.Yaw;

	PieceMesh->SetWorldRotation(Rotation);
}

FVector2D APiece::GetVector2D(EPieceSide Side) {
	switch (Side) {
		case EPieceSide::ELeft:
			return FVector2D(-1, 0);
		case EPieceSide::ETop:
			return FVector2D(0, -1);
		case EPieceSide::ERight:
			return FVector2D(1, 0);
		case EPieceSide::EBottom:
			return FVector2D(0, 1);
		default: return FVector2D();
	}
}

FVector APiece::GetPieceDirection(EPieceSide Side) {

	auto transform = PieceMesh->GetComponentTransform();

	switch (Side) {
		case EPieceSide::ELeft:
			return -PieceMesh->GetRightVector();
		case EPieceSide::ETop:
			return PieceMesh->GetForwardVector();
		case EPieceSide::ERight:
			return PieceMesh->GetRightVector();
		case EPieceSide::EBottom:
			return -PieceMesh->GetForwardVector();
		default: return FVector();
	}
}

// Called every frame
void APiece::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);

	HeightCheck();

	switch (CurrentState) {
		case EPieceState::EPieceSelect:
			RollingDefault(DeltaTime);
			break;
		case EPieceState::EPieceActive:
		case EPieceState::ENone:
		default:

			if (IsActiveSleepTimer && (SleepTimer += DeltaTime) > SLEEP_TIME) {
				IsActiveSleepTimer = false;
				PieceMesh->SetSimulatePhysics(false);
			}

			break;
	}
}

void APiece::ChangePieceState(EPieceState State) {

	if (State == CurrentState) return;

	switch (State) {
		case EPieceState::EPieceActive:
			PieceMaterial->SetVectorParameterValue(EMISSION_PARAM, ActiveEmissionColor);
			PieceMesh->SetCollisionProfileName("PhysicsActor");
			break;

		case EPieceState::EPieceSelect:
			PieceMaterial->SetVectorParameterValue(EMISSION_PARAM, SelectEmissionColor);
			PieceMesh->SetCollisionProfileName("NoCollision");
			break;

		case EPieceState::ENone:
		default:
			PieceMaterial->SetVectorParameterValue(EMISSION_PARAM, BaseEmissionColor);
			PieceMesh->SetCollisionProfileName("PhysicsActor");
			break;
	}

	CurrentState = State;

}

void APiece::StartCollisionSleepTimer() {

	if (IsActiveSleepTimer) return;

	IsActiveSleepTimer = true;
	SleepTimer = 0.0f;
}

void APiece::CancelCollisionSleepTimer() {
	IsActiveSleepTimer = false;
}

void APiece::OnHit() {
	StartCollisionSleepTimer();
}

void APiece::OnOverlapBegin(class UPrimitiveComponent* OverlappedComponent, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) {
	if (!OtherComp->ComponentHasTag(SIDE_COLLISION_TAGNAME)) return;
	UE_LOG(LogTemp, Log, TEXT("BeginOverlapPieceSide"));
	if (auto comp = Cast<UBoxComponent>(OverlappedComponent)) {
		for (int i = 0; i < 4;i++) {
			if (comp == PieceSideCollisionInfo[i].Collision) {
				PieceSideCollisionInfo[i].IsHitting = true;
				PieceSideCollisionInfo[i].HitOtherPiece = Cast<APiece>(OtherActor);
			}
		}
	}
}

void APiece::OnOverlapEnd(class UPrimitiveComponent* OverlappedComponent, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) {
	UE_LOG(LogTemp, Log, TEXT("EndOverlapPieceSide"));
	if (auto comp = Cast<UBoxComponent>(OverlappedComponent)) {
		for (int i = 0; i < 4; i++) {
			if (comp == PieceSideCollisionInfo[i].Collision) {
				PieceSideCollisionInfo[i].IsHitting = false;
				PieceSideCollisionInfo[i].HitOtherPiece = nullptr;
			}
		}
	}
}

// Called when the game starts or when spawned
void APiece::BeginPlay() {
	Super::BeginPlay();
}

void APiece::HeightCheck() {

	auto pos = PieceMesh->GetComponentLocation();
	if (pos.Z > FALL_CHECKHEIGHT) return;

	pos.Z = FALL_RESETHEIGHT;
	PieceMesh->SetWorldLocation(pos);
	PieceMesh->SetPhysicsLinearVelocity(FVector());
}
