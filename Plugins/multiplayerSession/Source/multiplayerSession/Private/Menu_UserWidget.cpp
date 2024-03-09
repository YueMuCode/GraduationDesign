// Fill out your copyright notice in the Description page of Project Settings.


#include "Menu_UserWidget.h"

#include "MultiplayerSessionsSubsystem.h"
#include "OnlineSessionSettings.h"
#include "OnlineSubsystem.h"
#include "Components/Border.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Engine/Canvas.h"


void UMenu_UserWidget::MenuSetup(int32 NumberOfPublicConnections,FString TypeOfMatch,FString LobbyPath)
{
	PathToLobby=FString::Printf(TEXT("%s?listen"),*LobbyPath);
	NumPublicConnections=NumberOfPublicConnections;
	MatchType=TypeOfMatch;
	AddToViewport();//将这个UI添加到视口
	SetVisibility(ESlateVisibility::Visible);//设置可见
	bIsFocusable=true;//让这个 UI 元素能够接收焦点，从而能够与用户的输入进行交互，比如接收键盘输入或鼠标点击。

	//设置输入模式
	UWorld* world=GetWorld();
	if(world)
	{
		APlayerController* PlayerController=world->GetFirstPlayerController();//获取这个世界中第一个玩家的控制器
		if(PlayerController)
		{
			FInputModeUIOnly InputModeData;//用于定义 UI 输入模式的结构体对象
			InputModeData.SetWidgetToFocus(TakeWidget());//TakeWidget() 函数用于获取当前 UI 元素的 Widget，并将其设置为输入模式的焦点，以便该 UI 元素能够接收用户的输入。
			InputModeData.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);//允许鼠标在视口中自由移动。
			
			PlayerController->SetInputMode(InputModeData);
			PlayerController->SetShowMouseCursor(true);//显示鼠标光针
		}
	}

	//游戏实例（Game Instance）创建完成之后，会自动创建一个游戏实例子系统（Game Instance Subsystem）
	//所有可以从GameInstance中访问到GameInstanceSubsystem
	UGameInstance* GameInstance=GetGameInstance();
	if(GameInstance)
	{
		MultiplayerSessionsSubsystem= GameInstance->GetSubsystem<UMultiplayerSessionsSubsystem>();
	}

	if(MultiplayerSessionsSubsystem	)//委托订阅函数
		{
		MultiplayerSessionsSubsystem->MultiplayerOnCreateSessionComplete.AddDynamic(this,&ThisClass::OnCreateSession);
		MultiplayerSessionsSubsystem->MultiplayerOnFindSessionComplete.AddUObject(this,&ThisClass::OnFindSession);
		MultiplayerSessionsSubsystem->MultiplayerOnJoinSessionComplete.AddUObject(this,&ThisClass::OnJoinSession);
		MultiplayerSessionsSubsystem->MultiplayerOnDestroySessionComplete.AddDynamic(this,&ThisClass::OnDestroySession);
		MultiplayerSessionsSubsystem->MultiplayerOnStartSessionComplete.AddDynamic(this,&ThisClass::OnStartSession);
		}
	
}

void UMenu_UserWidget::MenuTearDown()
{
	RemoveFromParent();
	UWorld* World=GetWorld();
	if(ensure(World))
	{
		APlayerController* PlayerController=World->GetFirstPlayerController();
		if(PlayerController)
		{
			FInputModeGameOnly InputModeData;
			PlayerController->SetInputMode(InputModeData);
			PlayerController->SetShowMouseCursor(false);
		}
	}
}

bool UMenu_UserWidget::Initialize()
{
	if(!Super::Initialize())
	{
		return false;
	}
	if(HostButton)
	{
		HostButton->OnClicked.AddDynamic(this,&ThisClass::HostButtonClicked);
	}
	if(SearchButton)
	{
		SearchButton->OnClicked.AddDynamic(this,&ThisClass::SearchButtonClicked);
	}
	if(JoinButton)
	{
		JoinButton->OnClicked.AddDynamic(this,&ThisClass::JoinButtonClicked);
	}
	if(CloseUIButton)
	{
		CloseUIButton->OnClicked.AddDynamic(this,&ThisClass::CloseButtonClicked);
	}
	return true;
}

//切换level之后恢复鼠标模式
void UMenu_UserWidget::NativeDestruct()
{
	MenuTearDown();
	Super::NativeDestruct();
}

void UMenu_UserWidget::OnCreateSession(bool bWasSuccessful)
{
	if(bWasSuccessful)
	{
		
		//创建好会话之后，当结果回调到menu时立马进入大厅
		UWorld* World=GetWorld();
		if(World)
		{
			
			World->ServerTravel(PathToLobby);//服务器端执行旅行操作，切换当前服务器上运行的地图或连接到其他服务器
		}
		if(GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1,15.0f,FColor::Cyan,FString(TEXT("会话创建成功")));
		}
	}
	else
	{
		if(GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1,15.0f,FColor::Red,FString(TEXT("创建会话失败，无法前往大厅")));
		}
		HostButton->SetIsEnabled(true);
	}
}

