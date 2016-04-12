// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Engine/GameInstance.h"
#include "COGameInstance.generated.h"

#define SAVEGAME SaveGame

/*Struct that is going to disapear, just temporal because i can't find how to make good find sessions node*/
USTRUCT()
struct FBPSessionSearchResult
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(BlueprintReadWrite, Category = SEARCHRESULT)
		FString ServerName;
	UPROPERTY(BlueprintReadWrite, Category = SEARCHRESULT)
		int32 MaxPlayer;
	UPROPERTY(BlueprintReadWrite, Category = SEARCHRESULT)
		int32 CurrentPlayer;
	UPROPERTY(BlueprintReadWrite, Category = SEARCHRESULT)
		int32 Ping;
};

UENUM()
enum class ECurrentState : uint8
{
	Entry,
	ProfileSelection,
	MainMenu,
	MainMenu_Settings,
	LoadingScreen,
	ErrorDialog,
	MainMenu_CreateJoin,
	MainMenu_ServerList,
	Lobby,
	Lobby_CharacterSelection,
	Lobby_GameModeSelection,
	Lobby_MapSelection,
	Playing,
	Unknown
};

/**
 * 
 */
UCLASS()
class CHAOSOVERRIDE_API UCOGameInstance : public UGameInstance
{
	GENERATED_BODY()

	UCOGameInstance(const FObjectInitializer& ObjectInitializer);

	/*-----------------------------------------------------------------------------------------------------------------------------------------------------*/
	/*----------------------------------------------------------ONLINE SYSTEM------------------------------------------------------------------------------*/
	/*-----------------------------------------------------------------------------------------------------------------------------------------------------*/


	/**
	*	Function to host a game!
	*
	*	@Param		UserID			User that started the request
	*	@Param		SessionName		Name of the Session
	*	@Param		bIsLAN			Is this is LAN Game?
	*	@Param		bIsPresence		"Is the Session to create a presence Session"
	*	@Param		MaxNumPlayers	        Number of Maximum allowed players on this "Session" (Server)
	*/
	bool HostSession(TSharedPtr<const FUniqueNetId> UserId, FName SessionName, bool bIsLAN, bool bIsPresence, int32 MaxNumPlayers, bool bIsPrivate);

	/* Delegate called when session created */
	FOnCreateSessionCompleteDelegate OnCreateSessionCompleteDelegate;
	/* Delegate called when session started */
	FOnStartSessionCompleteDelegate OnStartSessionCompleteDelegate;

	/** Handles to registered delegates for creating/starting a session */
	FDelegateHandle OnCreateSessionCompleteDelegateHandle;
	FDelegateHandle OnStartSessionCompleteDelegateHandle;

	TSharedPtr<class FOnlineSessionSettings> SessionSettings;

	/**
	*	Function fired when a session create request has completed
	*
	*	@param SessionName the name of the session this callback is for
	*	@param bWasSuccessful true if the async action completed without error, false if there was an error
	*/
	virtual void OnCreateSessionComplete(FName SessionName, bool bWasSuccessful);

	/**
	*	Function fired when a session start request has completed
	*
	*	@param SessionName the name of the session this callback is for
	*	@param bWasSuccessful true if the async action completed without error, false if there was an error
	*/
	void OnStartOnlineGameComplete(FName SessionName, bool bWasSuccessful);

	/**
	*	Find an online session
	*
	*	@param UserId user that initiated the request
	*	@param SessionName name of session this search will generate
	*	@param bIsLAN are we searching LAN matches
	*	@param bIsPresence are we searching presence sessions
	*/
	void FindSessions(TSharedPtr<const FUniqueNetId> UserId, FName SessionName, bool bIsLAN, bool bIsPresence);

	/** Delegate for searching for sessions */
	FOnFindSessionsCompleteDelegate OnFindSessionsCompleteDelegate;

	/** Handle to registered delegate for searching a session */
	FDelegateHandle OnFindSessionsCompleteDelegateHandle;

	TSharedPtr<class FOnlineSessionSearch> SessionSearch;

	/**
	*	Delegate fired when a session search query has completed
	*
	*	@param bWasSuccessful true if the async action completed without error, false if there was an error
	*/
	void OnFindSessionsComplete(bool bWasSuccessful);

