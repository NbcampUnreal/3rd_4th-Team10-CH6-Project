// LobbyGameMode.cpp


#include "GameSystem/GameMode/LobbyGameMode.h"
#include "GameSystem/GameMode/LobbyGameState.h"
#include "GameSystem/Player/TTTPlayerController.h"
#include "Character/PS/TTTPlayerState.h"
#include "TimerManager.h"
#include "Engine/World.h"
#include "GameFramework/PlayerState.h"
#include "AbilitySystemComponent.h"
#include "GameSystem/GameInstance/TTTGameInstance.h"

ALobbyGameMode::ALobbyGameMode()
{
	GameStateClass        = ALobbyGameState::StaticClass();
	PlayerControllerClass = ATTTPlayerController::StaticClass();
	PlayerStateClass      = ATTTPlayerState::StaticClass();

	MinPlayersToStart = 2; // <- 여기서 2인용 테스트
	InGameMapPath = TEXT("/Game/Maps/InGameMap"); // 실제 인게임 맵 경로로 수정

	CountdownStartValue = 5;

	bUseSeamlessTravel    = true;
	bStartPlayersAsSpectators = true;
}
static void ApplyGEToSelf(UAbilitySystemComponent* ASC, TSubclassOf<UGameplayEffect> GEClass, UObject* SourceObj)
{
	if (!ASC || !GEClass) return;

	FGameplayEffectContextHandle Ctx = ASC->MakeEffectContext();
	Ctx.AddSourceObject(SourceObj);

	FGameplayEffectSpecHandle Spec = ASC->MakeOutgoingSpec(GEClass, 1.f, Ctx);
	if (Spec.IsValid())
	{
		ASC->ApplyGameplayEffectSpecToTarget(*Spec.Data.Get(), ASC);
	}
}

static void RemoveGEFromSelf(UAbilitySystemComponent* ASC, TSubclassOf<UGameplayEffect> GEClass)
{
	if (!ASC || !GEClass) return;
	ASC->RemoveActiveGameplayEffectBySourceEffect(GEClass, ASC);
}
bool ALobbyGameMode::IsHost(const APlayerController* PC) const
{
	return HostPC.IsValid() && HostPC.Get() == PC;
}

void ALobbyGameMode::AssignHost(APlayerController* NewHost)
{
	if (!HasAuthority() || !NewHost) return;

	// 이미 같은 Host면 중복 방지
	if (HostPC.IsValid() && HostPC.Get() == NewHost)
	{
		return;
	}

	// 기존 Host 태그 제거(선택)
	if (HostPC.IsValid())
	{
		if (ATTTPlayerState* OldPS = HostPC->GetPlayerState<ATTTPlayerState>())
		{
			if (UAbilitySystemComponent* OldASC = OldPS->GetAbilitySystemComponent())
			{
				RemoveGEFromSelf(OldASC, HostGEClass);
			}
		}
	}

	HostPC = NewHost;

	// 새 Host 태그 부여
	if (ATTTPlayerState* PS = NewHost->GetPlayerState<ATTTPlayerState>())
	{
		if (UAbilitySystemComponent* ASC = PS->GetAbilitySystemComponent())
		{
			ApplyGEToSelf(ASC, HostGEClass, this);
		}
	}

	UE_LOG(LogTemp, Warning, TEXT("[Lobby] Host assigned: %s"), *GetNameSafe(NewHost));
}

