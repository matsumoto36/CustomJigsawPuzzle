// Fill out your copyright notice in the Description page of Project Settings.

#include "Piece.h"
#include "UObject/ConstructorHelpers.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/StaticMesh.h"
#include "Materials/MaterialInstance.h"

// Sets default values
APiece::APiece()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Structure to hold one-time initialization
	struct FConstructorStatics {

		ConstructorHelpers::FObjectFinderOptional<UStaticMesh> PieceMesh;
		ConstructorHelpers::FObjectFinderOptional<UMaterial> BaseMaterial;
		ConstructorHelpers::FObjectFinderOptional<UMaterialInstance> ActiveMaterial;
		ConstructorHelpers::FObjectFinderOptional<UMaterialInstance> SelectMaterial;
		FConstructorStatics()
			: PieceMesh(TEXT("/Game/Meshes/SM_TestPiece.SM_TestPiece"))
			, BaseMaterial(TEXT("/Game/Materials/MI_PieceBase.MI_PieceBase"))
			, ActiveMaterial(TEXT("/Game/Materials/MI_PieceActive.MI_PieceActive"))
			, SelectMaterial(TEXT("/Game/Materials/MI_PieceSelect.MI_PieceSelect"))
		{}
	};

	static FConstructorStatics ConstructorStatics;

	// Create dummy root scene component
	DummyRoot = CreateDefaultSubobject<USceneComponent>(TEXT("Dummy0"));
	RootComponent = DummyRoot;

	// Create static mesh component
	BlockMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BlockMesh0"));
	BlockMesh->SetStaticMesh(ConstructorStatics.PieceMesh.Get());
	BlockMesh->SetMaterial(0, ConstructorStatics.BaseMaterial.Get());
	BlockMesh->SetupAttachment(DummyRoot);
	BlockMesh->OnBeginCursorOver.AddDynamic(this, &APiece::MouseCursorOverBigin);
	BlockMesh->OnEndCursorOver.AddDynamic(this, &APiece::MouseCursorOverEnd);
	BlockMesh->OnClicked.AddDynamic(this, &APiece::BlockClicked);
	BlockMesh->OnInputTouchBegin.AddDynamic(this, &APiece::OnFingerPressedBlock);

	// Save a pointer to the orange material
	BaseMaterial = ConstructorStatics.BaseMaterial.Get();
	ActiveMaterial = ConstructorStatics.ActiveMaterial.Get();
	SelectMaterial = ConstructorStatics.SelectMaterial.Get();
}

// Called when the game starts or when spawned
void APiece::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void APiece::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}


void APiece::MouseCursorOverBigin(UPrimitiveComponent* TouchedComponent) {
	Highlight(true);
}

void APiece::MouseCursorOverEnd(UPrimitiveComponent* TouchedComponent) {
	Highlight(false);
}

void APiece::BlockClicked(UPrimitiveComponent* ClickedComp, FKey ButtonClicked) {
	HandleClicked();
}

void APiece::OnFingerPressedBlock(ETouchIndex::Type FingerIndex, UPrimitiveComponent* TouchedComponent) {
	HandleClicked();
}


void APiece::HandleClicked() {
	// Check we are not already active

	// Change material
	BlockMesh->SetMaterial(0, SelectMaterial);

}

void APiece::Highlight(bool bOn) {

	if (bOn) {
		BlockMesh->SetMaterial(0, BaseMaterial);
	}
	else {
		BlockMesh->SetMaterial(0, ActiveMaterial);
	}
}