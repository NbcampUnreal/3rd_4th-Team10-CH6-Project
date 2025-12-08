// TTTGameModeBase.cpp


#include "GameSystem/GameMode/TTTGameModeBase.h"
#include "TimerManager.h"
#include "Enemy/System/SpawnSubsystem.h"
#include "Enemy/System/PoolSubsystem.h"
#include "Engine/GameInstance.h"
#include "Engine/World.h"
#include "GameSystem/Player/TTTPlayerController.h"
#include "Character/PS/TTTPlayerState.h"
#include "GameFramework/PlayerStart.h"
#include "GameFramework/PlayerState.h"
#include "GameSystem/GameInstance/TTTGameInstance.h"
#include "Structure/Core/CoreStructure.h" 
#include "EngineUtils.h"
#include "Enemy/Base/EnemyBase.h"
#include "GameFramework/Actor.h"
#include "GameplayTagContainer.h"
#include "Character/GAS/AS/CharacterBase/AS_CharacterBase.h"
#include "Character/Characters/Base/BaseCharacter.h"

ATTTGameModeBase::ATTTGameModeBase()
{
	GameStateClass = ATTTGameStateBase::StaticClass();
	PlayerControllerClass = ATTTPlayerController::StaticClass();

	PlayerStateClass      = ATTTPlayerState::StaticClass();
	bUseSeamlessTravel    = true;
	bStartPlayersAsSpectators = true;
	UE_LOG(LogTemp, Warning, TEXT("TTTGameModeBase constructed"));
	RewardXPSetByCallerTag = FGameplayTag::RequestGameplayTag(FName("Data.XP"), false);
}

void ATTTGameModeBase::BeginPlay()
{
	Super::BeginPlay();
	SetupDataTables();
	UE_LOG(LogTemp, Warning, TEXT("TTTGameModeBase BeginPlay"));
	// 게임 시작 페이즈 초기화 (원래 쓰던 로직 있으면 그대로 유지)
	if (ATTTGameStateBase* GameStateBase = GS())
	{
		UE_LOG(LogTemp, Warning, TEXT("aaaTTTGameModeBase: Setting initial game phase to Waiting"));
		GameStateBase->Phase = ETTTGamePhase::Waiting;
		//효종 임시 추가 ㅠ
		StartPhase(ETTTGamePhase::Waiting, GetDefaultDurationFor(ETTTGamePhase::Waiting));
	}
	UE_LOG(LogTemp, Warning, TEXT("TTTGameModeBase: Game Phase set to Waiting"));
	// === 서버에서만 스폰 처리 ===
	if (!HasAuthority())
	{
		return;
	}
	BindCoreEvents();
	// Debug: PlayerArray 안에 PlayerState + SelectedClass 찍어보기
	UE_LOG(LogTemp, Warning, TEXT("=== PlayerArray Dump Start ==="));
	if (GameState)
	{
		UE_LOG(LogTemp, Warning, TEXT("[TTTGameModeBase::BeginPlay] PlayerArray dump"));
		for (APlayerState* PS : GameState->PlayerArray)
		{
			if (ATTTPlayerState* TTTPS = Cast<ATTTPlayerState>(PS))
			{
				UE_LOG(LogTemp, Warning,
					TEXT("  PS=%s  SelectedClass=%s  Ready=%d"),
					*TTTPS->GetPlayerName(),
					*GetNameSafe(TTTPS->SelectedCharacterClass),
					TTTPS->IsReady());
			}
		}
	}
	SetupDataTables();
}

void ATTTGameModeBase::SetupDataTables()
{

	if (UWorld* World = GetWorld())
	{
		if (UPoolSubsystem* PoolSystem = World->GetSubsystem<UPoolSubsystem>())
		{
			PoolSystem->SetupTable(WaveDataTableAsset);
		}
		if (USpawnSubsystem* SpawnSystem = World->GetSubsystem<USpawnSubsystem>())
		{
			SpawnSystem->SetupTable(WaveDataTableAsset);
		}
	}
}

