// Fill out your copyright notice in the Description page of Project Settings.

#include "PieceGenerator.h"
#include "Materials/MaterialInstance.h"
#include "Engine/StaticMesh.h"
#include "Runtime/Engine/Classes/Engine/World.h"
#include "Runtime/Core/Public/Math/UnrealMathUtility.h"

#include "Engine.h"

// Sets default values
APieceGenerator::APieceGenerator()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//createDummy
	CreateDefaultSubobject<USceneComponent>("Dummy")->SetupAttachment(RootComponent);
}

APiece* APieceGenerator::SpawnPiece(FTransform SpawnTransform, TArray<USplineComponent*> SplineArray, int Partition) {

	FActorSpawnParameters SpawnParams;
	SpawnParams.bAllowDuringConstructionScript = true;
	SpawnParams.bDeferConstruction = false;
	SpawnParams.bNoFail = true;
	SpawnParams.Instigator = Instigator;
	SpawnParams.Name = {};
	SpawnParams.ObjectFlags = EObjectFlags::RF_Dynamic;
	SpawnParams.OverrideLevel = nullptr;
	SpawnParams.Owner = this;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	SpawnParams.Template = nullptr;

	auto piece =
		GetWorld()->SpawnActor<APiece>(APiece::StaticClass(), SpawnTransform.GetLocation(), SpawnTransform.Rotator(), SpawnParams);

	piece->SetActorScale3D(SpawnTransform.GetScale3D());
	piece->UpdateComponentTransforms();

	//�s�[�X�̌`������
	//TArray<USplineComponent*> spline;
	//spline.Emplace(CreateSpline(CreateJigsawSplinePoints()));
	//spline.Emplace(nullptr);
	//spline.Emplace(CreateSpline(CreateJigsawSplinePoints()));
	//spline.Emplace(CreateSpline(CreateJigsawSplinePoints()));

	//CreatePieceMesh(piece->GetBody(), CreatePieceRoundVertices(spline, Partition));
	CreatePieceMesh(piece->GetBody(), CreatePieceRoundVertices(SplineArray, Partition));

	return piece;
}

