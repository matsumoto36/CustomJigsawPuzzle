// Fill out your copyright notice in the Description page of Project Settings.

#include "PlayerPawn.h"
#include "PieceInterface.h"
//#include "HeadMountedDisplayFunctionLibrary.h"
#include "Engine/StaticMesh.h"
#include "GameFramework/PlayerController.h"
#include "Engine/World.h"
//#include "DrawDebugHelpers.h"

//debug
#include "PieceGroup.h"


#include "Engine.h"


// Sets default values
APlayerPawn::APlayerPawn()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	AutoPossessPlayer = EAutoReceiveInput::Player0;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));

	//カメラの生成
	MainCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("MainCamera"));
	MainCamera->SetupAttachment(RootComponent);
	MainCamera->SetRelativeRotation(FRotator(-90.0f, 00.0f, 0.0f));
}

// Called every frame
void APlayerPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//移動の更新
	UpdatePieceMove(DeltaTime);
	UpdatePlayerMove(DeltaTime);
}

// Called to bind functionality to input
void APlayerPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	//入力にバインド//
	//決定キー
	PlayerInputComponent->BindAction(FName("SelectButton"), EInputEvent::IE_Pressed, this, &APlayerPawn::TriggerMouseDown);
	PlayerInputComponent->BindAction(FName("SelectButton"), EInputEvent::IE_Released, this, &APlayerPawn::TriggerMouseUp);

	//移動キー
	PlayerInputComponent->BindAxis(FName("MoveHorizontal"), this, &APlayerPawn::MoveHorizontal);
	PlayerInputComponent->BindAxis(FName("MoveVertical"), this, &APlayerPawn::MoveVertical);
	PlayerInputComponent->BindAxis(FName("MoveForword"), this, &APlayerPawn::MoveForword);

}

void APlayerPawn::CalcCamera(float DeltaTime, struct FMinimalViewInfo& OutResult) {
	Super::CalcCamera(DeltaTime, OutResult);

	//OutResult.Rotation = FRotator(-90.0f, -90.0f, 0.0f);
}

void APlayerPawn::TraceForBlock(const FVector& Start, const FVector& End, bool bDrawDebugHelpers) {
	FHitResult HitResult;

	GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECC_Visibility);
	if (bDrawDebugHelpers) {
		DrawDebugLine(GetWorld(), Start, HitResult.Location, FColor::Red);
		DrawDebugSolidBox(GetWorld(), HitResult.Location, FVector(20.0f), FColor::Red);
	}
	if (HitResult.Actor.IsValid()) {
		UObject* HitObject = HitResult.Actor.Get();

		if (HitObject->GetClass()->ImplementsInterface(UPieceInterface::StaticClass())) {

			if (auto hitOwner = IPieceInterface::Execute_GetOwnerInterface(HitObject)) {
				if (hitOwner.GetObject() == CurrentPieceFocus.GetObject()) return;

				if (CurrentPieceFocus) {
					IPieceInterface::Execute_SetActive(CurrentPieceFocus.GetObject(), false);
					CurrentPieceFocus = nullptr;
				}
				
				//
				if (!hitOwner.GetObject()) {
					UE_LOG(LogTemp, Error, TEXT("hitOwner object* fail"));
					return;
				}
				if (hitOwner.GetObject()->GetClass()->ImplementsInterface(UPieceInterface::StaticClass())) {
					//たまにエラー
					IPieceInterface::Execute_SetActive(hitOwner.GetObject(), true);
					CurrentPieceFocus = hitOwner;
				}
				else {
					UE_LOG(LogTemp, Error, TEXT("hitOwner fail"));
					UE_LOG(LogTemp, Error, TEXT("---hitOwner group log---"));
					auto g = Cast<UPieceGroup>(hitOwner.GetObject());
					if (!g) {
						volatile auto d_piece = Cast<APiece>(HitObject);
						volatile auto d_group = Cast<UPieceGroup>(HitObject);
					}
					else {
						for (int i = 0; i < g->linkedPieceArray.Num(); i++) {
							auto piece = g->linkedPieceArray[i];
							auto name = piece ? piece->GetName() : "null";
							UE_LOG(LogTemp, Error, TEXT("array %d is %s"), i, *name);
						}
					}
				}

				return;
			}
			else {

				if (CurrentPieceFocus) {
					IPieceInterface::Execute_SetActive(CurrentPieceFocus.GetObject(), false);
					CurrentPieceFocus = nullptr;
				}

				IPieceInterface::Execute_SetActive(HitObject, true);

				CurrentPieceFocus.SetObject(HitObject);
				CurrentPieceFocus.SetInterface(Cast<IPieceInterface>(HitObject));

				return;
			}

		}
	}
	if (CurrentPieceFocus) {
		IPieceInterface::Execute_SetActive(CurrentPieceFocus.GetObject(), false);
		CurrentPieceFocus = nullptr;
	}
}

