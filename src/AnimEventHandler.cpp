#include "AnimEventHandler.h"
#include "EldenParry.h"


constexpr uint32_t hash(const char * string,size_t const size) noexcept{

	uint32_t hash = 5381;

	for(const char * c = string;c < string + size;++c)
		hash = ((hash << 5) + hash) + (unsigned char) * c;

	return hash;
}


constexpr uint32_t operator"" _h(const char * string,size_t size) noexcept {
	return hash(string,size);
}


RE::BSEventNotifyControl animEventHandler::HookedProcessEvent(
	RE::BSAnimationGraphEvent & event,
	RE::BSTEventSource<RE::BSAnimationGraphEvent> * source
){
	
	FnProcessEvent function = fnHash.at(* (uint64_t *) this);
	
	//RE::ConsoleLog::GetSingleton() -> Print(event.tag.c_str());

	if(event.tag == "bashStop" && event.holder)
		EldenParry::GetSingleton() -> applyParryCost((RE::Actor *) event.holder);
	
	return function 
		? (this -> * function)(event,source) 
		: RE::BSEventNotifyControl::kContinue ;
}


std::unordered_map<uint64_t,animEventHandler::FnProcessEvent> animEventHandler::fnHash;
