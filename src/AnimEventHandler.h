#pragma once


using EventResult = RE::BSEventNotifyControl;

class animEventHandler {

	private:

		typedef RE::BSEventNotifyControl 
			(animEventHandler::* FnProcessEvent)
			(RE::BSAnimationGraphEvent &,RE::BSTEventSource<RE::BSAnimationGraphEvent> *);

		RE::BSEventNotifyControl HookedProcessEvent(
			RE::BSAnimationGraphEvent & event ,
			RE::BSTEventSource<RE::BSAnimationGraphEvent> * source);


		template <class Type>
		static Type SafeWrite64Function(uintptr_t address,Type data){
			
			void * bytes[2];
			memcpy(bytes,& data,sizeof(data));
			
			size_t length = sizeof(bytes[0]);

			DWORD oldProtect;
			VirtualProtect((void *) address,length,PAGE_EXECUTE_READWRITE,& oldProtect);
			
			Type old;
			
			memset(& old,0,sizeof(Type));
			memcpy(& old,(void *) address,length);
			memcpy((void *) address,& bytes[0],length);
			
			VirtualProtect((void *) address,length,oldProtect,& oldProtect);
			
			return old;
		}


		static void HookSink(uintptr_t pointer){
			FnProcessEvent function = SafeWrite64Function(pointer + 0x8,& animEventHandler::HookedProcessEvent);
			fnHash.insert(std::pair<uint64_t,FnProcessEvent>(pointer,function));
		}


	protected:

		static std::unordered_map<uint64_t,FnProcessEvent> fnHash;


	public:

		/**
		 *	@brief Hook animation event sink
		 */

		static void Register(bool isPlayer,bool isNPC){
			
			if(isPlayer){
				logger::info("Sinking animation event hook for player");
				REL::Relocation<uintptr_t> pointer { RE::VTABLE_PlayerCharacter[2] };
				HookSink(pointer.address());
				return;
			}
			
			if (isNPC) {
				logger::info("Sinking animation event hook for NPC");
				REL::Relocation<uintptr_t> pointer { RE::VTABLE_Character[2] };
				HookSink(pointer.address());
				return;
			}

			logger::info("Sinking complete.");
		}


		static void RegisterForPlayer(){
			Register(true,false);
		}
};
