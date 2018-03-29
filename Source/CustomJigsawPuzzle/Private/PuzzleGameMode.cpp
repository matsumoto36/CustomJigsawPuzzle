// Fill out your copyright notice in the Description page of Project Settings.

#include "PuzzleGameMode.h"
#include "PuzzlePlayerController.h"
#include "PlayerPawn.h"


APuzzleGameMode::APuzzleGameMode() {

	DefaultPawnClass = APlayerPawn::StaticClass();
	PlayerControllerClass = APuzzlePlayerController::StaticClass();

}


