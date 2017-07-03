#include "steamstorm.h"
#include <thread>
#include <fstream>
//#include <steam/isteamapplist.h>
#include <steam/isteamclient.h>
#include <steam/isteamapps.h>
//#include <steam/isteamappticket.h>
#include <steam/isteamcontroller.h>
#include <steam/isteamfriends.h>
//#include <steam/isteamgamecoordinator.h>
//#include <steam/isteamgameserver.h>
//#include <steam/isteamgameserverstats.h>
#include <steam/isteamhtmlsurface.h>
#include <steam/isteamhttp.h>
//#include <steam/isteaminventory.h>
#include <steam/isteammatchmaking.h>
#include <steam/isteammusic.h>
//#include <steam/isteammusicremote.h>
#include <steam/isteamnetworking.h>
#include <steam/isteamremotestorage.h>
#include <steam/isteamscreenshots.h>
#include <steam/isteamugc.h>
//#include <steam/isteamunifiedmessages.h>
#include <steam/isteamuser.h>
#include <steam/isteamuserstats.h>
#include <steam/isteamutils.h>
//#include <steam/isteamvideo.h>
#include <steam/steam_api_flat.h>
#include "dynamic_load.h"
#include "cast_if_required.h"

// reproductions pf steam declarations from headers we don't want to include directly
S_API bool S_CALLTYPE SteamAPI_IsSteamRunning();                                // from steam_api.h
S_API bool S_CALLTYPE SteamAPI_Init();                                          // from steam_api.h
S_API void S_CALLTYPE SteamAPI_Shutdown();                                      // from steam_api.h
S_API ISteamClient *S_CALLTYPE SteamClient();                                   // from steam_api.h
S_API HSteamUser SteamAPI_GetHSteamUser();                                      // from steam_api.h
S_API HSteamPipe SteamAPI_GetHSteamPipe();                                      // from steam_api.h

steamstorm::steamstorm(uint32_t this_appid)
  : appid(this_appid) {
  /// Default constructor
  #ifdef DEBUG_STEAMSTORM
    if(appid != 0) {
      std::cout << "SteamStorm: DEBUG: Launching with explicitly specified appid " << appid << std::endl;
    }
  #endif // DEBUG_STEAMSTORM
  init();
}

steamstorm::~steamstorm() {
  /// Default destructor
  shutdown();
}

