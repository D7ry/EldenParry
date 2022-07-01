#pragma once
#include <memory>
#include "lib/PrecisionAPI.h"
#include "lib/ValhallaCombatAPI.h"
#include <mutex>
#include <shared_mutex>

#include <unordered_set>

class EldenParry
{
public:
	static EldenParry* GetSingleton() {
		static EldenParry singleton;
		return std::addressof(singleton);
	}

	void init();

	/// <summary>
	/// Try to process a parry by the parrier.
	/// </summary>
	/// <param name="a_attacker"></param>
	/// <param name="a_parrier"></param>
	/// <returns>True if the parry is successful.</returns>
	bool processMeleeParry(RE::Actor* a_attacker, RE::Actor* a_parrier);

	bool processProjectileParry(RE::Actor* a_blocker, RE::Projectile* a_projectile, RE::hkpCollidable* a_projectile_collidable);

	void processGuardBash(RE::Actor* a_basher, RE::Actor* a_blocker);

	PRECISION_API::IVPrecision1* _precision_API;
	VAL_API::IVVAL1* _ValhallaCombat_API;

	void applyParryCost(RE::Actor* a_actor);
	void cacheParryCost(RE::Actor* a_actor, float a_cost);
	void negateParryCost(RE::Actor* a_actor);

	void playGuardBashEffects(RE::Actor* a_actor);

	void startTimingParry(RE::Actor* a_actor);
	void finishTimingParry(RE::Actor* a_actor);

	void update();

private:
	void playParryEffects(RE::Actor* a_parrier);

	inline bool inParryState(RE::Actor* a_parrier);
	inline bool canParry(RE::Actor* a_parrier, RE::TESObjectREFR* a_obj);
	inline bool inBlockAngle(RE::Actor* a_blocker, RE::TESObjectREFR* a_obj);
	static PRECISION_API::PreHitCallbackReturn precisionPrehitCallbackFunc(const PRECISION_API::PrecisionHitData& a_precisionHitData);

	std::unordered_map<RE::Actor*, float> _parryCostQueue;
	std::unordered_set<RE::Actor*> _parrySuccessActors;
	std::unordered_map<RE::Actor*, float> _parryTimer;



	RE::BGSSoundDescriptorForm* _parrySound_shd;
	RE::BGSSoundDescriptorForm* _parrySound_wpn;
	float _GMST_fCombatHitConeAngle;
	float _parryAngle;

	mutable std::shared_mutex mtx_parryCostQueue;
	mutable std::shared_mutex mtx_parrySuccessActors;
	mutable std::shared_mutex mtx_parryTimer;
};


namespace Offsets
{
	inline static float* g_deltaTime = (float*)RELOCATION_ID(523660, 410199).address();          // 2F6B948
	inline static float* g_deltaTimeRealTime = (float*)RELOCATION_ID(523661, 410200).address();  // 2F6B94C
}
