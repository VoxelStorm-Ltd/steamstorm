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
    #endif // DEBUG_STEAMSTORM

    #ifdef DEBUG_STEAMSTORM
      std::cout << "SteamStorm: DEBUG: Querying Steam for the latest stats..." << std::endl;
    #endif // DEBUG_STEAMSTORM
    ///std::cout << "SteamStorm: DEBUG 1: Requesting stat funds: " << get_statistic<float>("FUNDS") << std::endl;
    ///std::cout << "SteamStorm: DEBUG 1: Requesting hidden state of TAKEOVER: " << SteamUserStats()->GetAchievementDisplayAttribute("TAKEOVER", "hidden") << std::endl;
    if(!SteamUserStats()->RequestCurrentStats()) {                              // request an update on the current stats
#include "steamstorm.h"
      std::cout << "SteamStorm: ERROR: Failed to request user stats from Steam API, disabling Steam support this run." << std::endl;
#include <iostream>
      return;
#ifdef STEAM
    }
  #include <chrono>

  #include <thread>
    /**
#endif // STEAM
    // block synchronously until we've got the stats

    std::chrono::time_point<std::chrono::high_resolution_clock, std::chrono::duration<double>> time_next_check(std::chrono::high_resolution_clock::now());
steamstorm::steamstorm()
    unsigned int constexpr stats_timeout = 2000;  // milliseconds
  #ifdef STEAM
    unsigned int constexpr stats_timestep = 50;   // milliseconds, can tweak this for best performance
    //: internal_callback_stats(this, &steamstorm::callback_stats)
    for(unsigned int i = 0; i += stats_timestep; stats_ready == false) {
    // we need to initialise it as NULL because the constructor gets upset with our __stdcall modification, and register manually after
      std::cout << "SteamStorm: DEBUG: running callbacks..." << std::endl;
    : internal_callback_stats(NULL, NULL)
  #endif // STEAM
      SteamAPI_RunCallbacks();                                                  // run the callbacks to process any incoming data
  {
      std::this_thread::sleep_until(time_next_check);
  /// Default constructor
      time_next_check = std::chrono::high_resolution_clock::now() + std::chrono::milliseconds(stats_timestep);
  //internal_callback_stats.Register(this, &steamstorm::callback_stats);
      if(i > stats_timeout) {
  initialise();
        std::cout << "SteamStorm: ERROR: Could not retrieve user stats from Steam API, disabling Steam support this run." << std::endl;
}
        return;

      }
steamstorm::~steamstorm() {
    }
  /// Default destructor
    **/
  shutdown();

}

    ///std::cout << "SteamStorm: DEBUG 2: Requesting stat funds: " << get_statistic<float>("FUNDS") << std::endl;
void steamstorm::initialise() {
    ///std::cout << "SteamStorm: DEBUG 2: Requesting hidden state of TAKEOVER: " << SteamUserStats()->GetAchievementDisplayAttribute("TAKEOVER", "hidden") << std::endl;
  /// Attempt to initialise everything
    //std::this_thread::sleep_for(std::chrono::milliseconds(500));                // wait to make sure we have the data we need (nasty hack!)
  initialised = false;
    //SteamAPI_RunCallbacks();                                                    // run the callbacks once to process any incoming data
  #ifdef STEAM
    ///std::cout << "SteamStorm: DEBUG 3: Requesting stat funds int:   " << get_statistic<int>("FUNDS") << std::endl;
    std::cout << "SteamStorm: Initialising..." << std::endl;
    ///std::cout << "SteamStorm: DEBUG 3: Requesting stat funds float: " << get_statistic<float>("FUNDS") << std::endl;
    if(!SteamAPI_Init()) {
    ///std::cout << "SteamStorm: DEBUG 3: Requesting stat hired int:   " << get_statistic<int>("HIRED") << std::endl;
      /// A return of false indicates one of three conditions:
    ///std::cout << "SteamStorm: DEBUG 3: Requesting stat hired float: " << get_statistic<float>("HIRED") << std::endl;
      ///   The Steam client isn't running. A running Steam client is required to provide implementations of the various Steamworks interfaces.
    ///std::cout << "SteamStorm: DEBUG 3: Requesting hidden state of TAKEOVER: " << SteamUserStats()->GetAchievementDisplayAttribute("TAKEOVER", "hidden") << std::endl;
      ///   The Steam client couldn't determine the AppID of game. Make sure you have Steam_appid.txt in your game directory.
    ///set_statistic("FUNDS", 12.345f);
      ///   Your application is not running under the same user context as the Steam client, including admin privileges.
    ///set_statistic("HIRED", 1);
      std::cout << "SteamStorm: Could not initialise Steam API - disabling Steam support for this run." << std::endl;

      return;
    initialised = true;
    }
    std::cout << "SteamStorm: Initialised and ready." << std::endl;
    // register callbacks
  #else // STEAM
    internal_callback_stats.Register(this, &steamstorm::callback_stats);

    std::cout << "SteamStorm is disabled - no Steam support in this build." << std::endl;
    ///std::cout << "SteamStorm: DEBUG 0: Requesting hidden state of TAKEOVER: " << SteamUserStats()->GetAchievementDisplayAttribute("TAKEOVER", "hidden") << std::endl;
  #endif // STEAM
    if(!SteamUser()->BLoggedOn()) {
}
      std::cout << "SteamStorm: Steam is running but user is not logged in, disabling Steam support for this run." << std::endl;

      return;
void steamstorm::shutdown() {
    }
  /// Shut down cleanly
    appid = SteamUtils()->GetAppID();
  #ifdef STEAM
    //internal_user_id = SteamUser()->GetSteamID();                               // cache the user's Steam ID
    std::cout << "SteamStorm: Shutting down." << std::endl;
    // the above returns a struct and as such breaks due to calling conventions
    SteamAPI_Shutdown();
    username = SteamFriends()->GetPersonaName();
  #endif // STEAM
    // various debugging checks that we don't want to bother with in release
}
    #ifdef DEBUG_STEAMSTORM

      std::cout << "SteamStorm: DEBUG: Steam app ID:     " << appid << std::endl;