bool APieceGenerator::CreatePieceMesh(UProceduralMeshComponent* MeshComponent, TArray<FVector> PieceLinePoints) {
	
	TArray<FVector> vertices;
	TArray<FVector> normals;
	TArray<FVector2D> texcoords0;
	TArray<FLinearColor> vertex_colors;
	TArray<FProcMeshTangent> tangents;

	//�\�ʂ̒��_��ǉ�
	for (int i = 0; i < PieceLinePoints.Num(); i++) {
		FVector v = PieceLinePoints[i];
		v.Z = 0.5;
		vertices.Emplace(v);

		//�e�N�X�`���[���W��ݒ�
		FVector2D texCoords;
		texCoords.X = PieceLinePoints[i].Y;
		texCoords.Y = -PieceLinePoints[i].X;

		texcoords0.Emplace(texCoords);

		//���_�J���[
		vertex_colors.Emplace(FLinearColor(1, 1, 1));

		//�@���x�N�g��
		normals.Emplace(FVector(0, 0, 1));
	}

	//�ʂ�\��
	auto indices = ConvexPartitioning(PieceLinePoints);

	//���ʂ̒��_��ǉ�
	for (int i = 0; i < PieceLinePoints.Num(); i++) {
		FVector v = PieceLinePoints[i];
		v.Z = -0.5;
		vertices.Emplace(v);

		//�e�N�X�`���[���W��ݒ�
		FVector2D texCoords;
		texCoords.X = PieceLinePoints[i].Y;
		texCoords.Y = -PieceLinePoints[i].X;

		texcoords0.Emplace(texCoords);

		//���_�J���[
		vertex_colors.Emplace(FLinearColor(0, 0, 0));

		//�@���x�N�g��
		normals.Emplace(FVector(0, 0, -1));
	}

	//���ʂ͕\�ʂ̏��𗘗p����
	int offset = PieceLinePoints.Num();
	int indicesCount = indices.Num();
	for (int i = 0; i < indicesCount; i++) {

		int index = 0;
		switch (i % 3) {
			case 0: index = i; break;
			case 1: index = i + 1; break;
			case 2: index = i - 1; break;
		}

		// 0 2 1 3 5 4�̏��ɒǉ�����(�ʂ̌������t�ɂ���)
		indices.Emplace(indices[index] + offset);

	}

	//���̖ʂ�\��B
	int verticesCount = vertices.Num();
	int verticesCountHalf = verticesCount / 2;

	for (int i = 1; i < verticesCountHalf; i++) {
		indices.Emplace(i - 1);
		indices.Emplace(i);
		indices.Emplace(verticesCountHalf + i - 1);
		
		indices.Emplace(i);
		indices.Emplace(verticesCountHalf + i);
		indices.Emplace(verticesCountHalf + i - 1);
	}
	//�Ō�̉��̖ʂ�\��
	indices.Emplace(verticesCountHalf - 1);
	indices.Emplace(0);
	indices.Emplace(verticesCount - 1);

	indices.Emplace(0);
	indices.Emplace(verticesCountHalf);
	indices.Emplace(verticesCount - 1);

	//���_�@�����v�Z����
	//normals.Init(FVector(0, 0, 0), verticesCount);
	//indicesCount = indices.Num();
	//for (int i = 0; i < indicesCount; i += 3) {

	//	auto v1 = indices[i];
	//	auto v2 = indices[i + 1];
	//	auto v3 = indices[i + 2];
	//	
	//	//�ʖ@�����v�Z����
	//	auto cross = -((vertices[v2] - vertices[v1]) ^ (vertices[v3] - vertices[v2]))
	//		.GetSafeNormal();

	//	//���ꂼ��̒��_�ɉ��Z
	//	normals[v1] += cross;
	//	normals[v2] += cross;
	//	normals[v3] += cross;

	//}
	//for (int i = 0; i < verticesCount; i++) {
	//	//���K������
	//	normals[i] = normals[i].GetSafeNormal();
	//}

	// UProceduralMeshComponent::CreateMeshSection_LinearColor �Ń��b�V���𐶐��B
	// ��1����: �Z�N�V�����ԍ�; 0, 1, 2, ... ��^���鎖��1�� UProceduralMeshComponent �ɕ����̃��b�V��������I�ɓ����ɐ����ł��܂��B
	// ��2����: ���_�Q
	// ��3����: �C���f�b�N�X�Q
	// ��4����: �@���Q
	// ��5����: �e�N�X�`���[���W�Q
	// ��6����: ���_�J���[�Q
	// ��7����: �@���Q
	// ��8����: �R���W���������t���O
	MeshComponent->CreateMeshSection_LinearColor(0, vertices, indices, normals, texcoords0, vertex_colors, tangents, true);

	//�����蔻��̍쐬
	TArray<FVector> convexVertices;
	convexVertices.Emplace(FVector(-0.5, -0.5, -0.5));
	convexVertices.Emplace(FVector(-0.5, 0.5, -0.5));
	convexVertices.Emplace(FVector(0.5, -0.5, -0.5));
	convexVertices.Emplace(FVector(0.5, 0.5, -0.5));
	convexVertices.Emplace(FVector(-0.5, -0.5, 0.5));
	convexVertices.Emplace(FVector(-0.5, 0.5, 0.5));
	convexVertices.Emplace(FVector(0.5, -0.5, 0.5));
	convexVertices.Emplace(FVector(0.5, 0.5, 0.5));
	MeshComponent->AddCollisionConvexMesh(convexVertices);

	return true;
}