void ATTTGameModeBase::RestartPlayer(AController* NewPlayer)
{
	// 죽었다가 부활 같은 상황용: 선택된 캐릭터 있으면 그대로 다시 스폰
	if (!HasAuthority() || !NewPlayer)
	{
		return;
	}

	APawn* NewPawn = SpawnSelectedCharacter(NewPlayer);

	// 선택된 캐릭터 정보가 없으면, 그냥 기본 동작으로 돌려보내도 됨
	if (!NewPawn)
	{
		UE_LOG(LogTemp, Warning,
			TEXT("[RestartPlayer] Fallback to Super::RestartPlayer for %s"),
			*GetNameSafe(NewPlayer));
		Super::RestartPlayer(NewPlayer);
	}
}

void ATTTGameModeBase::HandleSeamlessTravelPlayer(AController*& C)
{
	Super::HandleSeamlessTravelPlayer(C);

	if (!HasAuthority() || !C)
	{
		return;
	}

	APlayerController* PC = Cast<APlayerController>(C);

	UE_LOG(LogTemp, Warning,
		TEXT("[HandleSeamlessTravelPlayer] Processing PC=%s PlayerState=%s"),
		*GetNameSafe(PC),
		*GetNameSafe(PC->PlayerState));


	// ==========================================================
	// 1. [Pawn 처리] 캐릭터 스폰 및 Possess (GE 적용 전에 Pawn을 준비)
	// ==========================================================
	// 이 함수 안에서 Pawn이 스폰되고 PC->Possess가 완료되어야 AttributeSet이 등록됩니다.
	SpawnSelectedCharacter(PC);


	// ==========================================================
	// 2. [GAS 처리] GE 제거 및 적용 로직 (Pawn 스폰 후에 실행)
	// ==========================================================
	ATTTPlayerState* TTTPS = PC ? PC->GetPlayerState<ATTTPlayerState>() : nullptr;
	UAbilitySystemComponent* ASC = TTTPS ? TTTPS->GetAbilitySystemComponent() : nullptr;

	if (ASC)
	{
		// A. 이전 로비 상태 태그를 가진 GE를 제거합니다.
		FGameplayTag LobbyTag = FGameplayTag::RequestGameplayTag(FName("State.Mode.Lobby"));
		if (LobbyTag.IsValid())
		{
			int32 RemovedCount = ASC->RemoveActiveEffectsWithGrantedTags(FGameplayTagContainer(LobbyTag));
			UE_LOG(LogTemp, Warning, TEXT("Server: Removed %d Lobby State GE from Player."), RemovedCount);
		}

		// B. 인게임 PlayState GE 적용 (이제 Pawn이 스폰되었으므로 AttributeSet 접근이 안전해집니다.)
		if (PlayStateGEClass)
		{
			FGameplayEffectContextHandle ContextHandle = ASC->MakeEffectContext();
			ContextHandle.AddSourceObject(this);

			FGameplayEffectSpecHandle SpecHandle = ASC->MakeOutgoingSpec(PlayStateGEClass, 1.0f, ContextHandle);

			if (SpecHandle.IsValid())
			{
				ASC->ApplyGameplayEffectSpecToTarget(*SpecHandle.Data.Get(), ASC);
				UE_LOG(LogTemp, Warning, TEXT("Server: Applied Play State GE to %s (AFTER Pawn Spawn)"), *C->GetName());
			}
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("[HandleSeamlessTravelPlayer] PlayerState or ASC is NULL. Cannot apply GE."));
	}


}

