#include "EldenParry.h"
#include "Settings.h"
#include "Utils.hpp"


void EldenParry::init(){

	INFO("Obtaining precision API...");
	
	_precision_API = reinterpret_cast
		<PRECISION_API::IVPrecision1 *>
		(PRECISION_API::RequestPluginAPI());
	
	if(_precision_API){

		INFO("Precision API successfully obtained.");
		
		Settings::facts::isPrecisionAPIObtained = true;
		
		auto result = _precision_API -> AddPreHitCallback(SKSE::GetPluginHandle(),precisionPrehitCallbackFunc);

		if(result == PRECISION_API::APIResult::OK)
			INFO("Successfully registered precision API prehit callback.");
	} else {
		INFO("Precision API not found.");
	}

	INFO("Obtaining Valhalla Combat API...");
	
	_ValhallaCombat_API = reinterpret_cast
		<VAL_API::IVVAL1 *>
		(VAL_API::RequestPluginAPI());
	
	if(_ValhallaCombat_API){
		INFO("Valhalla Combat API successfully obtained.");
		Settings::facts::isValhallaCombatAPIObtained = true;
	} else {
		INFO("Valhalla Combat API not found.");
	}

	
	// Read parry sound
	
	auto data = RE::TESDataHandler::GetSingleton();
	_parrySound_shd = data -> LookupForm<RE::BGSSoundDescriptorForm>(0xD62,"EldenParry.esp");
	_parrySound_wpn = data -> LookupForm<RE::BGSSoundDescriptorForm>(0xD63,"EldenParry.esp");
	
	if(!_parrySound_shd || !_parrySound_wpn)
		ERROR("Parry sound not found.");


	// Read fcombatHitConeAngle

	_GMST_fCombatHitConeAngle = RE::GameSettingCollection::GetSingleton()->GetSetting("fCombatHitConeAngle")->GetFloat();
	_parryAngle = _GMST_fCombatHitConeAngle;
}



/**
 *	@brief Check if the object is in the blocker's blocking angle.
 * 
 *	@param a_blocker 
 *	@param a_obj 
 *	@returns True if the object is in blocker's blocking angle.
 */

bool EldenParry::inBlockAngle(RE::Actor * blocker,RE::TESObjectREFR * object){
	
	auto position = object -> GetPosition();
	auto angle = blocker -> GetHeadingAngle(position,false);
	
	return angle <= _parryAngle 
		&& angle >= - _parryAngle;
}


/**
 *	@brief Check if the actor is in parry state i.e. they are able to parry the incoming attack/projectile.
 * 
 *	@param a_actor 
 *	@param projectileParry 
 */

bool EldenParry::inParryState(RE::Actor * actor,bool projectileParry){

	if(actor -> IsPlayerRef()){

		bool missedWindow = projectileParry
			? _bashButtonHeldTime > Settings::fParryTimeWindow_Projectile
			: _bashButtonHeldTime > Settings::fParryTimeWindow ;

		if(missedWindow)
			return false;
	}

	if(!Settings::bEnableNPCParry)
		return false;


	// Parrier has to be bashing

	if(a_actor -> GetAttackState() != RE::ATTACK_STATE_ENUM::kBash)
		return false;


	// Check settings conditions

	return Utils::isEquippedShield(a_actor)
		? Settings::bEnableShieldParry
		: Settings::bEnableWeaponParry ;
}


bool EldenParry::canParry(RE::Actor * parrier,RE::Actor * attacker){
	return inParryState(parrier,false) 
		&& inBlockAngle(parrier,attacker);
}


bool EldenParry::canParry(RE::Actor * parrier,RE::Projectile projectile){
	
	bool unDeflectable = (projectile -> spell)
		: Settings::bEnableMagicProjectileDeflection
		: Settings::bEnableArrowProjectileDeflection ;
	
	if(unDeflectable)
		return false;

	return inParryState(parrier,true) 
		&& inBlockAngle(parrier,projectile);
}


