// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Piece.h"
#include "PieceInterface.h"
#include "PieceGroup.generated.h"

/**
 * 
 */
UCLASS()
class UPieceGroup : public UObject, public IPieceInterface
{
	GENERATED_BODY()
	
private:

	FVector groupPosition;
	FVector mouseOffset;

public:

	UPROPERTY()
		TArray<class APiece*> linkedPieceArray;

	void AddGroup(class APiece* Piece);

	void SetGroupPosition(FVector Position) {
		groupPosition = Position;
	}

	void SetMouseOffset(FVector Offset) {
		mouseOffset = Offset;
	}

	UPieceGroup();
	~UPieceGroup();

protected:

	virtual FVector GetPosition_Implementation() override;

	virtual bool SetPosition_Implementation(FVector Position) override;

	virtual bool Select_Implementation(FVector ClickPos) override;

	virtual bool UnSelect_Implementation() override;

	virtual bool SetActive_Implementation(bool Enable) override;

	virtual TScriptInterface<IPieceInterface> GetOwnerInterface_Implementation() override {
		return nullptr;
	}
	
};