APawn* ATTTGameModeBase::SpawnSelectedCharacter(AController* NewPlayer)
{
	if (!HasAuthority() || !NewPlayer)
	{
		return nullptr;
	}

	APlayerController* PC = Cast<APlayerController>(NewPlayer);
	if (!PC)
	{
		return nullptr;
	}

	// 1) 플레이어 이름 가져오기
	FString PlayerName = TEXT("Unknown");
	if (APlayerState* PS_Base = PC->PlayerState)
	{
		PlayerName = PS_Base->GetPlayerName();
	}

	// 2) GameInstance에서 선택한 캐릭터 클래스 조회
	UTTTGameInstance* GI = GetGameInstance<UTTTGameInstance>();
	TSubclassOf<APawn> SelectedClass = nullptr;

	if (GI && !PlayerName.IsEmpty())
	{
		SelectedClass = GI->GetSelectedCharacter(PlayerName);
	}

	if (!SelectedClass)
	{
		UE_LOG(LogTemp, Warning,
			TEXT("[SpawnSelectedCharacter] NO SelectedClass for PlayerName=%s"),
			*PlayerName);
		return nullptr;
	}

	// (선택) PlayerState에도 다시 써줌 (일관성 유지용)
	if (ATTTPlayerState* PS = PC->GetPlayerState<ATTTPlayerState>())
	{
		if (PS->SelectedCharacterClass != SelectedClass)
		{
			PS->SelectedCharacterClass = SelectedClass;
		}
	}

	// 3) 기존 Pawn이 있고, 같은 클래스면 유지
	if (APawn* ExistingPawn = PC->GetPawn())
	{
		if (ExistingPawn->GetClass() == SelectedClass)
		{
			UE_LOG(LogTemp, Warning,
				TEXT("[SpawnSelectedCharacter] PC=%s already has Pawn=%s (class ok)"),
				*GetNameSafe(PC),
				*GetNameSafe(ExistingPawn));
			return ExistingPawn;
		}

		ExistingPawn->Destroy();
	}

	// 4) 시작 위치 찾기
	AActor* StartSpot = FindPlayerStart(PC);
	FTransform SpawnTransform = StartSpot
		? StartSpot->GetActorTransform()
		: FTransform(FRotator::ZeroRotator, FVector::ZeroVector);

	FActorSpawnParameters Params;
	Params.Owner = PC;
	Params.Instigator = PC->GetPawn();
	Params.SpawnCollisionHandlingOverride =
		ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

	// 5) 스폰 + 포제션
	APawn* NewPawn = GetWorld()->SpawnActor<APawn>(
		SelectedClass,
		SpawnTransform,
		Params);

	if (!NewPawn)
	{
		UE_LOG(LogTemp, Error,
			TEXT("[SpawnSelectedCharacter] SpawnActor FAILED PC=%s Class=%s"),
			*GetNameSafe(PC),
			*GetNameSafe(SelectedClass));
		return nullptr;
	}

	PC->Possess(NewPawn);

	UE_LOG(LogTemp, Warning,
		TEXT("[SpawnSelectedCharacter] Spawned Pawn=%s for PC=%s using Class=%s (PlayerName=%s)"),
		*GetNameSafe(NewPawn),
		*GetNameSafe(PC),
		*GetNameSafe(SelectedClass),
		*PlayerName);

	CheckAllCharactersSpawnedAndStartBuild();

	return NewPawn;
}

void ATTTGameModeBase::StartPhase(ETTTGamePhase NewPhase, int32 DurationSeconds)
{
	if (!HasAuthority()) return;

	if (ATTTGameStateBase* S = GS())
	{
		S->Phase = NewPhase;
		S->RemainingTime = DurationSeconds;
		if (NewPhase == ETTTGamePhase::Reward)
		{
			GrantRewardPhaseRewards(); // 아래 3)에서 새로 만드는 함수
		}
		if (NewPhase == ETTTGamePhase::Combat || NewPhase == ETTTGamePhase::Boss)
		{
			ResetPhaseKillTracking();
			S->SetRemainEnemy(0); 
		}

		// 서버에서도 즉시 UI/로직 반영되게 호출
		S->OnRep_Phase();
		S->OnRep_RemainingTime();

		GetWorldTimerManager().ClearTimer(TimerHandle_Tick1s);

		if (DurationSeconds > 0)
		{
			GetWorldTimerManager().SetTimer(
				TimerHandle_Tick1s,
				this,
				&ATTTGameModeBase::TickPhaseTimer,
				1.0f,
				true
			);
		}
	}
}
void ATTTGameModeBase::GrantRewardPhaseRewards()
{
	if (!HasAuthority()) return;

	ATTTGameStateBase* S = GS();
	if (!S) return;

	const int32 ClearedWave = S->Wave;

	if (LastRewardedWave == ClearedWave) return;
	LastRewardedWave = ClearedWave;

	for (APlayerState* BasePS : S->PlayerArray)
	{
		ATTTPlayerState* PS = Cast<ATTTPlayerState>(BasePS);
		if (!PS) continue;

		// 골드 +1000
		PS->AddGold(1000);

		// 경험치 +150 (GE가 있을 때만)
		if (RewardXPGEClass)
		{
			UAbilitySystemComponent* ASC = PS->GetAbilitySystemComponent();
			if (!ASC) continue;

			FGameplayEffectContextHandle Ctx = ASC->MakeEffectContext();
			Ctx.AddSourceObject(this);

			FGameplayEffectSpecHandle Spec = ASC->MakeOutgoingSpec(RewardXPGEClass, 1.0f, Ctx);
			if (!Spec.IsValid()) continue;

			Spec.Data->SetSetByCallerMagnitude(RewardXPSetByCallerTag, RewardXPPerWave); // RewardXPPerWave=150
			ASC->ApplyGameplayEffectSpecToSelf(*Spec.Data.Get());
		}
	}
}