void ALobbyGameMode::ReassignHost()
{
	if (!HasAuthority() || !GameState) return;

	for (APlayerState* PSBase : GameState->PlayerArray)
	{
		if (ATTTPlayerState* PS = Cast<ATTTPlayerState>(PSBase))
		{
			APlayerController* PC = Cast<APlayerController>(PS->GetOwner());
			if (PC)
			{
				AssignHost(PC);
				return;
			}
		}
	}

	UE_LOG(LogTemp, Warning, TEXT("[Lobby] No players to assign host."));
}
void ALobbyGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);
	UpdateLobbyCounts();
	if (GetGameState<ALobbyGameState>())
	{
		ALobbyGameState* GS = GetGameState<ALobbyGameState>();
		GS->ConnectedPlayers = GS->PlayerArray.Num();
		
	}

	/* 방장 지정:첫 입장자 */
	if (!HostPC.IsValid())
	{
		AssignHost(NewPlayer);
	}

	if (ATTTPlayerState* TTTPS = Cast<ATTTPlayerState>(NewPlayer->PlayerState))
	{
		UAbilitySystemComponent* ASC = TTTPS->GetAbilitySystemComponent();

		// ASC가 있고, 우리가 설정한 GE 클래스도 있는지 확인
		if (ASC && LobbyStateGEClass)
		{
			// 1. Context 생성 (누가 시전했냐? -> 게임모드다)
			FGameplayEffectContextHandle ContextHandle = ASC->MakeEffectContext();
			ContextHandle.AddSourceObject(this);

			// 2. Spec 생성 (어떤 GE를 적용할 거냐? -> LobbyStateGEClass)
			FGameplayEffectSpecHandle SpecHandle = ASC->MakeOutgoingSpec(LobbyStateGEClass, 1.0f, ContextHandle);
			FGameplayEffectSpecHandle SpecHandle2 = ASC->MakeOutgoingSpec(CharSelectGEClass, 1.0f, ContextHandle);
			FGameplayEffectSpecHandle SpecHandle3 = ASC->MakeOutgoingSpec(MapSelectGEClass, 1.0f, ContextHandle);


			if (SpecHandle.IsValid())
			{
				// 3. 적용! (이제 서버가 적용하면 클라로 자동 복제됨)
				ASC->ApplyGameplayEffectSpecToTarget(*SpecHandle.Data.Get(), ASC);
				ASC->ApplyGameplayEffectSpecToTarget(*SpecHandle2.Data.Get(), ASC);

				UE_LOG(LogTemp, Warning, TEXT("Server: Applied Lobby GE to %s"), *NewPlayer->GetName());

				//호스트 태그를 갖는 경우
				if (ASC->HasMatchingGameplayTag(GASTAG::State_Role_Host))
				{
					ASC->ApplyGameplayEffectSpecToTarget(*SpecHandle3.Data.Get(), ASC);
				}
			}
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("GameMode에 LobbyStateGEClass가 비어있거나 ASC가 없습니다!"));
		}
	}


	UE_LOG(LogTemp, Warning, TEXT("post Login"));
}

void ALobbyGameMode::Logout(AController* Exiting)
{
	Super::Logout(Exiting);
	UpdateLobbyCounts();
	if (ALobbyGameState* GS = GetGameState<ALobbyGameState>())
	{
		GS->ConnectedPlayers = GS->PlayerArray.Num();
	}
	if (HostPC.IsValid() && HostPC.Get() == Exiting)
	{
		UE_LOG(LogTemp, Warning, TEXT("[Lobby] Host left. Reassigning..."));
		HostPC = nullptr;
		ReassignHost();
	}
}

void ALobbyGameMode::HandlePlayerReadyChanged(ATTTPlayerState* ChangedPlayerState)
{
	UpdateLobbyCounts();
	CheckAllReady();
}

void ALobbyGameMode::GetSeamlessTravelActorList(bool bToTransition, TArray<AActor*>& ActorList)
{
	Super::GetSeamlessTravelActorList(bToTransition, ActorList);

	UE_LOG(LogTemp, Warning, TEXT("[LobbyGM::GetSeamlessTravelActorList] bToTransition=%d"), bToTransition);

	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
	{
		if (APlayerController* PC = It->Get())
		{
			if (ATTTPlayerState* PS = PC->GetPlayerState<ATTTPlayerState>())
			{
				UE_LOG(LogTemp, Warning,
					TEXT("  Add PlayerState %s  SelectedClass=%s  Ready=%d"),
					*PS->GetPlayerName(),
					*GetNameSafe(PS->SelectedCharacterClass),
					PS->IsReady() ? 1 : 0);

				ActorList.Add(PS);
			}
		}
	}
}

