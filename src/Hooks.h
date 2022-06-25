#pragma once
#include "PCH.h"
#include "EldenParry.h"
#include "Settings.h"
#include "Utils.hpp"
class Hook_OnMeleeCollision
{
public:
	static void install()
	{
		REL::Relocation<uintptr_t> hook{ REL::ID(37650) }; //SE:627930 AE:
		auto& trampoline = SKSE::GetTrampoline();
		_ProcessHit = trampoline.write_call<5>(hook.address() + 0x38B, processHit);
		logger::info("Melee Hit hook installed.");
	}

private:
	static inline bool isPowerAttacking(RE::Actor* a_actor) {
		if (a_actor->currentProcess && a_actor->currentProcess->high) {
			auto atkData = a_actor->currentProcess->high->attackData.get();
			if (atkData) {
				return atkData->data.flags.any(RE::AttackData::AttackFlag::kPowerAttack);
			}
		}
		return false;
	}
	static void processHit(RE::Actor* a_aggressor, RE::Actor* a_victim, std::int64_t a_int1, bool a_bool, void* a_unkptr)
	{
		//for aggressor: cancle parry hitframe.
		if (a_aggressor->GetAttackState() == RE::ATTACK_STATE_ENUM::kBash
			&& !isPowerAttacking(a_aggressor)) {
			if (a_aggressor->IsPlayerRef() || Settings::bEnableNPCParry) {
				if (Utils::isEquippedShield(a_aggressor)) {
					if (Settings::bEnableShieldParry) {
						return;
					}
				} else if (Settings::bEnableWeaponParry) {
					return;
				}
			}
		}
		//for vicitm: process parry.
		if (EldenParry::GetSingleton()->processPhysicalParry(a_aggressor, a_victim)) {
			return;
		}
		_ProcessHit(a_aggressor, a_victim, a_int1, a_bool, a_unkptr);
	}
	static inline REL::Relocation<decltype(processHit)> _ProcessHit;
};

class Hook_AttackBlockHandler
{
public:
	static void install() {
		REL::Relocation<std::uintptr_t> AttackBlockHandlerVtbl{ RE::VTABLE_AttackBlockHandler[0] };
		_ProcessButton = AttackBlockHandlerVtbl.write_vfunc(0x4, ProcessButton);
	}

private:
	static void ProcessButton(RE::AttackBlockHandler* a_this, RE::ButtonEvent* a_event, RE::PlayerControlsData * a_data) {
		auto pc = RE::PlayerCharacter::GetSingleton();
		if (pc && pc->GetAttackState() == RE::ATTACK_STATE_ENUM::kBash 
			&& a_event->QUserEvent() == "Right Attack/Block") {
			if (a_event->IsHeld()) {
				EldenParry::GetSingleton()->updateBashButtonHeldTime(a_event->HeldDuration());
			}
			if (a_event->IsUp()) {
				EldenParry::GetSingleton()->updateBashButtonHeldTime(0);
			}
		}
		_ProcessButton(a_this, a_event, a_data);
	}

	static inline REL::Relocation<decltype(ProcessButton)> _ProcessButton;
};

class Hook_OnProjectileCollision
{
public:
	static void install()
	{
		REL::Relocation<std::uintptr_t> arrowProjectileVtbl{ RE::VTABLE_ArrowProjectile[0] };
		REL::Relocation<std::uintptr_t> missileProjectileVtbl{ RE::VTABLE_MissileProjectile[0] };

		_arrowCollission = arrowProjectileVtbl.write_vfunc(190, OnArrowCollision);
		_missileCollission = missileProjectileVtbl.write_vfunc(190, OnMissileCollision);
	};

private:
	static inline bool initProjectileBlock(RE::Projectile* a_projectile, RE::hkpAllCdPointCollector* a_AllCdPointCollector)
	{
		if (a_AllCdPointCollector) {
			for (auto& hit : a_AllCdPointCollector->hits) {
				auto refrA = RE::TESHavokUtilities::FindCollidableRef(*hit.rootCollidableA);
				auto refrB = RE::TESHavokUtilities::FindCollidableRef(*hit.rootCollidableB);
				if (refrA && refrA->formType == RE::FormType::ActorCharacter) {
					return EldenParry::GetSingleton()->processProjectileParry(refrA->As<RE::Actor>(), a_projectile, const_cast<RE::hkpCollidable*>(hit.rootCollidableB));
				}
				if (refrB && refrB->formType == RE::FormType::ActorCharacter) {
					return EldenParry::GetSingleton()->processProjectileParry(refrB->As<RE::Actor>(), a_projectile, const_cast<RE::hkpCollidable*>(hit.rootCollidableA));
				}
			}
		}
		return false;
	}
	static void OnArrowCollision(RE::Projectile* a_this, RE::hkpAllCdPointCollector* a_AllCdPointCollector) {
		if (initProjectileBlock(a_this, a_AllCdPointCollector)) {
			return;
		};
		_arrowCollission(a_this, a_AllCdPointCollector);
	}

	static void OnMissileCollision(RE::Projectile* a_this, RE::hkpAllCdPointCollector* a_AllCdPointCollector) {
		if (initProjectileBlock(a_this, a_AllCdPointCollector)) {
			return;
		};
		_missileCollission(a_this, a_AllCdPointCollector);
	}
	static inline REL::Relocation<decltype(OnArrowCollision)> _arrowCollission;
	static inline REL::Relocation<decltype(OnMissileCollision)> _missileCollission;
};

class Hooks
{
public:
	static void install() {
		Hook_OnMeleeCollision::install();
		Hook_AttackBlockHandler::install();
		Hook_OnProjectileCollision::install();
	}
};
