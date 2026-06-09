// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Engine/Texture2D.h"
#include "Sound/SoundBase.h"
#include "Story.generated.h"
// 1. 스토리 페이지 데이터 구조체 정의
USTRUCT(BlueprintType)
struct FStoryPage
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Story")
	FText Text;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Story")
	UTexture2D* Image;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Story")
	USoundBase* Voice;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Story")
	float MinDuration = 3.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Story")
	bool bContinueFromPrevious = false;
};

UCLASS(BlueprintType)
class ECHOOFME_API UStory : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	// 데이터 에셋에서 보여줄 페이지 목록
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Story")
	TArray<FStoryPage> Pages;

	// 블루프린트에서 편하게 사용하기 위한 유틸리티 함수
	UFUNCTION(BlueprintCallable, Category = "Story")
	bool GetPage(int32 Index, FStoryPage& OutPage) const;
};