#ifdef STEAM
      std::cout << "SteamStorm: DEBUG: Steam username:   " << username << std::endl;
  // steam versions of getter functions (splitting it this way saves lots of space)
      //std::cout << "SteamStorm: DEBUG: Steam ID:         " << internal_user_id.ConvertToUint64() << std::endl;
  bool steamstorm::get_user_has_app(unsigned int const appid_to_check __attribute__((__unused__))) const {
      std::cout << "SteamStorm: DEBUG: Steam level:      " << SteamUser()->GetPlayerSteamLevel() << std::endl;
    /// Find out whether a user has the app ID specified - useful for checking DLC etc
      std::cout << "SteamStorm: DEBUG: Badge level:      " << SteamUser()->GetGameBadgeLevel(1, false) << std::endl;
    //return (SteamUser()->UserHasLicenseForApp(internal_user_id, appid_to_check) == EUserHasLicenseForAppResult::k_EUserHasLicenseResultHasLicense);
      std::cout << "SteamStorm: DEBUG: Foil Badge level: " << SteamUser()->GetGameBadgeLevel(1, true)  << std::endl;
    // this requires passing a struct, and as such fails due to the calling convention
      std::cout << "SteamStorm: DEBUG: Friends:          " << SteamFriends()->GetFriendCount(EFriendFlags::k_EFriendFlagNone) << std::endl;
    return false;
      //std::cout << "SteamStorm: DEBUG: TEST: advertcity     365800 " << get_user_has_app(365800);
  }
      //std::cout << "SteamStorm: DEBUG: TEST: advertcity ost 366130 " << get_user_has_app(366130);

      //std::cout << "SteamStorm: DEBUG: TEST: some random    274940 " << get_user_has_app(274940);
  bool steamstorm::get_achievement(std::string const &achievementname) const {
      //if(internal_user_id.BAnonAccount()) {
    /// Find out whether the specified achievement has been set
      //  std::cout << "SteamStorm: DEBUG: This is an anonymous account" << std::endl;
    bool alreadyachieved = false;
      //}
    uint32_t achievementtime = 0;
      if(SteamUser()->BIsBehindNAT()) {
    SteamUserStats()->GetAchievementAndUnlockTime(achievementname.c_str(), &alreadyachieved, &achievementtime);
        std::cout << "SteamStorm: DEBUG: User appears to be behind NAT" << std::endl;
    #ifdef DEBUG_STEAMSTORM
      }
      if(alreadyachieved) {
      switch(SteamFriends()->GetPersonaState()) {
      case EPersonaState::k_EPersonaStateOffline:
        std::cout << "SteamStorm: DEBUG: Achievement \"" << achievementname << "\" unlocked at " << achievementtime << "." << std::endl;
        std::cout << "SteamStorm: DEBUG: User not currently logged on" << std::endl;
      } else {
        break;
        std::cout << "SteamStorm: DEBUG: Achievement \"" << achievementname << "\" not yet unlocked." << std::endl;
      case EPersonaState::k_EPersonaStateOnline:
      }
        std::cout << "SteamStorm: DEBUG: User is logged on" << std::endl;
    #endif // DEBUG_STEAMSTORM
        break;
    return alreadyachieved;
      case EPersonaState::k_EPersonaStateBusy:
  }
        std::cout << "SteamStorm: DEBUG: User is on, but busy" << std::endl;

        break;
  std::string const steamstorm::get_achievement_name(std::string const &achievementname) const {
      case EPersonaState::k_EPersonaStateAway:
    /// Find out the full name of the specified achievement
        std::cout << "SteamStorm: DEBUG: User is auto-away" << std::endl;
    return SteamUserStats()->GetAchievementDisplayAttribute(achievementname.c_str(), "name");
        break;
  }
      case EPersonaState::k_EPersonaStateSnooze:
  std::string const steamstorm::get_achievement_description(std::string const &achievementname) const {
        std::cout << "SteamStorm: DEBUG: User is auto-away for a long time" << std::endl;
        break;
    /// Find out the full description of the specified achievement
      case EPersonaState::k_EPersonaStateLookingToTrade:
    return SteamUserStats()->GetAchievementDisplayAttribute(achievementname.c_str(), "desc");
        std::cout << "SteamStorm: DEBUG: User is online, trading" << std::endl;
  }
        break;
  bool steamstorm::get_achievement_hidden(std::string const &achievementname) const {
      case EPersonaState::k_EPersonaStateLookingToPlay:
    /// Find out whether the specified achievement is hidden
        std::cout << "SteamStorm: DEBUG: User is online, wanting to play" << std::endl;
    return SteamUserStats()->GetAchievementDisplayAttribute(achievementname.c_str(), "hidden");
        break;
  }
      default:
#else
        break;
  // dummy placeholder functions when steam is disabled
      }
  bool steamstorm::get_user_has_app(unsigned int const appid_to_check __attribute__((__unused__))) const {
    #endif // DEBUG_STEAMSTORM
    return false;

  }
    #ifdef DEBUG_STEAMSTORM
  bool steamstorm::get_achievement(std::string const &achievementname __attribute__((__unused__))) const {
      std::cout << "SteamStorm: DEBUG: Querying Steam for the latest stats..." << std::endl;
    return false;
    #endif // DEBUG_STEAMSTORM
  }
    ///std::cout << "SteamStorm: DEBUG 1: Requesting stat funds: " << get_statistic<float>("FUNDS") << std::endl;
  std::string const steamstorm::get_achievement_name(std::string const &achievementname __attribute__((__unused__))) const {
    ///std::cout << "SteamStorm: DEBUG 1: Requesting hidden state of TAKEOVER: " << SteamUserStats()->GetAchievementDisplayAttribute("TAKEOVER", "hidden") << std::endl;
    return "";
    if(!SteamUserStats()->RequestCurrentStats()) {                              // request an update on the current stats
  }
      std::cout << "SteamStorm: ERROR: Failed to request user stats from Steam API, disabling Steam support this run." << std::endl;
  std::string const steamstorm::get_achievement_description(std::string const &achievementname __attribute__((__unused__))) const {
      return;
    }
    return "";

  }
    /**
  bool steamstorm::get_achievement_hidden(std::string const &achievementname __attribute__((__unused__))) const {
    // block synchronously until we've got the stats
    return false;
    std::chrono::time_point<std::chrono::high_resolution_clock, std::chrono::duration<double>> time_next_check(std::chrono::high_resolution_clock::now());
  }
    unsigned int constexpr stats_timeout = 2000;  // milliseconds
#endif // STEAM
    unsigned int constexpr stats_timestep = 50;   // milliseconds, can tweak this for best performance

    for(unsigned int i = 0; i += stats_timestep; stats_ready == false) {
template <typename T>
      std::cout << "SteamStorm: DEBUG: running callbacks..." << std::endl;
T steamstorm::get_statistic(std::string const &statname
      SteamAPI_RunCallbacks();                                                  // run the callbacks to process any incoming data
                            #ifndef STEAM
      std::this_thread::sleep_until(time_next_check);
                              __attribute__((__unused__))
      time_next_check = std::chrono::high_resolution_clock::now() + std::chrono::milliseconds(stats_timestep);
                            #endif // STEAM
      if(i > stats_timeout) {
                            ) const {
        std::cout << "SteamStorm: ERROR: Could not retrieve user stats from Steam API, disabling Steam support this run." << std::endl;
  /// Attempt to update a statistic to the specified value - integer version
        return;
  #ifdef STEAM
      }
    #ifdef DEBUG_STEAMSTORM
    }
      std::cout << "SteamStorm: DEBUG: Getting statistic \"" << statname << " (" << typeid(T).name() << ")" << std::endl;
    **/
    #endif // DEBUG_STEAMSTORM

    T newstat = 0;
    ///std::cout << "SteamStorm: DEBUG 2: Requesting stat funds: " << get_statistic<float>("FUNDS") << std::endl;
    bool success = SteamUserStats()->GetStat(statname.c_str(), &newstat);
    ///std::cout << "SteamStorm: DEBUG 2: Requesting hidden state of TAKEOVER: " << SteamUserStats()->GetAchievementDisplayAttribute("TAKEOVER", "hidden") << std::endl;
    if(!success) {
    //std::this_thread::sleep_for(std::chrono::milliseconds(500));                // wait to make sure we have the data we need (nasty hack!)
      std::cout << "SteamStorm: ERROR: Could not get statistic \"" << statname << " (" << typeid(T).name() << ")" << std::endl;
    //SteamAPI_RunCallbacks();                                                    // run the callbacks once to process any incoming data
      return 0;
    ///std::cout << "SteamStorm: DEBUG 3: Requesting stat funds int:   " << get_statistic<int>("FUNDS") << std::endl;
    }
    ///std::cout << "SteamStorm: DEBUG 3: Requesting stat funds float: " << get_statistic<float>("FUNDS") << std::endl;
    return newstat;
    ///std::cout << "SteamStorm: DEBUG 3: Requesting stat hired int:   " << get_statistic<int>("HIRED") << std::endl;
  #endif // STEAM
    ///std::cout << "SteamStorm: DEBUG 3: Requesting stat hired float: " << get_statistic<float>("HIRED") << std::endl;
  return 0;
    ///std::cout << "SteamStorm: DEBUG 3: Requesting hidden state of TAKEOVER: " << SteamUserStats()->GetAchievementDisplayAttribute("TAKEOVER", "hidden") << std::endl;
}
    ///set_statistic("FUNDS", 12.345f);