const TArray<int32> APieceGenerator::ConvexPartitioning(TArray<FVector> RoundVertices, bool debugDraw) {

	//�v�Z�p���_�ԍ��f�[�^���쐬
	TArray<int32> verticesIndex;
	for (int i = 0; i < RoundVertices.Num(); i++) {
		verticesIndex.Emplace(i);
	}

	TArray<int32> indices;
	while (RoundVertices.Num() > 3) {

		int targetPoint = -1;

		/*���_�����ԉ����_�����߂�*/

		float temp = 0;
		for (int i = 0; i < RoundVertices.Num(); i++) {

			float temp2 = RoundVertices[i].SizeSquared2D();
			if (temp < temp2) {
				temp = temp2;
				targetPoint = i;
			}
		}

		/*���̓_�Ɨד��m�łł���O�p�`�̒��ɂق��̒��_�����邩���ׂ�*/

		//��������_�𐳖ʂɌ����ꍇ ----(left)----(targetPoint)----(right)----
		int left = targetPoint - 1 >= 0 ? targetPoint - 1 : RoundVertices.Num() - 1;
		int right = targetPoint + 1 < RoundVertices.Num() ? targetPoint + 1 : 0;

		//�O�p�`���쐬�ł���܂ő�����
		bool isCreateTriangle = false;
		while (!isCreateTriangle) {

			//�O�p�`�̊e���_�����ԂɈ�������Ƃ��ɓ_�����������ɂ��葱�����ꍇ�͎O�p�`�̒��ɓ_�͑��݂���
			bool isContain = false;
			for (int i = 0; i < RoundVertices.Num(); i++) {

				if (i == left || i == targetPoint || i == right) {

					//if (GEngine)
					//	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, "Continue");
					continue;
				}

				float side1 = ((RoundVertices[left] - RoundVertices[targetPoint]) ^ (RoundVertices[i] - RoundVertices[left])).Z;
				float side2 = ((RoundVertices[targetPoint] - RoundVertices[right]) ^ (RoundVertices[i] - RoundVertices[targetPoint])).Z;
				float side3 = ((RoundVertices[right] - RoundVertices[left]) ^ (RoundVertices[i] - RoundVertices[right])).Z;

				if ((side1 > 0 && side2 > 0 && side3 > 0) || (side1 < 0 && side2 < 0 && side3 < 0)) {

					//debug
					//if (GEngine)
					//	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, "Inside");

					//DrawDebugLine(GetWorld(), RoundVertices[left], RoundVertices[targetPoint], FColor::Yellow, false, 10.0f, 10);
					//DrawDebugLine(GetWorld(), RoundVertices[targetPoint], RoundVertices[right], FColor::Yellow, false, 10.0f, 10);
					//DrawDebugLine(GetWorld(), RoundVertices[right], RoundVertices[left], FColor::Yellow, false, 10.0f, 10);

					//DrawDebugPoint(GetWorld(), RoundVertices[i], 10, FColor::Yellow, false, 10, 10);

					isContain = true;
					break;
				}
			}

			if (isContain) {
				/*�O�p�`���쐬�ł��Ȃ��̂Œ��_��ύX����*/

				//�O�p�`�̌�����ۑ����Ă���
				float triangleSide1 = ((RoundVertices[left] - RoundVertices[targetPoint]) ^ (RoundVertices[right] - RoundVertices[left])).Z;

				float checkSide1;

				do {

					//��ׂɈړ�����
					left = targetPoint;
					targetPoint = right;
					right = right + 1 < RoundVertices.Num() ? right + 1 : 0;

					//����������������
					checkSide1 = ((RoundVertices[left] - RoundVertices[targetPoint]) ^ (RoundVertices[right] - RoundVertices[left])).Z;

					//�����������O�p�`�ɂȂ�܂Œ��ׂ�
				} while (!(
					(triangleSide1 < 0 && checkSide1 < 0) || 
					(triangleSide1 > 0 && checkSide1 > 0)));

			}
			else {
				/*�O�p�`���쐬�ł���̂ō쐬���č폜*/

				//debug
				if (debugDraw) {
					DrawDebugLine(GetWorld(), RoundVertices[left] * 100, RoundVertices[targetPoint] * 100, FColor::Red, false, 10.0f);
					DrawDebugLine(GetWorld(), RoundVertices[targetPoint] * 100, RoundVertices[right] * 100, FColor::Red, false, 10.0f);
					DrawDebugLine(GetWorld(), RoundVertices[right] * 100, RoundVertices[left] * 100, FColor::Red, false, 10.0f);

					DrawDebugPoint(GetWorld(), RoundVertices[targetPoint] * 100, 10, FColor::Red, false, 10);
				}

				//UE4�ł́u������W�nZ-top�v�́u�����v���ʐ����v
				indices.Emplace(verticesIndex[right]);
				indices.Emplace(verticesIndex[targetPoint]);
				indices.Emplace(verticesIndex[left]);

				//���_���폜���ĒE�o
				verticesIndex.RemoveAt(targetPoint);
				RoundVertices.RemoveAt(targetPoint);
				isCreateTriangle = true;
			}
		}
	}

	//debug
	//for (auto item : RoundVertices) {
	//	DrawDebugPoint(GetWorld(), item, 10, FColor::Green, false, 10);
	//}

	//�c�����O�p�`��ǉ����ďI��
	indices.Emplace(verticesIndex[2]);
	indices.Emplace(verticesIndex[1]);
	indices.Emplace(verticesIndex[0]);

	return indices;
}

