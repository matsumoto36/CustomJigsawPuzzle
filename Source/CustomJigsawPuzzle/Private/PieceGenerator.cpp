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
	CreateDefaultSubobject< USceneComponent >("Dummy")->SetupAttachment(RootComponent);
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

	//�����_���Ƀs�[�X�̌`������
	TArray<USplineComponent*> spline;
	spline.Emplace(CreateSpline(CreateJigsawSplinePoints()));
	spline.Emplace(CreateSpline(CreateJigsawSplinePoints()));
	spline.Emplace(nullptr);
	spline.Emplace(CreateSpline(CreateJigsawSplinePoints()));

	auto vertices = CreatePieceRoundVertices(spline, 32);
	for (int i = 0; i < vertices.Num(); i++) {
		vertices[i] *= 10;
	}

	CreatePieceMesh(piece->GetBody(), vertices);

	return piece;
}

bool APieceGenerator::CreatePieceMesh(UProceduralMeshComponent* MeshComponent, TArray<FVector> PieceLinePoints) {
	
	//�e�N�X�`���[���W��ݒ�B(���W�𔼕��ɂ��Ďg���B���R�̓s�[�X�̓ˋN������\��G���A���K�v�Ȃ���)
	//�������傫���l�p��UV�}�b�v�A�������l�p�̓s�[�X�B
	//������
	//������
	TArray<FVector2D> texcoords0;
	for (auto item : PieceLinePoints) {
		texcoords0.Emplace(item / 2);
	}

	//���_�J���[��ݒ�
	TArray<FLinearColor> vertex_colors;
	for (int i = 0; i < PieceLinePoints.Num(); i++) {
		vertex_colors.Emplace(FLinearColor(1, 1, 1));
	}

	// UProceduralMeshComponent::CreateMeshSection_LinearColor �Ń��b�V���𐶐��B
	// ��1����: �Z�N�V�����ԍ�; 0, 1, 2, ... ��^���鎖��1�� UProceduralMeshComponent �ɕ����̃��b�V��������I�ɓ����ɐ����ł��܂��B
	// ��2����: ���_�Q
	// ��3����: �C���f�b�N�X�Q
	// ��4����: �@���Q
	// ��5����: �e�N�X�`���[���W�Q
	// ��6����: ���_�J���[�Q
	// ��7����: �@���Q
	// ��8����: �R���W���������t���O
	MeshComponent->CreateMeshSection_LinearColor(0, PieceLinePoints, ConvexPartitioning(PieceLinePoints), TArray<FVector>(), texcoords0, vertex_colors, TArray<FProcMeshTangent>(), false);
	
	return true;
}

const TArray<int32> APieceGenerator::ConvexPartitioning(TArray<FVector> RoundVertices) {

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
				//DrawDebugLine(GetWorld(), RoundVertices[left], RoundVertices[targetPoint], FColor::Red, false, 10.0f);
				//DrawDebugLine(GetWorld(), RoundVertices[targetPoint], RoundVertices[right], FColor::Red, false, 10.0f);
				//DrawDebugLine(GetWorld(), RoundVertices[right], RoundVertices[left], FColor::Red, false, 10.0f);

				//DrawDebugPoint(GetWorld(), RoundVertices[targetPoint], 10, FColor::Red, false, 10);

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
		FVector(-0.5, 0.5, 0)
	};

	//Spline�̈ʒu����
	for (int i = 0; i < 4; i++) {
		if (SplineArray[i]) SplineArray[i]->SetWorldLocationAndRotation(cyclePoints[i], FRotator(0, i * 90, 0));

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