void ATTTGameModeBase::TickPhaseTimer()
{
	if (ATTTGameStateBase* S = GS())
	{
		S->RemainingTime = FMath::Max(0,S->RemainingTime - 1);
		S->OnRep_RemainingTime();

		if (S->RemainingTime <= 0)
		{
			GetWorldTimerManager().ClearTimer(TimerHandle_Tick1s);
			AdvancePhase();
		}
	}
}

int32 ATTTGameModeBase::GetDefaultDurationFor(ETTTGamePhase Phase) const
{
	switch (Phase)
	{
	case ETTTGamePhase::Waiting: return 5;
	case ETTTGamePhase::Build:   return 5;
	case ETTTGamePhase::Combat:  return 0;
	case ETTTGamePhase::Boss:	 return 0;
	case ETTTGamePhase::Reward:  return 5;
	default:                     return 0; // Victory/GameOver
	}
}
void ATTTGameModeBase::ResetPhaseKillTracking()
{
	PhaseTargetKillCount = 0;
	PhaseDeadKillCount = 0;
	PhaseSpawnedCount = 0;
	bPhaseClearProcessed = false;

	PhaseCountedEnemies.Empty();
}

void ATTTGameModeBase::SetPhaseTargetKillCount(int32 TargetKillCount)
{
	if (!HasAuthority()) return;

	ATTTGameStateBase* S = GS();
	if (!S) return;

	// Combat/Boss 페이즈에서만 유효
	if (S->Phase != ETTTGamePhase::Combat && S->Phase != ETTTGamePhase::Boss)
	{
		return;
	}

	PhaseTargetKillCount = FMath::Max(0, TargetKillCount);

	// UI용: 남은 적 수 갱신
	const int32 Remain = FMath::Max(PhaseTargetKillCount - PhaseDeadKillCount, 0);
	S->SetRemainEnemy(Remain);

	UE_LOG(LogTemp, Log, TEXT("[KillCount] Set Target=%d, Dead=%d, Remain=%d, Phase=%d"),
		PhaseTargetKillCount, PhaseDeadKillCount, Remain, (int32)S->Phase);

	TryClearPhaseByKillCount();
}

void ATTTGameModeBase::NotifyEnemySpawned(AActor* SpawnedEnemy)
{
	if (!HasAuthority()) return;

	// 디버그용 (실제 종료 조건은 '테이블 기반 TargetKillCount'를 사용)
	PhaseSpawnedCount++;

	UE_LOG(LogTemp, VeryVerbose, TEXT("[KillCount] Spawned=%d (Actor=%s)"), PhaseSpawnedCount, *GetNameSafe(SpawnedEnemy));
}

