// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

//#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "PieceInterface.generated.h"

// ��ԉ�(1): UINTERFACE �� UInterface �� public �p������ U �v���t�B�b�N�X�̃N���X���`����

UINTERFACE(Blueprintable)
class CUSTOMJIGSAWPUZZLE_API UPieceInterface : public UInterface {

	// �v����: �����ł�������� U �v���t�B�b�N�X�ł��g��
	//GENERATED_UINTERFACE_BODY()

	//4.18���炱�����ł�����
	GENERATED_BODY()

};


 // ��ԉ�(2): UE4�̃N���X��`�}�N����t�����ɐ�ɒ�`���� U �v���t�B�b�N�X�̃N���X�� I �v���t�B�b�N�X�ł̃N���X���`����
class CUSTOMJIGSAWPUZZLE_API IPieceInterface
{
	// �v����: �����ł�������� I �v���t�B�b�N�X�ł��g��
	//GENERATED_IINTERFACE_BODY()

	//4.18���炱�����ł�����
	GENERATED_BODY()


public:

	// ��ԉ�(3) Blueprint �֊֐������J����߂�ǂ������}�N�������炾�珑��
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
