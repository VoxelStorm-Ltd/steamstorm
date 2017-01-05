    //void callback_stats(UserStatsReceived_t *pParam) __attribute__((__stdcall__));
  #endif // STEAM
};

#endif // STEAMSTORM_H_INCLUDED
#ifndef STEAMSTORM_H_INCLUDED
#define STEAMSTORM_H_INCLUDED

#include <string>
#ifdef STEAM
  #include <atomic>
  #pragma GCC diagnostic push
  #pragma GCC diagnostic ignored "-Wnon-virtual-dtor"
  #pragma GCC diagnostic ignored "-Wunknown-pragmas"
  #pragma GCC diagnostic ignored "-Wignored-qualifiers"
  #ifdef _WIN32
    // Opensteamworks says: "This ugly hack allows us to provide GCC compatibility on windows without much effort"
    #pragma push_macro("virtual")
    #undef virtual
    #define virtual virtual __thiscall
  #endif // _WIN32
  #include <steam/steam_api.h>
  #ifdef _WIN32
    #pragma pop_macro("virtual")
  #endif // _WIN32
  /*
  #include "opensteamworks/SteamAPI.h"
  #include "opensteamworks/CCallback.h"
  #include "opensteamworks/RemoteStorageCommon.h"
  #include "opensteamworks/UserStatsCommon.h"
  #include "opensteamworks/SteamClient.h"
  */
  #pragma GCC diagnostic pop
  #define CONST_IF_NO_STEAM
#else
  #define CONST_IF_NO_STEAM __attribute__((__const__))
#endif // STEAM

/// Define STEAM to enable steam support; without it all functionality here is noop.
/// If STEAM is defined, the relevant dynamic libraries have to be available at runtime.

class steamstorm {
private:
  #ifdef STEAM
    CSteamID internal_user_id;

    std::atomic_bool stats_ready{false};                                        // flag for whether stats are ready
  #endif // STEAM

public:
  bool initialised = false;                                                     // whether our steam connection is operational
  uint32_t appid = 0;                                                           // what Steam tells us this app's ID is
  std::string username;                                                         // the currently logged in steam user's name

  steamstorm();
  ~steamstorm();

  void initialise();
  void shutdown() CONST_IF_NO_STEAM;

  // queries
  bool              get_user_has_app(unsigned int const appid) const CONST_IF_NO_STEAM;
  bool              get_achievement(            std::string const &achievementname) const CONST_IF_NO_STEAM;
  std::string const get_achievement_name(       std::string const &achievementname) const CONST_IF_NO_STEAM;
  std::string const get_achievement_description(std::string const &achievementname) const CONST_IF_NO_STEAM;
  bool              get_achievement_hidden(     std::string const &achievementname) const CONST_IF_NO_STEAM;
  template <typename T> T get_statistic(        std::string const &statname)        const CONST_IF_NO_STEAM;

  // updating stats and achievements
  void set_achievement(std::string const &achievementname) const CONST_IF_NO_STEAM;
  template <typename T> void set_statistic(std::string const &statname, T const statvalue) const CONST_IF_NO_STEAM;
  void store() const CONST_IF_NO_STEAM;

private:
  #ifdef STEAM
    std::string const decode_result(EResult result) const;

    STEAM_CALLBACK(steamstorm, callback_stats, UserStatsReceived_t, internal_callback_stats);
    // manually defined version of the above to allow specification of calling convention:
    //#define STEAM_CALLBACK(thisclass, func, param, var) CCallback< thisclass, param, false > var; void func(param *pParam)
    //CCallback<steamstorm, UserStatsReceived_t, false> internal_callback_stats;
    //void callback_stats(UserStatsReceived_t *pParam);
    //void __stdcall callback_stats(UserStatsReceived_t *pParam);
    //void callback_stats(UserStatsReceived_t *pParam) __attribute__((__stdcall__));
  #endif // STEAM
};

#endif // STEAMSTORM_H_INCLUDED