void ATTTGameModeBase::NotifyEnemyDead(AActor* DeadEnemy)
{
	if (!HasAuthority() || !DeadEnemy) return;

	ATTTGameStateBase* S = GS();
	if (!S) return;

	if (S->Phase != ETTTGamePhase::Combat && S->Phase != ETTTGamePhase::Boss)
		return;

	if (bPhaseClearProcessed)
		return;

	// 중복 카운트 방지 (같은 적은 페이즈당 1번만)
	if (PhaseCountedEnemies.Contains(DeadEnemy))
		return;

	PhaseCountedEnemies.Add(DeadEnemy);

	PhaseDeadKillCount++;

	const int32 Remain = FMath::Max(PhaseTargetKillCount - PhaseDeadKillCount, 0);
	S->SetRemainEnemy(Remain);

	UE_LOG(LogTemp, Log, TEXT("[KillCount] Dead=%d/%d (Remain=%d) Actor=%s"),
		PhaseDeadKillCount, PhaseTargetKillCount, Remain, *GetNameSafe(DeadEnemy));

	TryClearPhaseByKillCount();
}


void ATTTGameModeBase::TryClearPhaseByKillCount()
{
	if (!HasAuthority()) return;

	ATTTGameStateBase* S = GS();
	if (!S) return;

	if (bPhaseClearProcessed)
	{
		return;
	}

	// Target이 아직 세팅 안 됐으면(=테이블 값 못 받았으면) 종료 판단 불가
	if (PhaseTargetKillCount <= 0)
	{
		return;
	}

	if (PhaseDeadKillCount < PhaseTargetKillCount)
	{
		return;
	}

	bPhaseClearProcessed = true;

	// 남은 적 0으로
	S->SetRemainEnemy(0);

	// Combat 종료 → (BossWave면 Boss) / (아니면 Reward)
	if (S->Phase == ETTTGamePhase::Combat)
	{
		UE_LOG(LogTemp, Warning, TEXT("[KillCount] Combat Cleared! (%d/%d)"), PhaseDeadKillCount, PhaseTargetKillCount);

		GetWorldTimerManager().ClearTimer(TimerHandle_CombatMonitor);

		if (UWorld* World = GetWorld())
		{
			if (USpawnSubsystem* SpawnSystem = World->GetSubsystem<USpawnSubsystem>())
			{
				SpawnSystem->EndWave();
			}
		}

		if (IsBossWave(S->Wave))
		{
			StartBossPhase();
		}
		else
		{
			StartPhase(ETTTGamePhase::Reward, GetDefaultDurationFor(ETTTGamePhase::Reward));
		}
		return;
	}

	// Boss 종료 → Reward
	if (S->Phase == ETTTGamePhase::Boss)
	{
		UE_LOG(LogTemp, Warning, TEXT("[KillCount] Boss Cleared! (%d/%d)"), PhaseDeadKillCount, PhaseTargetKillCount);

		GetWorldTimerManager().ClearTimer(TimerHandle_BossPhase);
		StartPhase(ETTTGamePhase::Reward, GetDefaultDurationFor(ETTTGamePhase::Reward));
		return;
	}
}

void ATTTGameModeBase::StartCombatMonitoring()
{
	UE_LOG(LogTemp, Warning, TEXT("[CombatMonitor] Deprecated. Use KillCount tracking (SetPhaseTargetKillCount / NotifyEnemyDead)."));
}

void ATTTGameModeBase::UpdateCombatMonitoring()
{
    
}

void ATTTGameModeBase::EndGame(bool bVictory)
{
	// 1) GameState Phase 정리 (기존 로직 유지)
	if (ATTTGameStateBase* S = GS())
	{
		S->Phase = bVictory ? ETTTGamePhase::Victory : ETTTGamePhase::GameOver;
		S->OnRep_Phase();
		GetWorldTimerManager().ClearTimer(TimerHandle_Tick1s);
	}

	// 2) GameInstance에 마지막 결과 저장
	if (UTTTGameInstance* GI = GetGameInstance<UTTTGameInstance>())
	{
		int32 WaveForResult = 0;
		if (ATTTGameStateBase* S = GS())
		{
			WaveForResult = S->Wave;  // 현재 웨이브 번호 저장
		}

		GI->SaveLastGameResult(bVictory, WaveForResult);
	}

	// 3) 즉시 로비로 이동
	ReturnToLobby();
}

