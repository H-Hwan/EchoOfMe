#pragma once

#include "CoreMinimal.h"
#include "Data/StorySequence.h" // FStoryPage 구조체가 정의된 곳
#include "Story.generated.h"

UCLASS(BlueprintType)
class ECHOOFME_API UStory : public UPrimaryDataAsset
{
	GENERATED_BODY() // public: 아래에 두는 것을 권장

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Story")
	TArray<FStoryPage> Pages;

	UFUNCTION(BlueprintCallable, Category = "Story")
	bool GetPage(int32 Index, FStoryPage& OutPage) const;
};
