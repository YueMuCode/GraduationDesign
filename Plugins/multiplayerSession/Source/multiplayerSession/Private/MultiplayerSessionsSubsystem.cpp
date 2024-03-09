// Fill out your copyright notice in the Description page of Project Settings.


#include "MultiplayerSessionsSubsystem.h"
#include "OnlineSubsystem.h"
#include "OnlineSessionSettings.h"

//将函数OnCreateSessionComplete订阅到CreateSessionCompleteDelegate委托中，这个委托是子系统自带的函数，当创建会话成功时会自动触发
UMultiplayerSessionsSubsystem::UMultiplayerSessionsSubsystem():
CreateSessionCompleteDelegate(FOnCreateSessionCompleteDelegate::CreateUObject(this,&ThisClass::OnCreateSessionComplete)),
FindSessionCompleteDelegate(FOnFindSessionsCompleteDelegate::CreateUObject(this,&ThisClass::OnFindSessionsComplete)),
JoinSessionCompleteDelegate(FOnJoinSessionCompleteDelegate::CreateUObject(this,&ThisClass::OnJoinSessionComplete)),
DestroySessionCompleteDelegate(FOnDestroySessionCompleteDelegate::CreateUObject(this,&ThisClass::OnDestroySessionComplete)),
StartSessionCompleteDelegate(FOnStartSessionCompleteDelegate::CreateUObject(this,&ThisClass::OnStartSessionComplete))
{
	//Get() 是一个静态函数，它返回当前正在使用的在线子系统的实例，如果没有在线子系统可用，则返回空指针。
	IOnlineSubsystem* Subsystem=IOnlineSubsystem::Get();
	if(Subsystem)
	{
		// 在在线子系统可用时，这行代码获取这个子系统创建的在线会话接口的实例，并将其分配给 SessionInterface 变量。
		// 在线会话接口 (IOnlineSession) 提供了管理多人游戏会话的功能，如创建、查找、加入和销毁游戏会话等
		SessionInterface=Subsystem->GetSessionInterface();
	}
}

void UMultiplayerSessionsSubsystem::CreateSession(int32 NumPublicConnections, FString MatchType)
{
	//如果会话接口不存在
	if(!SessionInterface.IsValid())
	{
		return ;
	}
	//如果存在会话则销毁
	auto ExistingSession=SessionInterface->GetNamedSession(NAME_GameSession);
	if(ExistingSession!=nullptr)
	{
		//可以快速的在会话销毁之后再创建会话
		bCreateSessionOnDestroy=true;
		LastNumPublicConnections=NumPublicConnections;
		LastMatchType=MatchType;
		DestroySession();
		//SessionInterface->DestroySession(NAME_GameSession);
	}

	//开始正式创建会话
	//用handle存储delegate，方便订阅和解除订阅
	CreateSessionCompleteDelegateHandle=SessionInterface->AddOnCreateSessionCompleteDelegate_Handle(CreateSessionCompleteDelegate);//将一个创建会话完成的委托注册到 SessionInterface 上，并获得一个句柄用于标识该委托

	//属性设置
	LastSessionSettings=MakeShareable(new FOnlineSessionSettings());
	LastSessionSettings->bIsLANMatch=IOnlineSubsystem::Get()->GetSubsystemName()=="NULL"?true:false;//判断当前会话是否为局域网匹配
	LastSessionSettings->NumPublicConnections=NumPublicConnections;//设置会话的公共连接数，即最大允许连接的玩家数量。
	LastSessionSettings->bAllowJoinInProgress=true;//允许玩家在游戏进行中加入会话
	LastSessionSettings->bAllowJoinViaPresence=true;//允许玩家通过在线状态加入会话。
	LastSessionSettings->bShouldAdvertise=true;//允许会话进行广播，以便其他玩家能够找到并加入。
	LastSessionSettings->bUsesPresence=true;//会话使用在线状态来管理玩家的存在。
	LastSessionSettings->bUseLobbiesIfAvailable=true;//如果在线子系统支持大厅（lobbies），则使用大厅来管理会话。
	LastSessionSettings->Set(FName("MatchType"),MatchType,EOnlineDataAdvertisementType::ViaOnlineServiceAndPing);//设置会话的匹配类型字符串，以及通过何种方式进行在线数据的广播。
	LastSessionSettings->BuildUniqueId=1;//设置会话的建立唯一标识,防止搜索出现重叠
	
	const ULocalPlayer* LocalPlayer=GetWorld()->GetFirstLocalPlayerFromController();
	if(!SessionInterface->CreateSession(*LocalPlayer->GetPreferredUniqueNetId(),NAME_GameSession,*LastSessionSettings))//GetPreferredUniqueNetId()获取本地玩家的唯一网络标识符,创建者网络hostid，会话名字，会话属性设置
	{
		//如果创建失败
		SessionInterface->ClearOnCreateSessionCompleteDelegate_Handle(CreateSessionCompleteDelegateHandle);//移除委托句柄下的委托
		//广播委托
		MultiplayerOnCreateSessionComplete.Broadcast(false);
	}
	
} 