void UMenu_UserWidget::OnFindSession(const TArray<FOnlineSessionSearchResult>& SessionResults, bool bWasSuccessful)
{
	//可以在这里设置显示搜索列表
	if(MultiplayerSessionsSubsystem==nullptr)
	{
		return;
	}
	//遍历查找到的会话，符合条件的就加入
	for(auto Result:SessionResults)
	{
		FString SettingValue;
		Result.Session.SessionSettings.Get(FName("MatchType"),SettingValue);
		if(SettingValue==MatchType)
		{
			//FindMessageText->SetText(FText::FromString(TEXT("搜索结果如下")));
			//搜索得到结果之后，显示房间名称以及加入按钮
			FindMessageBorder->SetVisibility(ESlateVisibility::Hidden);
			CloseUIButton->SetVisibility(ESlateVisibility::Visible);
			JoinBorder->SetVisibility(ESlateVisibility::Visible);
			JoinButton->SetVisibility(ESlateVisibility::Visible);
			JoinTitle->SetVisibility(ESlateVisibility::Visible);
			SessionName->SetText(FText::FromString(SettingValue));//将按钮显示房间名
			SearchResult=Result;
			//MultiplayerSessionsSubsystem->JoinSession(Result);
			return;
		}
	}
	if(!bWasSuccessful||SessionResults.Num()==0)
	{
		SearchButton->SetIsEnabled(true);

		//设置textblock空间的文本属性
		CloseUIButton->SetVisibility(ESlateVisibility::Visible);
		FindMessageText->SetText(FText::FromString(TEXT("没有查找到对应的房间，请确认你已经打开了steam并且你的好友已经创建好了房间")));
		FSlateFontInfo FontInfo = FindMessageText->Font;
		FontInfo.Size=50;
		FindMessageText->SetFont(FontInfo);
		FindMessageText->SetAutoWrapText(true);
	}
}

void UMenu_UserWidget::OnJoinSession(EOnJoinSessionCompleteResult::Type Result)
{
	IOnlineSubsystem* Subsystem=IOnlineSubsystem::Get();
	if(Subsystem)
	{
		IOnlineSessionPtr SessionInterface=Subsystem->GetSessionInterface();
		if(SessionInterface.IsValid())
		{
			FString Address;
			//执行到这里，说明客户端已经连接到服务器，也就是说，此时的NAME_GameSession的值就是目标会话的名字，也就能获取他对应的地址了
			SessionInterface->GetResolvedConnectString(NAME_GameSession,Address);
			APlayerController* PlayerController=GetGameInstance()->GetFirstLocalPlayerController();
			if(PlayerController)
			{
				PlayerController->ClientTravel(Address,TRAVEL_Absolute);
			}
		}
	}
	else
	{
		if(GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1,4.0f,FColor::Orange,FString(TEXT("Subsystem为空，加入失败")));
			JoinMessageText->SetText(FText::FromString(TEXT("加入失败")));
		}
	}

	if(Result!=EOnJoinSessionCompleteResult::Success)
	{
		SearchButton->SetIsEnabled(true);
		CloseUIButton->SetVisibility(ESlateVisibility::Visible);
		JoinMessageText->SetText(FText::FromString(TEXT("加入失败")));
	}
}

void UMenu_UserWidget::OnDestroySession(bool bWasSuccessful)
{
}

void UMenu_UserWidget::OnStartSession(bool bWasSuccessful)
{
}

void UMenu_UserWidget::HostButtonClicked()
{
	HostmessageBorder->SetVisibility(ESlateVisibility::Visible);
	//CloseUIButton->SetVisibility(ESlateVisibility::Visible);
	HostButton->SetIsEnabled(false);
	//屏幕调试信息
	if(GEngine)
	{
		GEngine->AddOnScreenDebugMessage(
			-1,
			15.f,
			FColor::Yellow,
			FString((TEXT("创建按钮点击了，正在试图创建会话")))
			);
	}
	
	//正式调用前面创建的函数创建联机会话
	if(MultiplayerSessionsSubsystem)
	{
		MultiplayerSessionsSubsystem->CreateSession(NumPublicConnections,MatchType);//创建一个名为“FreeForAll”的容纳四个人的会话
	}
}

void UMenu_UserWidget::SearchButtonClicked()
{
	FindMessageBorder->SetVisibility(ESlateVisibility::Visible);
	//CloseUIButton->SetVisibility(ESlateVisibility::Visible);
	SearchButton->SetIsEnabled(false);
	if(GEngine)
	{
		GEngine->AddOnScreenDebugMessage(
			-1,
			15.f,
			FColor::Yellow,
			FString((TEXT("加入按钮点击了，正在试图加入会话")))
			);
	}
	if(MultiplayerSessionsSubsystem)
	{
		MultiplayerSessionsSubsystem->FindSessions(10000);
	}
}

void UMenu_UserWidget::JoinButtonClicked()
{
	JoinMessageBorder->SetVisibility(ESlateVisibility::Visible);
	CloseUIButton->SetVisibility(ESlateVisibility::Hidden);
	JoinBorder->SetVisibility(ESlateVisibility::Hidden);
	JoinButton->SetVisibility(ESlateVisibility::Hidden);
	JoinTitle->SetVisibility(ESlateVisibility::Hidden);
	if(MultiplayerSessionsSubsystem==nullptr)
	{
		return;
	}
	MultiplayerSessionsSubsystem->JoinSession(SearchResult);
}

void UMenu_UserWidget::CloseButtonClicked()
{
	JoinMessageBorder->SetVisibility(ESlateVisibility::Hidden);
	FindMessageBorder->SetVisibility(ESlateVisibility::Hidden);
	JoinBorder->SetVisibility(ESlateVisibility::Hidden);
	JoinTitle->SetVisibility(ESlateVisibility::Hidden);
	CloseUIButton->SetVisibility(ESlateVisibility::Hidden);
}
                                                                                                