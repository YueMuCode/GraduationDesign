// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "OnlineSessionSettings.h"
#include "Blueprint/UserWidget.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "Menu_UserWidget.generated.h"

/**
 * 
 */
UCLASS()
class MULTIPLAYERSESSION_API UMenu_UserWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable)
	void MenuSetup(int32 NumberOfPublicConnections=4,FString TypeOfMatch=FString(TEXT("FreeForAll")),FString LobbyPath=FString(TEXT("/Game/ThirdPerson/Maps/Lobby")));

	UFUNCTION(BlueprintCallable)
	void MenuTearDown();

protected:
	virtual  bool Initialize() override;//为什么要使用Initialize
	virtual  void NativeDestruct() override;//当切换场景时，widget组件将会调用这个虚函数

	//回调函数，委托给子系统
	UFUNCTION()//有些委托订阅的回调函数必须是U函数
	void OnCreateSession(bool bWasSuccessful);
	void OnFindSession(const TArray<FOnlineSessionSearchResult>& SessionResults,bool bWasSuccessful);
	void OnJoinSession(EOnJoinSessionCompleteResult::Type Result);
	UFUNCTION()
	void OnDestroySession(bool bWasSuccessful);
	UFUNCTION()
	void OnStartSession(bool bWasSuccessful);
	
private:
	//并指定该属性应该与一个 UI 控件进行绑定。这样，在使用该属性时，
	//引擎会自动将其与相应的 UI 控件关联起来，方便你在代码中直接操作该控件。
	//前提是：控件的名称 蓝图和C++中的必须一一致
	UPROPERTY(meta=(BindWidget))
	class UButton* HostButton;
	
	UPROPERTY(meta=(BindWidget))
	UButton* SearchButton;
	
	UPROPERTY(meta=(BindWidget))
	UButton* JoinButton;

	UPROPERTY(meta=(BindWidget))
	UButton* CloseUIButton;


	//点击按钮后的UI信息
	UPROPERTY(meta=(BindWidget))
	class UBorder* HostmessageBorder;
	
	UPROPERTY(meta=(BindWidget))
	UBorder* FindMessageBorder;
	
	UPROPERTY(meta=(BindWidget))
	class  UTextBlock* FindMessageText;
	
	UPROPERTY(meta=(BindWidget))
	UBorder* JoinMessageBorder;

	UPROPERTY(meta=(BindWidget))
	UTextBlock* JoinMessageText;

	UPROPERTY(meta=(BindWidget))
	UBorder* JoinBorder;

	UPROPERTY(meta=(BindWidget))
	UTextBlock*JoinTitle;

	UPROPERTY(meta=(BindWidget))
	UTextBlock*SessionName;

	

	//当按钮点击时需要完成的逻辑

	UFUNCTION()
	void HostButtonClicked();

	UFUNCTION()
	void SearchButtonClicked();

	UFUNCTION()
	void JoinButtonClicked();

	UFUNCTION()
	void CloseButtonClicked();
	//菜单类中访问子系统的指针。
	class UMultiplayerSessionsSubsystem* MultiplayerSessionsSubsystem;


	int32 NumPublicConnections{4};
	FString MatchType{TEXT("FreeForAll")};
	FString PathToLobby{TEXT("")};
	FOnlineSessionSearchResult SearchResult;
};