void ATTTGameModeBase::AdvancePhase()
{
	if (!HasAuthority()) return;

	UE_LOG(LogTemp, Warning, TEXT("[AdvancePhase] Advancing to next phase"));
	if (ATTTGameStateBase* S = GS())
	{
		UE_LOG(LogTemp, Warning, TEXT("[AdvancePhase] Current Phase=%d"), static_cast<int32>(S->Phase));
		switch (S->Phase)
		{
		case ETTTGamePhase::Waiting:
			StartPhase(ETTTGamePhase::Build,  GetDefaultDurationFor(ETTTGamePhase::Build));
			break;

		case ETTTGamePhase::Build:
			{
				StartPhase(ETTTGamePhase::Combat, GetDefaultDurationFor(ETTTGamePhase::Combat));

				if (UWorld* World = GetWorld())
				{
					if (USpawnSubsystem* SpawnSystem = World->GetSubsystem<USpawnSubsystem>())
					{
						SpawnSystem->StartWave(S->Wave);
					}
				}
				// Combat 진입 시 "몬스터 감시" 시작
				StartCombatMonitoring();
				break;
			}

		case ETTTGamePhase::Combat:
			StartPhase(ETTTGamePhase::Reward, GetDefaultDurationFor(ETTTGamePhase::Reward));
			if (UWorld* World = GetWorld())
			{
				if (USpawnSubsystem* SpawnSystem = World->GetSubsystem<USpawnSubsystem>())
				{
					SpawnSystem->EndWave(); 
				}
			}

			break;

		case ETTTGamePhase::Reward:
			S->Wave += 1;
			if (S->Wave >= MaxWaves) {EndGame(true);}
			else{StartPhase(ETTTGamePhase::Build,  GetDefaultDurationFor(ETTTGamePhase::Build));}
			break;

			
		default:
			// Victory/GameOver: 추가 진행 없음
			break;
		}
	}
}

void ATTTGameModeBase::ReturnToLobby()
{
	if (!HasAuthority())
	{
		return;
	}

	if (bHasReturnedToLobby)
	{
		UE_LOG(LogTemp, Warning, TEXT("[GameMode] ReturnToLobby called multiple times, ignored."));
		return;
	}
	bHasReturnedToLobby = true;
	FString LobbyMapPath = TEXT("/Game/Maps/LobbyMap?listen");
	UE_LOG(LogTemp, Warning, TEXT("[GameMode] ServerTravel -> %s"), *LobbyMapPath);
	GetWorld()->ServerTravel(LobbyMapPath);
}

void ATTTGameModeBase::PM_SetPhase(const FString& Name)
{
	if (!HasAuthority()) return;

	const FString L = Name.ToLower();

	if (L == TEXT("waiting")) StartPhase(ETTTGamePhase::Waiting, GetDefaultDurationFor(ETTTGamePhase::Waiting));
	else if (L == TEXT("build")) StartPhase(ETTTGamePhase::Build,  GetDefaultDurationFor(ETTTGamePhase::Build));
	else if (L == TEXT("combat")) StartPhase(ETTTGamePhase::Combat,  GetDefaultDurationFor(ETTTGamePhase::Combat));
	else if (L == TEXT("reward")) StartPhase(ETTTGamePhase::Reward,  GetDefaultDurationFor(ETTTGamePhase::Reward));
}

void ATTTGameModeBase::CheckAllCharactersSpawnedAndStartBuild()
{
	if (!HasAuthority())
	{
		return;
	}

	UWorld* World = GetWorld();
	ATTTGameStateBase* S = GS();
	if (!World || !S)
	{
		return;
	}

	// Waiting 상태가 아닐 때는 신경 쓰지 않음
	if (S->Phase != ETTTGamePhase::Waiting)
	{
		return;
	}

	int32 TotalPlayers      = 0;
	int32 PlayersWithPawn   = 0;

	// GameState의 PlayerArray 기준으로만 체크
	for (APlayerState* PS : S->PlayerArray)
	{
		ATTTPlayerState* TTTPS = Cast<ATTTPlayerState>(PS);
		if (!TTTPS)
		{
			continue;
		}

		// 굳이 IsReady() 다시 볼 필요 없음
		// if (!TTTPS->IsReady()) continue;

		++TotalPlayers;

		// PlayerState의 Owner는 보통 해당 PlayerController
		AController* PC = Cast<AController>(TTTPS->GetOwner());
		if (PC && PC->GetPawn())
		{
			++PlayersWithPawn;
		}
	}

	UE_LOG(LogTemp, Warning,
		TEXT("[CheckAllCharactersSpawned] PlayersWithPawn=%d / %d (Phase=%d)"),
		PlayersWithPawn,
		TotalPlayers,
		static_cast<int32>(S->Phase));

	if (TotalPlayers == 0)
	{
		return;
	}

	// 현재 매치에 참가한 모든 플레이어가 Pawn을 가진 시점
	if (PlayersWithPawn == TotalPlayers)
	{
		UE_LOG(LogTemp, Warning,
			TEXT("[CheckAllCharactersSpawned] ALL players spawned -> Start Build Phase"));

		StartPhase(ETTTGamePhase::Build, GetDefaultDurationFor(ETTTGamePhase::Build));
	}
}