// instantiate the template explicitly for the two permissible types
    ///set_statistic("HIRED", 1);

template int32_t steamstorm::get_statistic<int32_t>(std::string const&) const;
    initialised = true;
template float   steamstorm::get_statistic<float  >(std::string const&) const;
    std::cout << "SteamStorm: Initialised and ready." << std::endl;

  #else // STEAM

    std::cout << "SteamStorm is disabled - no Steam support in this build." << std::endl;
void steamstorm::set_achievement(std::string const &achievementname
  #endif // STEAM
                                 #ifndef STEAM
}
                                   __attribute__((__unused__))

                                 #endif // STEAM
void steamstorm::shutdown() {
  /// Shut down cleanly
                                 ) const {
  #ifdef STEAM
  /// Attempt to set an achievement described by the specified string
    std::cout << "SteamStorm: Shutting down." << std::endl;
  #ifdef STEAM
    SteamAPI_Shutdown();
    if(!initialised) {
  #endif // STEAM
      return;                                                                   // exit silently if the api isn't initialised
}
    }

    #ifdef DEBUG_STEAMSTORM
#ifdef STEAM
      std::string const achievementfullname(get_achievement_name(achievementname));
  // steam versions of getter functions (splitting it this way saves lots of space)
      std::string const achievementfulldesc(get_achievement_description(achievementname));
  bool steamstorm::get_user_has_app(unsigned int const appid_to_check __attribute__((__unused__))) const {
      if(achievementfullname.size() == 0 && achievementfulldesc.size() == 0) {
    /// Find out whether a user has the app ID specified - useful for checking DLC etc
        std::cout << "SteamStorm: ERROR: No such achievement found: \"" << achievementname << "\"" << std::endl;
    //return (SteamUser()->UserHasLicenseForApp(internal_user_id, appid_to_check) == EUserHasLicenseForAppResult::k_EUserHasLicenseResultHasLicense);
        return;
    // this requires passing a struct, and as such fails due to the calling convention
      }
    return false;
  }
      std::cout << "SteamStorm: DEBUG: Setting achievement \"" << achievementname << "\" - " << achievementfullname << ": " << achievementfullname;

      if(get_achievement_hidden(achievementname)) {
  bool steamstorm::get_achievement(std::string const &achievementname) const {
        std::cout << " (hidden)";
    /// Find out whether the specified achievement has been set
      }
    bool alreadyachieved = false;
      std::cout << std::endl;
    uint32_t achievementtime = 0;
    #endif // DEBUG_STEAMSTORM
    SteamUserStats()->GetAchievementAndUnlockTime(achievementname.c_str(), &alreadyachieved, &achievementtime);
    if(get_achievement(achievementname)) {
    #ifdef DEBUG_STEAMSTORM
      return;
      if(alreadyachieved) {
    }
        std::cout << "SteamStorm: DEBUG: Achievement \"" << achievementname << "\" unlocked at " << achievementtime << "." << std::endl;
    bool const result = SteamUserStats()->SetAchievement(achievementname.c_str());
      } else {
    if(!result) {
        std::cout << "SteamStorm: DEBUG: Achievement \"" << achievementname << "\" not yet unlocked." << std::endl;
      /// If the call failed then either Steam is not initialized or you still haven't processed the callback from the inital call to RequestStats().
      }
      /// You can't set any achievements until that callback has been received.
    #endif // DEBUG_STEAMSTORM
      std::cout << "SteamStorm: ERROR: Setting achievement \"" << achievementname << "\" failed." << std::endl;
    return alreadyachieved;
  }
      //initialised = false;

      return;
  std::string const steamstorm::get_achievement_name(std::string const &achievementname) const {
    }
    /// Find out the full name of the specified achievement
    #ifdef DEBUG_STEAMSTORM
    return SteamUserStats()->GetAchievementDisplayAttribute(achievementname.c_str(), "name");
      std::cout << "SteamStorm: DEBUG: Setting achievement \"" << achievementname << "\" succeeded." << std::endl;
  }
    #endif // DEBUG_STEAMSTORM
  std::string const steamstorm::get_achievement_description(std::string const &achievementname) const {
    store();
    /// Find out the full description of the specified achievement
  #endif // STEAM
    return SteamUserStats()->GetAchievementDisplayAttribute(achievementname.c_str(), "desc");
}
  }

  bool steamstorm::get_achievement_hidden(std::string const &achievementname) const {
template <typename T>
    /// Find out whether the specified achievement is hidden
void steamstorm::set_statistic(std::string const &statname
    return SteamUserStats()->GetAchievementDisplayAttribute(achievementname.c_str(), "hidden");
                               #ifndef STEAM
  }
                                 __attribute__((__unused__))
#else
                               #endif // STEAM
  // dummy placeholder functions when steam is disabled
                               , T const statvalue
  bool steamstorm::get_user_has_app(unsigned int const appid_to_check __attribute__((__unused__))) const {
                               #ifndef STEAM
    return false;
  }
                                 __attribute__((__unused__))
  bool steamstorm::get_achievement(std::string const &achievementname __attribute__((__unused__))) const {
                               #endif // STEAM
    return false;
                               ) const {
  }
  /// Attempt to update a statistic to the specified value - integer version
  std::string const steamstorm::get_achievement_name(std::string const &achievementname __attribute__((__unused__))) const {
  #ifdef STEAM
    return "";
    #ifdef DEBUG_STEAMSTORM
  }
      std::cout << "SteamStorm: DEBUG: Setting statistic \"" << statname << "\" to " << statvalue << " (" << typeid(T).name() << ")" << std::endl;
  std::string const steamstorm::get_achievement_description(std::string const &achievementname __attribute__((__unused__))) const {
    #endif // DEBUG_STEAMSTORM
    return "";
    bool success = SteamUserStats()->SetStat(statname.c_str(), statvalue);
  }
    if(!success) {
  bool steamstorm::get_achievement_hidden(std::string const &achievementname __attribute__((__unused__))) const {
      std::cout << "SteamStorm: ERROR: Could not set statistic \"" << statname << "\" to " << statvalue << " (" << typeid(T).name() << ")" << std::endl;
    return false;
      return;
  }
    }
#endif // STEAM

    store();
template <typename T>
  #endif // STEAM
T steamstorm::get_statistic(std::string const &statname
}
                            #ifndef STEAM
// instantiate the template explicitly for the two permissible types
                              __attribute__((__unused__))
template void steamstorm::set_statistic<int32_t>(std::string const&, int32_t const) const;
                            #endif // STEAM
template void steamstorm::set_statistic<float  >(std::string const&, float   const) const;
                            ) const {

  /// Attempt to update a statistic to the specified value - integer version
void steamstorm::store() const {
  #ifdef STEAM
  /// Attempt to save achievements and stats
    #ifdef DEBUG_STEAMSTORM
  #ifdef STEAM
      std::cout << "SteamStorm: DEBUG: Getting statistic \"" << statname << " (" << typeid(T).name() << ")" << std::endl;
    if(!SteamUserStats()->StoreStats()) {                                       // push the new stats to the server
    #endif // DEBUG_STEAMSTORM
      std::cout << "SteamStorm: ERROR: Could not store stats!" << std::endl;
    T newstat = 0;
      return;
    bool success = SteamUserStats()->GetStat(statname.c_str(), &newstat);
    }
    if(!success) {
  #endif // STEAM
      std::cout << "SteamStorm: ERROR: Could not get statistic \"" << statname << " (" << typeid(T).name() << ")" << std::endl;
}
      return 0;

    }
#ifdef STEAM
    return newstat;
  #endif // STEAM
  // Internal functionality only ever useful when built in Steam mode
  return 0;
  std::string const steamstorm::decode_result(EResult result) const {
}
    /// Look up a Steam API result code and return a string describing it
// instantiate the template explicitly for the two permissible types
    switch(result) {
template int32_t steamstorm::get_statistic<int32_t>(std::string const&) const;
    case k_EResultOK:
template float   steamstorm::get_statistic<float  >(std::string const&) const;
      return "success";

    case k_EResultFail:

      return "generic failure";
void steamstorm::set_achievement(std::string const &achievementname
    case k_EResultNoConnection:
                                 #ifndef STEAM
      return "no/failed network connection";
                                   __attribute__((__unused__))
    //case k_EResultNoConnectionRetry:
                                 #endif // STEAM
    //  return "OBSOLETE - removed";
                                 ) const {
    case k_EResultInvalidPassword:
  /// Attempt to set an achievement described by the specified string
      return "password/ticket is invalid";
  #ifdef STEAM
    case k_EResultLoggedInElsewhere:
    if(!initialised) {
      return "same user logged in elsewhere";
      return;                                                                   // exit silently if the api isn't initialised
    case k_EResultInvalidProtocolVer:
    }
      return "protocol version is incorrect";
    #ifdef DEBUG_STEAMSTORM
    case k_EResultInvalidParam:
      std::string const achievementfullname(get_achievement_name(achievementname));
      return "a parameter is incorrect";
      std::string const achievementfulldesc(get_achievement_description(achievementname));
    case k_EResultFileNotFound:
      if(achievementfullname.size() == 0 && achievementfulldesc.size() == 0) {
      return "file was not found";
        std::cout << "SteamStorm: ERROR: No such achievement found: \"" << achievementname << "\"" << std::endl;
    case k_EResultBusy:
        return;
      }
      return "called method busy - action not taken";
      std::cout << "SteamStorm: DEBUG: Setting achievement \"" << achievementname << "\" - " << achievementfullname << ": " << achievementfullname;
    case k_EResultInvalidState:
      if(get_achievement_hidden(achievementname)) {
      return "called object was in an invalid state";
        std::cout << " (hidden)";
    case k_EResultInvalidName:
      }
      return "name is invalid";
      std::cout << std::endl;
    case k_EResultInvalidEmail:
    #endif // DEBUG_STEAMSTORM
      return "email is invalid";
    if(get_achievement(achievementname)) {
    case k_EResultDuplicateName:
      return;
      return "name is not unique";
    }
    case k_EResultAccessDenied:
    bool const result = SteamUserStats()->SetAchievement(achievementname.c_str());
      return "access is denied";
    if(!result) {
    case k_EResultTimeout:
      /// If the call failed then either Steam is not initialized or you still haven't processed the callback from the inital call to RequestStats().
      return "operation timed out";
      /// You can't set any achievements until that callback has been received.
    case k_EResultBanned:
      std::cout << "SteamStorm: ERROR: Setting achievement \"" << achievementname << "\" failed." << std::endl;
      return "VAC2 banned";
      //initialised = false;
    case k_EResultAccountNotFound:
      return;
      return "account not found";
    }
    case k_EResultInvalidSteamID:
    #ifdef DEBUG_STEAMSTORM
      return "steamID is invalid";
      std::cout << "SteamStorm: DEBUG: Setting achievement \"" << achievementname << "\" succeeded." << std::endl;
    case k_EResultServiceUnavailable:
    #endif // DEBUG_STEAMSTORM
    store();
      return "The requested service is currently unavailable";
  #endif // STEAM
    case k_EResultNotLoggedOn:
}
      return "The user is not logged on";

    case k_EResultPending:
template <typename T>
      return "Request is pending (may be in process, or waiting on third party)";
void steamstorm::set_statistic(std::string const &statname
    case k_EResultEncryptionFailure:
                               #ifndef STEAM
      return "Encryption or Decryption failed";
                                 __attribute__((__unused__))
    case k_EResultInsufficientPrivilege:
                               #endif // STEAM
      return "Insufficient privilege";
                               , T const statvalue
    case k_EResultLimitExceeded:
                               #ifndef STEAM
      return "Too much of a good thing";
                                 __attribute__((__unused__))
    case k_EResultRevoked:
                               #endif // STEAM
                               ) const {
      return "Access has been revoked (used for revoked guest passes)";
  /// Attempt to update a statistic to the specified value - integer version
    case k_EResultExpired:
  #ifdef STEAM
      return "License/Guest pass the user is trying to access is expired";
    #ifdef DEBUG_STEAMSTORM
    case k_EResultAlreadyRedeemed:
      std::cout << "SteamStorm: DEBUG: Setting statistic \"" << statname << "\" to " << statvalue << " (" << typeid(T).name() << ")" << std::endl;
      return "Guest pass has already been redeemed by account, cannot be acked again";
    #endif // DEBUG_STEAMSTORM
    case k_EResultDuplicateRequest:
    bool success = SteamUserStats()->SetStat(statname.c_str(), statvalue);
      return "The request is a duplicate and the action has already occurred in the past, ignored this time";
    if(!success) {
    case k_EResultAlreadyOwned:
      std::cout << "SteamStorm: ERROR: Could not set statistic \"" << statname << "\" to " << statvalue << " (" << typeid(T).name() << ")" << std::endl;
      return "All the games in this guest pass redemption request are already owned by the user";
      return;
    case k_EResultIPNotFound:
    }
      return "IP address not found";
    store();
    case k_EResultPersistFailed:
  #endif // STEAM
}
      return "failed to write change to the data store";
// instantiate the template explicitly for the two permissible types
    case k_EResultLockingFailed:
template void steamstorm::set_statistic<int32_t>(std::string const&, int32_t const) const;
      return "failed to acquire access lock for this operation";
template void steamstorm::set_statistic<float  >(std::string const&, float   const) const;
    case k_EResultLogonSessionReplaced:

      return "LogonSessionReplaced";
void steamstorm::store() const {
    case k_EResultConnectFailed:
  /// Attempt to save achievements and stats
      return "ConnectFailed";
  #ifdef STEAM
    case k_EResultHandshakeFailed:
    if(!SteamUserStats()->StoreStats()) {                                       // push the new stats to the server
      return "HandshakeFailed";
      std::cout << "SteamStorm: ERROR: Could not store stats!" << std::endl;
    case k_EResultIOFailure:
      return;
      return "IOFailure";
    }
    case k_EResultRemoteDisconnect:
  #endif // STEAM
      return "RemoteDisconnect";
}
    case k_EResultShoppingCartNotFound:

      return "failed to find the shopping cart requested";
#ifdef STEAM
    case k_EResultBlocked:
  // Internal functionality only ever useful when built in Steam mode
      return "a user didn't allow it";
  std::string const steamstorm::decode_result(EResult result) const {
    case k_EResultIgnored:
    /// Look up a Steam API result code and return a string describing it
      return "target is ignoring sender";
    switch(result) {
    case k_EResultNoMatch:
    case k_EResultOK:
      return "success";
      return "nothing matching the request found";
    case k_EResultFail:
    case k_EResultAccountDisabled:
      return "generic failure";
      return "AccountDisabled";
    case k_EResultNoConnection:
    case k_EResultServiceReadOnly:
      return "no/failed network connection";
      return "this service is not accepting content changes right now";
    //case k_EResultNoConnectionRetry:
    case k_EResultAccountNotFeatured:
    //  return "OBSOLETE - removed";
      return "account doesn't have value, so this feature isn't available";
    case k_EResultInvalidPassword:
    case k_EResultAdministratorOK:
      return "password/ticket is invalid";
      return "allowed to take this action, but only because requester is admin";
    case k_EResultLoggedInElsewhere:
    case k_EResultContentVersion:
      return "same user logged in elsewhere";
    case k_EResultInvalidProtocolVer:
      return "A Version mismatch in content transmitted within the Steam protocol.";
      return "protocol version is incorrect";
    case k_EResultTryAnotherCM:
    case k_EResultInvalidParam:
      return "The current CM can't service the user making a request, user should try another.";
      return "a parameter is incorrect";
    case k_EResultPasswordRequiredToKickSession:
    case k_EResultFileNotFound:
      return "You are already logged in elsewhere, this cached credential login has failed.";
      return "file was not found";
    case k_EResultAlreadyLoggedInElsewhere:
    case k_EResultBusy:
      return "called method busy - action not taken";
      return "You are already logged in elsewhere, you must wait";
    case k_EResultInvalidState:
    case k_EResultSuspended:
      return "called object was in an invalid state";
      return "Long running operation (content download) suspended/paused";
    case k_EResultInvalidName:
    case k_EResultCancelled:
      return "name is invalid";
      return "Operation canceled (typically by user: content download)";
    case k_EResultInvalidEmail:
    case k_EResultDataCorruption:
      return "email is invalid";
      return "Operation canceled because data is ill formed or unrecoverable";
    case k_EResultDuplicateName:
    case k_EResultDiskFull:
      return "name is not unique";
      return "Operation canceled - not enough disk space.";
    case k_EResultAccessDenied:
    case k_EResultRemoteCallFailed:
      return "access is denied";
      return "an remote call or IPC call failed";
    case k_EResultTimeout:
    case k_EResultPasswordUnset:
      return "operation timed out";
    case k_EResultBanned:
      return "Password could not be verified as it's unset server side";
      return "VAC2 banned";
    case k_EResultExternalAccountUnlinked:
    case k_EResultAccountNotFound:
      return "External account (PSN, Facebook...) is not linked to a Steam account";
      return "account not found";
    case k_EResultPSNTicketInvalid:
    case k_EResultInvalidSteamID:
      return "PSN ticket was invalid";
      return "steamID is invalid";
    case k_EResultExternalAccountAlreadyLinked:
    case k_EResultServiceUnavailable:
      return "External account (PSN, Facebook...) is already linked to some other account, must explicitly request to replace/delete the link first";
      return "The requested service is currently unavailable";
    case k_EResultRemoteFileConflict:
    case k_EResultNotLoggedOn:
      return "The sync cannot resume due to a conflict between the local and remote files";
      return "The user is not logged on";
    case k_EResultPending:
    case k_EResultIllegalPassword:
      return "Request is pending (may be in process, or waiting on third party)";
      return "The requested new password is not legal";
    case k_EResultEncryptionFailure:
    case k_EResultSameAsPreviousValue:
      return "Encryption or Decryption failed";
      return "new value is the same as the old one (secret question and answer)";
    case k_EResultInsufficientPrivilege:
    case k_EResultAccountLogonDenied:
      return "Insufficient privilege";
      return "account login denied due to 2nd factor authentication failure";
    case k_EResultLimitExceeded:
    case k_EResultCannotUseOldPassword:
      return "Too much of a good thing";
      return "The requested new password is not legal";
    case k_EResultRevoked:
    case k_EResultInvalidLoginAuthCode:
      return "Access has been revoked (used for revoked guest passes)";
      return "account login denied due to auth code invalid";
    case k_EResultExpired:
    case k_EResultAccountLogonDeniedNoMail:
      return "License/Guest pass the user is trying to access is expired";
      return "account login denied due to 2nd factor auth failure - and no mail has been sent";
    case k_EResultAlreadyRedeemed:
    case k_EResultHardwareNotCapableOfIPT:
      return "Guest pass has already been redeemed by account, cannot be acked again";
      return "HardwareNotCapableOfIPT";
    case k_EResultDuplicateRequest:
    case k_EResultIPTInitError:
      return "The request is a duplicate and the action has already occurred in the past, ignored this time";
      return "IPTInitError";
    case k_EResultAlreadyOwned:
    case k_EResultParentalControlRestricted:
      return "All the games in this guest pass redemption request are already owned by the user";
    case k_EResultIPNotFound:
      return "operation failed due to parental control restrictions for current user";
      return "IP address not found";
    case k_EResultFacebookQueryError:
    case k_EResultPersistFailed:
      return "Facebook query returned an error";
      return "failed to write change to the data store";
    case k_EResultExpiredLoginAuthCode:
    case k_EResultLockingFailed:
      return "account login denied due to auth code expired";
      return "failed to acquire access lock for this operation";
    case k_EResultIPLoginRestrictionFailed:
    case k_EResultLogonSessionReplaced:
      return "IPLoginRestrictionFailed";
      return "LogonSessionReplaced";
    case k_EResultAccountLockedDown:
    case k_EResultConnectFailed:
      return "AccountLockedDown";
      return "ConnectFailed";
    case k_EResultHandshakeFailed:
    case k_EResultAccountLogonDeniedVerifiedEmailRequired:
      return "HandshakeFailed";
      return "AccountLogonDeniedVerifiedEmailRequired";
    case k_EResultIOFailure:
    case k_EResultNoMatchingURL:
      return "IOFailure";
      return "NoMatchingURL";
    case k_EResultRemoteDisconnect:
    case k_EResultBadResponse:
      return "RemoteDisconnect";
      return "parse failure, missing field, etc.";
    case k_EResultShoppingCartNotFound:
    case k_EResultRequirePasswordReEntry:
      return "failed to find the shopping cart requested";
      return "The user cannot complete the action until they re-enter their password";
    case k_EResultBlocked:
    case k_EResultValueOutOfRange:
      return "a user didn't allow it";
      return "The value entered is outside the acceptable range";
    case k_EResultIgnored:
    case k_EResultUnexpectedError:
      return "target is ignoring sender";
    case k_EResultNoMatch:
      return "Something happened that we didn't expect to ever happen";
      return "nothing matching the request found";
    case k_EResultDisabled:
    case k_EResultAccountDisabled:
      return "The requested service has been configured to be unavailable";
      return "AccountDisabled";
    case k_EResultInvalidCEGSubmission:
    case k_EResultServiceReadOnly:
      return "The set of files submitted to the CEG server are not valid !";
      return "this service is not accepting content changes right now";
    case k_EResultRestrictedDevice:
    case k_EResultAccountNotFeatured:
      return "The device being used is not allowed to perform this action";
      return "account doesn't have value, so this feature isn't available";
    case k_EResultRegionLocked:
    case k_EResultAdministratorOK:
      return "The action could not be complete because it is region restricted";
      return "allowed to take this action, but only because requester is admin";
    case k_EResultRateLimitExceeded:
    case k_EResultContentVersion:
      return "Temporary rate limit exceeded, try again later, different from k_EResultLimitExceeded which may be permanent";
      return "A Version mismatch in content transmitted within the Steam protocol.";
    case k_EResultAccountLoginDeniedNeedTwoFactor:
    case k_EResultTryAnotherCM:
      return "Need two-factor code to login";
      return "The current CM can't service the user making a request, user should try another.";
    case k_EResultItemDeleted:
    case k_EResultPasswordRequiredToKickSession:
      return "The thing we're trying to access has been deleted";
      return "You are already logged in elsewhere, this cached credential login has failed.";
    case k_EResultAccountLoginDeniedThrottle:
    case k_EResultAlreadyLoggedInElsewhere:
      return "Login attempt failed, try to throttle response to possible attacker";
      return "You are already logged in elsewhere, you must wait";
    case k_EResultSuspended:
    case k_EResultTwoFactorCodeMismatch:
      return "Long running operation (content download) suspended/paused";
      return "Two factor code mismatch";
    case k_EResultCancelled:
    case k_EResultTwoFactorActivationCodeMismatch:
      return "Operation canceled (typically by user: content download)";
      return "Activation code for two-factor didn't match";
    case k_EResultDataCorruption:
    case k_EResultAccountAssociatedToMultiplePartners:
      return "Operation canceled because data is ill formed or unrecoverable";
      return "Account has been associated with multiple partners";
    case k_EResultDiskFull:
    case k_EResultNotModified:
      return "Operation canceled - not enough disk space.";
      return "Data not modified";
    case k_EResultRemoteCallFailed:
    case k_EResultNoMobileDevice:
      return "an remote call or IPC call failed";
      return "The account does not have a mobile device associated with it";
    case k_EResultPasswordUnset:
    case k_EResultTimeNotSynced:
      return "Password could not be verified as it's unset server side";
      return "The time presented is out of range or tolerance";
    case k_EResultExternalAccountUnlinked:
    case k_EResultSmsCodeFailed:
      return "External account (PSN, Facebook...) is not linked to a Steam account";
      return "SMS code failure (no match, none pending, etc.)";
    case k_EResultPSNTicketInvalid:
    case k_EResultAccountLimitExceeded:
      return "PSN ticket was invalid";
      return "Too many accounts access this resource";
    case k_EResultExternalAccountAlreadyLinked:
    case k_EResultAccountActivityLimitExceeded:
      return "External account (PSN, Facebook...) is already linked to some other account, must explicitly request to replace/delete the link first";
      return "Too many changes to this account";
    case k_EResultRemoteFileConflict:
    case k_EResultPhoneActivityLimitExceeded:
      return "The sync cannot resume due to a conflict between the local and remote files";
      return "Too many changes to this phone";
    case k_EResultIllegalPassword:
    case k_EResultRefundToWallet:
      return "The requested new password is not legal";
    case k_EResultSameAsPreviousValue:
      return "Cannot refund to payment method, must use wallet";
      return "new value is the same as the old one (secret question and answer)";
    case k_EResultEmailSendFailure:
    case k_EResultAccountLogonDenied:
      return "Cannot send an email";
      return "account login denied due to 2nd factor authentication failure";
    case k_EResultNotSettled:
    case k_EResultCannotUseOldPassword:
      return "Can't perform operation till payment has settled";
      return "The requested new password is not legal";
    case k_EResultNeedCaptcha:
    case k_EResultInvalidLoginAuthCode:
      return "Needs to provide a valid captcha";
      return "account login denied due to auth code invalid";
    case k_EResultGSLTDenied:
    case k_EResultAccountLogonDeniedNoMail:
      return "A game server login token owned by this token's owner has been banned";
      return "account login denied due to 2nd factor auth failure - and no mail has been sent";
    case k_EResultGSOwnerDenied:
    case k_EResultHardwareNotCapableOfIPT:
      return "Game server owner is denied for other reason (account lock, community ban, vac ban, missing phone)";
      return "HardwareNotCapableOfIPT";
    case k_EResultInvalidItemType:
    case k_EResultIPTInitError:
      return "The type of thing we were requested to act on is invalid";
      return "IPTInitError";
    case k_EResultIPBanned:
    case k_EResultParentalControlRestricted:
      return "The ip address has been banned from taking this action";
      return "operation failed due to parental control restrictions for current user";
    default:
    case k_EResultFacebookQueryError:
      return "Unknown result";
      return "Facebook query returned an error";
    }
    case k_EResultExpiredLoginAuthCode:
  }
      return "account login denied due to auth code expired";
#endif // STEAM
    case k_EResultIPLoginRestrictionFailed:

      return "IPLoginRestrictionFailed";
#ifdef STEAM
    case k_EResultAccountLockedDown:
      return "AccountLockedDown";
  // Internal callbacks that are only ever created when building with Steam
    case k_EResultAccountLogonDeniedVerifiedEmailRequired:
  void steamstorm::callback_stats(UserStatsReceived_t *callback) {
      return "AccountLogonDeniedVerifiedEmailRequired";
    /// Steam callback for player stats info
    case k_EResultNoMatchingURL:
    if(!callback) {                                                             // this might never be null, but i don't trust steam not to fuck it up
      return "NoMatchingURL";
      #ifdef DEBUG_STEAMSTORM
    case k_EResultBadResponse:
        std::cout << "SteamStorm: ERROR: Received a null callback!" << std::endl;
      return "parse failure, missing field, etc.";
      #endif // DEBUG_STEAMSTORM
    case k_EResultRequirePasswordReEntry:
      return;
      return "The user cannot complete the action until they re-enter their password";
    }
    case k_EResultValueOutOfRange:
    #ifdef DEBUG_STEAMSTORM
      return "The value entered is outside the acceptable range";
    case k_EResultUnexpectedError:
      std::cout << "SteamStorm: DEBUG: Received stats for game ID " << callback->m_nGameID << std::endl;
      return "Something happened that we didn't expect to ever happen";
    #endif // DEBUG_STEAMSTORM
    case k_EResultDisabled:
    if(callback->m_nGameID != appid) {                                          // make sure we're receiving stats for THIS game
      return "The requested service has been configured to be unavailable";
      #ifdef DEBUG_STEAMSTORM
    case k_EResultInvalidCEGSubmission:
        std::cout << "SteamStorm: DEBUG: That is not our game, ignoring." << std::endl;
      return "The set of files submitted to the CEG server are not valid !";
      #endif // DEBUG_STEAMSTORM
    case k_EResultRestrictedDevice:
      return;
      return "The device being used is not allowed to perform this action";
    }
    case k_EResultRegionLocked:
    if(callback->m_eResult != k_EResultOK) {                                    // did we succeed?
      return "The action could not be complete because it is region restricted";
      std::cout << "SteamStorm: ERROR: Could not fetch stats, \"" << decode_result(callback->m_eResult) << "\"" << std::endl;
    case k_EResultRateLimitExceeded:
      return;
      return "Temporary rate limit exceeded, try again later, different from k_EResultLimitExceeded which may be permanent";
    }
    case k_EResultAccountLoginDeniedNeedTwoFactor:
    // what user these are for
      return "Need two-factor code to login";
    // TODO
    case k_EResultItemDeleted:
    #ifdef DEBUG_STEAMSTORM
      return "The thing we're trying to access has been deleted";
      std::cout << "SteamStorm: DEBUG: Stats received and processed." << std::endl;
    case k_EResultAccountLoginDeniedThrottle:
    #endif // DEBUG_STEAMSTORM
      return "Login attempt failed, try to throttle response to possible attacker";
    stats_ready = true;
    case k_EResultTwoFactorCodeMismatch:

      return "Two factor code mismatch";
    case k_EResultTwoFactorActivationCodeMismatch:
    return;                                                                     // this explicit return may be needed because of the calling convention nonsense
      return "Activation code for two-factor didn't match";
  }
    case k_EResultAccountAssociatedToMultiplePartners:
#endif // STEAM
      return "Account has been associated with multiple partners";
    case k_EResultNotModified:
      return "Data not modified";
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
    case k_EResultRefundToWallet:
      return "Cannot refund to payment method, must use wallet";
    case k_EResultEmailSendFailure:
      return "Cannot send an email";
    case k_EResultNotSettled:
      return "Can't perform operation till payment has settled";
    case k_EResultNeedCaptcha:
      return "Needs to provide a valid captcha";
    case k_EResultGSLTDenied:
      return "A game server login token owned by this token's owner has been banned";
    case k_EResultGSOwnerDenied:
      return "Game server owner is denied for other reason (account lock, community ban, vac ban, missing phone)";
    case k_EResultInvalidItemType:
      return "The type of thing we were requested to act on is invalid";
    case k_EResultIPBanned:
      return "The ip address has been banned from taking this action";
    default:
      return "Unknown result";
    }
  }
