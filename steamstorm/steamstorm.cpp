#include "steamstorm.h"
#include <iostream>
#ifdef STEAM
  #include <chrono>
  #include <thread>
#endif // STEAM

steamstorm::steamstorm()
  #ifdef STEAM
    //: internal_callback_stats(this, &steamstorm::callback_stats)
    // we need to initialise it as NULL because the constructor gets upset with our __stdcall modification, and register manually after
    : internal_callback_stats(NULL, NULL)
  #endif // STEAM
  {
  /// Default constructor
  //internal_callback_stats.Register(this, &steamstorm::callback_stats);
  initialise();
}

steamstorm::~steamstorm() {
  /// Default destructor
  shutdown();
}

void steamstorm::initialise() {
  /// Attempt to initialise everything
  initialised = false;
  #ifdef STEAM
    std::cout << "SteamStorm: Initialising..." << std::endl;
    if(!SteamAPI_Init()) {
      /// A return of false indicates one of three conditions:
      ///   The Steam client isn't running. A running Steam client is required to provide implementations of the various Steamworks interfaces.
      ///   The Steam client couldn't determine the AppID of game. Make sure you have Steam_appid.txt in your game directory.
      ///   Your application is not running under the same user context as the Steam client, including admin privileges.
      std::cout << "SteamStorm: Could not initialise Steam API - disabling Steam support for this run." << std::endl;
      return;
    }
    // register callbacks
    internal_callback_stats.Register(this, &steamstorm::callback_stats);

    ///std::cout << "SteamStorm: DEBUG 0: Requesting hidden state of TAKEOVER: " << SteamUserStats()->GetAchievementDisplayAttribute("TAKEOVER", "hidden") << std::endl;
    if(!SteamUser()->BLoggedOn()) {
      std::cout << "SteamStorm: Steam is running but user is not logged in, disabling Steam support for this run." << std::endl;
      return;
    }
    appid = SteamUtils()->GetAppID();
    //internal_user_id = SteamUser()->GetSteamID();                               // cache the user's Steam ID
    // the above returns a struct and as such breaks due to calling conventions
    username = SteamFriends()->GetPersonaName();
    // various debugging checks that we don't want to bother with in release
    #ifdef DEBUG_STEAMSTORM
      std::cout << "SteamStorm: DEBUG: Steam app ID:     " << appid << std::endl;
      std::cout << "SteamStorm: DEBUG: Steam username:   " << username << std::endl;
      //std::cout << "SteamStorm: DEBUG: Steam ID:         " << internal_user_id.ConvertToUint64() << std::endl;
      std::cout << "SteamStorm: DEBUG: Steam level:      " << SteamUser()->GetPlayerSteamLevel() << std::endl;
      std::cout << "SteamStorm: DEBUG: Badge level:      " << SteamUser()->GetGameBadgeLevel(1, false) << std::endl;
      std::cout << "SteamStorm: DEBUG: Foil Badge level: " << SteamUser()->GetGameBadgeLevel(1, true)  << std::endl;
      std::cout << "SteamStorm: DEBUG: Friends:          " << SteamFriends()->GetFriendCount(EFriendFlags::k_EFriendFlagNone) << std::endl;
      //std::cout << "SteamStorm: DEBUG: TEST: advertcity     365800 " << get_user_has_app(365800);
      //std::cout << "SteamStorm: DEBUG: TEST: advertcity ost 366130 " << get_user_has_app(366130);
      //std::cout << "SteamStorm: DEBUG: TEST: some random    274940 " << get_user_has_app(274940);
      //if(internal_user_id.BAnonAccount()) {
      //  std::cout << "SteamStorm: DEBUG: This is an anonymous account" << std::endl;
      //}
      if(SteamUser()->BIsBehindNAT()) {
        std::cout << "SteamStorm: DEBUG: User appears to be behind NAT" << std::endl;
      }
      switch(SteamFriends()->GetPersonaState()) {
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
    #endif // DEBUG_STEAMSTORM

    #ifdef DEBUG_STEAMSTORM
      std::cout << "SteamStorm: DEBUG: Querying Steam for the latest stats..." << std::endl;
    #endif // DEBUG_STEAMSTORM
    ///std::cout << "SteamStorm: DEBUG 1: Requesting stat funds: " << get_statistic<float>("FUNDS") << std::endl;
    ///std::cout << "SteamStorm: DEBUG 1: Requesting hidden state of TAKEOVER: " << SteamUserStats()->GetAchievementDisplayAttribute("TAKEOVER", "hidden") << std::endl;
    if(!SteamUserStats()->RequestCurrentStats()) {                              // request an update on the current stats
      std::cout << "SteamStorm: ERROR: Failed to request user stats from Steam API, disabling Steam support this run." << std::endl;
      return;
    }

    /**
    // block synchronously until we've got the stats
    std::chrono::time_point<std::chrono::high_resolution_clock, std::chrono::duration<double>> time_next_check(std::chrono::high_resolution_clock::now());
    unsigned int constexpr stats_timeout = 2000;  // milliseconds
    unsigned int constexpr stats_timestep = 50;   // milliseconds, can tweak this for best performance
    for(unsigned int i = 0; i += stats_timestep; stats_ready == false) {
      std::cout << "SteamStorm: DEBUG: running callbacks..." << std::endl;
      SteamAPI_RunCallbacks();                                                  // run the callbacks to process any incoming data
      std::this_thread::sleep_until(time_next_check);
      time_next_check = std::chrono::high_resolution_clock::now() + std::chrono::milliseconds(stats_timestep);
      if(i > stats_timeout) {
        std::cout << "SteamStorm: ERROR: Could not retrieve user stats from Steam API, disabling Steam support this run." << std::endl;
        return;
      }
    }
    **/

    ///std::cout << "SteamStorm: DEBUG 2: Requesting stat funds: " << get_statistic<float>("FUNDS") << std::endl;
    ///std::cout << "SteamStorm: DEBUG 2: Requesting hidden state of TAKEOVER: " << SteamUserStats()->GetAchievementDisplayAttribute("TAKEOVER", "hidden") << std::endl;
    //std::this_thread::sleep_for(std::chrono::milliseconds(500));                // wait to make sure we have the data we need (nasty hack!)
    //SteamAPI_RunCallbacks();                                                    // run the callbacks once to process any incoming data
    ///std::cout << "SteamStorm: DEBUG 3: Requesting stat funds int:   " << get_statistic<int>("FUNDS") << std::endl;
    ///std::cout << "SteamStorm: DEBUG 3: Requesting stat funds float: " << get_statistic<float>("FUNDS") << std::endl;
    ///std::cout << "SteamStorm: DEBUG 3: Requesting stat hired int:   " << get_statistic<int>("HIRED") << std::endl;
    ///std::cout << "SteamStorm: DEBUG 3: Requesting stat hired float: " << get_statistic<float>("HIRED") << std::endl;
    ///std::cout << "SteamStorm: DEBUG 3: Requesting hidden state of TAKEOVER: " << SteamUserStats()->GetAchievementDisplayAttribute("TAKEOVER", "hidden") << std::endl;
    ///set_statistic("FUNDS", 12.345f);
    ///set_statistic("HIRED", 1);

    initialised = true;
    std::cout << "SteamStorm: Initialised and ready." << std::endl;
  #else // STEAM
    std::cout << "SteamStorm is disabled - no Steam support in this build." << std::endl;
  #endif // STEAM
}

void steamstorm::shutdown() {
  /// Shut down cleanly
  #ifdef STEAM
    std::cout << "SteamStorm: Shutting down." << std::endl;
    SteamAPI_Shutdown();
  #endif // STEAM
}

#ifdef STEAM
  // steam versions of getter functions (splitting it this way saves lots of space)
  bool steamstorm::get_user_has_app(unsigned int const appid_to_check __attribute__((__unused__))) const {
    /// Find out whether a user has the app ID specified - useful for checking DLC etc
    //return (SteamUser()->UserHasLicenseForApp(internal_user_id, appid_to_check) == EUserHasLicenseForAppResult::k_EUserHasLicenseResultHasLicense);
    // this requires passing a struct, and as such fails due to the calling convention
    return false;
  }

  bool steamstorm::get_achievement(std::string const &achievementname) const {
    /// Find out whether the specified achievement has been set
    bool alreadyachieved = false;
    uint32_t achievementtime = 0;
    SteamUserStats()->GetAchievementAndUnlockTime(achievementname.c_str(), &alreadyachieved, &achievementtime);
    #ifdef DEBUG_STEAMSTORM
      if(alreadyachieved) {
        std::cout << "SteamStorm: DEBUG: Achievement \"" << achievementname << "\" unlocked at " << achievementtime << "." << std::endl;
      } else {
        std::cout << "SteamStorm: DEBUG: Achievement \"" << achievementname << "\" not yet unlocked." << std::endl;
      }
    #endif // DEBUG_STEAMSTORM
    return alreadyachieved;
  }

  std::string const steamstorm::get_achievement_name(std::string const &achievementname) const {
    /// Find out the full name of the specified achievement
    return SteamUserStats()->GetAchievementDisplayAttribute(achievementname.c_str(), "name");
  }
  std::string const steamstorm::get_achievement_description(std::string const &achievementname) const {
    /// Find out the full description of the specified achievement
    return SteamUserStats()->GetAchievementDisplayAttribute(achievementname.c_str(), "desc");
  }
  bool steamstorm::get_achievement_hidden(std::string const &achievementname) const {
    /// Find out whether the specified achievement is hidden
    return SteamUserStats()->GetAchievementDisplayAttribute(achievementname.c_str(), "hidden");
  }
#else
  // dummy placeholder functions when steam is disabled
  bool steamstorm::get_user_has_app(unsigned int const appid __attribute__((__unused__))) const {
    return false;
  }
  bool steamstorm::get_achievement(std::string const &achievementname __attribute__((__unused__))) const {
    return false;
  }
  std::string const steamstorm::get_achievement_name(std::string const &achievementname __attribute__((__unused__))) const {
    return "";
  }
  std::string const steamstorm::get_achievement_description(std::string const &achievementname __attribute__((__unused__))) const {
    return "";
  }
  bool steamstorm::get_achievement_hidden(std::string const &achievementname __attribute__((__unused__))) const {
    return false;
  }
#endif // STEAM

template <typename T>
T steamstorm::get_statistic(std::string const &statname
                            #ifndef STEAM
                              __attribute__((__unused__))
                            #endif // STEAM
                            ) const {
  /// Attempt to update a statistic to the specified value - integer version
  #ifdef STEAM
    #ifdef DEBUG_STEAMSTORM
      std::cout << "SteamStorm: DEBUG: Getting statistic \"" << statname << " (" << typeid(T).name() << ")" << std::endl;
    #endif // DEBUG_STEAMSTORM
    T newstat = 0;
    bool success = SteamUserStats()->GetStat(statname.c_str(), &newstat);
    if(!success) {
      std::cout << "SteamStorm: ERROR: Could not get statistic \"" << statname << " (" << typeid(T).name() << ")" << std::endl;
      return 0;
    }
    return newstat;
  #endif // STEAM
  return 0;
}
// instantiate the template explicitly for the two permissible types
template int32_t steamstorm::get_statistic<int32_t>(std::string const&) const;
template float   steamstorm::get_statistic<float  >(std::string const&) const;


void steamstorm::set_achievement(std::string const &achievementname
                                 #ifndef STEAM
                                   __attribute__((__unused__))
                                 #endif // STEAM
                                 ) const {
  /// Attempt to set an achievement described by the specified string
  #ifdef STEAM
    if(!initialised) {
      return;                                                                   // exit silently if the api isn't initialised
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
    bool const result = SteamUserStats()->SetAchievement(achievementname.c_str());
    if(!result) {
      /// If the call failed then either Steam is not initialized or you still haven't processed the callback from the inital call to RequestStats().
      /// You can't set any achievements until that callback has been received.
      std::cout << "SteamStorm: ERROR: Setting achievement \"" << achievementname << "\" failed." << std::endl;
      //initialised = false;
      return;
    }
    #ifdef DEBUG_STEAMSTORM
      std::cout << "SteamStorm: DEBUG: Setting achievement \"" << achievementname << "\" succeeded." << std::endl;
    #endif // DEBUG_STEAMSTORM
    store();
  #endif // STEAM
}

template <typename T>
void steamstorm::set_statistic(std::string const &statname
                               #ifndef STEAM
                                 __attribute__((__unused__))
                               #endif // STEAM
                               , T const statvalue
                               #ifndef STEAM
                                 __attribute__((__unused__))
                               #endif // STEAM
                               ) const {
  /// Attempt to update a statistic to the specified value - integer version
  #ifdef STEAM
    #ifdef DEBUG_STEAMSTORM
      std::cout << "SteamStorm: DEBUG: Setting statistic \"" << statname << "\" to " << statvalue << " (" << typeid(T).name() << ")" << std::endl;
    #endif // DEBUG_STEAMSTORM
    bool success = SteamUserStats()->SetStat(statname.c_str(), statvalue);
    if(!success) {
      std::cout << "SteamStorm: ERROR: Could not set statistic \"" << statname << "\" to " << statvalue << " (" << typeid(T).name() << ")" << std::endl;
      return;
    }
    store();
  #endif // STEAM
}
// instantiate the template explicitly for the two permissible types
template void steamstorm::set_statistic<int32_t>(std::string const&, int32_t const) const;
template void steamstorm::set_statistic<float  >(std::string const&, float   const) const;

void steamstorm::store() const {
  /// Attempt to save achievements and stats
  #ifdef STEAM
    if(!SteamUserStats()->StoreStats()) {                                       // push the new stats to the server
      std::cout << "SteamStorm: ERROR: Could not store stats!" << std::endl;
      return;
    }
  #endif // STEAM
}

#ifdef STEAM
  // Internal functionality only ever useful when built in Steam mode
  std::string const steamstorm::decode_result(EResult result) const {
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
      return "new value is the same as the old one ( secret question and answer )";
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
    case k_EResultAccountAssociatedToMultiplePartners:
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

    return;   // this explicit return may be needed because of the calling convention nonsense
  }
#endif // STEAM