void steamstorm::init() {
  /// Attempt to initialise everything
  enabled = false;
  std::cout << "SteamStorm: Initialising..." << std::endl;
  try {
    // dynamic library load
    #if defined(PLATFORM_WINDOWS)
      #
      lib = load_dynamic({"./steam_api.dll", "steam_api.dll"});
      lib = load_dynamic({"./steam_api.dll", "steam_api.dll"});
    #elif defined(PLATFORM_LINUX)
      #ifdef NDEBUG
        lib = load_dynamic({"./libsteam_api.so", "libsteam_api.so"});
      #else
        lib = load_dynamic({"./libsteam_api.so.dbg", "libsteam_api.so.dbg", "./libsteam_api.so", "libsteam_api.so"});
      #endif // NDEBUG
    #elif defined(PLATFORM_MACOS)
      lib = load_dynamic({"./libsteam_api.dylib", "libsteam_api.dylib"});
    #else
      #error ""
    #endif
    if(!lib) {
      std::cout << "SteamStorm: No Steam dynamic library found, not initialising." << std::endl;
      shutdown();
      return;
    }

    auto api_IsSteamRunning   = load_symbol<decltype(&SteamAPI_IsSteamRunning)>(lib, "SteamAPI_IsSteamRunning");
    auto api_Init             = load_symbol<decltype(&SteamAPI_Init          )>(lib, "SteamAPI_Init");
    //auto api_Init             = load_symbol<decltype(&SteamAPI_Init          )>(lib, "SteamAPI_InitSafe");

    if(!api_IsSteamRunning()) {
      std::cout << "SteamStorm: Steam is not running - disabling Steam support." << std::endl;
      shutdown();
      return;
    }
    if(!api_Init()) {
      /// A return of false indicates one of three conditions:
      ///   The Steam client isn't running. A running Steam client is required to provide implementations of the various Steamworks interfaces.
      ///   The Steam client couldn't determine the AppID of game. Make sure you have Steam_appid.txt in your game directory.
      ///   Your application is not running under the same user context as the Steam client, including admin privileges.
      if(appid != 0) {
        // we have an explicit app id, so try to create the file, and initialise again
        #ifdef DEBUG_STEAMSTORM
          std::cout << "SteamStorm: WARNING: Unable to initialise the API despite library being present, attempting to create steam appid file" << std::endl;
        #endif // DEBUG_STEAMSTORM
        {
          std::ofstream appid_file("steam_appid.txt", std::ios::trunc);
          if(!appid_file.fail() && appid_file.is_open() && appid_file.good()) {
            appid_file << std::to_string(appid) << '\n';                        // don't use locale-dependent conversion, and use fixed newline char
            #ifdef DEBUG_STEAMSTORM
              std::cout << "SteamStorm: Successfully wrote appid file" << std::endl;
            #endif // DEBUG_STEAMSTORM
          } else {
            std::cout << "SteamStorm: ERROR: Unable to write to appid file and cannot initialise without it - disabling Steam support." << std::endl;
            shutdown();
            return;
          }
        }
        if(!api_Init()) {                                                       // try again after creating the file
          std::cout << "SteamStorm: Could not initialise Steam API - disabling Steam support." << std::endl;
          shutdown();
          return;
        }
      } else {
        std::cout << "SteamStorm: We don't know our Steam app id, so cannot attempt to create the appid file - disabling Steam support." << std::endl;
        shutdown();
        return;
      }
    }
    #ifdef DEBUG_STEAMSTORM
      std::cout << "SteamStorm: DEBUG: Initialised Steam API, loading symbols..." << std::endl;
    #endif // DEBUG_STEAMSTORM
    auto api_SteamClient                     = load_symbol<decltype(&SteamClient                             )>(lib, "SteamClient");
    auto api_GetHSteamUser                   = load_symbol<decltype(&SteamAPI_GetHSteamUser                  )>(lib, "SteamAPI_GetHSteamUser");
    auto api_GetHSteamPipe                   = load_symbol<decltype(&SteamAPI_GetHSteamPipe                  )>(lib, "SteamAPI_GetHSteamPipe");
    auto api_ISteamClient_GetISteamUtils     = load_symbol<decltype(&SteamAPI_ISteamClient_GetISteamUtils    )>(lib, "SteamAPI_ISteamClient_GetISteamUtils");
    auto api_ISteamClient_GetISteamUser      = load_symbol<decltype(&SteamAPI_ISteamClient_GetISteamUser     )>(lib, "SteamAPI_ISteamClient_GetISteamUser");
    auto api_ISteamClient_GetISteamApps      = load_symbol<decltype(&SteamAPI_ISteamClient_GetISteamApps     )>(lib, "SteamAPI_ISteamClient_GetISteamApps");
    auto api_ISteamClient_GetISteamFriends   = load_symbol<decltype(&SteamAPI_ISteamClient_GetISteamFriends  )>(lib, "SteamAPI_ISteamClient_GetISteamFriends");
    auto api_ISteamClient_GetISteamUserStats = load_symbol<decltype(&SteamAPI_ISteamClient_GetISteamUserStats)>(lib, "SteamAPI_ISteamClient_GetISteamUserStats");
    auto api_ISteamUser_GetSteamID           = load_symbol<decltype(&SteamAPI_ISteamUser_GetSteamID          )>(lib, "SteamAPI_ISteamUser_GetSteamID");
    //auto api_ISteamUtils_GetAPICallResult    = load_symbol<decltype(&SteamAPI_ISteamUtils_GetAPICallResult   )>(lib, "SteamAPI_ISteamUtils_GetAPICallResult");
    #ifdef DEBUG_STEAMSTORM
      std::cout << "SteamStorm: DEBUG: Loaded all symbols." << std::endl;
    #endif // DEBUG_STEAMSTORM
    client_user = api_GetHSteamUser();
    client_pipe = api_GetHSteamPipe();
    #ifdef DEBUG_STEAMSTORM
      std::cout << "SteamStorm: DEBUG: HSteamUser " << client_user << ", HSteamPipe " << client_pipe << std::endl;
    #endif // DEBUG_STEAMSTORM

    api_client = api_SteamClient();
    api_flat_client = reinterpret_cast<intptr_t>(api_client);
    if(!(api_client)) {
      std::cout << "SteamStorm: Could not initialise API client module - disabling Steam support." << std::endl;
      shutdown();
      return;
    }
    api_utils     = api_ISteamClient_GetISteamUtils(    api_flat_client,              client_pipe, STEAMUTILS_INTERFACE_VERSION);
    api_user      = api_ISteamClient_GetISteamUser(     api_flat_client, client_user, client_pipe, STEAMUSER_INTERFACE_VERSION);
    api_apps      = api_ISteamClient_GetISteamApps(     api_flat_client, client_user, client_pipe, STEAMAPPS_INTERFACE_VERSION);
    api_friends   = api_ISteamClient_GetISteamFriends(  api_flat_client, client_user, client_pipe, STEAMFRIENDS_INTERFACE_VERSION);
    api_userstats = api_ISteamClient_GetISteamUserStats(api_flat_client, client_user, client_pipe, STEAMUSERSTATS_INTERFACE_VERSION);
    if(!(api_utils && api_user && api_apps && api_friends && api_userstats)) {
      std::cout << "SteamStorm: Could not initialise all API modules - disabling Steam support." << std::endl;
      shutdown();
      return;
    }
    api_flat_utils     = reinterpret_cast<intptr_t>(api_utils);
    api_flat_user      = reinterpret_cast<intptr_t>(api_user);
    api_flat_apps      = reinterpret_cast<intptr_t>(api_apps);
    api_flat_friends   = reinterpret_cast<intptr_t>(api_friends);
    api_flat_userstats = reinterpret_cast<intptr_t>(api_userstats);
    #ifdef DEBUG_STEAMSTORM
      std::cout << "SteamStorm: DEBUG: Loaded all modules." << std::endl;
    #endif // DEBUG_STEAMSTORM

    if(!api_user->BLoggedOn()) {
      std::cout << "SteamStorm: Steam is running but user is not logged in - disabling Steam support." << std::endl;
      shutdown();
      return;
    }
    enabled = true;                                                             // necessary to allow some following functions to work
    {
      auto new_appid = api_utils->GetAppID();
      if(appid != 0 && appid != new_appid) {
        std::cout << "SteamStorm: WARNING: App ID declared as " << appid << " but Steam tells us it's " << new_appid << "!" << std::endl;
      }
      appid = new_appid;
    }
    user_id = api_ISteamUser_GetSteamID(api_flat_user);
    api_user_id.SetFromUint64(user_id);
    username = api_friends->GetPersonaName();
    #ifdef DEBUG_STEAMSTORM
      std::cout << "SteamStorm: DEBUG: Querying Steam for the latest stats..." << std::endl;
    #endif // DEBUG_STEAMSTORM
    if(!api_userstats->RequestCurrentStats()) {                                 // request an update on the current stats
      std::cout << "SteamStorm: ERROR: Failed to request user stats from Steam API - disabling Steam support." << std::endl;
      shutdown();
      return;
    }

    #ifdef DEBUG_STEAMSTORM
      std::cout << "SteamStorm: DEBUG: Got stats from Steam api." << std::endl;
      // various debugging checks that we don't want to bother with in release
      std::cout << "SteamStorm: DEBUG: Steam app ID:      " << appid << " build " << get_build() << std::endl;
      std::cout << "SteamStorm: DEBUG: Steam username:    " << username << std::endl;
      std::cout << "SteamStorm: DEBUG: Steam ID:          " << user_id << std::endl;
      std::cout << "SteamStorm: DEBUG: Steam level:       " << api_user->GetPlayerSteamLevel() << std::endl;
      std::cout << "SteamStorm: DEBUG: Badge level:       " << api_user->GetGameBadgeLevel(1, false) << std::endl;
      std::cout << "SteamStorm: DEBUG: Foil Badge level:  " << api_user->GetGameBadgeLevel(1, true)  << std::endl;
      std::cout << "SteamStorm: DEBUG: Friends:           " << api_friends->GetFriendCount(EFriendFlags::k_EFriendFlagImmediate) << " / " << api_friends->GetFriendCount(EFriendFlags::k_EFriendFlagAll) << std::endl;
      std::cout << "SteamStorm: DEBUG: Game language:     " << api_apps->GetCurrentGameLanguage() << std::endl;
      std::cout << "SteamStorm: DEBUG: Available langs:   " << api_apps->GetAvailableGameLanguages() << std::endl;
      std::cout << "SteamStorm: DEBUG: Steam UI language: " << api_utils->GetSteamUILanguage() << std::endl;
      std::cout << "SteamStorm: DEBUG: IP country:        " << api_utils->GetIPCountry() << std::endl;
      std::cout << "SteamStorm: DEBUG: Battery status:    " << static_cast<int>(api_utils->GetCurrentBatteryPower()) << std::endl; // 255 == AC power
      std::cout << "SteamStorm: DEBUG: User has this app: " << std::boolalpha << get_user_has_app() << ", DLC items: " << api_apps->GetDLCCount() << std::endl;
      if(api_utils->IsSteamRunningInVR()) {
        std::cout << "SteamStorm: DEBUG: Steam is running in VR mode" << std::endl;
      }
      if(api_apps->BIsLowViolence()) {
        std::cout << "SteamStorm: DEBUG: User is low-violence" << std::endl;
      }
      if(api_apps->BIsCybercafe()) {
        std::cout << "SteamStorm: DEBUG: User is cyber cafe" << std::endl;
      }
      if(api_apps->BIsVACBanned()) {
        std::cout << "SteamStorm: DEBUG: User is VAC banned" << std::endl;
      }
      if(api_user->BIsBehindNAT()) {
        std::cout << "SteamStorm: DEBUG: User appears to be behind NAT" << std::endl;
      }
      #if STEAM_API_VERSION >= 137
        if(api_user->BIsPhoneVerified()) {
          std::cout << "SteamStorm: DEBUG: User's phone is verified" << std::endl;
        }
        if(api_user->BIsPhoneRequiringVerification()) {
          std::cout << "SteamStorm: DEBUG: User's phone is awaiting re-verification" << std::endl;
        }
        if(api_user->BIsPhoneIdentifying()) {
          std::cout << "SteamStorm: DEBUG: User's phone number is considered identifying" << std::endl;
        }
        if(api_user->BIsTwoFactorEnabled()) {
          std::cout << "SteamStorm: DEBUG: User has two-factor auth" << std::endl;
        }
      #endif // STEAM_API_VERSION >= 137
      switch(api_friends->GetPersonaState()) {
      case EPersonaState::k_EPersonaStateOffline:
        std::cout << "SteamStorm: DEBUG: User not currently logged on" << std::endl;
        break;
      case EPersonaState::k_EPersonaStateOnline:
        std::cout << "SteamStorm: DEBUG: User is logged on" << std::endl;
        break;
      case EPersonaState::k_EPersonaStateBusy:
        std::cout << "SteamStorm: DEBUG: User is on, but busy" << std::endl;
        break;
      case EPersonaState::k_EPersonaStateAway:
        std::cout << "SteamStorm: DEBUG: User is auto-away" << std::endl;
        break;
      case EPersonaState::k_EPersonaStateSnooze:
        std::cout << "SteamStorm: DEBUG: User is auto-away for a long time" << std::endl;
        break;
      case EPersonaState::k_EPersonaStateLookingToTrade:
        std::cout << "SteamStorm: DEBUG: User is online, trading" << std::endl;
        break;
      case EPersonaState::k_EPersonaStateLookingToPlay:
        std::cout << "SteamStorm: DEBUG: User is online, wanting to play" << std::endl;
        break;
      default:
        break;
      }
      unsigned int const achievements_available = api_userstats->GetNumAchievements();
      unsigned int achievements_unlocked = 0;
      for(unsigned int i = 0; i != achievements_available; ++i) {
        bool alreadyachieved = false;
        uint32_t achievementtime = 0;
        std::string const &achievement_name(api_userstats->GetAchievementName(i));
        api_userstats->GetAchievementAndUnlockTime(achievement_name.c_str(), &alreadyachieved, &achievementtime);
        #ifdef DEBUG_STEAMSTORM_ACHIEVEMENTS
          std::cout << "SteamStorm: DEBUG:   " << achievement_name;
        #endif // DEBUG_STEAMSTORM_ACHIEVEMENTS
        if(alreadyachieved) {
          ++achievements_unlocked;
          #ifdef DEBUG_STEAMSTORM_ACHIEVEMENTS
            std::cout << " * ";
          #endif // DEBUG_STEAMSTORM_ACHIEVEMENTS
        }
        #ifdef DEBUG_STEAMSTORM_ACHIEVEMENTS
          std::cout << std::endl;
        #endif // DEBUG_STEAMSTORM_ACHIEVEMENTS
      }
      std::cout << "SteamStorm: DEBUG: Achievements:      " << achievements_unlocked << " / " << achievements_available << std::endl;
      {
        auto result = call_sync<NumberOfCurrentPlayers_t>(api_userstats->GetNumberOfCurrentPlayers());
        if(result) {
          std::cout << "SteamStorm: DEBUG: Players online:    " << result->m_cPlayers << std::endl;
        }
      }
    #else
      std::cout << "SteamStorm: Steam build: " << get_build() << std::endl;
      std::cout << "SteamStorm: Steam user: " << username << " (" << user_id << ")" << std::endl;
    #endif // DEBUG_STEAMSTORM

    std::cout << "SteamStorm: Initialised and ready." << std::endl;
  } catch(std::exception &e) {
    std::cout << "SteamStorm: Exception at startup: " << e.what() << std::endl;
    shutdown();
  }
}

