#include "EldenParry.h"
#include "Settings.h"
#include "Utils.hpp"


void EldenParry::init() {
	INFO("Obtaining precision API...");
	_precision_API = reinterpret_cast<PRECISION_API::IVPrecision1*>(PRECISION_API::RequestPluginAPI());
	if (_precision_API) {
		INFO("Precision API successfully obtained.");
		Settings::facts::isPrecisionAPIObtained = true;
		if (_precision_API->AddPreHitCallback(SKSE::GetPluginHandle(), precisionPrehitCallbackFunc) ==
			PRECISION_API::APIResult::OK) {
			INFO("Successfully registered precision API prehit callback.");
		}
	} else {
		INFO("Precision API not found.");
	}
	INFO("Obtaining Valhalla Combat API...");
	_ValhallaCombat_API = reinterpret_cast<VAL_API::IVVAL1*>(VAL_API::RequestPluginAPI());
	if (_ValhallaCombat_API) {
		INFO("Valhalla Combat API successfully obtained.");
		Settings::facts::isValhallaCombatAPIObtained = true;
	}
	else {
		INFO("Valhalla Combat API not found.");
	}
	//read parry sound
	auto data = RE::TESDataHandler::GetSingleton();
	_parrySound_shd = data->LookupForm<RE::BGSSoundDescriptorForm>(0xD62, "EldenParry.esp");
	_parrySound_wpn = data->LookupForm<RE::BGSSoundDescriptorForm>(0xD63, "EldenParry.esp");
	if (!_parrySound_shd || !_parrySound_wpn) {
		ERROR("Parry sound not found.");
	}

	//read fcombatHitConeAngle
	_GMST_fCombatHitConeAngle = RE::GameSettingCollection::GetSingleton()->GetSetting("fCombatHitConeAngle")->GetFloat();
	_parryAngle = _GMST_fCombatHitConeAngle;
}
/// <summary>
/// Check if the object is in the blocker's blocking angle.
/// </summary>
/// <param name="a_blocker"></param>
/// <param name="a_obj"></param>
/// <returns>True if the object is in blocker's blocking angle.</returns>
bool EldenParry::inBlockAngle(RE::Actor* a_blocker, RE::TESObjectREFR* a_obj)
{
	auto angle = a_blocker->GetHeadingAngle(a_obj->GetPosition(), false);
	return (angle <= _parryAngle && angle >= -_parryAngle);
}
/// <summary>
/// Check if the actor is in parry state i.e. they are able to parry the incoming attack/projectile.
/// </summary>
/// <param name="a_actor"></param>
/// <returns></returns>
bool EldenParry::inParryState(RE::Actor* a_actor, bool projectileParry)
{
	if (a_actor->IsPlayerRef()) {
		if (projectileParry) {
			if (_bashButtonHeldTime > Settings::fParryTimeWindow_Projectile) {
				return false;
			}
		} else {
			if (_bashButtonHeldTime > Settings::fParryTimeWindow) {
				return false;
			}
		}
	} else if (!Settings::bEnableNPCParry) {
		return false;
	}
	if (a_actor->GetAttackState() != RE::ATTACK_STATE_ENUM::kBash) {  //parrier has to be bashing
		return false;
	}
	if (Utils::isEquippedShield(a_actor)) {  //check settings conditions
		if (!Settings::bEnableShieldParry) {
			return false;
		}
	} else {
		if (!Settings::bEnableWeaponParry) {
			return false;
		}
	}

	return true;
}

bool EldenParry::canParry(RE::Actor* a_parrier, RE::Actor* a_attacker)
{
	return inParryState(a_parrier, false) && inBlockAngle(a_parrier, a_attacker);
}

bool EldenParry::canParry(RE::Actor* a_parrier, RE::Projectile* a_proj) {
	if (a_proj->spell) {
		if (!Settings::bEnableMagicProjectileDeflection) {
			return false;
		}
	} else {
		if (!Settings::bEnableArrowProjectileDeflection) {
			return false;
		}
	}
	return inParryState(a_parrier, true) && inBlockAngle(a_parrier, a_proj);
}


