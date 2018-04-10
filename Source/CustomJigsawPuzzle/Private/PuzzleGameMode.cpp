// Fill out your copyright notice in the Description page of Project Settings.

#include "PuzzleGameMode.h"
#include "PuzzlePlayerController.h"
#include "PlayerPawn.h"

#include "Engine.h"

#include "Developer/DesktopPlatform/Public/DesktopPlatformModule.h"
#include "Runtime/Engine/Classes/Engine/Texture2D.h"
#include "Runtime/Slate/Public/Framework/Application/SlateApplication.h"
#include "Runtime/Core/Public/Misc/FileHelper.h"
#include "Runtime/Engine/Classes/Engine/World.h"
#include "Editor/MainFrame/Public/Interfaces/IMainFrameModule.h"


APuzzleGameMode::APuzzleGameMode() {

	DefaultPawnClass = APlayerPawn::StaticClass();
	PlayerControllerClass = APuzzlePlayerController::StaticClass();
}

void APuzzleGameMode::InitializeGame() {

	if (GEngine)
		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, "Initialize GameMode");

	UWorld* const World = GetWorld();

	// Null�`�F�b�N
	if (!World) return;

	if (GEngine)
		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, "GetWorld");

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

	PieceGenerator = GetWorld()->SpawnActor<APieceGenerator>(FVector(0, 0, 0), FRotator(0, 0, 0), SpawnParams);
}

bool APuzzleGameMode::LoadPuzzleTextureData() {

	const void* parentWindowHandle = nullptr;

	//�E�B���h�E�n���h���̎擾
	IMainFrameModule& MainFrameModule = FModuleManager::LoadModuleChecked<IMainFrameModule>(TEXT("MainFrame"));
	const TSharedPtr<SWindow>& MainFrameParentWindow = MainFrameModule.GetParentWindow();
	if (MainFrameParentWindow.IsValid() && MainFrameParentWindow->GetNativeWindow().IsValid()) {
		parentWindowHandle = MainFrameParentWindow->GetNativeWindow()->GetOSWindowHandle();
	}

	//�t�@�C���p�X�̎擾
	TArray<FString> outFilenames;
	int32 outFilterIndex;

	bool result = FDesktopPlatformModule::Get()->OpenFileDialog(
		parentWindowHandle,
		TEXT("SelectPuzzleTexture"),
		TEXT(""),
		TEXT(""),
		TEXT("Texture|*.png;*.jpg;*.bmp|All Files|*.*"),
		EFileDialogFlags::None,
		outFilenames,
		outFilterIndex);

	//�_�C�A���O���L�����Z�����ꂽ�ꍇ
	if (!result) return false;

	//�p�X����ǂݍ���
	int32 width, height;
	bool isLoaded;
	PuzzleTexture = LoadTexture2DFromFile(outFilenames[0], isLoaded, width, height);

	if (isLoaded) {
		if (GEngine)
			GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, FString::FromInt(PuzzleTexture->GetSizeX()));

	}

	return isLoaded;
}

TArray<APiece*> APuzzleGameMode::GeneratePuzzle(int RowCount, int ColumnCount, FVector2D DistSize, int EdgePartition) {

	/*�����_���ȋȐ��f�[�^�𐶐�*/

	//�s�[�X�̏c�̐ڑ����i�[ ��) - - - -
	TArray<TArray<USplineComponent*>> splinesConnectVerical;
	for (int y = 0; y < RowCount - 1; y++) {
		
		TArray<USplineComponent*> bff;
		for (int x = 0; x < ColumnCount; x++) {

			bff.Add(PieceGenerator
				->CreateSpline(PieceGenerator
					->CreateJigsawSplinePoints()));
		}
		splinesConnectVerical.Add(bff);
	}

	//�s�[�X�̉��̐ڑ����i�[ ��) | | | |
	TArray<TArray<USplineComponent*>> splinesConnectHorizontal;
	for (int y = 0; y < RowCount; y++) {

		TArray<USplineComponent*> bff;
		for (int x = 0; x < ColumnCount - 1; x++) {

			bff.Add(PieceGenerator
				->CreateSpline(PieceGenerator
					->CreateJigsawSplinePoints()));
		}
		splinesConnectHorizontal.Add(bff);
	}

	/*�g�ݍ��킹�Ē��_���쐬*/

	#pragma region memo
	//�������玞�v���ɁA�ւ��݂���肽���Ȃ��ꍇ��nullptr�ɂ���
	//  2
	// 1��3
	//  4
	//
	//������
	//������
	//�������̏ꍇ( n = nullptr, h = horizontal, v = vertical, ! = inverse)
	//
	//        n,        n,  h[0][0],  v[0][0]
	// !h[0][0],        n,  h[0][1],  v[0][1]
	// !h[0][1],        n,        n,  v[0][2]
						    		  
	//        n, !v[0][0],  h[1][0],  v[1][0]
	// !h[1][0], !v[0][1],  h[1][1],  v[1][1]
	// !h[1][1], !v[0][2],        n,  v[1][2]
	//			            		  
	//        n, !v[1][0],  h[2][0],        n
	// !h[2][0], !v[1][1],  h[2][1],        n
	// !h[2][1], !v[1][2],        n,        n
	#pragma endregion

	for (int y = 0; y < RowCount; y++) {
		for (int x = 0; x < ColumnCount; x++) {

		}
	}

}