void steamstorm::shutdown() {
  /// Shut down cleanly
  if(lib && enabled) {
    std::cout << "SteamStorm: Shutting down." << std::endl;
    try {
      auto api_Shutdown = load_symbol<decltype(&SteamAPI_Shutdown)>(lib, "SteamAPI_Shutdown");
      if(api_Shutdown) {
        api_Shutdown();
      }
    } catch(std::exception &e) {
      std::cout << "SteamStorm: Exception at shutdown: " << e.what() << std::endl;
    }
  }
  unload_dynamic(lib);
  enabled = false;
}

bool steamstorm::get_user_has_app() const {
  /// Find out whether the user has this app in their library
  if(!enabled) {
    return false;                                                               // exit silently if the api isn't initialised
  }
  return api_apps->BIsSubscribed();
}

bool steamstorm::get_user_has_app(unsigned int const appid_to_check) const {
  /// Find out whether a user has the app ID specified - useful for checking DLC etc
  if(!enabled) {
    return false;                                                               // exit silently if the api isn't initialised
  }
  //return (api_user->UserHasLicenseForApp(user_id, appid_to_check) == EUserHasLicenseForAppResult::k_EUserHasLicenseResultHasLicense);
  // the above doesn't work, never has perhaps
  return api_apps->BIsSubscribedApp(appid_to_check);
}
bool steamstorm::get_dlc_installed(unsigned int const appid_to_check) const {
  /// Find out whether a user has the DLC ID specified and it's installed
  if(!enabled) {
    return false;                                                               // exit silently if the api isn't initialised
  }
  return api_apps->BIsDlcInstalled(appid_to_check);
}

