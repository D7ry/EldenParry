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

	void updateBashButtonHeldTime(float a_time);

	PRECISION_API::IVPrecision1* _precision_API;
	VAL_API::IVVAL1* _ValhallaCombat_API;

	void applyParryCost(RE::Actor* a_actor);
	void cacheParryCost(RE::Actor* a_actor, float a_cost);
	void negateParryCost(RE::Actor* a_actor);

private:
	void playParryEffects(RE::Actor* a_parrier);

	inline bool inParryState(RE::Actor* a_parrier, bool projectileParry);
	inline bool canParry(RE::Actor* a_parrier, RE::Projectile* a_proj);
	inline bool canParry(RE::Actor* a_parrier, RE::Actor* a_obj);
	inline bool inBlockAngle(RE::Actor* a_blocker, RE::TESObjectREFR* a_obj);
	static PRECISION_API::PreHitCallbackReturn precisionPrehitCallbackFunc(const PRECISION_API::PrecisionHitData& a_precisionHitData);

	std::unordered_map<RE::Actor*, float> _parryCostQueue;
	std::unordered_set<RE::Actor*> _parrySuccessActors;
	RE::BGSSoundDescriptorForm* _parrySound_shd;
	RE::BGSSoundDescriptorForm* _parrySound_wpn;
	float _GMST_fCombatHitConeAngle;
	float _parryAngle;
	float _bashButtonHeldTime;

	mutable std::shared_mutex mtx_parryCostQueue;
	mutable std::shared_mutex mtx_parrySuccessActors;
};


