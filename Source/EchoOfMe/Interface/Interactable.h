#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Interactable.generated.h"


UINTERFACE(MinimalAPI)
class UInteractable : public UInterface {
	GENERATED_BODY()
};


class ECHOOFME_API IInteractable {
	GENERATED_BODY()

public:
	// 상호작용 실행
	UFUNCTION(BlueprintNativeEvent, Category = "Interaction")
	void Interact(AActor* Interactor);

	//상호작용 프롬포트에 띄울 텍스트
	UFUNCTION(BlueprintNativeEvent, Category = "Interaction")
	FText GetInteractionPrompt() const;
};