bool steamstorm::get_achievement(std::string const &achievementname) const {
  /// Find out whether the specified achievement has been set
  if(!enabled) {
    return false;                                                               // exit silently if the api isn't initialised
  }
  bool alreadyachieved = false;
  uint32_t achievementtime = 0;
  api_userstats->GetAchievementAndUnlockTime(achievementname.c_str(), &alreadyachieved, &achievementtime);
  #ifdef DEBUG_STEAMSTORM
    if(alreadyachieved) {
      std::cout << "SteamStorm: DEBUG: Achievement \"" << achievementname << "\" unlocked at " << achievementtime << "." << std::endl;
    } else {
      std::cout << "SteamStorm: DEBUG: Achievement \"" << achievementname << "\" not yet unlocked." << std::endl;
    }
  #endif // DEBUG_STEAMSTORM
  return alreadyachieved;
}

std::string steamstorm::get_achievement_name(std::string const &achievementname) const {
  /// Find out the full name of the specified achievement
  if(!enabled) {
    return {};                                                                  // exit silently if the api isn't initialised
  }
  return api_userstats->GetAchievementDisplayAttribute(achievementname.c_str(), "name");
}
std::string steamstorm::get_achievement_description(std::string const &achievementname) const {
  /// Find out the full description of the specified achievement
  if(!enabled) {
    return {};                                                                  // exit silently if the api isn't initialised
  }
  return api_userstats->GetAchievementDisplayAttribute(achievementname.c_str(), "desc");
}
bool steamstorm::get_achievement_hidden(std::string const &achievementname) const {
  /// Find out whether the specified achievement is hidden
  if(!enabled) {
    return false;                                                               // exit silently if the api isn't initialised
  }
  return api_userstats->GetAchievementDisplayAttribute(achievementname.c_str(), "hidden");
}

std::string steamstorm::get_store_url() const {
  return std::string{"http://store.steampowered.com/app/"} + std::to_string(appid) + "/";
}
std::string steamstorm::get_review_url() const {
  return std::string{"http://store.steampowered.com/recommended/recommendgame/"} + std::to_string(appid);
}

bool steamstorm::get_overlay_enabled() const {
  if(!enabled) {
    return false;                                                               // exit silently if the api isn't initialised
  }
  return api_utils->IsOverlayEnabled();
}

std::string steamstorm::get_language_code() const {
  /// Convert a Steam language name to a LanguageStorm-compatible language code
  // Language list from https://partner.steamgames.com/documentation/languages
  if(!enabled) {
    return "en";                                                                // exit with a sensible default if the api isn't initialised
  }
  std::string const lang(api_apps->GetCurrentGameLanguage());
  if(lang == "english") {                                                       // prioritise most common case first
    return "en";
  } else if(lang == "arabic") {
    return "ar";
  } else if(lang == "brazilian") {                                              // Brazilian Portuguese
    return "pt_br";
  } else if(lang == "bulgarian") {
    return "bg";
  } else if(lang == "czech") {
    return "cs";
  } else if(lang == "danish") {
    return "da";
  } else if(lang == "dutch") {
    return "nl";
  } else if(lang == "finnish") {
    return "fi";
  } else if(lang == "french") {
    return "fr";
  } else if(lang == "german") {
    return "de";
  } else if(lang == "greek") {
    return "el";
  } else if(lang == "hungarian") {
    return "hu";
  } else if(lang == "italian") {
    return "it";
  } else if(lang == "japanese") {
    return "jp";
  } else if(lang == "korean") {
    return "ko";
  } else if(lang == "norwegian") {
    return "no";
  } else if(lang == "polish") {
    return "pl";
  } else if(lang == "portuguese") {
    return "pt";
  } else if(lang == "romanian") {
    return "";
  } else if(lang == "russian") {
    return "ru";
  } else if(lang == "schinese") {                                               // simplified Chinese
    return "zh_Hans";
  } else if(lang == "spanish") {
    return "es";
  } else if(lang == "swedish") {
    return "sv";
  } else if(lang == "tchinese") {                                               // traditional Chinese
    return "zh_Hant";
  } else if(lang == "thai") {
    return "th";
  } else if(lang == "turkish") {
    return "tr";
  } else if(lang == "ukranian") {
    return "uk";
  } else {
    return "en";
  }
}

