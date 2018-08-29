// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PieceInterface.h"
#include "Piece.h"

/**
 * リンクされているピースを同時に操作するためのクラス
 */
class PieceGroup : public IPieceInterface
{

	FVector mousePositionOffset;

public:

	TArray<APiece*> linkedPieceArray;
	
	void AddGroup(APiece* Piece);

	PieceGroup();
	~PieceGroup();

protected:

	virtual FVector GetPosition();

	virtual bool SetPosition(FVector Position);

	virtual bool Select();

	virtual bool UnSelect();

	virtual bool SetActive(bool Enable);

};
