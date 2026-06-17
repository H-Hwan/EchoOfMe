// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "StorySequence.generated.h"


class UTexture2D;
class USoundBase;


/*	[FStoryPage]
	스토리 시퀀스의 한 페이지.
	텍스트(자막), 이미지, 음성, 최소 표시 시간을 묶음	*/
USTRUCT(BlueprintType)
struct FStoryPage
{
	GENERATED_BODY()

	// 페이지 자막
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(MultiLine="true"))
	FText Text;

	// 페이지 이미지 (그림책 톤)
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<UTexture2D> Image;

	/*	페이지 음성 (선택)
		비워두면 텍스트만으로 진행	*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<USoundBase> Voice;

	/*	최소 표시 시간 (초)
		음성이 없거나 짧을 때 페이지가 너무 빨리 넘어가지 않도록 보장
		음성 길이가 더 길면 음성 종료가 우선	*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(ClampMin="0.5"))
	float MinDuration = 3.f;

	// true면 이전 페이지의 이미지/누적 텍스트를 유지
	// false면 이미지 교체 + Old/New 모두 새로 세팅
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool bContinueFromPrevious = false;
};


/*	[UStorySequence]: 스토리 시퀀스 데이터 에셋
	- 도입부: NextLevelName 지정, bPauseWorldDuringPlayback = false
	- 메모리 회수 컷: NextLevelName 비움, bPauseWorldDuringPlayback = true
	- 엔딩: 상황에 맞춰 설정	*/
UCLASS(BlueprintType)
class ECHOOFME_API UStorySequence : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	// 페이지 시퀀스
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Story")
	TArray<FStoryPage> Pages;

	/*	재생 종료 후 이동할 레벨
		NAME_None이면 위젯만 닫고 현재 레벨 유지	*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Story")
	FName NextLevelName = NAME_None;

	/*	본편 진행 중 인터럽트 컷 여부
		true면 재생 중 게임을 일시정지하고 입력 모드를 UI Only로 전환
		(도입부는 false, 메모리 회수 컷은 true)	*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Story")
	bool bPauseWorldDuringPlayback = false;

	/*	스토리 재생 동안 루프될 배경음악 (선택)
		비워두면 기존 레벨의 음악이 유지되거나 조용히 재생됩니다. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Story|Audio")
	TObjectPtr<USoundBase> BackgroundMusic;

	// 배경음악의 기본 볼륨 크기 (0.0 ~ 1.0)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Story|Audio", meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float BgmVolume = 1.0f;

	// 음악이 처음 시작될 때 부드럽게 커지는 시간 (초)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Story|Audio", meta = (ClampMin = "0.0"))
	float BgmFadeInTime = 1.5f;

	/*	스토리가 끝날 때 음악을 부드럽게 끌 시간 (초)
		*주의: NextLevelName을 지정해 레벨이 바로 바뀌는 상황이라면 페이드아웃이 잘리지 않게 처리가 필요합니다. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Story|Audio", meta = (ClampMin = "0.0"))
	float BgmFadeOutTime = 1.5f;
};
