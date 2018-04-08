// Fill out your copyright notice in the Description page of Project Settings.

#include "PlayerPawn.h"
#include "Piece.h"
#include "HeadMountedDisplayFunctionLibrary.h"
#include "Engine/StaticMesh.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/PlayerController.h"
#include "Engine/World.h"
#include "DrawDebugHelpers.h"

#include "Engine.h"


// Sets default values
APlayerPawn::APlayerPawn()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	AutoPossessPlayer = EAutoReceiveInput::Player0;
}

// Called every frame
void APlayerPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (APlayerController* PC = Cast<APlayerController>(GetController())) {
		if (bIsSelectPiece) {

			//移動先を計算
			CalcPieceLocation(PieceMovePosition);

			//ゆっくり移動
			auto movePos = FMath::VInterpTo(CurrentPieceFocus->GetActorLocation(), PieceMovePosition, DeltaTime, 10.0f);
			CurrentPieceFocus->SetActorLocation(movePos);
		}
		else {

			FVector Start, Dir, End;
			PC->DeprojectMousePositionToWorld(Start, Dir);
			End = Start + (Dir * 8000.0f);
			TraceForBlock(Start, End, false);
		}
	}

}

// Called to bind functionality to input
void APlayerPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	//入力にバインド
	PlayerInputComponent->BindAction("SelectButton", EInputEvent::IE_Pressed, this, &APlayerPawn::TriggerMouseDown);
	PlayerInputComponent->BindAction("SelectButton", EInputEvent::IE_Released, this, &APlayerPawn::TriggerMouseUp);

}

void APlayerPawn::CalcCamera(float DeltaTime, struct FMinimalViewInfo& OutResult) {
	Super::CalcCamera(DeltaTime, OutResult);

	//OutResult.Rotation = FRotator(-90.0f, -90.0f, 0.0f);
}

void APlayerPawn::TraceForBlock(const FVector& Start, const FVector& End, bool bDrawDebugHelpers) {
	FHitResult HitResult;
	GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECC_PhysicsBody);
	if (bDrawDebugHelpers) {
		DrawDebugLine(GetWorld(), Start, HitResult.Location, FColor::Red);
		DrawDebugSolidBox(GetWorld(), HitResult.Location, FVector(20.0f), FColor::Red);
	}
	if (HitResult.Actor.IsValid()) {
		APiece* HitPiece = Cast<APiece>(HitResult.Actor.Get());
		if (CurrentPieceFocus != HitPiece) {
			if (CurrentPieceFocus) {
				CurrentPieceFocus->Highlight(false);
			}
			if (HitPiece) {
				HitPiece->Highlight(true);
			}
			CurrentPieceFocus = HitPiece;
		}
	}
	else if (CurrentPieceFocus) {
		CurrentPieceFocus->Highlight(false);
		CurrentPieceFocus = nullptr;
	}
}

void APlayerPawn::TriggerMouseDown() {
	if (CurrentPieceFocus) {
		bIsSelectPiece = true;
		CurrentPieceFocus->HandleMouseDown();
	}
}

void APlayerPawn::TriggerMouseUp() {
	if (CurrentPieceFocus) {
		bIsSelectPiece = false;
		CurrentPieceFocus->HandleMouseUp();
	}
}

// Called when the game starts or when spawned
void APlayerPawn::BeginPlay() {
	Super::BeginPlay();

}

bool APlayerPawn::CalcPieceLocation(FVector &PieceLocation) {

	if (!CurrentPieceFocus) return false;
	
	FVector pieceLocation;

	//目標の高度を計算
	auto pieceBody = CurrentPieceFocus->GetBody();
	
	auto startPosition = pieceBody->GetComponentLocation();
	auto endPosition = startPosition;
	endPosition.Z -= 100; //後でconstに移行

	FHitResult HitResult;
	GetWorld()->LineTraceSingleByChannel(HitResult, startPosition, endPosition, ECC_Visibility);

	if(HitResult.Actor.IsValid()){
		
		//if (GEngine)
		//	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, HitResult.GetComponent()->GetFName().ToString());

		pieceLocation.Z = HitResult.ImpactPoint.Z + SelectedPieceHeight;
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
		if (fabsf(pieceLocation.X) > MaxMoveArea.X)
			pieceLocation.X = pieceLocation.X > 0 ? MaxMoveArea.X : -MaxMoveArea.X;
		if (fabsf(pieceLocation.Y) > MaxMoveArea.Y)
			pieceLocation.Y = pieceLocation.Y > 0 ? MaxMoveArea.Y : -MaxMoveArea.Y;

		PieceLocation = pieceLocation;
		return true;
	}

	return false;
}