void UMultiplayerSessionsSubsystem::FindSessions(int32 MaxSearchResults)
{
	if(!SessionInterface.IsValid())
	{
		return;
	}
	//给句柄赋值
	FindSessionCompleteDelegateHandle=SessionInterface->AddOnFindSessionsCompleteDelegate_Handle(FindSessionCompleteDelegate);

	//设置搜索的条件
	LastSessionSearch=MakeShareable(new FOnlineSessionSearch());//创建智能指针
	LastSessionSearch->MaxSearchResults=MaxSearchResults;//设置会话搜索的最大结果数
	LastSessionSearch->bIsLanQuery=IOnlineSubsystem::Get()->GetSubsystemName()=="NULL"?true:false;//根据当前使用的在线子系统名称来确定是否是局域网查询
	LastSessionSearch->QuerySettings.Set(SEARCH_PRESENCE,true,EOnlineComparisonOp::Equals);//设置查询条件，这里设置了查询是否在线的条件。
	const ULocalPlayer* LocalPlayer = GetWorld()->GetFirstLocalPlayerFromController();//获取本地玩家的指针
	if(!SessionInterface->FindSessions(*LocalPlayer->GetPreferredUniqueNetId(),LastSessionSearch.ToSharedRef()))//ToSharedRef() 函数将智能指针转换为共享引用，以便在传递给 SessionInterface->FindSessions 函数时能够正确地传递参数。
	{
		SessionInterface->ClearOnFindSessionsCompleteDelegate_Handle(FindSessionCompleteDelegateHandle);
		MultiplayerOnFindSessionComplete.Broadcast(TArray<FOnlineSessionSearchResult>(),false);//查找失败就返回一个空的数组和false
		if(GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1,15.0f,FColor::Red,FString(TEXT("子系统---查找会话失败")));
		}
	}
}

void UMultiplayerSessionsSubsystem::JoinSession(const FOnlineSessionSearchResult& SessionResult)
{
	if(!SessionInterface.IsValid())
	{
		MultiplayerOnJoinSessionComplete.Broadcast(EOnJoinSessionCompleteResult::UnknownError);
		return;
	}
	JoinSessionCompleteDelegateHandle=SessionInterface->AddOnJoinSessionCompleteDelegate_Handle(JoinSessionCompleteDelegate);
	const ULocalPlayer* LocalPlayer = GetWorld()->GetFirstLocalPlayerFromController();
	if(!SessionInterface->JoinSession(*LocalPlayer->GetPreferredUniqueNetId(),NAME_GameSession,SessionResult))
	{
		SessionInterface->ClearOnJoinSessionCompleteDelegate_Handle(JoinSessionCompleteDelegateHandle);
		MultiplayerOnJoinSessionComplete.Broadcast(EOnJoinSessionCompleteResult::UnknownError);
		if(GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1,4.0f,FColor::Orange,FString(TEXT("EOnJoinSessionCompleteResult::UnknownError加入失败")));
		}
	}
}

void UMultiplayerSessionsSubsystem::DestroySession()
{
	if(!SessionInterface.IsValid())
	{
		MultiplayerOnDestroySessionComplete.Broadcast(false);
		return;
	}
	DestroySessionCompleteDelegateHandle=SessionInterface->AddOnDestroySessionCompleteDelegate_Handle(DestroySessionCompleteDelegate);

	if(!SessionInterface->DestroySession(NAME_GameSession))
	{
		SessionInterface->ClearOnDestroySessionCompleteDelegate_Handle(DestroySessionCompleteDelegateHandle);
		MultiplayerOnDestroySessionComplete.Broadcast(false);
	}
}

void UMultiplayerSessionsSubsystem::StartSession()
{
}

//如果子系统成功创建会话，那么他会执行自带的回调。不明白第一个参数的意义
void UMultiplayerSessionsSubsystem::OnCreateSessionComplete(FName SessionName, bool bWasSuccessful)
{
	if(SessionInterface)
	{
		SessionInterface->ClearOnCreateSessionCompleteDelegate_Handle(CreateSessionCompleteDelegateHandle);
	}
	MultiplayerOnCreateSessionComplete.Broadcast(bWasSuccessful);
}

void UMultiplayerSessionsSubsystem::OnFindSessionsComplete(bool bWasSuccessful)
{
	if(SessionInterface)
	{
		SessionInterface->ClearOnFindSessionsCompleteDelegate_Handle(FindSessionCompleteDelegateHandle);
	}
	if(LastSessionSearch->SearchResults.Num()<=0)
	{
		MultiplayerOnFindSessionComplete.Broadcast(TArray<FOnlineSessionSearchResult>(),false);
		if(GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1,3,FColor::Red,FString(TEXT("SearchResults.Num()<=0")));
		}
		return;
	}
	
	MultiplayerOnFindSessionComplete.Broadcast(LastSessionSearch->SearchResults,bWasSuccessful);
	if(GEngine&&!bWasSuccessful)
	{
		GEngine->AddOnScreenDebugMessage(-1,4.0f,FColor::Orange,FString(TEXT("UMultiplayerSessionsSubsystem::OnFindSessionsComplete查找失败")));
	}
}

void UMultiplayerSessionsSubsystem::OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result)
{
	if(SessionInterface)
	{
		SessionInterface->ClearOnJoinSessionCompleteDelegate_Handle(JoinSessionCompleteDelegateHandle);
	}
	MultiplayerOnJoinSessionComplete.Broadcast(Result);
}

void UMultiplayerSessionsSubsystem::OnDestroySessionComplete(FName SessionName, bool bWasSuccessful)
{
	if(SessionInterface)
	{
		SessionInterface->ClearOnDestroySessionCompleteDelegate_Handle(DestroySessionCompleteDelegateHandle);
	}
	if(bWasSuccessful&&bCreateSessionOnDestroy)//成功销毁
		{
		bCreateSessionOnDestroy=false;
		CreateSession(LastNumPublicConnections,LastMatchType);
		}
	MultiplayerOnDestroySessionComplete.Broadcast(bWasSuccessful);
}

void UMultiplayerSessionsSubsystem::OnStartSessionComplete(FName SessionName, bool bWasSuccessful)
{
}
