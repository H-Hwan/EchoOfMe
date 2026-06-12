// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/TextBlock.h" // 텍스트 위젯을 위해 필수
#include "Components/Image.h"     //
#include "StoryWidget.generated.h"


class UStorySequence;
struct FStoryPage;


UCLASS()
class ECHOOFME_API UStoryWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	// 컴포넌트가 호출하는 용도
	UFUNCTION(BlueprintImplementableEvent, Category = "Story")
	void PlaySequence(UStorySequence* Sequence);

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Story")
	TArray<FStoryPage> Pages;

	UFUNCTION(BlueprintCallable, Category = "Story")
	bool GetPage(int32 Index, FStoryPage& OutPage) const;

	/*UFUNCTION()
	void ShowPage();



	UFUNCTION(BlueprintCallable, Category = "Sequence")
    void FinishSequence();

	UPROPERTY()
	UAudioComponent* CurrentVoiceAudio;

	UFUNCTION()
	void PlaySequence();
*/


};