void ALobbyGameMode::UpdateLobbyCounts()
{
	ALobbyGameState* LobbyGS = GetGameState<ALobbyGameState>();
	if (!LobbyGS || !GameState)
	{
		return;
	}

	int32 Total = 0;
	int32 Ready = 0;

	for (APlayerState* PS : GameState->PlayerArray)
	{
		if (ATTTPlayerState* TTTPS = Cast<ATTTPlayerState>(PS))
		{
			++Total;

			// 선택된 캐릭터가 있고, Ready면 카운트
			if (TTTPS->SelectedCharacterClass && TTTPS->IsReady())
			{
				++Ready;
			}
		}
	}

	LobbyGS->SetConnectedPlayers(Total);
	LobbyGS->SetReadyPlayers(Ready);
}

void ALobbyGameMode::CheckAllReady()
{
	ALobbyGameState* GS = GetGameState<ALobbyGameState>();
	if (!GS) return;

	// 이미 Loading/인게임이면 다시 시작하지 않음
	if (GS->LobbyPhase != ELobbyPhase::Waiting)
	{
		return;
	}

	// 모든 플레이어가 Ready && 최소 인원 이상
	if (GS->ReadyPlayers == GS->ConnectedPlayers &&
		GS->ConnectedPlayers >= MinPlayersToStart)
	{
		// 1) Phase를 Loading으로 전환
		GS->LobbyPhase = ELobbyPhase::Loading;
		GS->OnRep_LobbyPhase();

		// 2) 카운트다운 값 세팅 (5초)
		GS->CountdownSeconds = CountdownStartValue;
		GS->OnRep_CountdownSeconds();

		// 3) 1초 간격으로 TickCountdown() 호출
		GetWorld()->GetTimerManager().SetTimer(
			StartCountdownTimerHandle,
			this,
			&ALobbyGameMode::TickCountdown,
			1.0f,
			true  // 반복
		);
	}
}

void ALobbyGameMode::StartGameTravel()
{
	ALobbyGameState* GS = GetGameState<ALobbyGameState>();
	if (GS)
	{
		GS->CountdownSeconds = 0;
		GS->OnRep_CountdownSeconds();
	}

	UWorld* World = GetWorld();
	if (!World) return;

	// 1) GameInstance에서 선택된 맵 경로 얻기
	UTTTGameInstance* GI = World->GetGameInstance<UTTTGameInstance>();
	FString MapPath;

	const bool bHasSelected =
		(GI && GI->HasSelectedMap() && GI->ResolvePlayMapPath(GI->GetSelectedMapIndex(), MapPath));

	// 선택값이 없으면 폴백
	if (!bHasSelected)
	{
		MapPath = InGameMapPath;
	}

	// 2) DedicatedServer면 ?listen 붙이지 않기
	const ENetMode NetMode = World->GetNetMode();
	const bool bDedicated = (NetMode == NM_DedicatedServer);

	const FString Url = bDedicated
		? FString::Printf(TEXT("%s"), *MapPath)
		: FString::Printf(TEXT("%s?listen"), *MapPath);

	UE_LOG(LogTemp, Warning, TEXT("[Lobby] ServerTravel (%s) -> %s"),
		bDedicated ? TEXT("Dedicated") : TEXT("Listen"),
		*Url);

	// 3) 이동
	World->ServerTravel(Url, /*bAbsolute*/ false);
}

void ALobbyGameMode::TickCountdown()
{
	ALobbyGameState* GS = GetGameState<ALobbyGameState>();
	if (!GS)
	{
		return;
	}

	// 안전장치: 로비 Phase가 아니면 카운트다운 중지
	if (GS->LobbyPhase != ELobbyPhase::Loading)
	{
		GetWorld()->GetTimerManager().ClearTimer(StartCountdownTimerHandle);
		return;
	}

	// 초 줄이기
	if (GS->CountdownSeconds > 0)
	{
		GS->CountdownSeconds--;
		GS->OnRep_CountdownSeconds();
	}

	// 0이 되면 타이머 정지 후 인게임으로 이동
	if (GS->CountdownSeconds <= 0)
	{
		GetWorld()->GetTimerManager().ClearTimer(StartCountdownTimerHandle);
		StartGameTravel();
	}
}
