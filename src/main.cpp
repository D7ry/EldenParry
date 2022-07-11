#include "Hooks.h"
#include "EldenParry.h"
#include "AnimEventHandler.h"


void MessageHandler(SKSE::MessagingInterface::Message * message){

	const auto type = message -> type;

	if(type == SKSE::MessagingInterface::kDataLoaded){

		// It is now safe to access form data.s
	
		EldenParry::GetSingleton() -> init();
	
		if(Settings::bSuccessfulParryNoCost)
			animEventHandler::Register(true,Settings::bEnableNPCParry);
		
		return;
	}


	/*
	 *	Skyrim LifeCycle Events
	 *
	 * 	PostLoad     : Plugins finished running SKSEPlugin_Load.
	 * 				   It is now safe to do multithreaded operations, 
	 * 				   as well as operations against other plugins.	
	 * 	PostPostLoad : PostLoad messagae handlers finished running.
	 * 	InputLoaded  : Game data has been found. 
	 * 	DataLoaded	 : ESM / ESL / ESP plugins have loaded, main menu is active.
	 */

	// switch(type){
	// case SKSE::MessagingInterface::kPostLoad :
	// case SKSE::MessagingInterface::kPostPostLoad :  
	// case SKSE::MessagingInterface::kInputLoaded :   
	// 	break;
	// }


	/*
	 *	Skyrim Game Events
	 *
	 *	NewGame 	 : Player starts a new game from main menu.
	 *	PreLoadGame  : Player selected a game to load, but it hasn't loaded yet.
	 *				   Data will be the name of the loaded save.
	 *	PostLoadGame : Player's selected save game has finished loading.
	 *				   Data will be a boolean indicating whether the load was successful.
	 *	SaveGame 	 : The player has saved a game.
	 *				   Data will be the save name.
	 *	DeleteGame 	 : The player deleted a saved game from within the load menu.
	 */

	// switch(type){
	// case SKSE::MessagingInterface::kNewGame :
	// case SKSE::MessagingInterface::kPreLoadGame :
	// case SKSE::MessagingInterface::kPostLoadGame :
	// case SKSE::MessagingInterface::kSaveGame :
	// case SKSE::MessagingInterface::kDeleteGame :
	// 	break;
	// }
}


void InitializeLog(){

	#ifndef NDEBUG
		auto sink = std::make_shared<spdlog::sinks::msvc_sink_mt>();
	#else
		auto path = logger::log_directory();
		
		if(!path)
			util::report_and_fail("Failed to find standard logging directory"sv);
		
		* path /= fmt::format("{}.log"sv,Plugin::NAME);
		
		auto sink = std::make_shared<spdlog::sinks::basic_file_sink_mt>(path -> string(),true);
	#endif

	#ifndef NDEBUG
		const auto level = spdlog::level::trace;
	#else
		const auto level = spdlog::level::info;
	#endif

	auto log = std::make_shared<spdlog::logger>("global log"s,std::move(sink));
	log -> set_level(level);
	log -> flush_on(level);

	spdlog::set_default_logger(std::move(log));
	spdlog::set_pattern("%g(%#): [%^%l%$] %v"s);
}


extern "C" DLLEXPORT bool SKSEAPI SKSEPlugin_Query(
	const SKSE::QueryInterface * skse ,
	SKSE::PluginInfo * info
){
	
	info -> infoVersion = SKSE::PluginInfo::kVersion;
	info -> version = Plugin::VERSION[0];
	info -> name = Plugin::NAME.data();

	if(skse -> IsEditor()){
		logger::critical("Loaded in editor, marking as incompatible"sv);
		return false;
	}

	const auto version = skse -> RuntimeVersion();

	if(ver < SKSE::RUNTIME_1_5_39){
		logger::critical(FMT_STRING("Unsupported runtime version {}"),version.string());
		return false;
	}
	
	return true;
}


#ifdef SKYRIM_SUPPORT_AE

extern "C" DLLEXPORT constinit auto SKSEPlugin_Version = [](){

	SKSE::PluginVersionData version;

	version.PluginVersion(Plugin::VERSION);
	version.PluginName(Plugin::NAME);

	version.UsesAddressLibrary(true);
	version.CompatibleVersions({ SKSE::RUNTIME_LATEST });

	return version;
}();

#endif


extern "C" DLLEXPORT bool SKSEAPI SKSEPlugin_Load(const SKSE::LoadInterface * skse){

	#ifndef NDEBUG
		while(!IsDebuggerPresent()) 
			Sleep(100);
	#endif

	InitializeLog();
	logger::info("{} v{}"sv,Plugin::NAME,Plugin::VERSION.string());

	SKSE::Init(skse);

	auto messaging = SKSE::GetMessagingInterface();
	

	// Do stuff when SKSE initializes here:

	if(messaging -> RegisterListener("SKSE",MessageHandler)){

		Settings::readSettings();
		Hooks::install();

		return true;
	}
	
	
	return false;	
}

