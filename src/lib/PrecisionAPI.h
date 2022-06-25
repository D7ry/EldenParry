#pragma once
#include <functional>
#include <stdint.h>
#include <queue>
#include <PCH.h>
/*
* For modders: Copy this file into your own project if you wish to use this API
*/
namespace PRECISION_API
{
	constexpr const auto PrecisionPluginName = "Precision";

	// Available Precision interface versions
	enum class InterfaceVersion : uint8_t
	{
		V1
	};

	// Error types that may be returned by the True HUD
	enum class APIResult : uint8_t
	{
		// Your API call was successful
		OK,

		// A callback from this plugin has already been registered
		AlreadyRegistered,

		// A callback from this plugin has not been registered
		NotRegistered,
	};

	struct PreHitModifier
	{
		enum class ModifierType : uint8_t
		{
			Damage,
			Stagger
		};

		enum class ModifierOperation : uint8_t
		{
			Additive,
			Multiplicative
		};

		ModifierType modifierType;
		ModifierOperation modifierOperation;
		float modifierValue;
	};

	struct PreHitCallbackReturn
	{
		// if set to true, the hit will be ignored, no matter what. Do this if you need the game to ignore a hit that would otherwise happen (e.g. a parry)
		bool bIgnoreHit = false;

		// modifiers to the hit
		std::vector<PreHitModifier> modifiers;
	};

	struct PrecisionHitData
	{
		PrecisionHitData(RE::Actor* a_attacker, RE::TESObjectREFR* a_target, RE::hkpRigidBody* a_hitRigidBody, RE::hkpRigidBody* a_hittingRigidBody, const RE::NiPoint3& a_hitPos,
			const RE::NiPoint3& a_separatingNormal, const RE::NiPoint3& a_hitPointVelocity, RE::hkpShapeKey a_hitBodyShapeKey, RE::hkpShapeKey a_hittingBodyShapeKey) :
			attacker(a_attacker),
			target(a_target), hitRigidBody(a_hitRigidBody), hittingRigidBody(a_hittingRigidBody), hitPos(a_hitPos), separatingNormal(a_separatingNormal),
			hitPointVelocity(a_hitPointVelocity), hitBodyShapeKey(a_hitBodyShapeKey), hittingBodyShapeKey(a_hittingBodyShapeKey)
		{}

		RE::Actor* attacker;
		RE::TESObjectREFR* target;
		RE::hkpRigidBody* hitRigidBody;
		RE::hkpRigidBody* hittingRigidBody;

		RE::NiPoint3 hitPos;
		RE::NiPoint3 separatingNormal;
		RE::NiPoint3 hitPointVelocity;

		RE::hkpShapeKey hitBodyShapeKey;
		RE::hkpShapeKey hittingBodyShapeKey;
	};

	enum class CollisionFilterComparisonResult : uint8_t
	{
		Continue,  // Do not affect whether the two objects should collide
		Collide,   // Force the two objects to collide
		Ignore,    // Force the two objects to not collide
	};

	using PreHitCallback = std::function<PreHitCallbackReturn(const PrecisionHitData&)>;
	using PostHitCallback = std::function<void(const PrecisionHitData&, const RE::HitData&)>;
	using PrePhysicsStepCallback = std::function<void(RE::bhkWorld*)>;
	using CollisionFilterComparisonCallback = std::function<CollisionFilterComparisonResult(RE::bhkCollisionFilter*, uint32_t, uint32_t)>;

	// Precision's modder interface
	class IVPrecision1
	{
	public:
		/// <summary>
		/// Adds a callback that will run before Precision's hit function.
		/// </summary>
		/// <param name="a_myPluginHandle">Your assigned plugin handle</param>
		/// <param name="a_preHitCallback">The callback function</param>
		/// <returns>OK, AlreadyRegistered</returns>
		virtual APIResult AddPreHitCallback(SKSE::PluginHandle a_myPluginHandle, PreHitCallback&& a_preHitCallback) noexcept = 0;

