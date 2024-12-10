#pragma once

/// Steam API support library
///   Wrapping of commonly used Steam functions
///   Dynamic loading of the Steam dynamic lib at runtime, with graceful fallback
///
/// Defines:
///   DEBUG_STEAMSTORM - extra debugging information
///   DEBUG_STEAMSTORM_ACHIEVEMENTS - load and enumerate achievements on startup
///   DEBUG_STEAMSTORM_CALLBACKS - extended debugging information about callbacks

#include <string>
#include <iostream>
#include <chrono>
#include <vector>
#include <unordered_map>
#include <optional>

// our own declaration of the steam api version, because the api doesn't export one
#define STEAM_API_VERSION 140

// Steam API export macro
#if defined( _WIN32 ) && !defined( _X360 )
  #if defined( STEAM_API_EXPORTS )
    #define S_API extern "C" __declspec( dllexport )
  #elif defined( STEAM_API_NODLL )
    #define S_API extern "C"
  #else
    #define S_API extern "C" __declspec( dllimport )
  #endif // STEAM_API_EXPORTS
#elif defined( GNUC )
  #if defined( STEAM_API_EXPORTS )
    #define S_API extern "C" __attribute__ ((visibility("default")))
  #else
    #define S_API extern "C"
  #endif // STEAM_API_EXPORTS
#else // !WIN32
  #define S_API extern "C"
#endif

#if defined( POSIX ) && !defined(__MINGW32__)
  // don't try to use cdecl for non-windows targets
  #define __cdecl
#endif

#include <steam/steamclientpublic.h>

// steamworks forward declarations
class ISteamClient;
class ISteamUtils;
class ISteamUser;
class ISteamApps;
class ISteamFriends;
class ISteamUserStats;
//class ISteamGameServer;
//class ISteamMatchmaking;
//class ISteamContentServer;
//class ISteamMatchmakingServers;
//class ISteamNetworking;
//class ISteamRemoteStorage;
//class ISteamScreenshots;
//class ISteamMusic;
//class ISteamMusicRemote;
//class ISteamGameServerStats;
//class ISteamPS3OverlayRender;
//class ISteamHTTP;
//class ISteamUnifiedMessages;
//class ISteamController;
//class ISteamUGC;
//class ISteamAppList;
//class ISteamHTMLSurface;
//class ISteamInventory;
//class ISteamVideo;

// duplicates of Steam api function definitions, for dynamic loading reference
using HSteamPipe = int32_t;
using HSteamUser = int32_t;

class steamstorm {
private:
  void *lib = nullptr;                                                          // pointer to the dynamically loaded steam library

  std::unordered_map<std::string, uint64_t> leaderboards;                       // SteamLeaderboard_t is uint64_t here

public:
  HSteamUser client_user = 0;                                                   // the steam client user id
  HSteamPipe client_pipe = 0;                                                   // the steam client pipe id

  // versioned steam api module pointers
  ISteamClient    *api_client    = nullptr;
  ISteamUtils     *api_utils     = nullptr;
  ISteamUser      *api_user      = nullptr;
  ISteamApps      *api_apps      = nullptr;
  ISteamFriends   *api_friends   = nullptr;
  ISteamUserStats *api_userstats = nullptr;

  // flat api cached int pointer conversions of the above objects
  intptr_t api_flat_client    = 0;
  intptr_t api_flat_utils     = 0;
  intptr_t api_flat_user      = 0;
  intptr_t api_flat_apps      = 0;
  intptr_t api_flat_friends   = 0;
  intptr_t api_flat_userstats = 0;

  CSteamID api_user_id;                                                         // our own generated steam user - we can't fetch this, so we make one locally

  bool enabled = false;                                                         // whether our steam connection is operational
  uint32_t appid = 0;                                                           // what Steam tells us this app's ID is
  uint64_t user_id = 0;                                                         // internal Steam user id
  std::string username;                                                         // the currently logged in steam user's name

  steamstorm(uint32_t appid = 0);
  ~steamstorm();

  void init();
  void shutdown();

  // queries
  bool        get_user_has_app() const;
  bool        get_user_has_app(unsigned int const appid) const;
  bool        get_dlc_installed(unsigned int const appid) const;
  bool        get_achievement(            std::string const &achievementname) const;
  std::string get_achievement_name(       std::string const &achievementname) const;
  std::string get_achievement_description(std::string const &achievementname) const;
  bool        get_achievement_hidden(     std::string const &achievementname) const;
  int         get_build() const;
  std::string get_store_url() const;
  std::string get_review_url() const;
  bool        get_overlay_enabled() const;
  std::string get_language_code() const;

  std::string get_last_api_failure(SteamAPICall_t callback) const;

  // actions
  void open_url_in_overlay(std::string const &url) const;
  void open_store_in_overlay(uint32_t appid = 0) const;
  void open_store_in_overlay_and_add_to_basket(uint32_t appid = 0) const;
  void open_store_in_overlay_and_add_to_basket_and_show(uint32_t appid = 0) const;

  // updating stats and achievements
  void set_achievement(std::string const &achievementname) const;
  template<typename T> T    get_stat(std::string const &statname) const;
  template<typename T> void set_stat(std::string const &statname, T statvalue) const;
  template<typename T> void add_stat(std::string const &statname, T statvalue) const;
  template<typename T> void sub_stat(std::string const &statname, T statvalue) const;
  void inc_stat(std::string const &statname) const;
  void dec_stat(std::string const &statname) const;
  void store() const;

  // leaderboards
  uint64_t load_leaderboard(std::string const &name);
  void set_leaderboard_score(std::string const &name, int32_t score, std::vector<int32_t> const &score_details = {});

  // callback helpers
  template<typename T>
  std::optional<T> call_sync(SteamAPICall_t callback, unsigned int timeout = 2'000) const;
  bool wait_sync(SteamAPICall_t callback, unsigned int timeout = 2'000) const;

private:
  bool get_api_call_result(SteamAPICall_t callback, void *result, int result_size, int result_type, bool &callback_failed) const;

  std::string decode_result(EResult result) const;
};

template<typename T>
std::optional<T> steamstorm::call_sync(SteamAPICall_t callback, unsigned int timeout) const {
  /// Execute a callback synchronously and return the result, if it's available
  if(!enabled) {
    return {};                                                                  // exit silently if the api isn't initialised
  }
  #ifdef DEBUG_STEAMSTORM_CALLBACKS
    std::cout << "SteamStorm: DEBUG: Polling callback synchronously, timeout " << timeout << "ms" << std::endl;
  #endif // DEBUG_STEAMSTORM_CALLBACKS
  if(!wait_sync(callback, timeout)) {                                           // block synchronously until we've got the stats
    #ifdef DEBUG_STEAMSTORM_CALLBACKS
      std::cout << "SteamStorm: DEBUG: Synchronous API call failed at the wait stage." << std::endl;
    #endif // DEBUG_STEAMSTORM_CALLBACKS
    return {};
  }

  T result;
  bool callback_failed = false;
  bool callback_result_userstats = get_api_call_result(callback, &result, sizeof(result), result.k_iCallback, callback_failed);
  if(!callback_result_userstats) {
    return {};
  }
  if(callback_failed) {
    std::cout << "SteamStorm: ERROR: Synchronous API call failed: " << get_last_api_failure(callback) << std::endl;
    return {};
  }
  return result;
}
