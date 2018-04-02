// Fill out your copyright notice in the Description page of Project Settings.

#include "PuzzleGameMode.h"
#include "PuzzlePlayerController.h"
#include "PlayerPawn.h"
#include "Runtime/Engine/Classes/Engine/Texture2D.h"
#include "Runtime/Slate/Public/Framework/Application/SlateApplication.h"
#include "Runtime/Core/Public/Misc/FileHelper.h"
#include "Developer/DesktopPlatform/Public/DesktopPlatformModule.h"

#include "Editor/MainFrame/Public/Interfaces/IMainFrameModule.h"

#include "Engine.h"

APuzzleGameMode::APuzzleGameMode() {

	DefaultPawnClass = APlayerPawn::StaticClass();
	PlayerControllerClass = APuzzlePlayerController::StaticClass();
}

bool APuzzleGameMode::LoadPuzzleTextureData() {

	const void* parentWindowHandle = nullptr;

	//ウィンドウハンドルの取得
	IMainFrameModule& MainFrameModule = FModuleManager::LoadModuleChecked<IMainFrameModule>(TEXT("MainFrame"));
	const TSharedPtr<SWindow>& MainFrameParentWindow = MainFrameModule.GetParentWindow();
	if (MainFrameParentWindow.IsValid() && MainFrameParentWindow->GetNativeWindow().IsValid()) {
		parentWindowHandle = MainFrameParentWindow->GetNativeWindow()->GetOSWindowHandle();
	}

	//ファイルパスの取得
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

	//ダイアログがキャンセルされた場合
	if (!result) return false;

	//パスから読み込み
	int32 width, height;
	bool isLoaded;
	PuzzleTexture = LoadTexture2DFromFile(outFilenames[0], isLoaded, width, height);

	if (isLoaded) {
		if (GEngine)
			//TODO : Fixed Error PuzzleTexture->GetSizeX()
			GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, FString::FromInt(PuzzleTexture->GetSizeX()));

	}

	return isLoaded;
}

UTexture2D* APuzzleGameMode::LoadTexture2DFromFile(const FString& FileName, bool& IsValid, int32& Width, int32& Height) {

	IsValid = false;

	if (GEngine)
		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, "Start LoadTexture2DFromFile");

	//ファイルパスから拡張子を抜き出す
	FString fileNameWithOutExt, fileExt;
	if (!FileName.Split(".", &fileNameWithOutExt, &fileExt, ESearchCase::CaseSensitive, ESearchDir::FromEnd)) return nullptr;

	IImageWrapperModule& ImageWrapperModule = FModuleManager::LoadModuleChecked<IImageWrapperModule>(FName("ImageWrapper"));
	IImageWrapperPtr ImageWrapper = ImageWrapperModule.CreateImageWrapper(ConvertImageFormat(fileExt));

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

	return EImageFormat::BMP;
}