std::string steamstorm::get_last_api_failure(SteamAPICall_t callback) const {
  /// Report the type of failure the last API call had
  switch(api_utils->GetAPICallFailureReason(callback)) {
  default:
  case k_ESteamAPICallFailureNone:
    return "no failure";
  case k_ESteamAPICallFailureSteamGone:
    return "Steam process is absent";
  case k_ESteamAPICallFailureNetworkFailure:
    return "Network connection to Steam broken";                                // SteamServersDisconnected_t callback will be sent around the same time, SteamServersConnected_t will be sent when the client is able to talk to the Steam servers again
  case k_ESteamAPICallFailureInvalidHandle:
    return "Invalid api call handle";
  case k_ESteamAPICallFailureMismatchedCallback:
    return "GetAPICallResult() called with wrong callback type for this call";
  }
}

void steamstorm::open_url_in_overlay(std::string const &url) const {
  if(!enabled) {
    return;                                                                     // exit silently if the api isn't initialised
  }
  #ifdef DEBUG_STEAMSTORM
    std::cout << "SteamStorm: DEBUG: Opening Steam overlay to url \"" << url << std::endl;
  #endif // DEBUG_STEAMSTORM
  api_friends->ActivateGameOverlayToWebPage(url.c_str());
}

void steamstorm::set_achievement(std::string const &achievementname) const {
  /// Attempt to set an achievement described by the specified string
  if(!enabled) {
    return;                                                                     // exit silently if the api isn't initialised
  }
  #ifdef DEBUG_STEAMSTORM
    std::string const achievementfullname(get_achievement_name(achievementname));
    std::string const achievementfulldesc(get_achievement_description(achievementname));
    if(achievementfullname.size() == 0 && achievementfulldesc.size() == 0) {
      std::cout << "SteamStorm: ERROR: No such achievement found: \"" << achievementname << "\"" << std::endl;
      return;
    }
    std::cout << "SteamStorm: DEBUG: Setting achievement \"" << achievementname << "\" - " << achievementfullname << ": " << achievementfullname;
    if(get_achievement_hidden(achievementname)) {
      std::cout << " (hidden)";
    }
    std::cout << std::endl;
  #endif // DEBUG_STEAMSTORM
  if(get_achievement(achievementname)) {
    return;
  }
  bool const result = api_userstats->SetAchievement(achievementname.c_str());
  if(!result) {
    /// If the call failed then either Steam is not initialized or you still haven't processed the callback from the inital call to RequestStats().
    /// You can't set any achievements until that callback has been received.
    std::cout << "SteamStorm: ERROR: Setting achievement \"" << achievementname << "\" failed." << std::endl;
    //enabled = false;
    return;
  }
  #ifdef DEBUG_STEAMSTORM
    std::cout << "SteamStorm: DEBUG: Setting achievement \"" << achievementname << "\" succeeded." << std::endl;
  #endif // DEBUG_STEAMSTORM
  store();
}

template<typename T>
T steamstorm::get_stat(std::string const &statname) const {
  /// Attempt to return a statistic value
  if(!enabled) {
    return 0;                                                                   // exit silently if the api isn't initialised
  }
  #ifdef DEBUG_STEAMSTORM
    std::cout << "SteamStorm: DEBUG: Getting statistic \"" << statname << " (" << typeid(T).name() << ")" << std::endl;
  #endif // DEBUG_STEAMSTORM
  T newstat = 0;
  bool success = api_userstats->GetStat(statname.c_str(), &newstat);
  if(!success) {
    std::cout << "SteamStorm: ERROR: Could not get statistic \"" << statname << " (" << typeid(T).name() << ")" << std::endl;
    return 0;
  }
  return newstat;
}

template<typename T>
void steamstorm::set_stat(std::string const &statname,
                          T const statvalue) const {
  /// Attempt to update a statistic to the specified value
  if(!enabled) {
    return;                                                                     // exit silently if the api isn't initialised
  }
  #ifdef DEBUG_STEAMSTORM
    std::cout << "SteamStorm: DEBUG: Setting statistic \"" << statname << "\" to " << statvalue << " (" << typeid(T).name() << ")" << std::endl;
  #endif // DEBUG_STEAMSTORM
  bool success = api_userstats->SetStat(statname.c_str(), statvalue);
  if(!success) {
    std::cout << "SteamStorm: ERROR: Could not set statistic \"" << statname << "\" to " << statvalue << " (" << typeid(T).name() << ")" << std::endl;
    return;
  }
  store();
}
template<typename T>
void steamstorm::add_stat(std::string const &statname,
                          T const statvalue) const {
  /// Attempt to add a value to a statistic
  if(!enabled) {
    return;                                                                     // exit silently if the api isn't initialised
  }
  T old_stat = 0;
  bool success = api_userstats->GetStat(statname.c_str(), &old_stat);
  if(!success) {
    std::cout << "SteamStorm: ERROR: Could not adjust statistic \"" << statname << "\" by " << statvalue << " (" << typeid(T).name() << ")" << std::endl;
    return;
  }
  #ifdef DEBUG_STEAMSTORM
    std::cout << "SteamStorm: DEBUG: Adjusting statistic \"" << statname << "\" from " << old_stat << " to " << old_stat + statvalue << " (" << typeid(T).name() << ")" << std::endl;
  #endif // DEBUG_STEAMSTORM
  success = api_userstats->SetStat(statname.c_str(), old_stat + statvalue);
  if(!success) {
    std::cout << "SteamStorm: ERROR: Could not adjust statistic \"" << statname << "\" by " << statvalue << " (" << typeid(T).name() << ")" << std::endl;
    return;
  }
  store();
}
template<typename T>
void steamstorm::sub_stat(std::string const &statname,
                          T const statvalue) const {
  /// Attempt to subtract a value from a statistic
  add_stat(statname, -statvalue);
}
void steamstorm::inc_stat(std::string const &statname) const {
  /// Attempt to increment a statistic
  add_stat(statname, 1);
}
void steamstorm::dec_stat(std::string const &statname) const {
  /// Attempt to increment a statistic
  sub_stat(statname, 1);
}