void ATTTGameModeBase::BindCoreEvents()
{
	if (!HasAuthority())
	{
		return;
	}

	// 이미 바인딩 되어 있으면 다시 할 필요 없음
	if (CoreStructure && CoreStructure->OnDead.IsAlreadyBound(this, &ATTTGameModeBase::HandleCoreDead))
	{
		return;
	}

	// 에디터에서 직접 세팅한 게 없으면, 월드에서 찾아보기
	if (!CoreStructure)
	{
		for (TActorIterator<ACoreStructure> It(GetWorld()); It; ++It)
		{
			CoreStructure = *It;
			break; // 첫 번째 코어만 사용 (코어가 1개라는 전제)
		}
	}

	if (CoreStructure)
	{
		CoreStructure->OnDead.AddDynamic(this, &ATTTGameModeBase::HandleCoreDead);

		UE_LOG(LogTemp, Warning,
			TEXT("[GameMode] BindCoreEvents: CoreStructure=%s"),
			*GetNameSafe(CoreStructure));
	}
	else
	{
		UE_LOG(LogTemp, Warning,
			TEXT("[GameMode] BindCoreEvents: No CoreStructure found in world"));
	}
}

void ATTTGameModeBase::HandleCoreDead()
{
	UE_LOG(LogTemp, Warning,
		TEXT("[GameMode] Core HP reached 0 -> GameOver"));

	// 이미 EndGame(false) 안에서 Phase = GameOver 로 바꾸고
	// 타이머도 정리해주고 있으니 그대로 호출
	EndGame(false);
}

bool ATTTGameModeBase::IsBossWave(int32 WaveIndex) const
{
	return (WaveIndex == 3 || WaveIndex == 7);
}

void ATTTGameModeBase::StartBossPhase()
{
	if (!HasAuthority()) return;

	ATTTGameStateBase* S = GS();
	const int32 CurrentWave = S ? S->Wave : -1;

	UE_LOG(LogTemp, Warning, TEXT("[BossPhase] Enter Boss Phase. Wave=%d"), CurrentWave);

	// Boss 페이즈로 진입 (시간 제한 없음)
	StartPhase(ETTTGamePhase::Boss, GetDefaultDurationFor(ETTTGamePhase::Boss));

	// Boss 페이즈는 보통 '보스 1마리 처치'가 종료 조건이므로 기본 목표치 세팅
	ResetPhaseKillTracking();
	SetPhaseTargetKillCount(DefaultBossKillTarget);

	// (임시) 보스가 아직 없어서 테스트용으로만 쓸 때
	if (bUseTempBossTimer)
	{
		GetWorldTimerManager().ClearTimer(TimerHandle_BossPhase);
		GetWorldTimerManager().SetTimer(
			TimerHandle_BossPhase,
			this,
			&ATTTGameModeBase::FinishBossPhaseTemp,
			BossPhaseDuration,
			false
		);
	}
}

void ATTTGameModeBase::FinishBossPhaseTemp()
{
	if (!HasAuthority()) return;

	UE_LOG(LogTemp, Warning, TEXT("[BossPhase] TEMP Finish -> RewardPhase"));

	if (ATTTGameStateBase* S = GS())
	{
		if (S->Phase != ETTTGamePhase::Boss)
		{
			return;
		}

		StartPhase(ETTTGamePhase::Reward, GetDefaultDurationFor(ETTTGamePhase::Reward));
	}
}