UTexture2D* APuzzleGameMode::LoadTexture2DFromFile(const FString& FileName, bool& IsValid, int32& Width, int32& Height) {

	IsValid = false;

	if (GEngine)
		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, "Start LoadTexture2DFromFile");

	//�t�@�C���p�X����g���q�𔲂��o��
	FString fileNameWithOutExt, fileExt;
	if (!FileName.Split(".", &fileNameWithOutExt, &fileExt, ESearchCase::CaseSensitive, ESearchDir::FromEnd)) return nullptr;

	IImageWrapperModule& ImageWrapperModule = FModuleManager::LoadModuleChecked<IImageWrapperModule>(FName("ImageWrapper"));
	EImageFormat format = ConvertImageFormat(fileExt);

	//�g���q�����f�ł��Ȃ������ꍇ
	if (format == EImageFormat::Invalid) return nullptr;

	IImageWrapperPtr ImageWrapper = ImageWrapperModule.CreateImageWrapper(format);

	//Load From File
	TArray<uint8> RawFileData;
	if (!FFileHelper::LoadFileToArray(RawFileData, *FileName)) return nullptr;

	if (GEngine)
		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, "Load RawData");

	//Create T2D!
	if (ImageWrapper.IsValid() && ImageWrapper->SetCompressed(RawFileData.GetData(), RawFileData.Num())) {
		const TArray<uint8>* UncompressedBGRA = nullptr;
		if (ImageWrapper->GetRaw(ERGBFormat::BGRA, 8, UncompressedBGRA)) {
			UTexture2D* loadTexture = UTexture2D::CreateTransient(ImageWrapper->GetWidth(), ImageWrapper->GetHeight(), PF_B8G8R8A8);

			if (GEngine)
				GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, "Create Texture");

			//Valid?
			if (!loadTexture) return nullptr;
			//~~~~~~~~~~~~~~

			if (GEngine)
				GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, "Texture Is Valid");

			//Out!
			Width = ImageWrapper->GetWidth();
			Height = ImageWrapper->GetHeight();

			FString WidthStr = "Texture Width : "; WidthStr.Append(FString::FromInt(Width));
			FString HeightStr = "Texture Height : "; HeightStr.Append(FString::FromInt(Height));

			if (GEngine)
				GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, WidthStr);
			if (GEngine)
				GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, HeightStr);

			//Copy!
			void* TextureData = loadTexture->PlatformData->Mips[0].BulkData.Lock(LOCK_READ_WRITE);
			FMemory::Memcpy(TextureData, UncompressedBGRA->GetData(), UncompressedBGRA->Num());
			loadTexture->PlatformData->Mips[0].BulkData.Unlock();

			//Update!
			loadTexture->UpdateResource();

			// Success!
			IsValid = true;
			return loadTexture;
		}
	}

	return nullptr;
}

EImageFormat APuzzleGameMode::ConvertImageFormat(FString FileExt) {

	FString ext = FileExt.ToLower();

	if (ext == "png") return EImageFormat::PNG;
	if (ext == "jpeg") return EImageFormat::JPEG;
	if (ext == "jpg") return EImageFormat::JPEG;
	if (ext == "bmp") return EImageFormat::BMP;

	return EImageFormat::Invalid;
}