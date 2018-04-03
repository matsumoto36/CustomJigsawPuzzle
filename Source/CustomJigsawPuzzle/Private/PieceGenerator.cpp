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

	return piece;
}

bool APieceGenerator::CreatePieceMesh(UProceduralMeshComponent* MeshComponent, UMaterialInterface* Material, int x, int y) {
	
	// 頂点群
	TArray<FVector> vertices;
	// インデックス群
	TArray<int32> indices;
	// 法線群（今回は空っぽのまま）
	TArray<FVector> normals;
	// テクスチャー座標群
	TArray<FVector2D> texcoords0;
	// 頂点カラー群（今回は空っぽのまま）
	TArray<FLinearColor> vertex_colors;
	// 接線群（今回は空っぽのまま）
	TArray<FProcMeshTangent> tangents;
	
	//頂点群を定義
	vertices.Emplace(-5.0, -5.0, 0);
	vertices.Emplace(-5.0, 5.0, 0);
	vertices.Emplace(5.0, -5.0, 0);
	vertices.Emplace(5.0, 5.0, 0);

	// UE は「左手座標系Z-top」の「反時計回り面生成」なので↑の頂点をこの順序で繋ぐと+Z向きに三角面1つを生成できる。
	indices.Emplace(0);
	indices.Emplace(1);
	indices.Emplace(2);
	indices.Emplace(1);
	indices.Emplace(2);
	indices.Emplace(3);

	// テクスチャー座標を設定しておけばこんなエグザンプルでも適当なマテリアルをセットして模様出し確認はできます。
	texcoords0.Emplace(0, 0);
	texcoords0.Emplace(0, 0);
	texcoords0.Emplace(0, 0);
	texcoords0.Emplace(0, 0);

	// UProceduralMeshComponent::CreateMeshSection_LinearColor でメッシュを生成。
	// 第1引数: セクション番号; 0, 1, 2, ... を与える事で1つの UProceduralMeshComponent に複数のメッシュを内部的に同時に生成できます。
	// 第2引数: 頂点群
	// 第3引数: インデックス群
	// 第4引数: 法線群
	// 第5引数: テクスチャー座標群
	// 第6引数: 頂点カラー群
	// 第7引数: 法線群
	// 第8引数: コリジョン生成フラグ
	MeshComponent->CreateMeshSection_LinearColor(0, vertices, indices, normals, texcoords0, vertex_colors, tangents, false);

	//マテリアルを設定
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