#pragma region UI_Region
void ATTTGameModeBase::InitializeAllPlayerStructureLists()
{
	//서버에서만 실행되는지 확인
	if (!HasAuthority() || !GameState)
	{
		return;
	}

	// 1. GameInstance에서 StructureDataTable 참조 가져오기
	UTTTGameInstance* TTTGI = Cast<UTTTGameInstance>(GetGameInstance());
	if (!TTTGI || !TTTGI->StructureDataTable)
	{
		UE_LOG(LogTemp, Error, TEXT("GameInstance나 StructureDataTable을 찾을 수 없습니다. UTTTGameInstance에 StructureDataTable이 할당되었는지 확인하십시오."));
		return;
	}

	//// 2. 초기 리스트 생성
	//TArray<FInventoryItemData> InitialList = CreateInitialStructureList(TTTGI->StructureDataTable);

	//if (InitialList.Num() == 0)
	//{
	//	UE_LOG(LogTemp, Warning, TEXT("DB에서 초기 StructureList 항목을 찾을 수 없습니다."));
	//	return;
	//}

	//// 3. GameState의 모든 PlayerState 순회하며 초기화
	//for (APlayerState* PS : GameState->PlayerArray)
	//{
	//	if (ATTTPlayerState* TTTPS = Cast<ATTTPlayerState>(PS))
	//	{
	//		//TTTPS->InitializeStructureList(InitialList);
	//		// 4. StructureList는 RepNotify로 설정되어 있으므로, 클라이언트에게 복제 후 OnRep 함수 호출이 발생합니다.
	//	}
	//}
}


//TArray<FInventoryItemData> ATTTGameModeBase::CreateInitialStructureList(UDataTable* DataTable)
//{
//	TArray<FInventoryItemData> InitialList;
//
//	if (!DataTable)
//	{
//		UE_LOG(LogTemp, Error, TEXT("StructureDataTable이 유효하지 않습니다. 초기화 불가."));
//		return InitialList;
//	}
//
//	// DB의 모든 행(Row) 데이터를 가져옵니다.
//	TArray<FInventoryItemData*> AllRows;
//	DataTable->GetAllRows<FInventoryItemData>(TEXT("ATTTSGameMode::CreateInitialStructureList"), AllRows);
//
//	for (const FInventoryItemData* RowData : AllRows)
//	{
//		if (RowData)
//		{
//			FInventoryItemData NewItem;
//
//			NewItem.ItemName = RowData->ItemName;
//
//			NewItem.Count = 0;
//			NewItem.Level = 0;
//
//			InitialList.Add(NewItem);
//		}
//	}
//
//	return InitialList;
//}

void ATTTGameModeBase::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);

	// 새로운 플레이어에게도 리스트를 초기화해줍니다.
	InitializeAllPlayerStructureLists();

	if (ATTTGameStateBase* GSBase = GS()) // GS()는 ATTTGameStateBase*를 반환하는 헬퍼 함수로 가정
	{
		if (ATTTPlayerState* PS = NewPlayer->GetPlayerState<ATTTPlayerState>())
		{
			// 서버에서만 호출
			GSBase->NotifyPlayerJoined(PS);
			UE_LOG(LogTemp, Log, TEXT("Server Notified Player Joined (PostLogin): %s"), *PS->GetPlayerName());
		}
	}
}
void ATTTGameModeBase::Logout(AController* Exiting)
{
	// 🚨 2. 플레이어 접속 해제 시 GameState에 알림 (추가)
	if (ATTTGameStateBase* GSBase = GS()) // GS()는 ATTTGameStateBase*를 반환하는 헬퍼 함수로 가정
	{
		if (ATTTPlayerState* PS = Exiting->GetPlayerState<ATTTPlayerState>())
		{
			// 서버에서만 호출
			GSBase->NotifyPlayerLeft(PS);
			UE_LOG(LogTemp, Log, TEXT("Server Notified Player Left (Logout): %s"), *PS->GetPlayerName());
		}
	}

	// 부모 클래스 호출은 항상 마지막에
	Super::Logout(Exiting);
}
#pragma endregion


