#include "CMenu.h"
#include "Components/Button.h"
#include "Global.h"
#include "Components/WidgetSwitcher.h"
#include "Components/ScrollBox.h"
#include "Components/TextBlock.h"
#include "Components/EditableTextBox.h"
#include "CSessionRow.h"

UCMenu::UCMenu(const FObjectInitializer& ObjectInitializer)
{
	CHelpers::GetClass(&SessionRowClass, "WidgetBlueprint'/Game/Widgets/WB_SessionRow.WB_SessionRow_C'");
}

bool UCMenu::Initialize()
{
	bool bSuccess = Super::Initialize();
	CheckFalseResult(bSuccess, false);

	// WB_Menu -> How to get HostButton
	//for(Cast<UPannelWidget>(GetRootWidget())->GetAllChildren);	// 자식 1, 2, 3... 버튼타입인지 계속 캐스팅하면서 찾아야 함(뻘짓)

	//for(auto component : Cast<UPanelWidget>(GetRootWidget())->GetAllChildren())
	//{
	//	for (Cast<UPanelWidget>(component)->GetAllChildren())
	//	{
	//		for
	//			for
	//				for...
	//	}
	//}

	CheckNullResult(HostButton, false);
	HostButton->OnClicked.AddDynamic(this, &UCMenu::OpenHostMenu);

	CheckNullResult(CancelHostMenuButton, false);
	CancelHostMenuButton->OnClicked.AddDynamic(this, &UCMenu::OpenMainMenu);

	CheckNullResult(ConfirmHostMenuButton, false);
	ConfirmHostMenuButton->OnClicked.AddDynamic(this, &UCMenu::HostServer);

	CheckNullResult(JoinButton, false);
	JoinButton->OnClicked.AddDynamic(this, &UCMenu::OpenJoinMenu);

	CheckNullResult(CancelJoinMenuButton, false);
	CancelJoinMenuButton->OnClicked.AddDynamic(this, &UCMenu::OpenMainMenu);

	CheckNullResult(ConfirmJoinMenuButton, false);
	ConfirmJoinMenuButton->OnClicked.AddDynamic(this, &UCMenu::JoinServer);

	CheckNullResult(QuitButton, false);
	QuitButton->OnClicked.AddDynamic(this, &UCMenu::QuitGame);

	return true;
}

void UCMenu::SetSessionList(TArray<FSessionData> InSessionDatas)
{
	UWorld* world = GetWorld();
	CheckNull(world);

	SessionList->ClearChildren();

	uint32 i = 0;
	for (const auto& data : InSessionDatas)
	{
		UCSessionRow* sessionRow = CreateWidget<UCSessionRow>(world, SessionRowClass);
		CheckNull(sessionRow);

		sessionRow->SessionName->SetText(FText::FromString(data.Name));
		sessionRow->HostUserName->SetText(FText::FromString(data.HostUserName));

		FString fractionStr = FString::Printf(L"%d/%d", data.CurrentPlayers, data.MaxPlayers);
		sessionRow->ConnectionFractions->SetText(FText::FromString(fractionStr));

		sessionRow->SetSelfIndex(this, i++);

		SessionList->AddChild(sessionRow);
	}
}

void UCMenu::SetSelectedRowIndex(uint32 InIndex)
{
	// 기본 값이 nullptr
	SelectedRowIndex = InIndex;

	SelectedSessionRow();
}

void UCMenu::HostServer()
{
	//GetOwner
	CheckNull(OwingGameInstance);

	// 동일한 세션이름으로 생성되지 않게 하려면, TArray에 sessionName을 저장한 후에 sessionName과 이름이 같다면 생성X
	FString sessionName = SessionNameText->GetText().ToString();

	OwingGameInstance->Host(sessionName);

	CLog::Log("Host Button Pressed");
}

void UCMenu::JoinServer()
{
	CheckNull(OwingGameInstance);

	if (SelectedRowIndex.IsSet())
	{
		CLog::Log("SelectedRowIndex : " + FString::FromInt(SelectedRowIndex.GetValue()));

		OwingGameInstance->Join(SelectedRowIndex.GetValue());
	}
	else
	{
		CLog::Log("SelectedRowIndex is not set");
	}


	CLog::Log("Join Button Pressed");
}

void UCMenu::OpenJoinMenu()
{
	CheckNull(MenuSwitcher);
	CheckNull(JoinMenu);
	MenuSwitcher->SetActiveWidget(JoinMenu);

	if(!!OwingGameInstance)
		OwingGameInstance->FindSession();
}

void UCMenu::OpenMainMenu()
{
	CheckNull(MenuSwitcher);
	CheckNull(MainMenu);
	MenuSwitcher->SetActiveWidget(MainMenu);
}

void UCMenu::OpenHostMenu()
{
	CheckNull(MenuSwitcher);
	CheckNull(HostMenu);
	MenuSwitcher->SetActiveWidget(HostMenu);
}

void UCMenu::QuitGame()
{
	UWorld* world = GetWorld();
	CheckNull(world);
	
	APlayerController* controller = world->GetFirstPlayerController();
	
	controller->ConsoleCommand("Quit");
}

void UCMenu::SelectedSessionRow()
{
	for (int32 i = 0; i < SessionList->GetChildrenCount(); i++)
	{
		UCSessionRow* sessionRow = Cast<UCSessionRow>(SessionList->GetChildAt(i));
		if (!!sessionRow)
			sessionRow->bSelfClicked = (SelectedRowIndex.IsSet() && i == SelectedRowIndex);
	}
}