#endif // STEAM

#ifdef STEAM
  // Internal callbacks that are only ever created when building with Steam
  void steamstorm::callback_stats(UserStatsReceived_t *callback) {
    /// Steam callback for player stats info
    if(!callback) {                                                             // this might never be null, but i don't trust steam not to fuck it up
      #ifdef DEBUG_STEAMSTORM
        std::cout << "SteamStorm: ERROR: Received a null callback!" << std::endl;
      #endif // DEBUG_STEAMSTORM
      return;
    }
    #ifdef DEBUG_STEAMSTORM
      std::cout << "SteamStorm: DEBUG: Received stats for game ID " << callback->m_nGameID << std::endl;
    #endif // DEBUG_STEAMSTORM
    if(callback->m_nGameID != appid) {                                          // make sure we're receiving stats for THIS game
      #ifdef DEBUG_STEAMSTORM
        std::cout << "SteamStorm: DEBUG: That is not our game, ignoring." << std::endl;
      #endif // DEBUG_STEAMSTORM
      return;
    }
    if(callback->m_eResult != k_EResultOK) {                                    // did we succeed?
      std::cout << "SteamStorm: ERROR: Could not fetch stats, \"" << decode_result(callback->m_eResult) << "\"" << std::endl;
      return;
    }
    // what user these are for
    // TODO
    #ifdef DEBUG_STEAMSTORM
      std::cout << "SteamStorm: DEBUG: Stats received and processed." << std::endl;
    #endif // DEBUG_STEAMSTORM
    stats_ready = true;

    return;                                                                     // this explicit return may be needed because of the calling convention nonsense
  }
#endif // STEAM
