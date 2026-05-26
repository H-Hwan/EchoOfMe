// Fill out your copyright notice in the Description page of Project Settings.


#include "Component/InventoryComponent.h"

#include "Data/InventoryItemDefinition.h"


// Sets default values for this component's properties
UInventoryComponent::UInventoryComponent() {
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;
}


bool UInventoryComponent::AddItem(UInventoryItemDefinition* Item) {
	// 중복 방지
	if (!Item || HasItem(Item->ItemID)) return false;

	Items.Add(Item);
	UE_LOG(LogTemp, Warning, TEXT("[Inventory] AddItem 성공, 총 %d"), Items.Num());
	OnInventoryChanged.Broadcast();
	UE_LOG(LogTemp, Warning, TEXT("[Inventory] AddItem 방송, this=%p"), this);

	return true;
}


bool UInventoryComponent::HasItem(FName ItemID) const {
	return Items.ContainsByPredicate(
		[ItemID](const UInventoryItemDefinition* It) {return It && It->ItemID == ItemID; }
	);
}


void UInventoryComponent::UseItem(UInventoryItemDefinition* Item) {
	if (Item && Item->bUsable && HasItem(Item->ItemID)) OnItemUsed.Broadcast(Item);
}
