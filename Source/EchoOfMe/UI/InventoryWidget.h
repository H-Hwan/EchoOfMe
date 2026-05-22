// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "InventoryWidget.generated.h"


class UInventoryComponent;
class UInventoryItemDefinition;
class UInventoryEntryWidget;
class UPanelWidget;


UCLASS()
class ECHOOFME_API UInventoryWidget : public UUserWidget {
	GENERATED_BODY()

public:
	// 컨트롤러에서 호출
	void BindInventory(UInventoryComponent* Inventory);

protected:
	virtual void NativeDestruct() override;

	UFUNCTION()
	void RefreshItems();

	UFUNCTION()
	void HandleEntryClicked(UInventoryItemDefinition* Item);

	// 엔트리가 채워질 컨테이너
	TObjectPtr<UPanelWidget> ItemList;

	// 엔트리 위젯 클래스
	UPROPERTY(EditDefaultsOnly, Category = "Inventory")
	TSubclassOf<UInventoryEntryWidget> EntryWidgetClass;

private:
	UPROPERTY()
	TObjectPtr<UInventoryComponent> InventoryComp;
};