bool EldenParry::processMeleeParry(RE::Actor * attacker,RE::Actor * parrier){

	if(!canParry(parrier,attacker))
		return false;


	playParryEffects(parrier);
	
	Utils::triggerStagger(parrier,attacker,10);

	if(Settings::facts::isValhallaCombatAPIObtained)
		_ValhallaCombat_API -> processStunDamage(VAL_API::STUNSOURCE::parry,nullptr,parrier,attacker,0);

	if(parrier -> IsPlayerRef())
		RE::PlayerCharacter::GetSingleton() -> AddSkillExperience(RE::ActorValue::kBlock,Settings::fMeleeParryExp);

	negateParryCost(parrier);

	return true;
}



/**
 * @brief Process a projectile parry; Return if the parry is successful.
 * 
 * @param a_parrier 
 * @param a_projectile 
 * @param a_projectile_collidable 
 * @returns True if the projectile parry is successful.
 */

bool EldenParry::processProjectileParry(
	RE::Actor * parrier ,
	RE::Projectile * projectile ,
	RE::hkpCollidable * collidable
){
	if(!canParry(a_parrier, a_projectile))
		return false;

	RE::TESObjectREFR * shooter = nullptr;
	
	if(projectile -> shooter && projectile -> shooter.get())
		shooter = projectile
			-> shooter
			.  get()
			.  get();

	Utils::resetProjectileOwner(projectile,parrier,collidable);

	if(shooter && shooter -> Is3DLoaded())
		Utils::RetargetProjectile(parrier,projectile,shooter);
	else
		Utils::ReflectProjectile(projectile);
	
	playParryEffects(parrier);

	if(parrier -> IsPlayerRef())
		RE::PlayerCharacter::GetSingleton()
			-> AddSkillExperience(RE::ActorValue::kBlock,Settings::fProjectileParryExp);

	negateParryCost(parrier);

	return true;
}


void EldenParry::playParryEffects(RE::Actor * parrier){

	if(Settings::bEnableParrySoundEffect){

		auto sound = Utils::isEquippedShield(parrier)
			? _parrySound_shd
			: _parrySound_wpn ;

		Utils::playSound(parrier,sound);
	}
	
	if(Settings::bEnableParrySparkEffect)
		blockSpark::playBlockSpark(parrier);

	if(parrier -> IsPlayerRef()){

		if(Settings::bEnableSlowTimeEffect)
			Utils::slowTime(0.2f,0.3f);

		if(Settings::bEnableScreenShakeEffect)
			inlineUtils::shakeCamera(1.5,parrier -> GetPosition(),0.4f);
	}
}


void EldenParry::updateBashButtonHeldTime(float time){
	_bashButtonHeldTime = time;
}


using uniqueLocker = std::unique_lock<std::shared_mutex>;
using sharedLocker = std::shared_lock<std::shared_mutex>;


void EldenParry::applyParryCost(RE::Actor * actor){
	
	// logger::info("apply parry cost for {}",actor->GetName());
	
	{
		uniqueLocker lock(mtx_parryCostQueue);
		sharedLocker lock2(mtx_parrySuccessActors);
	
		if(_parryCostQueue.contains(actor)){

			if(!_parrySuccessActors.contains(actor))
				inlineUtils::damageAv(actor,RE::ActorValue::kStamina,_parryCostQueue[actor]);

			_parryCostQueue.erase(actor);
		}
	}
	
	uniqueLocker lock(mtx_parrySuccessActors);
	_parrySuccessActors.erase(actor);
}


void EldenParry::cacheParryCost(RE::Actor * actor,float cost){

	//logger::info("cache parry cost for {}: {}",actor -> GetName(),cost);

	uniqueLocker lock(mtx_parryCostQueue);
	_parryCostQueue[actor] = cost;
}


void EldenParry::negateParryCost(RE::Actor * actor){

	//logger::info("negate parry cost for {}",actor -> GetName());
	
	uniqueLocker lock(mtx_parrySuccessActors);
	_parrySuccessActors.insert(actor);
}


PRECISION_API::PreHitCallbackReturn EldenParry::precisionPrehitCallbackFunc(
	const PRECISION_API::PrecisionHitData & hit
){

	PRECISION_API::PreHitCallbackReturn outcome;
	
	if(!hit.target)
		return outcome;
		
	if(!hit.target -> Is(RE::FormType::ActorCharacter))
		return outcome;

	auto defender = hit.target -> As<RE::Actor>();

	if(EldenParry::GetSingleton() -> processMeleeParry(hit.attacker,defender))
		outcome.bIgnoreHit = true;
	
	return outcome;
}
