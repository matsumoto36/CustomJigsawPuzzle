// Fill out your copyright notice in the Description page of Project Settings.

#include "PieceGroup.h"

void UPieceGroup::AddGroup(APiece* Piece) {
	linkedPieceArray.Add(Piece);
}

UPieceGroup::UPieceGroup() {}

UPieceGroup::~UPieceGroup() {}

FVector UPieceGroup::GetPosition_Implementation() {
	return groupPosition - mouseOffset;
}

bool UPieceGroup::SetPosition_Implementation(FVector Position) {
	groupPosition = Position + mouseOffset;
	
	//IPieceInterface::Execute_SetPosition(linkedPieceArray[0], Position);
	//回転角度は大体すべて同じ
	auto vecX = linkedPieceArray[0]->GetPieceDirection(EPieceSide::ERight);
	auto vecY = linkedPieceArray[0]->GetPieceDirection(EPieceSide::EBottom);

	//コメントアウトしたforは初回限定で既存のGroupにピースを追加したときにエラー
	//for (auto piece : linkedPieceArray) {
	for (int i = 0; i < linkedPieceArray.Num(); i++) {
		int x, y;
		linkedPieceArray[i]->GetPieceMapPosition(x, y);
		auto scale = linkedPieceArray[i]->GetBody()->GetComponentScale();
		auto pos = groupPosition;

		//回転を考慮した移動を実装
		pos += vecX * x * scale.X;
		pos += vecY * y * scale.Y;
		if(linkedPieceArray[i]->GetBody())
			linkedPieceArray[i]->GetBody()->SetWorldLocation(pos);
		else
			UE_LOG(LogTemp, Error, TEXT("piece->GetBody() is null"));

	}
	return true;
}

bool UPieceGroup::Select_Implementation(FVector ClickPos) {

	mouseOffset = groupPosition - ClickPos;
	mouseOffset.Z = 0;

	for (auto piece : linkedPieceArray) {
		IPieceInterface::Execute_Select(piece, ClickPos);
	}
	return true;
}

bool UPieceGroup::UnSelect_Implementation() {
	for (auto piece : linkedPieceArray) {
		IPieceInterface::Execute_UnSelect(piece);
	}
	return true;
}

bool UPieceGroup::SetActive_Implementation(bool Enable) {
	for (auto piece : linkedPieceArray) {
		IPieceInterface::Execute_SetActive(piece, Enable);
	}
	return true;
}
