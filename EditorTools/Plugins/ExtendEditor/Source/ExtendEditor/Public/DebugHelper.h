﻿#pragma once

#include "Misc/MessageDialog.h"
#include "Widgets/Notifications/SNotificationList.h"
#include "Framework/Notifications/NotificationManager.h"

static void Print(const FString& Message, const FColor& Color)
{
    if(GEngine)
        GEngine->AddOnScreenDebugMessage(-1,8.f, Color, Message);
}

static void PrintLog(const FString& Message)
{
    UE_LOG(LogTemp, Warning, TEXT("%s"), *Message);
}

static EAppReturnType::Type ShowMsg(EAppMsgType::Type MsgType, const FString& Msg, bool bShowMsgAsWarning = true)
{
    if(bShowMsgAsWarning)
    {
        FText title = FText::FromString(TEXT("Warning"));
        return FMessageDialog::Open(MsgType, FText::FromString(Msg), &title);
    }

    return FMessageDialog::Open(MsgType, FText::FromString(Msg));
}

static void ShowNotify(const FString& Msg)
{
    FNotificationInfo NotifyInfo(FText::FromString(Msg));
    NotifyInfo.bUseLargeFont = true;
    NotifyInfo.FadeOutDuration = 5.f;

    FSlateNotificationManager::Get().AddNotification(NotifyInfo);
}