	/**
	*	Joins a session via a search result
	*
	*	@param SessionName name of session
	*	@param SearchResult Session to join
	*
	*	@return bool true if successful, false otherwise
	*/
	bool JoinSessionOverride(TSharedPtr<const FUniqueNetId> UserId, FName SessionName, const FOnlineSessionSearchResult& SearchResult);


	/** Delegate for joining a session */
	FOnJoinSessionCompleteDelegate OnJoinSessionCompleteDelegate;

	/** Handle to registered delegate for joining a session */
	FDelegateHandle OnJoinSessionCompleteDelegateHandle;

	/**
	*	Delegate fired when a session join request has completed
	*
	*	@param SessionName the name of the session this callback is for
	*	@param bWasSuccessful true if the async action completed without error, false if there was an error
	*/
	void OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result);


	/** Delegate for destroying a session */
	FOnDestroySessionCompleteDelegate OnDestroySessionCompleteDelegate;

	/** Handle to registered delegate for destroying a session */
	FDelegateHandle OnDestroySessionCompleteDelegateHandle;

	/**
	*	Delegate fired when a destroying an online session has completed
	*
	*	@param SessionName the name of the session this callback is for
	*	@param bWasSuccessful true if the async action completed without error, false if there was an error
	*/
	virtual void OnDestroySessionComplete(FName SessionName, bool bWasSuccessful);

	/*The property that defined the max number of player that can be connected in every game mode, as player of spectator(the game mode will handle that)*/
	
	uint8 DefaultMaxNumberOfPlayerInAllGameMode=32;

public:
	/*The maximum number of player allow on server, can be different from the real number of people that can actually play(some may be spectators)*/
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Online)
		int32 MaxNumberOfPlayer = 16;

	/** Called via blueprint to create a lobby
	*	Function to host a game!
	*
	*	@Param		UserID			User that started the request
	*	@Param		SessionName		Name of the Session
	*	@Param		bIsLAN			Is this is LAN Game?
	*	@Param		bIsPresence		"Is the Session to create a presence Session"
	*	@Param		MaxNumPlayers	        Number of Maximum allowed players on this "Session" (Server)
	*/	

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = MAP)
	FName EntryMapName = "Entry";
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = MAP)
	FName LobbyMapName = "Lobby";

	UFUNCTION(BlueprintCallable, Category = LOBBY)
		bool CreateLobby(FName SessionName, bool bIsLAN, bool bIsPresence, int32 MaxNumPlayers, bool bIsPrivate, bool bAllowInvitation);

	UFUNCTION(BlueprintCallable, Category = LOBBY)
		TArray<struct FBPSessionSearchResult> FindLobby(bool bIsLan);

	UPROPERTY(BlueprintReadWrite, Category = LOBBY)
	TArray<struct FBPSessionSearchResult> BPSessionSearch;

	UFUNCTION(BlueprintCallable, Category = LOBBY)
		void JoinLobby();

	UFUNCTION(BlueprintCallable, Category = LOBBY)
		void DestroySessionAndLeaveGame();


	/*-----------------------------------------------------------------------------------------------------------------------------------------------------*/
	/*------------------------------------------------------------SAVE SYSTEM------------------------------------------------------------------------------*/
	/*-----------------------------------------------------------------------------------------------------------------------------------------------------*/

public:
	/*Save all the local profile contains in all local PC
	* @return bool if save suceed */
	UFUNCTION(BlueprintCallable, Category = SAVEGAME)
	bool SaveAllLocalProfile();

	/*Find Session*/

	/*-----------------------------------------------------------------------------------------------------------------------------------------------------*/
	/*------------------------------------------------------------NAVIGATION SYSTEM------------------------------------------------------------------------*/
	/*-----------------------------------------------------------------------------------------------------------------------------------------------------*/

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = STATE)
	ECurrentState CurrentState = ECurrentState::Entry;

	UFUNCTION(BlueprintCallable, Category = TRAVEL)
	virtual void TravelToDemo(FString MapName, int32 LocalPlayerNb, int32 AIPlayerNb, int32 MatchLength, TSubclassOf<AGameMode> GameModeTemplate);

};

