// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Component/CueComponentBase.h"
#include "MemoryCueComponent.generated.h"


// 기억조각에 부착 — 위치 안내 큐 (상시음 + 듣기음). 사운드는 MemoryFragmentDefinition에서 읽음
UCLASS(ClassGroup=(EchoOfMe), meta=(BlueprintSpawnableComponent))
class ECHOOFME_API UMemoryCueComponent : public UCueComponentBase
{
	GENERATED_BODY()

protected:
	virtual void ResolveCueData() override;
};
