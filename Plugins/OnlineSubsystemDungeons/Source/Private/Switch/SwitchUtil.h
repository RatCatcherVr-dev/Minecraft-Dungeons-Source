#pragma once

#include <nn/account/account_Result.h>
#include "OnlineAsyncTaskManagerDungeons.h"

namespace SwitchUtil
{

typedef TFunction < void(bool, bool)> FSwitchNSCheckResultFunc;

class FDungeonsSwitchNSACheck : public FOnlineAsyncTaskDungeons
{
public:
	FDungeonsSwitchNSACheck(int LocalUserNum, FSwitchNSCheckResultFunc func);
	void Initialize() override;
	void Finalize() override;
	virtual void Tick();
protected:
	bool CheckOnline();
	nn::account::Uid Uid;
	nn::account::UserHandle UserHandle;
	nn::account::NetworkServiceAccountId NetworkServiceAccountId;
	nn::Result NetworkServiceAccountIdTokenResult;
	FString NetworkServiceAccountIdToken;
	FSwitchNSCheckResultFunc ResultFunc;
	bool Offline;
};


class FDungeonsSwitchNSOCheck : public FDungeonsSwitchNSACheck
{
public:
	FDungeonsSwitchNSOCheck(int LocalUserNum, FSwitchNSCheckResultFunc func);
	void Tick() override;
};




}