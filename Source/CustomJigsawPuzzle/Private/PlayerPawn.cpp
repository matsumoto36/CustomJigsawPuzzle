// Fill out your copyright notice in the Description page of Project Settings.

#include "PlayerPawn.h"
#include "Piece.h"
#include "HeadMountedDisplayFunctionLibrary.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/PlayerController.h"
#include "Engine/World.h"
#include "DrawDebugHelpers.h"

// Sets default values
APlayerPawn::APlayerPawn()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	AutoPossessPlayer = EAutoReceiveInput::Player0;
}

// Called when the game starts or when spawned
void APlayerPawn::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void APlayerPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (APlayerController* PC = Cast<APlayerController>(GetController())) {

		FVector Start, Dir, End;
		PC->DeprojectMousePositionToWorld(Start, Dir);
		End = Start + (Dir * 8000.0f);
		TraceForBlock(Start, End, false);
	}

}

// Called to bind functionality to input
void APlayerPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAction("SelectButton", EInputEvent::IE_Pressed, this, &APlayerPawn::TriggerMouseDown);
	PlayerInputComponent->BindAction("SelectButton", EInputEvent::IE_Released, this, &APlayerPawn::TriggerMouseUp);

}

void APlayerPawn::CalcCamera(float DeltaTime, struct FMinimalViewInfo& OutResult) {
	Super::CalcCamera(DeltaTime, OutResult);

	OutResult.Rotation = FRotator(-90.0f, -90.0f, 0.0f);
}

void APlayerPawn::TriggerMouseDown() {
	if (CurrentPieceFocus) {
		CurrentPieceFocus->HandleMouseDown();
	}
}

void APlayerPawn::TriggerMouseUp() {
	if (CurrentPieceFocus) {
		CurrentPieceFocus->HandleMouseUp();
	}
}

void APlayerPawn::TraceForBlock(const FVector& Start, const FVector& End, bool bDrawDebugHelpers) {
	FHitResult HitResult;
	GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECC_Visibility);
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