bool EldenParry::processMeleeParry(RE::Actor* a_attacker, RE::Actor* a_parrier)
{
	if (canParry(a_parrier, a_attacker)) {
		inlineUtils::restoreav(a_parrier, RE::ActorValue::kStamina, Settings::fParryStaminaRecovery);
		playParryEffects(a_parrier);
		Utils::triggerStagger(a_parrier, a_attacker, 10);
		if (Settings::facts::isValhallaCombatAPIObtained) {
			_ValhallaCombat_API->processStunDamage(VAL_API::STUNSOURCE::parry, nullptr, a_parrier, a_attacker, 0);
		}
		if (a_parrier->IsPlayerRef()) {
			RE::PlayerCharacter::GetSingleton()->AddSkillExperience(RE::ActorValue::kBlock, Settings::fMeleeParryExp);
		}
		return true;
	}

	return false;

	
}

/// <summary>
/// Process a projectile parry; Return if the parry is successful.
/// </summary>
/// <param name="a_parrier"></param>
/// <param name="a_projectile"></param>
/// <param name="a_projectile_collidable"></param>
/// <returns>True if the projectile parry is successful.</returns>
bool EldenParry::processProjectileParry(RE::Actor* a_parrier, RE::Projectile* a_projectile, RE::hkpCollidable* a_projectile_collidable)
{
	if (canParry(a_parrier, a_projectile)) {
		RE::TESObjectREFR* shooter = nullptr;
		if (a_projectile->shooter && a_projectile->shooter.get()) {
			shooter = a_projectile->shooter.get().get();
		}

		Utils::resetProjectileOwner(a_projectile, a_parrier, a_projectile_collidable);

		if (shooter && shooter->Is3DLoaded()) {
			Utils::DeflectProjectile(a_parrier, a_projectile, shooter);
		} else {
			Utils::ReflectProjectile(a_projectile);
		}
		
		playParryEffects(a_parrier);
		if (a_parrier->IsPlayerRef()) {
			RE::PlayerCharacter::GetSingleton()->AddSkillExperience(RE::ActorValue::kBlock, Settings::fProjectileParryExp);
		}
		return true;
	}
	return false;

}

void EldenParry::playParryEffects(RE::Actor* a_parrier) {
	if (Settings::bEnableParrySoundEffect) {
		if (Utils::isEquippedShield(a_parrier)) {
			Utils::playSound(a_parrier, _parrySound_shd);
		} else {
			Utils::playSound(a_parrier, _parrySound_wpn);
		}
	}
	if (Settings::bEnableParrySparkEffect) {
		blockSpark::playBlockSpark(a_parrier);
	}
	if (a_parrier->IsPlayerRef()) {
		if (Settings::bEnableSlowTimeEffect) {
			Utils::slowTime(0.2f, 0.3f);
		}
		if (Settings::bEnableScreenShakeEffect) {
			inlineUtils::shakeCamera(1.5, a_parrier->GetPosition(), 0.4f);
		}
	}
	
}

void EldenParry::updateBashButtonHeldTime(float a_time) {
	_bashButtonHeldTime = a_time;
}

PRECISION_API::PreHitCallbackReturn EldenParry::precisionPrehitCallbackFunc(const PRECISION_API::PrecisionHitData& a_precisionHitData) {
	PRECISION_API::PreHitCallbackReturn returnData;
	if (!a_precisionHitData.target || !a_precisionHitData.target->Is(RE::FormType::ActorCharacter)) {
		return returnData;
	}
	if (EldenParry::GetSingleton()->processMeleeParry(a_precisionHitData.attacker, a_precisionHitData.target->As<RE::Actor>())) {
		returnData.bIgnoreHit = true;
	}
	return returnData;
}

