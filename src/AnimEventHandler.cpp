#include "AnimEventHandler.h"
#include "EldenParry.h"
#include "Settings.h"
constexpr uint32_t hash(const char* data, size_t const size) noexcept
{
	uint32_t hash = 5381;

	for (const char* c = data; c < data + size; ++c) {
		hash = ((hash << 5) + hash) + (unsigned char)*c;
	}

	return hash;
}

constexpr uint32_t operator"" _h(const char* str, size_t size) noexcept
{
	return hash(str, size);
}

RE::BSEventNotifyControl animEventHandler::HookedProcessEvent(RE::BSAnimationGraphEvent& a_event, RE::BSTEventSource<RE::BSAnimationGraphEvent>* src)
{
	FnProcessEvent fn = fnHash.at(*(uint64_t*)this);
	//RE::ConsoleLog::GetSingleton()->Print(a_event.tag.c_str());
	if (!a_event.holder) {
		return fn ? (this->*fn)(a_event, src) : RE::BSEventNotifyControl::kContinue;
	}
	std::string_view eventTag = a_event.tag.data();
	switch (hash(eventTag.data(), eventTag.size())) {
	case "blockStop"_h:
		if (const_cast<RE::TESObjectREFR*>(a_event.holder)->As<RE::Actor>()->GetAttackState() == RE::ATTACK_STATE_ENUM::kBash) {
			EldenParry::GetSingleton()->startTimingParry((RE::Actor*)(a_event.holder));
		}
		break;
	case "bashStop"_h:
		auto EP = EldenParry::GetSingleton();
		if (Settings::bSuccessfulParryNoCost) {
			EP->applyParryCost((RE::Actor*)a_event.holder);
		}
		EP->finishTimingParry((RE::Actor*)a_event.holder);
		break;
	}
	return fn ? (this->*fn)(a_event, src) : RE::BSEventNotifyControl::kContinue;
}

std::unordered_map<uint64_t, animEventHandler::FnProcessEvent> animEventHandler::fnHash;