// instantiate the templates explicitly for the two permissible types
template int32_t steamstorm::get_stat<int32_t>(std::string const&)          const;
template float   steamstorm::get_stat<float  >(std::string const&)          const;
template void    steamstorm::set_stat<int32_t>(std::string const&, int32_t) const;
template void    steamstorm::set_stat<float  >(std::string const&, float  ) const;
template void    steamstorm::add_stat<int32_t>(std::string const&, int32_t) const;
template void    steamstorm::add_stat<float  >(std::string const&, float  ) const;
template void    steamstorm::sub_stat<int32_t>(std::string const&, int32_t) const;
template void    steamstorm::sub_stat<float  >(std::string const&, float  ) const;

int steamstorm::get_build() const {
  /// Return the build ID
  if(!enabled) {
    return 0;
  }
  #if STEAM_API_VERSION >= 132
    return api_apps->GetAppBuildId();
  #else
    return 0;
  #endif // STEAM_API_VERSION
}

void steamstorm::store() const {
  /// Attempt to save achievements and stats
  if(!enabled) {
    return;                                                                     // exit silently if the api isn't initialised
  }
  if(!api_userstats->StoreStats()) {                                            // push the new stats to the server
    std::cout << "SteamStorm: ERROR: Could not store stats!" << std::endl;
    return;
  }
}

uint64_t steamstorm::load_leaderboard(std::string const &name) {
  /// Cache a leaderboard handle synchronously in our internal map, for later use
  if(!enabled) {
    return 0;                                                                   // exit silently if the api isn't initialised
  }
  auto result = call_sync<LeaderboardFindResult_t>(api_userstats->FindLeaderboard(name.c_str()));
  if(!result) {
    std::cout << "SteamStorm: ERROR: No result when loading leaderboard named \"" << name << "\"." << std::endl;
    return 0;
  }
  if(result->m_bLeaderboardFound == 0) {
    std::cout << "SteamStorm: ERROR: Could not find a leaderboard named \"" << name << "\"." << std::endl;
  }
  if(result->m_hSteamLeaderboard == 0) {
    std::cout << "SteamStorm: ERROR: Got ID 0 for leaderboard named \"" << name << "\"." << std::endl;
  }
  #ifdef DEBUG_STEAMSTORM
    std::cout << "SteamStorm: DEBUG: Leaderboard \"" << name << "\" handle is " << result->m_hSteamLeaderboard << std::endl;
  #endif // DEBUG_STEAMSTORM
  leaderboards.emplace(name, result->m_hSteamLeaderboard);
  return result->m_hSteamLeaderboard;
}

