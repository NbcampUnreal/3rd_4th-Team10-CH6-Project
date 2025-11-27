#include "Structure/Base/StructureBase.h"
#include "Components/StaticMeshComponent.h"
#include "Net/UnrealNetwork.h"

AStructureBase::AStructureBase()
{
	bReplicates = true;
	PrimaryActorTick.bCanEverTick = false;

	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
	RootComponent = MeshComp;
	
    // 콜리전 설정 (InteractionSystemComponent가 감지할 수 있도록)
	MeshComp->SetCollisionProfileName(TEXT("BlockAllDynamic")); 
}

void AStructureBase::BeginPlay()
{
	Super::BeginPlay();
    
    // 데이터 테이블에서 정보 읽어오기
	if (!StructureDataHandle.IsNull())
	{
		FStructureData* RowData = StructureDataHandle.GetRow<FStructureData>(TEXT("StructureBase"));
		if (RowData)
		{
			CachedStructureData = *RowData;
		}
	}
}

void AStructureBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AStructureBase, CurrentLevel);
}

void AStructureBase::OnInteract_Implementation(ACharacter* InstigatorCharacter)
{
	UE_LOG(LogTemp, Log, TEXT("[StructureBase] Interact! Open UI Here."));
}

void AStructureBase::Server_TryUpgrade_Implementation(ATTTPlayerState* RequestorPS)
{
	if (!RequestorPS) return;

	if (!CanUpgrade()) return; // 만렙 체크

	int32 Cost = GetUpgradeCost();
	if (RequestorPS->GetGold() >= Cost)
	{
		RequestorPS->AddGold(-Cost); 
		CurrentLevel++;
		OnRep_CurrentLevel(); // 서버에서도 갱신
	}
}

void AStructureBase::Server_TrySell_Implementation(ATTTPlayerState* RequestorPS)
{
    if (!RequestorPS) return;
    
    // 판매 로직 (예: 설치 비용의 50% 환불)
	int32 Refund = GetSellRefund();
	RequestorPS->AddGold(Refund);
    Destroy();
}

void AStructureBase::OnRep_CurrentLevel()
{
	UE_LOG(LogTemp, Log, TEXT("Level Changed: %d"), CurrentLevel);
}

int32 AStructureBase::GetUpgradeCost() const
{
	if (CachedStructureData.UpgradeCosts.IsValidIndex(CurrentLevel))
	{
		return static_cast<int32>(CachedStructureData.UpgradeCosts[CurrentLevel]);
	}
	return 999999;
}

bool AStructureBase::CanUpgrade() const
{
	return CurrentLevel < CachedStructureData.MaxUpgradeLevel;
}

int32 AStructureBase::GetSellRefund() const
{
	return static_cast<int32>(CachedStructureData.InstallCost * 0.5f);
}
