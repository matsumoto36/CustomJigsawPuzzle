// Fill out your copyright notice in the Description page of Project Settings.

#include "PieceGenerator.h"
#include "Materials/MaterialInstance.h"
#include "Engine/StaticMesh.h"
#include "Runtime/Engine/Classes/Engine/World.h"

// Sets default values
APieceGenerator::APieceGenerator()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

APiece* APieceGenerator::SpawnPiece(FTransform SpawnTransform) {

	UWorld* const World = GetWorld();

	// Null�`�F�b�N
	if (!World) return nullptr;

	FActorSpawnParameters SpawnParams;
	SpawnParams.bAllowDuringConstructionScript = true;
	SpawnParams.bDeferConstruction = false;
	SpawnParams.bNoFail = true;
	SpawnParams.Instigator = Instigator;
	SpawnParams.Name = {};
	SpawnParams.ObjectFlags = EObjectFlags::RF_NoFlags;
	SpawnParams.OverrideLevel = nullptr;
	SpawnParams.Owner = this;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	SpawnParams.Template = nullptr;

	auto piece =
		GetWorld()->SpawnActor<APiece>(SpawnTransform.GetLocation(), SpawnTransform.Rotator(), SpawnParams);

	return piece;
}

bool APieceGenerator::CreatePieceMesh(UProceduralMeshComponent* MeshComponent, UMaterialInterface* Material, int x, int y) {
	
	// ���_�Q
	TArray<FVector> vertices;
	// �C���f�b�N�X�Q
	TArray<int32> indices;
	// �@���Q�i����͋���ۂ̂܂܁j
	TArray<FVector> normals;
	// �e�N�X�`���[���W�Q
	TArray<FVector2D> texcoords0;
	// ���_�J���[�Q�i����͋���ۂ̂܂܁j
	TArray<FLinearColor> vertex_colors;
	// �ڐ��Q�i����͋���ۂ̂܂܁j
	TArray<FProcMeshTangent> tangents;
	
	//���_�Q���`
	vertices.Emplace(-5.0, -5.0, 0);
	vertices.Emplace(-5.0, 5.0, 0);
	vertices.Emplace(5.0, -5.0, 0);
	vertices.Emplace(5.0, 5.0, 0);

	// UE �́u������W�nZ-top�v�́u�����v���ʐ����v�Ȃ̂Ł��̒��_�����̏����Ōq����+Z�����ɎO�p��1�𐶐��ł���B
	indices.Emplace(0);
	indices.Emplace(1);
	indices.Emplace(2);
	indices.Emplace(1);
	indices.Emplace(2);
	indices.Emplace(3);

	// �e�N�X�`���[���W��ݒ肵�Ă����΂���ȃG�O�U���v���ł��K���ȃ}�e���A�����Z�b�g���Ė͗l�o���m�F�͂ł��܂��B
	texcoords0.Emplace(0, 0);
	texcoords0.Emplace(0, 0);
	texcoords0.Emplace(0, 0);
	texcoords0.Emplace(0, 0);

	// UProceduralMeshComponent::CreateMeshSection_LinearColor �Ń��b�V���𐶐��B
	// ��1����: �Z�N�V�����ԍ�; 0, 1, 2, ... ��^���鎖��1�� UProceduralMeshComponent �ɕ����̃��b�V��������I�ɓ����ɐ����ł��܂��B
	// ��2����: ���_�Q
	// ��3����: �C���f�b�N�X�Q
	// ��4����: �@���Q
	// ��5����: �e�N�X�`���[���W�Q
	// ��6����: ���_�J���[�Q
	// ��7����: �@���Q
	// ��8����: �R���W���������t���O
	MeshComponent->CreateMeshSection_LinearColor(0, vertices, indices, normals, texcoords0, vertex_colors, tangents, false);

	//�}�e���A����ݒ�
	MeshComponent->SetMaterial(0, Material);
	
	return true;
}

// Called every frame
void APieceGenerator::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);

}

// Called when the game starts or when spawned
void APieceGenerator::BeginPlay()
{
	Super::BeginPlay();
	
}



