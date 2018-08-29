// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

//#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "PieceInterface.generated.h"

// 手間暇(1): UINTERFACE で UInterface を public 継承した U プリフィックスのクラスを定義する

UINTERFACE(Blueprintable)
class CUSTOMJIGSAWPUZZLE_API UPieceInterface : public UInterface {

	// 要注意: ここでもこちらは U プリフィックス版を使う
	//GENERATED_UINTERFACE_BODY()

	//4.18からこっちでもいい
	GENERATED_BODY()

};


 // 手間暇(2): UE4のクラス定義マクロを付けずに先に定義した U プリフィックスのクラスの I プリフィックス版のクラスを定義する
class CUSTOMJIGSAWPUZZLE_API IPieceInterface
{
	// 要注意: ここでもこちらは I プリフィックス版を使う
	//GENERATED_IINTERFACE_BODY()

	//4.18からこっちでもいい
	GENERATED_BODY()


public:

	// 手間暇(3) Blueprint へ関数を公開するめんどくさいマクロをだらだら書く
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "PieceInterface")
		FVector GetPosition();

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "PieceInterface")
		bool SetPosition(FVector Position);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "PieceInterface")
		bool Select();

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "PieceInterface")
		bool UnSelect();

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "PieceInterface")
		bool SetActive(bool Enable);

};
