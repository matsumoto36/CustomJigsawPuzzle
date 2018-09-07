// Fill out your copyright notice in the Description page of Project Settings.

#include "PuzzleGameMode.h"
#include "PuzzlePlayerController.h"
#include "PlayerPawn.h"

#include "Engine.h"

//#include "Developer/DesktopPlatform/Public/DesktopPlatformModule.h"
#include "Runtime/Engine/Classes/Engine/Texture2D.h"
#include "Runtime/Slate/Public/Framework/Application/SlateApplication.h"
#include "Runtime/Core/Public/Misc/FileHelper.h"
#include "Runtime/Engine/Classes/Engine/World.h"
//#include "Editor/MainFrame/Public/Interfaces/IMainFrameModule.h"


APuzzleGameMode::APuzzleGameMode() {

	DefaultPawnClass = APlayerPawn::StaticClass();
	PlayerControllerClass = APuzzlePlayerController::StaticClass();
}

void APuzzleGameMode::InitializeGame() {

 	if (GEngine)
		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, "Initialize GameMode");

	UWorld* const World = GetWorld();

	// Nullチェック
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

	//DesktopPlatformはパッケージ不可
	return false;

	//const void* parentWindowHandle = nullptr;

	////ウィンドウハンドルの取得
	//IMainFrameModule& MainFrameModule = FModuleManager::LoadModuleChecked<IMainFrameModule>(TEXT("MainFrame"));
	//const TSharedPtr<SWindow>& MainFrameParentWindow = MainFrameModule.GetParentWindow();
	//if (MainFrameParentWindow.IsValid() && MainFrameParentWindow->GetNativeWindow().IsValid()) {
	//	parentWindowHandle = MainFrameParentWindow->GetNativeWindow()->GetOSWindowHandle();
	//}

	////ファイルパスの取得
	//TArray<FString> outFilenames;
	//int32 outFilterIndex;

	//bool result = FDesktopPlatformModule::Get()->OpenFileDialog(
	//	parentWindowHandle,
	//	TEXT("SelectPuzzleTexture"),
	//	TEXT(""),
	//	TEXT(""),
	//	TEXT("Texture|*.png;*.jpg;*.bmp|All Files|*.*"),
	//	EFileDialogFlags::None,
	//	outFilenames,
	//	outFilterIndex);

	////ダイアログがキャンセルされた場合
	//if (!result) return false;

	////パスから読み込み
	//int32 width, height;
	//bool isLoaded;
	//PuzzleTexture = LoadTexture2DFromFile(outFilenames[0], isLoaded, width, height);

	//if (isLoaded) {
	//	if (GEngine)
	//		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, FString::FromInt(PuzzleTexture->GetSizeX()));

	//}

	//return isLoaded;	//const void* parentWindowHandle = nullptr;

	////ウィンドウハンドルの取得
	//IMainFrameModule& MainFrameModule = FModuleManager::LoadModuleChecked<IMainFrameModule>(TEXT("MainFrame"));
	//const TSharedPtr<SWindow>& MainFrameParentWindow = MainFrameModule.GetParentWindow();
	//if (MainFrameParentWindow.IsValid() && MainFrameParentWindow->GetNativeWindow().IsValid()) {
	//	parentWindowHandle = MainFrameParentWindow->GetNativeWindow()->GetOSWindowHandle();
	//}

	////ファイルパスの取得
	//TArray<FString> outFilenames;
	//int32 outFilterIndex;

	//bool result = FDesktopPlatformModule::Get()->OpenFileDialog(
	//	parentWindowHandle,
	//	TEXT("SelectPuzzleTexture"),
	//	TEXT(""),
	//	TEXT(""),
	//	TEXT("Texture|*.png;*.jpg;*.bmp|All Files|*.*"),
	//	EFileDialogFlags::None,
	//	outFilenames,
	//	outFilterIndex);

	////ダイアログがキャンセルされた場合
	//if (!result) return false;

	////パスから読み込み
	//int32 width, height;
	//bool isLoaded;
	//PuzzleTexture = LoadTexture2DFromFile(outFilenames[0], isLoaded, width, height);

	//if (isLoaded) {
	//	if (GEngine)
	//		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, FString::FromInt(PuzzleTexture->GetSizeX()));

	//}

	//return isLoaded;
}

