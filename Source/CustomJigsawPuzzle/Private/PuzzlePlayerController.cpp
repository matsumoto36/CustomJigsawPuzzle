// Fill out your copyright notice in the Description page of Project Settings.

#include "PuzzlePlayerController.h"


APuzzlePlayerController::APuzzlePlayerController() {

	bShowMouseCursor = true;
	bEnableClickEvents = true;
	bEnableMouseOverEvents = true;
	bEnableTouchEvents = true;
	DefaultMouseCursor = EMouseCursor::Crosshairs;

}