		/// <summary>
		/// Adds a callback that will run after Precision's hit function.
		/// </summary>
		/// <param name="a_myPluginHandle">Your assigned plugin handle</param>
		/// <param name="a_postHitCallback">The callback function</param>
		/// <returns>OK, AlreadyRegistered</returns>
		virtual APIResult AddPostHitCallback(SKSE::PluginHandle a_myPluginHandle, PostHitCallback&& a_postHitCallback) noexcept = 0;

		/// <summary>
		/// Adds a callback that will run right before hkpWorld::stepDeltaTime is called.
		/// </summary>
		/// <param name="a_myPluginHandle">Your assigned plugin handle</param>
		/// <param name="a_prePhysicsStepCallback">The callback function</param>
		/// <returns>OK, AlreadyRegistered</returns>
		virtual APIResult AddPrePhysicsStepCallback(SKSE::PluginHandle a_myPluginHandle, PrePhysicsStepCallback&& a_prePhysicsStepCallback) noexcept = 0;

		/// <summary>
		/// Adds a callback that will run when havok compares collision filter info to determine if two objects should collide. This can be called hundreds of times per frame, so be brief.
		/// </summary>
		/// <param name="a_myPluginHandle">Your assigned plugin handle</param>
		/// <param name="a_collisionFilterComparisonCallback">The callback function</param>
		/// <returns>OK, AlreadyRegistered</returns>
		virtual APIResult AddCollisionFilterComparisonCallback(SKSE::PluginHandle a_myPluginHandle, CollisionFilterComparisonCallback&& a_collisionFilterComparisonCallback) noexcept = 0;

		/// <summary>
		/// Removes the callback that will run before Precision's hit function.
		/// </summary>
		/// <param name="a_myPluginHandle">Your assigned plugin handle</param>
		/// <returns>OK, NotRegistered</returns>
		virtual APIResult RemovePreHitCallback(SKSE::PluginHandle a_myPluginHandle) noexcept = 0;

		/// <summary>
		/// Removes the callback that will run after Precision's hit function.
		/// </summary>
		/// <param name="a_myPluginHandle">Your assigned plugin handle</param>
		/// <returns>OK, NotRegistered</returns>
		virtual APIResult RemovePostHitCallback(SKSE::PluginHandle a_myPluginHandle) noexcept = 0;

		/// <summary>
		/// Removes the callback that will run right before hkpWorld::stepDeltaTime is called.
		/// </summary>
		/// <param name="a_myPluginHandle">Your assigned plugin handle</param>
		/// <returns>OK, NotRegistered</returns>
		virtual APIResult RemovePrePhysicsStepCallback(SKSE::PluginHandle a_myPluginHandle) noexcept = 0;

		/// <summary>
		/// Removes the callback that will run when havok compares collision filter info to determine if two objects should collide.
		/// </summary>
		/// <param name="a_myPluginHandle">Your assigned plugin handle</param>
		/// <returns>OK, NotRegistered</returns>
		virtual APIResult RemoveCollisionFilterComparisonCallback(SKSE::PluginHandle a_myPluginHandle) noexcept = 0;
	};

	typedef void* (*_RequestPluginAPI)(const InterfaceVersion interfaceVersion);

	/// <summary>
	/// Request the Precision API interface.
	/// Recommended: Send your request when you need to use the API and cache the pointer. SKSEMessagingInterface::kMessage_PostLoad seems to be unreliable for some users for unknown reasons.
	/// </summary>
	/// <param name="a_interfaceVersion">The interface version to request</param>
	/// <returns>The pointer to the API singleton, or nullptr if request failed</returns>
	[[nodiscard]] inline void* RequestPluginAPI(const InterfaceVersion a_interfaceVersion = InterfaceVersion::V1)
	{
		auto pluginHandle = GetModuleHandle("Precision.dll");
		_RequestPluginAPI requestAPIFunction = (_RequestPluginAPI)GetProcAddress(pluginHandle, "RequestPluginAPI");
		if (requestAPIFunction) {
			return requestAPIFunction(a_interfaceVersion);
		}
		return nullptr;
	}
}