void APuzzleGameMode::ShuffleAndSetPiece(TArray<APiece*> PieceArray, FVector2D ShuffleArea, float StartHeight) {

	auto size = PieceArray.Num();

	//混ぜる
	for (int i = 0; i < size; i++) {
		auto select = FMath::RandRange(0, size - 1);
		auto t = PieceArray[i];
		PieceArray[i] = PieceArray[select];
		PieceArray[select] = t;
	}

	//適当に置く
	float setHeight = StartHeight;
	float dh = PieceArray[0]->GetBody()->GetComponentScale().Z + 0.1f;
	for (int i = 0; i < size; i++) {
		
		auto body = PieceArray[i]->GetBody();

		//回転
		//反転するか決める
		body->AddWorldRotation(FRotator(FMath::RandBool() ? 180 : 0, 0, 0));
		body->AddWorldRotation(FRotator(0, FMath::RandRange(0.0f, 360.0f), 0));

		//位置決め
		body->SetWorldLocation(FVector(
			FMath::RandRange(-ShuffleArea.X, ShuffleArea.X),
			FMath::RandRange(-ShuffleArea.Y, ShuffleArea.Y),
			setHeight
		));

		setHeight += dh;
	}
}

TArray<APiece*> APuzzleGameMode::GeneratePuzzle(UTexture2D* PieceTexture, int RowCount, int ColumnCount, int EdgePartition) {

	if (RowCount <= 0) {
		UE_LOG(LogTemp, Error, TEXT("RowCount is Too Small."));
		return TArray<APiece*>();
	}

	if (ColumnCount <= 0) {
		UE_LOG(LogTemp, Error, TEXT("ColumnCount is Too Small."));
		return TArray<APiece*>();
	}

	if (EdgePartition <= 1) {
		UE_LOG(LogTemp, Error, TEXT("EdgePartition is Too Small."));
		return TArray<APiece*>();
	}

	/*ランダムな曲線データを生成*/

	//ピースの縦の接続を格納 例) - - - -
	TArray<TArray<USplineComponent*>> splinesConnectVerical;
	TArray<TArray<bool>> scvInverseFlg;
	for (int y = 0; y < RowCount - 1; y++) {
		
		TArray<USplineComponent*> bff;
		TArray<bool> bffFlg;
		for (int x = 0; x < ColumnCount; x++) {

			bff.Emplace(PieceGenerator
				->CreateSpline(PieceGenerator
					->CreateJigsawSplinePoints()));

			bffFlg.Emplace(false);
		}
		splinesConnectVerical.Emplace(bff);
		scvInverseFlg.Emplace(bffFlg);
	}

	//ピースの横の接続を格納 例) | | | |
	TArray<TArray<USplineComponent*>> splinesConnectHorizontal;
	TArray<TArray<bool>> schInverseFlg;
	for (int y = 0; y < RowCount; y++) {

		TArray<USplineComponent*> bff;
		TArray<bool> bffFlg;
		for (int x = 0; x < ColumnCount - 1; x++) {

			bff.Emplace(PieceGenerator
				->CreateSpline(PieceGenerator
					->CreateJigsawSplinePoints()));

			bffFlg.Emplace(false);
		}
		splinesConnectHorizontal.Add(bff);
		schInverseFlg.Emplace(bffFlg);
	}

	/*組み合わせて頂点を作成*/

	#pragma region memo
	//左側から時計回りに、へこみを作りたくない場合はnullptrにする
	//  2
	// 1□3
	//  4
	//
	//□□□
	//□□□
	//□□□の場合( n = nullptr, h = horizontal, v = vertical, ! = inverse)
	//   <left>     <top>   <right>  <bottom>
	//00        n,        n,  h[0][0],  v[0][0]
	//01 !h[0][0],        n,  h[0][1],  v[0][1]
	//02 !h[0][1],        n,        n,  v[0][2]
						    		  
	//10        n, !v[0][0],  h[1][0],  v[1][0]
	//11 !h[1][0], !v[0][1],  h[1][1],  v[1][1]
	//12 !h[1][1], !v[0][2],        n,  v[1][2]
	//			            		  
	//20        n, !v[1][0],  h[2][0],        n
	//21 !h[2][0], !v[1][1],  h[2][1],        n
	//22 !h[2][1], !v[1][2],        n,        n
	#pragma endregion

	auto uDelta = 1.0f / RowCount;
	auto vDelta = 1.0f / ColumnCount;
	TArray<APiece*> pieceArray;

	for (int y = 0; y < RowCount; y++) {
		for (int x = 0; x < ColumnCount; x++) {
			
			TArray<USplineComponent*> splineArray;
			TArray<bool> splineInverseFlg;

			//set left
			if (x == 0) {
				splineArray.Emplace(nullptr);
				splineInverseFlg.Emplace(false);
			}
			else {
				auto spline = splinesConnectHorizontal[y][x - 1];
				splineArray.Emplace(spline);

				splineInverseFlg.Emplace(schInverseFlg[y][x - 1]);
				schInverseFlg[y][x - 1] = !schInverseFlg[y][x - 1];
			}

			//set top
			if (y == 0) {
				splineArray.Emplace(nullptr);
				splineInverseFlg.Emplace(false);
			}
			else {
				auto spline = splinesConnectVerical[y - 1][x];
				splineArray.Emplace(spline);

				splineInverseFlg.Emplace(scvInverseFlg[y - 1][x]);
				scvInverseFlg[y - 1][x] = !scvInverseFlg[y - 1][x];
			}

			//set right
			if (x == ColumnCount - 1) {
				splineArray.Emplace(nullptr);
				splineInverseFlg.Emplace(false);
			}
			else {
				auto spline = splinesConnectHorizontal[y][x];
				splineArray.Emplace(spline);

				splineInverseFlg.Emplace(schInverseFlg[y][x]);
				schInverseFlg[y][x] = !schInverseFlg[y][x];
			}

			//set bottom
			if (y == RowCount - 1) {
				splineArray.Emplace(nullptr);
				splineInverseFlg.Emplace(false);
			}
			else {
				auto spline = splinesConnectVerical[y][x];
				splineArray.Emplace(spline);

				splineInverseFlg.Emplace(scvInverseFlg[y][x]);
				scvInverseFlg[y][x] = !scvInverseFlg[y][x];
			}

			//スポーン位置を設定
			FTransform transform = FTransform();

			transform.SetLocation(FVector(x + ColumnCount / 2.0f, y + RowCount / 2.0f, 100.0f));
			transform.SetScale3D(FVector(1.0f, 1.0f, 1.0f));

			//ピースの形状を作成
			auto pieceLinePoints = PieceGenerator->CreatePieceRoundVertices(splineArray, splineInverseFlg, EdgePartition);

			//生成
			auto piece = PieceGenerator->SpawnPiece(transform, pieceLinePoints);

			//配置の設定
			piece->SetPieceMapPosition(x, y);

			//画像の変更
			piece->SetPuzzleTexture(PieceTexture, uDelta, vDelta, uDelta * x + uDelta * 0.5f, vDelta * y + vDelta * 0.5f);

			pieceArray.Emplace(piece);
		}
	}

	return pieceArray;
}

UTexture2D* APuzzleGameMode::LoadTexture2DFromFile(const FString& FileName, bool& IsValid, int32& Width, int32& Height) {

	IsValid = false;

	if (GEngine)
		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, "Start LoadTexture2DFromFile");

	//ファイルパスから拡張子を抜き出す
	FString fileNameWithOutExt, fileExt;
	if (!FileName.Split(".", &fileNameWithOutExt, &fileExt, ESearchCase::CaseSensitive, ESearchDir::FromEnd)) return nullptr;

	IImageWrapperModule& ImageWrapperModule = FModuleManager::LoadModuleChecked<IImageWrapperModule>(FName("ImageWrapper"));
	EImageFormat format = ConvertImageFormat(fileExt);

	//拡張子が判断できなかった場合
	if (format == EImageFormat::Invalid) return nullptr;

	auto ImageWrapper = ImageWrapperModule.CreateImageWrapper(format).ToSharedRef();

	//Load From File
	TArray<uint8> RawFileData;
	if (!FFileHelper::LoadFileToArray(RawFileData, *FileName)) return nullptr;

	if (GEngine)
		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, "Load RawData");

	//Create T2D!
	if (ImageWrapper->SetCompressed(RawFileData.GetData(), RawFileData.Num())) {
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