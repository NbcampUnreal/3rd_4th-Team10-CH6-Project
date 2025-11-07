// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "TTTGamePlayTags.h"
#include "NativeGameplayTags.h"

namespace GASTAG
{
	//어빌리티
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_Jump)
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_Dash)
	//이벤트
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Event_Jump_Cost)
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Event_LevelUP)
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Event_Fireball_Charging)
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Event_Fireball_Release)
	//스테이트
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(State_IsJumping)
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(State_IsDashing)
	//데이터
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Data_Jump_Stamina)
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Data_Enemy_Damage)
	//게임플레이 큐
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(GameplayCue_Jump)
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(GameplayCue_Dash)
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(GameplayCue_Fireball_Charging)
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(GameplayCue_Fireball_Cast)
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(GameplayCue_Fireball_Explode)
	//쿨다운
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Cooldown_Dash)
	//구조물
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(State_IsInstall)
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(State_Structure_Confirm)
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(State_Structure_Cancel)

	// --Enemy
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Enemy_State_Combat)
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Enemy_State_Move)
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Enemy_State_Dead)

	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Enemy_Ability_Attack)


}
