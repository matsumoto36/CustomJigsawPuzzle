// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "PieceGenerator.h"

#include "Runtime/ImageWrapper/Public/IImageWrapperModule.h"
#include "Runtime/ImageWrapper/Public/IImageWrapper.h"

#include "PuzzleGameMode.generated.h"

/**
 * 
 */
UCLASS()
class APuzzleGameMode : public AGameMode
{
	GENERATED_BODY()
private:

	UPROPERTY(EditAnyWhere)
		APieceGenerator* PieceGenerator;

	UPROPERTY(EditAnyWhere)
		UTexture2D* PuzzleTexture;

public:
	APuzzleGameMode();


	UFUNCTION(BlueprintCallable)
		APieceGenerator* GetPieceGenerator() { return PieceGenerator; }

	UFUNCTION(BlueprintCallable)
		UTexture2D* GetPuzzleTexture() { return PuzzleTexture; }

	UFUNCTION(BlueprintCallable)
		void InitializeGame();

	UFUNCTION(BlueprintCallable)
		bool LoadPuzzleTextureData();
	
	UFUNCTION()
		UTexture2D* LoadTexture2DFromFile(const FString& FileName, bool& IsValid, int32& Width, int32& Height);

	EImageFormat ConvertImageFormat(FString FileExt);
};
