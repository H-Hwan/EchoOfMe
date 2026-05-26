// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "InventoryComponent.generated.h"


class UInventoryItemDefinition;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnInventoryChanged);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnItemUsed, UInventoryItemDefinition*, Item);


UCLASS( ClassGroup=(EchoOfMe), meta=(BlueprintSpawnableComponent) )
class ECHOOFME_API UInventoryComponent : public UActorComponent {
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UInventoryComponent();

public:
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	bool AddItem(UInventoryItemDefinition* Item);

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	bool HasItem(FName ItemID) const;

	// 인벤토리UI에서 아이템을 [사용/재생]할 때 호출
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	void UseItem(UInventoryItemDefinition* Item);

	const TArray<TObjectPtr<UInventoryItemDefinition>>& GetItem() const { return Items; }

	// 목록이 바뀌면 방송 >> AI
	UPROPERTY(BlueprintAssignable, Category = "Inventory")
	FOnInventoryChanged OnInventoryChanged;

	// 사용 가능한 아이템이 사용되면 방송 >> 녹음기 시스템
	UPROPERTY(BlueprintAssignable, Category = "Inventory")
	FOnItemUsed OnItemUsed;

protected:
	UPROPERTY(VisibleAnywhere, Category = "Inventory")
	TArray<TObjectPtr<UInventoryItemDefinition>> Items;
};
