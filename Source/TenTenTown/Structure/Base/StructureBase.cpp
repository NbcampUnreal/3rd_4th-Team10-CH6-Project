#include "Structure/Base/StructureBase.h"

AStructureBase::AStructureBase()
{
	PrimaryActorTick.bCanEverTick = false;

	AbilitySystemComponent = CreateDefaultSubobject<UAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
	AbilitySystemComponent->SetIsReplicated(true);
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Minimal);

	AttributeSet = CreateDefaultSubobject<UAS_StructureAttributeSet>(TEXT("AttributeSet"));
}

UAbilitySystemComponent* AStructureBase::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

void AStructureBase::BeginPlay()
{
	Super::BeginPlay();

	if (AbilitySystemComponent)
	{
		AbilitySystemComponent->InitAbilityActorInfo(this, this);
		
		if (AttributeSet)
		{
			AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(
				UAS_StructureAttributeSet::GetHealthAttribute()
			).AddUObject(this, &AStructureBase::OnHealthChanged);
		}
	}

	// 시작 시 데이터 로드 및 초기화
	InitializeStructure();
}

void AStructureBase::InitializeStructure()
{
	if (!StructureDataTable || StructureRowName.IsNone()) return;

	static const FString ContextString(TEXT("Structure Initialization"));
	FStructureData* Data = StructureDataTable->FindRow<FStructureData>(StructureRowName, ContextString);

	if (Data)
	{
		CachedStructureData = *Data;
		CurrentUpgradeLevel = 1;

		// GAS 체력 초기화
		if (AttributeSet)
		{
			AttributeSet->SetMaxHealth(CachedStructureData.Health);
			AttributeSet->SetHealth(CachedStructureData.Health);
		}
	}
}

int32 AStructureBase::GetUpgradeCost() const
{
	if (CurrentUpgradeLevel >= CachedStructureData.MaxUpgradeLevel) return 0;

	// 현재 레벨에 따라 다음 레벨 비용 반환
	if (CurrentUpgradeLevel == 1) return CachedStructureData.UpgradeCost_Lv2;
	if (CurrentUpgradeLevel == 2) return CachedStructureData.UpgradeCost_Lv3;

	return 99999;
}

int32 AStructureBase::GetSellReturnAmount() const
{
	return CachedStructureData.InstallCost;
	
	/*int32 TotalSpent = CachedStructureData.InstallCost;

	// 업그레이드에 쓴 돈도 계산에 포함
	if (CurrentUpgradeLevel >= 2) TotalSpent += CachedStructureData.UpgradeCost_Lv2;
	if (CurrentUpgradeLevel >= 3) TotalSpent += CachedStructureData.UpgradeCost_Lv3;

	// 70% 환급
	return FMath::FloorToInt(TotalSpent * 0.7f);*/
}

void AStructureBase::UpgradeStructure()
{
	if (CurrentUpgradeLevel >= CachedStructureData.MaxUpgradeLevel) return;

	CurrentUpgradeLevel++;
	
	UE_LOG(LogTemp, Log, TEXT("[StructureBase] Upgraded to Level %d"), CurrentUpgradeLevel);
}

void AStructureBase::SellStructure()
{
	UE_LOG(LogTemp, Log, TEXT("[StructureBase] Selling Structure..."));
	Destroy();
}

void AStructureBase::OnHealthChanged(const FOnAttributeChangeData& Data)
{
	if (Data.NewValue <= 0.0f)
	{
		HandleDestruction();
	}
}

void AStructureBase::HandleDestruction()
{
	if (IsPendingKillPending()) return;

	// 이벤트 브로드캐스트
	if (OnStructureDestroyed.IsBound())
	{
		OnStructureDestroyed.Broadcast(this);
	}

	// 서버 체크
	if (HasAuthority())
	{
		SetActorEnableCollision(false); // 더 이상 안 맞게 충돌 끄기
		Destroy();
	}
}
