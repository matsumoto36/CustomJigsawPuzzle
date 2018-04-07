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

	// Nullチェック
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

	//ランダムにピースの形状を作る
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
	
	//テクスチャー座標を設定。(座標を半分にして使う。理由はピースの突起部分を貼るエリアが必要なため)
	//┌─┐大きい四角はUVマップ、小さい四角はピース。
	//│□│
	//└─┘
	TArray<FVector2D> texcoords0;
	for (auto item : PieceLinePoints) {
		texcoords0.Emplace(item / 2);
	}

	//頂点カラーを設定
	TArray<FLinearColor> vertex_colors;
	for (int i = 0; i < PieceLinePoints.Num(); i++) {
		vertex_colors.Emplace(FLinearColor(1, 1, 1));
	}

	// UProceduralMeshComponent::CreateMeshSection_LinearColor でメッシュを生成。
	// 第1引数: セクション番号; 0, 1, 2, ... を与える事で1つの UProceduralMeshComponent に複数のメッシュを内部的に同時に生成できます。
	// 第2引数: 頂点群
	// 第3引数: インデックス群
	// 第4引数: 法線群
	// 第5引数: テクスチャー座標群
	// 第6引数: 頂点カラー群
	// 第7引数: 法線群
	// 第8引数: コリジョン生成フラグ
	MeshComponent->CreateMeshSection_LinearColor(0, PieceLinePoints, ConvexPartitioning(PieceLinePoints), TArray<FVector>(), texcoords0, vertex_colors, TArray<FProcMeshTangent>(), false);
	
	return true;
}

const TArray<int32> APieceGenerator::ConvexPartitioning(TArray<FVector> RoundVertices) {

	//計算用頂点番号データを作成
	TArray<int32> verticesIndex;
	for (int i = 0; i < RoundVertices.Num(); i++) {
		verticesIndex.Emplace(i);
	}

	TArray<int32> indices;
	while (RoundVertices.Num() > 3) {

		int targetPoint = -1;

		/*原点から一番遠い点を求める*/

		float temp = 0;
		for (int i = 0; i < RoundVertices.Num(); i++) {

			float temp2 = RoundVertices[i].SizeSquared2D();
			if (temp < temp2) {
				temp = temp2;
				targetPoint = i;
			}
		}

		/*その点と隣同士でできる三角形の中にほかの頂点があるか調べる*/

		//内側から点を正面に見た場合 ----(left)----(targetPoint)----(right)----
		int left = targetPoint - 1 >= 0 ? targetPoint - 1 : RoundVertices.Num() - 1;
		int right = targetPoint + 1 < RoundVertices.Num() ? targetPoint + 1 : 0;

		//三角形が作成できるまで続ける
		bool isCreateTriangle = false;
		while (!isCreateTriangle) {

			//三角形の各頂点を順番に一周したときに点が同じ方向にあり続けた場合は三角形の中に点は存在する
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
				/*三角形が作成できないので頂点を変更する*/

				//三角形の向きを保存しておく
				float triangleSide1 = ((RoundVertices[left] - RoundVertices[targetPoint]) ^ (RoundVertices[right] - RoundVertices[left])).Z;

				float checkSide1;

				do {

					//一つ隣に移動する
					left = targetPoint;
					targetPoint = right;
					right = right + 1 < RoundVertices.Num() ? right + 1 : 0;

					//向きが同じか検査
					checkSide1 = ((RoundVertices[left] - RoundVertices[targetPoint]) ^ (RoundVertices[right] - RoundVertices[left])).Z;

					//向きが同じ三角形になるまで調べる
				} while (!(
					(triangleSide1 < 0 && checkSide1 < 0) || 
					(triangleSide1 > 0 && checkSide1 > 0)));

			}
			else {
				/*三角形が作成できるので作成して削除*/

				//debug
				//DrawDebugLine(GetWorld(), RoundVertices[left], RoundVertices[targetPoint], FColor::Red, false, 10.0f);
				//DrawDebugLine(GetWorld(), RoundVertices[targetPoint], RoundVertices[right], FColor::Red, false, 10.0f);
				//DrawDebugLine(GetWorld(), RoundVertices[right], RoundVertices[left], FColor::Red, false, 10.0f);

				//DrawDebugPoint(GetWorld(), RoundVertices[targetPoint], 10, FColor::Red, false, 10);

				//UE4では「左手座標系Z-top」の「反時計回り面生成」
				indices.Emplace(verticesIndex[right]);
				indices.Emplace(verticesIndex[targetPoint]);
				indices.Emplace(verticesIndex[left]);

				//頂点を削除して脱出
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

	//残った三角形を追加して終了
	indices.Emplace(verticesIndex[2]);
	indices.Emplace(verticesIndex[1]);
	indices.Emplace(verticesIndex[0]);

	return indices;
}

const TArray<FVector> APieceGenerator::CreatePieceRoundVertices(TArray<USplineComponent*> SplineArray, int Partition) {

	//MeshComponentは左側から時計回りに、へこみを作りたくない場合はnullptrにする

	TArray<FVector> cyclePoints = { 
		FVector(-0.5, -0.5, 0),
		FVector(0.5,-0.5, 0),
		FVector(0.5, 0.5, 0),
		FVector(-0.5, 0.5, 0)
	};

	//Splineの位置調整
	for (int i = 0; i < 4; i++) {
		if (SplineArray[i]) SplineArray[i]->SetWorldLocationAndRotation(cyclePoints[i], FRotator(0, i * 90, 0));

	}

	//点に変換
	TArray<FVector> points;
	float p = Partition;

	for (int i = 0; i < 4; i++) {

		if (SplineArray[i]) {
			//比率1の頂点は重複するので追加しない
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

	//スプライン曲線を定義
	auto spline = NewObject<USplineComponent>(this);
	spline->RegisterComponent();
	spline->AttachToComponent(RootComponent, { EAttachmentRule::SnapToTarget, true }, TEXT("Spline"));

	//最初から含まれている2点を削除
	spline->ClearSplinePoints();

	//点データを定義
	TArray<FSplinePoint> splinePoints;
	for (int i = 0; i < Points.Num();i++) {
		splinePoints.Emplace(FSplinePoint(i, Points[i]));
	}

	//定義したデータを追加
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