void APlayerPawn::TriggerMouseDown() {
	if (CurrentPieceFocus) {
		bIsSelectPiece = true;
		FVector selectPos;
		CalcPieceLocation(selectPos);
		IPieceInterface::Execute_Select(CurrentPieceFocus.GetObject(), selectPos);
	}
}

void APlayerPawn::TriggerMouseUp() {
	if (CurrentPieceFocus) {
		bIsSelectPiece = false;
		IPieceInterface::Execute_UnSelect(CurrentPieceFocus.GetObject());
	}
}

void APlayerPawn::MoveHorizontal(float axis) {
	MoveVector.Y = axis;
}

void APlayerPawn::MoveVertical(float axis) {
	MoveVector.X = axis;
}

void APlayerPawn::MoveForword(float axis) {
	MoveVector.Z = axis;
}

// Called when the game starts or when spawned
void APlayerPawn::BeginPlay() {
	Super::BeginPlay();

}

bool APlayerPawn::CalcPieceLocation(FVector &PieceLocation) {

	if (!CurrentPieceFocus) return false;
	
	FVector pieceLocation;

	//目標の高度を計算
	auto piecePosition = IPieceInterface::Execute_GetPosition(CurrentPieceFocus.GetObject());
	
	auto startPosition = piecePosition;
	auto endPosition = startPosition;
	endPosition.Z -= PIECE_LOCATION_TRACE_LENGTH;

	FCollisionQueryParams collisionQueryParams;
	collisionQueryParams.AddIgnoredActor(Cast<AActor>(CurrentPieceFocus.GetObject()));

	FHitResult HitResult;
	GetWorld()->LineTraceSingleByChannel(HitResult, startPosition, endPosition, ECC_Visibility, collisionQueryParams);

	if(HitResult.Actor.IsValid()){
		
		//if (GEngine)
		//	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, HitResult.GetComponent()->GetFName().ToString());

		pieceLocation.Z = HitResult.ImpactPoint.Z + SelectedPieceHeight;
	}
	else {
		UE_LOG(LogTemp, Log, TEXT("dont hit Actor"));
	}

	//目標の位置を計算(PieceLocation.Zとマウス方向が交差する点)
	if (APlayerController* PC = Cast<APlayerController>(GetController())) {

		FVector start, dir;
		PC->DeprojectMousePositionToWorld(start, dir);

		auto ratio = (PieceLocation.Z - start.Z) / dir.Z;
		pieceLocation.X = start.X + dir.X * ratio;
		pieceLocation.Y = start.Y + dir.Y * ratio;

		DrawDebugSolidBox(GetWorld(), pieceLocation, FVector(1.0f), FColor::Red);

		//可動範囲外であれば収める
		if (fabsf(pieceLocation.X) > MaxMovePieceArea.X)
			pieceLocation.X = pieceLocation.X > 0 ? MaxMovePieceArea.X : -MaxMovePieceArea.X;
		if (fabsf(pieceLocation.Y) > MaxMovePieceArea.Y)
			pieceLocation.Y = pieceLocation.Y > 0 ? MaxMovePieceArea.Y : -MaxMovePieceArea.Y;

		PieceLocation = pieceLocation;
		return true;
	}

	UE_LOG(LogTemp, Error, TEXT("Fail CalcPieceLocation"));

	return false;
}

void APlayerPawn::UpdatePieceMove(float DeltaTime) {

	if (APlayerController* PC = Cast<APlayerController>(GetController())) {
		if (bIsSelectPiece) {
			//Ownerチェック
			if(auto pieceOwner = IPieceInterface::Execute_GetOwnerInterface(CurrentPieceFocus.GetObject()))
				CurrentPieceFocus = pieceOwner;

			//移動先を計算
			CalcPieceLocation(PieceMovePosition);
			//ゆっくり移動
			auto piecePos = IPieceInterface::Execute_GetPosition(CurrentPieceFocus.GetObject());
			auto movePos = FMath::VInterpTo(piecePos, PieceMovePosition, DeltaTime, 10.0f);
			IPieceInterface::Execute_SetPosition(CurrentPieceFocus.GetObject(), movePos);
		}
		else {

			FVector Start, Dir, End;
			PC->DeprojectMousePositionToWorld(Start, Dir);
			End = Start + (Dir * 8000.0f);
			TraceForBlock(Start, End, false);
		}
	}

}

void APlayerPawn::UpdatePlayerMove(float DeltaTime) {

	auto deltaLocation = MoveVector.GetSafeNormal() * MoveSpeed * DeltaTime;
	AddActorWorldOffset(deltaLocation, true);

}