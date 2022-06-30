#pragma once
#include "PCH.h"
#include "EldenParry.h"
#include "Settings.h"
#include "Utils.hpp"
namespace Hooks
{

	class Hook_getAttackStaminaCost  //Actor__sub_140627930+16E	call ActorValueOwner__sub_1403BEC90
	{
		/*to cancel out vanilla power attack stamina consumption.*/
	public:
		static void install()
		{
			auto& trampoline = SKSE::GetTrampoline();

			REL::Relocation<uintptr_t> hook{ RELOCATION_ID(37650, 38603) };  //SE:627930 + 16E => 3BEC90 AE:64D350 + 171 => 3D6720
#ifdef SKYRIM_SUPPORT_AE
			_getAttackStaminaCost = trampoline.write_call<5>(hook.address() + 0x171, getAttackStaminaCost);
#else
			_getAttackStaminaCost = trampoline.write_call<5>(hook.address() + 0x16E, getAttackStaminaCost);
#endif
			logger::info("Attack stamina hook installed.");
		}

	private:
		static float getAttackStaminaCost(uintptr_t avOwner, RE::BGSAttackData* atkData) {
			if (atkData->data.flags.any(RE::AttackData::AttackFlag::kBashAttack)) {
				EldenParry::GetSingleton()->cacheParryCost((RE::Actor*)(avOwner - 0xB0), _getAttackStaminaCost(avOwner, atkData));
				return 0;
			}
			return _getAttackStaminaCost(avOwner, atkData);
		}
		static inline REL::Relocation<decltype(getAttackStaminaCost)> _getAttackStaminaCost;
	};

	class MeleeCollision
	{
	public:
		static void install()
		{
			REL::Relocation<uintptr_t> hook{ RELOCATION_ID(37650, 38603) };  //SE:627930 + 38B AE:64D350 + 40A / 45A
			auto& trampoline = SKSE::GetTrampoline();
#ifdef SKYRIM_SUPPORT_AE
			//_ProcessHit = trampoline.write_call<5>(hook.address() + 0x40A, processHit); //This func is also called on 38603, which doesn't seem to interfere with melee collision.
			_ProcessHit = trampoline.write_call<5>(hook.address() + 0x45A, processHit);
#else
			_ProcessHit = trampoline.write_call<5>(hook.address() + 0x38B, processHit);
#endif
			logger::info("Melee Hit hook installed.");
		}

	private:
		static inline bool shouldIgnoreHit(RE::Actor* a_aggressor, RE::Actor* a_victim, std::int64_t a_int1, bool a_bool, void* a_unkptr)
		{
			//for aggressor: cancle parry hitframe.
			if (a_aggressor->GetAttackState() == RE::ATTACK_STATE_ENUM::kBash && !inlineUtils::isPowerAttacking(a_aggressor)) {
				if (a_aggressor->IsPlayerRef() || Settings::bEnableNPCParry) {
					if (Utils::isEquippedShield(a_aggressor)) {
						if (Settings::bEnableShieldParry) {
							return true;
						}
					} else if (Settings::bEnableWeaponParry) {
						return true;
					}
				}
			}
			//for vicitm: process parry.
			if (EldenParry::GetSingleton()->processMeleeParry(a_aggressor, a_victim)) {
				return true;
			}
			return false;
		}
		static void processHit(RE::Actor* a_aggressor, RE::Actor* a_victim, std::int64_t a_int1, bool a_bool, void* a_unkptr)
		{
			if (shouldIgnoreHit(a_aggressor, a_victim, a_int1, a_bool, a_unkptr)) {
				return;
			}
			_ProcessHit(a_aggressor, a_victim, a_int1, a_bool, a_unkptr);
		}
		static void processHit1(RE::Actor* a_aggressor, RE::Actor* a_victim, std::int64_t a_int1, bool a_bool, void* a_unkptr)
		{
			if (shouldIgnoreHit(a_aggressor, a_victim, a_int1, a_bool, a_unkptr)) {
				return;
			}
			_ProcessHit1(a_aggressor, a_victim, a_int1, a_bool, a_unkptr);
		}
		static inline REL::Relocation<decltype(processHit)> _ProcessHit;
		static inline REL::Relocation<decltype(processHit1)> _ProcessHit1;
	};

	class AttackBlockHandler
	{
	public:
		static void install()
		{
			REL::Relocation<std::uintptr_t> AttackBlockHandlerVtbl{ RE::VTABLE_AttackBlockHandler[0] };
			_ProcessButton = AttackBlockHandlerVtbl.write_vfunc(0x4, ProcessButton);
		}

	private:
		static void ProcessButton(RE::AttackBlockHandler* a_this, RE::ButtonEvent* a_event, RE::PlayerControlsData* a_data)
		{
			auto pc = RE::PlayerCharacter::GetSingleton();

			if (pc && pc->GetAttackState() == RE::ATTACK_STATE_ENUM::kBash && a_event->QUserEvent() == "Right Attack/Block") {
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

	class ProjectileCollision
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
		static void OnArrowCollision(RE::Projectile* a_this, RE::hkpAllCdPointCollector* a_AllCdPointCollector)
		{
			if (initProjectileBlock(a_this, a_AllCdPointCollector)) {
				return;
			};
			_arrowCollission(a_this, a_AllCdPointCollector);
		}

		static void OnMissileCollision(RE::Projectile* a_this, RE::hkpAllCdPointCollector* a_AllCdPointCollector)
		{
			if (initProjectileBlock(a_this, a_AllCdPointCollector)) {
				return;
			};
			_missileCollission(a_this, a_AllCdPointCollector);
		}
		static inline REL::Relocation<decltype(OnArrowCollision)> _arrowCollission;
		static inline REL::Relocation<decltype(OnMissileCollision)> _missileCollission;
	};
	static void install()
	{
		//SKSE::AllocTrampoline(1 << 4);
		SKSE::AllocTrampoline(1 << 5);
		if (Settings::bSuccessfulParryNoCost) {
			Hook_getAttackStaminaCost::install();
		}
		AttackBlockHandler::install();
		MeleeCollision::install();
		ProjectileCollision::install();
	}
}