void steamstorm::set_leaderboard_score(std::string const &name,
                                       int32_t score,
                                       std::vector<int32_t> const &score_details) {
  /// Set a new score for the specified leaderboard, loading it if necessary
  // Details are extra game-defined information regarding how the user got that score
  if(!enabled) {
    return;                                                                     // exit silently if the api isn't initialised
  }
  #ifdef DEBUG_STEAMSTORM
    std::cout << "SteamStorm: DEBUG: Leaderboard \"" << name << "\" setting score " << score << std::endl;
  #endif // DEBUG_STEAMSTORM
  uint64_t leaderboard_id = leaderboards[name];
  if(leaderboard_id == 0) {
    #ifdef DEBUG_STEAMSTORM
      std::cout << "SteamStorm: DEBUG: Leaderboard \"" << name << "\" not loaded, loading now." << std::endl;
    #endif // DEBUG_STEAMSTORM
    leaderboard_id = load_leaderboard(name);
    if(leaderboard_id == 0) {
      std::cout << "SteamStorm: ERROR: Could not load leaderboard \"" << name << "\" to set new score." << std::endl;
      return;
    }
  }
  auto result = call_sync<LeaderboardScoreUploaded_t>(api_userstats->UploadLeaderboardScore(
    leaderboard_id,
    k_ELeaderboardUploadScoreMethodKeepBest,
    score,
    score_details.data(),
    cast_if_required<int>(score_details.size())                                 // what kind of idiot uses a signed "int" for a size type...
  ), 2'000);
  if(!result) {
    std::cout << "SteamStorm: ERROR: No result when uploading to leaderboard \"" << name << "\"." << std::endl;
    return;
  }
  if(result->m_bSuccess != 1) {
    std::cout << "SteamStorm: ERROR: Failed to set score for leaderboard \"" << name << "\"." << std::endl;
    return;
  }
  #ifdef DEBUG_STEAMSTORM
    if(result->m_bScoreChanged == 1) {
      std::cout << "SteamStorm: DEBUG: Leaderboard \"" << name << "\" score updated, global rank moved from " << result->m_nGlobalRankPrevious << " to " << result->m_nGlobalRankNew << "." << std::endl;
    } else {
      std::cout << "SteamStorm: DEBUG: Leaderboard \"" << name << "\" score unchanged, global rank " << result->m_nGlobalRankNew << "." << std::endl;
    }
  #endif // DEBUG_STEAMSTORM
}

bool steamstorm::wait_sync(SteamAPICall_t callback, unsigned int timeout) const {
  /// Wait for the specified callback to complete synchronously, or time out - return success or failure
  #ifdef DEBUG_STEAMSTORM_CALLBACKS
    std::cout << "SteamStorm: DEBUG: Waiting synchronously for callback to complete, timeout " << timeout << "ms" << std::endl;
  #endif // DEBUG_STEAMSTORM_CALLBACKS
  // block synchronously until we've got a completion or we hit timeout
  std::chrono::time_point<std::chrono::high_resolution_clock, std::chrono::duration<double>> time_next_check(std::chrono::high_resolution_clock::now());
  unsigned int constexpr const timestep_ms = 50;                                // milliseconds, can tweak this for best performance
  auto constexpr const timestep = std::chrono::milliseconds(timestep_ms);
  bool got_result = false;
  for(unsigned int i = 0; i < timeout; i += timestep_ms) {
    bool callback_failed = false;
    if(api_utils->IsAPICallCompleted(callback, &callback_failed)) {
      #ifdef DEBUG_STEAMSTORM_CALLBACKS
        std::cout << "SteamStorm: DEBUG: API call completed after " << i << "ms" << std::endl;
      #endif // DEBUG_STEAMSTORM_CALLBACKS
      if(callback_failed) {
        std::cout << "SteamStorm: ERROR: API call failed: " << get_last_api_failure(callback) << std::endl;
      } else {
        got_result = true;
      }
      break;
    }
    std::this_thread::sleep_until(time_next_check);
    time_next_check = std::chrono::high_resolution_clock::now() + timestep;
  }
  if(!got_result) {
    std::cout << "SteamStorm: ERROR: Synchronous API wait timed out after " << timeout << "ms" << std::endl;
    return false;
  }
  return true;
}

bool steamstorm::get_api_call_result(SteamAPICall_t callback,
                                     void *result,
                                     int result_size,
                                     int result_type,
                                     bool &callback_failed) const {
  /// Helper function to forward a SteamUtils->GetAPICallResult call, without having to include the steam header wherever it's used
  return api_utils->GetAPICallResult(callback, result, result_size, result_type, &callback_failed);
}

std::string steamstorm::decode_result(EResult result) const {
  /// Look up a Steam API result code and return a string describing it
  switch(result) {
  case k_EResultOK:
    return "success";
  case k_EResultFail:
    return "generic failure";
  case k_EResultNoConnection:
    return "no/failed network connection";
  //case k_EResultNoConnectionRetry:
  //  return "OBSOLETE - removed";
  case k_EResultInvalidPassword:
    return "password/ticket is invalid";
  case k_EResultLoggedInElsewhere:
    return "same user logged in elsewhere";
  case k_EResultInvalidProtocolVer:
    return "protocol version is incorrect";
  case k_EResultInvalidParam:
    return "a parameter is incorrect";
  case k_EResultFileNotFound:
    return "file was not found";
  case k_EResultBusy:
    return "called method busy - action not taken";
  case k_EResultInvalidState:
    return "called object was in an invalid state";
  case k_EResultInvalidName:
    return "name is invalid";
  case k_EResultInvalidEmail:
    return "email is invalid";
  case k_EResultDuplicateName:
    return "name is not unique";
  case k_EResultAccessDenied:
    return "access is denied";
  case k_EResultTimeout:
    return "operation timed out";
  case k_EResultBanned:
    return "VAC2 banned";
  case k_EResultAccountNotFound:
    return "account not found";
  case k_EResultInvalidSteamID:
    return "steamID is invalid";
  case k_EResultServiceUnavailable:
    return "The requested service is currently unavailable";
  case k_EResultNotLoggedOn:
    return "The user is not logged on";
  case k_EResultPending:
    return "Request is pending (may be in process, or waiting on third party)";
  case k_EResultEncryptionFailure:
    return "Encryption or Decryption failed";
  case k_EResultInsufficientPrivilege:
    return "Insufficient privilege";
  case k_EResultLimitExceeded:
    return "Too much of a good thing";
  case k_EResultRevoked:
    return "Access has been revoked (used for revoked guest passes)";
  case k_EResultExpired:
    return "License/Guest pass the user is trying to access is expired";
  case k_EResultAlreadyRedeemed:
    return "Guest pass has already been redeemed by account, cannot be acked again";
  case k_EResultDuplicateRequest:
    return "The request is a duplicate and the action has already occurred in the past, ignored this time";
  case k_EResultAlreadyOwned:
    return "All the games in this guest pass redemption request are already owned by the user";
  case k_EResultIPNotFound:
    return "IP address not found";
  case k_EResultPersistFailed:
    return "failed to write change to the data store";
  case k_EResultLockingFailed:
    return "failed to acquire access lock for this operation";
  case k_EResultLogonSessionReplaced:
    return "LogonSessionReplaced";
  case k_EResultConnectFailed:
    return "ConnectFailed";
  case k_EResultHandshakeFailed:
    return "HandshakeFailed";
  case k_EResultIOFailure:
    return "IOFailure";
  case k_EResultRemoteDisconnect:
    return "RemoteDisconnect";
  case k_EResultShoppingCartNotFound:
    return "failed to find the shopping cart requested";
  case k_EResultBlocked:
    return "a user didn't allow it";
  case k_EResultIgnored:
    return "target is ignoring sender";
  case k_EResultNoMatch:
    return "nothing matching the request found";
  case k_EResultAccountDisabled:
    return "AccountDisabled";
  case k_EResultServiceReadOnly:
    return "this service is not accepting content changes right now";
  case k_EResultAccountNotFeatured:
    return "account doesn't have value, so this feature isn't available";
  case k_EResultAdministratorOK:
    return "allowed to take this action, but only because requester is admin";
  case k_EResultContentVersion:
    return "A Version mismatch in content transmitted within the Steam protocol.";
  case k_EResultTryAnotherCM:
    return "The current CM can't service the user making a request, user should try another.";
  case k_EResultPasswordRequiredToKickSession:
    return "You are already logged in elsewhere, this cached credential login has failed.";
  case k_EResultAlreadyLoggedInElsewhere:
    return "You are already logged in elsewhere, you must wait";
  case k_EResultSuspended:
    return "Long running operation (content download) suspended/paused";
  case k_EResultCancelled:
    return "Operation canceled (typically by user: content download)";
  case k_EResultDataCorruption:
    return "Operation canceled because data is ill formed or unrecoverable";
  case k_EResultDiskFull:
    return "Operation canceled - not enough disk space.";
  case k_EResultRemoteCallFailed:
    return "an remote call or IPC call failed";
  case k_EResultPasswordUnset:
    return "Password could not be verified as it's unset server side";
  case k_EResultExternalAccountUnlinked:
    return "External account (PSN, Facebook...) is not linked to a Steam account";
  case k_EResultPSNTicketInvalid:
    return "PSN ticket was invalid";
  case k_EResultExternalAccountAlreadyLinked:
    return "External account (PSN, Facebook...) is already linked to some other account, must explicitly request to replace/delete the link first";
  case k_EResultRemoteFileConflict:
    return "The sync cannot resume due to a conflict between the local and remote files";
  case k_EResultIllegalPassword:
    return "The requested new password is not legal";
  case k_EResultSameAsPreviousValue:
    return "new value is the same as the old one (secret question and answer)";
  case k_EResultAccountLogonDenied:
    return "account login denied due to 2nd factor authentication failure";
  case k_EResultCannotUseOldPassword:
    return "The requested new password is not legal";
  case k_EResultInvalidLoginAuthCode:
    return "account login denied due to auth code invalid";
  case k_EResultAccountLogonDeniedNoMail:
    return "account login denied due to 2nd factor auth failure - and no mail has been sent";
  case k_EResultHardwareNotCapableOfIPT:
    return "HardwareNotCapableOfIPT";
  case k_EResultIPTInitError:
    return "IPTInitError";
  case k_EResultParentalControlRestricted:
    return "operation failed due to parental control restrictions for current user";
  case k_EResultFacebookQueryError:
    return "Facebook query returned an error";
  case k_EResultExpiredLoginAuthCode:
    return "account login denied due to auth code expired";
  case k_EResultIPLoginRestrictionFailed:
    return "IPLoginRestrictionFailed";
  case k_EResultAccountLockedDown:
    return "AccountLockedDown";
  case k_EResultAccountLogonDeniedVerifiedEmailRequired:
    return "AccountLogonDeniedVerifiedEmailRequired";
  case k_EResultNoMatchingURL:
    return "NoMatchingURL";
  case k_EResultBadResponse:
    return "parse failure, missing field, etc.";
  case k_EResultRequirePasswordReEntry:
    return "The user cannot complete the action until they re-enter their password";
  case k_EResultValueOutOfRange:
    return "The value entered is outside the acceptable range";
  case k_EResultUnexpectedError:
    return "Something happened that we didn't expect to ever happen";
  case k_EResultDisabled:
    return "The requested service has been configured to be unavailable";
  case k_EResultInvalidCEGSubmission:
    return "The set of files submitted to the CEG server are not valid !";
  case k_EResultRestrictedDevice:
    return "The device being used is not allowed to perform this action";
  case k_EResultRegionLocked:
    return "The action could not be complete because it is region restricted";
  case k_EResultRateLimitExceeded:
    return "Temporary rate limit exceeded, try again later, different from k_EResultLimitExceeded which may be permanent";
  case k_EResultAccountLoginDeniedNeedTwoFactor:
    return "Need two-factor code to login";
  case k_EResultItemDeleted:
    return "The thing we're trying to access has been deleted";
  case k_EResultAccountLoginDeniedThrottle:
    return "Login attempt failed, try to throttle response to possible attacker";
  case k_EResultTwoFactorCodeMismatch:
    return "Two factor code mismatch";
  case k_EResultTwoFactorActivationCodeMismatch:
    return "Activation code for two-factor didn't match";
  #if STEAM_API_VERSION >= 132
    case k_EResultAccountAssociatedToMultiplePartners:
      return "Account has been associated with multiple partners";
    case k_EResultNotModified:
      return "Data not modified";
  #endif // STEAM_API_VERSION >= 132
  #if STEAM_API_VERSION >= 133
    case k_EResultNoMobileDevice:
      return "The account does not have a mobile device associated with it";
    case k_EResultTimeNotSynced:
      return "The time presented is out of range or tolerance";
    case k_EResultSmsCodeFailed:
      return "SMS code failure (no match, none pending, etc.)";
    case k_EResultAccountLimitExceeded:
      return "Too many accounts access this resource";
    case k_EResultAccountActivityLimitExceeded:
      return "Too many changes to this account";
    case k_EResultPhoneActivityLimitExceeded:
      return "Too many changes to this phone";
  #endif // STEAM_API_VERSION >= 133
  #if STEAM_API_VERSION >= 134
    case k_EResultRefundToWallet:
      return "Cannot refund to payment method, must use wallet";
    case k_EResultEmailSendFailure:
      return "Cannot send an email";
    case k_EResultNotSettled:
      return "Can't perform operation till payment has settled";
  #endif // STEAM_API_VERSION >= 134
  #if STEAM_API_VERSION >= 135
    case k_EResultNeedCaptcha:
      return "Needs to provide a valid captcha";
  #endif // STEAM_API_VERSION >= 135
  #if STEAM_API_VERSION >= 136
    case k_EResultGSLTDenied:
      return "A game server login token owned by this token's owner has been banned";
    case k_EResultGSOwnerDenied:
      return "Game server owner is denied for other reason (account lock, community ban, vac ban, missing phone)";
    case k_EResultInvalidItemType:
      return "The type of thing we were requested to act on is invalid";
  #endif // STEAM_API_VERSION >= 136
  #if STEAM_API_VERSION >= 137
    case k_EResultIPBanned:
      return "The ip address has been banned from taking this action";
  #endif // STEAM_API_VERSION >= 137
  default:
    return "Unknown result";
  }
}