const TArray<FVector> APieceGenerator::CreatePieceRoundVertices(TArray<USplineComponent*> SplineArray, int Partition) {

	//MeshComponent�͍������玞�v���ɁA�ւ��݂���肽���Ȃ��ꍇ��nullptr�ɂ���

	TArray<FVector> cyclePoints = { 
		FVector(-0.5, -0.5, 0),
		FVector(0.5,-0.5, 0),
		FVector(0.5, 0.5, 0),
		FVector(-0.5, 0.5, 0),
	};

	//Spline�̈ʒu����
	for (int i = 0; i < 4; i++) {
		if (SplineArray[i]) {
			SplineArray[i]->SetWorldLocationAndRotation(cyclePoints[i], FRotator(0, i * 90, 0));
		}

	}

	//�_�ɕϊ�
	TArray<FVector> points;
	float p = Partition;

	for (int i = 0; i < 4; i++) {

		if (SplineArray[i]) {
			//�䗦1�̒��_�͏d������̂Œǉ����Ȃ�
			for (int j = 0; j < Partition; j++) {
				points.Emplace(SplineArray[i]->GetWorldLocationAtTime(j / p));
			}
		}
		else {
			points.Emplace(cyclePoints[i]);
		}

	}
	
	return points;
}

USplineComponent* APieceGenerator::CreateSpline(TArray<FVector> Points) {

	//�X�v���C���Ȑ����`
	auto spline = NewObject<USplineComponent>(this);
	spline->RegisterComponent();
	spline->AttachToComponent(RootComponent, { EAttachmentRule::SnapToTarget, true }, TEXT("Spline"));

	//�ŏ�����܂܂�Ă���2�_���폜
	spline->ClearSplinePoints();

	//�_�f�[�^���`
	TArray<FSplinePoint> splinePoints;
	for (int i = 0; i < Points.Num();i++) {
		splinePoints.Emplace(FSplinePoint(i, Points[i]));
	}

	//��`�����f�[�^��ǉ�
	spline->AddPoints(splinePoints);

	return spline;
}

const TArray<FVector> APieceGenerator::CreateJigsawSplinePoints() {
	
	TArray<FVector> points;
	points.Emplace(FVector(0, 0, 0));
	points.Emplace(FVector(
		FMath::RandRange(baseLineOffsets.xMin, baseLineOffsets.xMax),
		FMath::RandRange(baseLineOffsets.yMin, baseLineOffsets.yMax), 0
	));
	points.Emplace(FVector(
		FMath::RandRange(upperOffsets.xMin, upperOffsets.xMax),
		FMath::RandRange(upperOffsets.yMin, upperOffsets.yMax), 0
	));
	points.Emplace(FVector(
		FMath::RandRange(1 - upperOffsets.xMax, 1 - upperOffsets.xMin),
		FMath::RandRange(upperOffsets.yMin, upperOffsets.yMax), 0
	));
	points.Emplace(FVector(
		FMath::RandRange(1 - baseLineOffsets.xMax, 1 - baseLineOffsets.xMin),
		FMath::RandRange(baseLineOffsets.yMin, baseLineOffsets.yMax), 0
	));
	points.Emplace(FVector(1, 0, 0));

	return points;
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

