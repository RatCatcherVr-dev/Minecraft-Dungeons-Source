#include "Dungeons.h"
#include "game/actor/character/player/PlayerCharacter.h"
#include "game/component/CharacterSerializeComponent.h"
#include "game/actor/character/player/PlayerCommonTypes.h"
#include "PlayerAvatarComponent.h"
#include "game/skins/SkinsUtil.h"
#include "Engine/AssetManager.h"
#include "DungeonsGameInstance.h"
#include "online/entitlements/EntitlementsRepositoryFactory.h"
#include <Themida/Anticheat.hpp>

static const FColor sPlayerIdentityErrorColor(FColor(255, 0, 255));

FEntitledSkin::FEntitledSkin(const FName skinId, const FEntitlement entitlement)
	: SkinId(skinId)
	, Entitlement(entitlement) {
}

UPlayerAvatarComponent::UPlayerAvatarComponent() {
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = false;
	bReplicates = true;	
}

void UPlayerAvatarComponent::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction)
{
	Super::TickComponent(DeltaTime,TickType,ThisTickFunction);

	if (DesiredSkin.SkinId != NAME_None)
	{
		if (SkinStreamingHandle.IsValid())
		{
			if (SkinStreamingHandle->HasLoadCompleted())
			{
				TArray<UObject *> LoadedAssets;
				SkinStreamingHandle->GetLoadedAssets(LoadedAssets);

				UMaterialInstance* pAvatarSkin = Cast<UMaterialInstance>(LoadedAssets[0]);
				UMaterialInstance* pPortraitSkin = Cast<UMaterialInstance>(LoadedAssets[1]);

				//done streaming in skin
				RefreshMeshMaterial(pAvatarSkin);
				UpdatePortrait(pPortraitSkin);
				DesiredSkin.SkinId = NAME_None;
				SetComponentTickEnabled(false);
				SkinStreamingHandle->ReleaseHandle();
				SkinStreamingHandle.Reset();
			}
		}
		else
		{
			//stream skin
			TSoftObjectPtr<UMaterialInstance> AvatarSoftObj = USkinsUtil::GetSkinAvatarMaterialSoft(this, DesiredSkin.SkinId);
			TSoftObjectPtr<UMaterialInstance> PortraitSoftObj = USkinsUtil::GetSkinPortraitMaterialSoft(this, DesiredSkin.SkinId);

			TArray<FSoftObjectPath> TargetsToStream = {AvatarSoftObj.ToSoftObjectPath(), PortraitSoftObj.ToSoftObjectPath()};
			SkinStreamingHandle = UAssetManager::GetStreamableManager().RequestAsyncLoad(TargetsToStream, [](){} ,FStreamableManager::AsyncLoadHighPriority, true,false,TEXT("StreamingSkin"));
		}
	}

}

void UPlayerAvatarComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const {
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(UPlayerAvatarComponent, mColor);
	DOREPLIFETIME(UPlayerAvatarComponent, SelectedSkin);
}

void UPlayerAvatarComponent::BeginPlay() {
	Super::BeginPlay();	
	DesiredSkin = SelectedSkin;
	SetComponentTickEnabled(true);

	if (IsOwnedByLocalPlayer()) {
		GetDungeonsGameInstance()->GetEntitlementsRepository()->OnEntitlementsProvided.AddUObject(this, &UPlayerAvatarComponent::OnEntitlementsProvided);
	}
}

bool UPlayerAvatarComponent::IsOwnedByLocalPlayer() const {
	if (const auto* owner = Cast<APawn>(GetOwner())) {
		return owner->IsLocallyControlled();
	}	
	return false;
}

UDungeonsGameInstance* UPlayerAvatarComponent::GetDungeonsGameInstance() const {
	return GetWorld()->GetGameInstance<UDungeonsGameInstance>();
}

void UPlayerAvatarComponent::OnEntitlementsProvided(const TArray<FEntitlement>& entitlements) {
	DeserializeSkin(entitlements);
}

void UPlayerAvatarComponent::OnLocalPawnPossessed() {
	auto* repository = GetDungeonsGameInstance()->GetEntitlementsRepository();
	DeserializeSkin(repository->GetEntitlements());
}


void UPlayerAvatarComponent::DeserializeSkin(const TArray<FEntitlement>& entitlements) {
	ANTICHEAT_OBFUSCATE_BEGIN

	if (!GetOwner()->GetGameInstance()->IsDedicatedServerInstance()) {
		if (const auto serializedSkin = GetSerializedSkin()) {
			const auto skinId = serializedSkin.GetValue();
			
			if (IgnoreEntitlement) //D11.PS we want to ignore it in situations like Cloud save preview.
			{
				SelectedSkin.SkinId = skinId;
			}
			else if (const auto entitlement = USkinsUtil::FindEntitlement(GetOwner(), entitlements, skinId)) 
			{
				ServerSelectSkin(skinId, entitlement.GetValue());
			}
		}
	}
	ANTICHEAT_OBFUSCATE_END
}


TOptional<FName> UPlayerAvatarComponent::GetSerializedSkin() const {
	auto owner = GetOwner();
	if (owner)
	{
		if (const auto* serializer = owner->FindComponentByClass<UCharacterSerializeComponent>()) {
			if (serializer->HasProfile())
			{
				return serializer->ReadSkin();
			}
			
		}
	}
	return TOptional<FName>();
}

void UPlayerAvatarComponent::RefreshMeshMaterial(UMaterialInstance* pMaterial) const 
{
	if (!pMaterial)
	{
		//not passed in, retrieve in a sync manner
		pMaterial = USkinsUtil::GetSkinAvatarMaterialInstance(this, DesiredSkin.SkinId);
	}

	if (pMaterial)
	{
		OnApplyMeshMaterial.Broadcast(pMaterial);
	}
}

FAvatarData UPlayerAvatarComponent::GetPlayerAvatarData() const {
	return FAvatarData({
		PortraitMaterial,
		mColor,
		FrameBackgroundColor
	});
}

void UPlayerAvatarComponent::SetPortraitMaterial(UMaterialInstance* portrait) {
	PortraitMaterial = portrait;
}

void UPlayerAvatarComponent::OnRep_SelectedSkin() {
	ANTICHEAT_OBFUSCATE_BEGIN
	const auto validateEntitlement = SelectedSkin.SkinId != NAME_None && !GetOwner()->HasAuthority();
	if (!validateEntitlement || IsValidEntitlement(SelectedSkin.SkinId, SelectedSkin.Entitlement)) {

		//stop streaming if we are loading a different skin
		if (SelectedSkin.SkinId != DesiredSkin.SkinId)
		{
			if (SkinStreamingHandle.IsValid())
			{
				SkinStreamingHandle->CancelHandle();
				SkinStreamingHandle.Reset();
			}
		}

		DesiredSkin = SelectedSkin;
		SetComponentTickEnabled(true);
	}
	ANTICHEAT_OBFUSCATE_END
}

void UPlayerAvatarComponent::OnRep_Color() {
	UpdatePortrait();
	OnSetPlayerColor.Broadcast(mColor);
}

void UPlayerAvatarComponent::ServerSelectSkin_Implementation(const FName skinId, const FEntitlement& entitlement) {
	SelectSkin(skinId, entitlement);
}

ANTICHEAT_NO_OPTIMIZATION_BEGIN
bool UPlayerAvatarComponent::ServerSelectSkin_Validate(const FName skinId, const FEntitlement& entitlement) {
	ANTICHEAT_OBFUSCATE_BEGIN
	return skinId.IsNone() || IsValidEntitlement(skinId, entitlement);
	ANTICHEAT_OBFUSCATE_END
}
ANTICHEAT_NO_OPTIMIZATION_END

void UPlayerAvatarComponent::SelectSkin(const FName skinId, const FEntitlement& entitlement) {
	if (!USkinsUtil::IsSkinValid(this, skinId)) {
		return;
	}	

	SelectedSkin = FEntitledSkin(skinId, entitlement);

	if (GetOwner()->HasAuthority()) {
		OnRep_SelectedSkin();
	}
}

const FName& UPlayerAvatarComponent::GetSkinId() const {
	return SelectedSkin.SkinId;
}

void UPlayerAvatarComponent::SetPlayerColorByPlayerNumber(const int playerNumber) {
	SetPlayerColor(USkinsUtil::GetPlayerColor(playerNumber));
}

void UPlayerAvatarComponent::SetPlayerColor(const FColor playerColor) {
	mColor = playerColor;

	if (GetOwner()->HasAuthority()) {
		OnRep_Color();
	}
}

void UPlayerAvatarComponent::UpdatePortrait(UMaterialInstance* pMaterial) {
	if (mColor == sPlayerIdentityErrorColor) {
		UE_LOG(LogDungeons, Warning, TEXT("Player Color not set! will draw with Error color!"));
	}
		
	FrameBackgroundColor = GetFrameBackgroundColor();

	if (!pMaterial)
	{
		//not passed in, retrieve in a sync manner
		pMaterial = USkinsUtil::GetSkinPortraitMaterialInstance(this, DesiredSkin.SkinId);
	}

	if (pMaterial)
	{
		SetPortraitMaterial(pMaterial);
	}
	
	OnPortraitChanged.Broadcast();
}

FColor UPlayerAvatarComponent::GetFrameBackgroundColor() const {
	return {
		CalculateFrameBackgroundColorParameter(mColor.R),
		CalculateFrameBackgroundColorParameter(mColor.G),
		CalculateFrameBackgroundColorParameter(mColor.B),
		mColor.A
	};
}

uint8 UPlayerAvatarComponent::CalculateFrameBackgroundColorParameter(const uint8 colorParameter) const {
	return static_cast<uint8>(static_cast<float>(colorParameter) * BackgroundDarkness);
}

bool UPlayerAvatarComponent::IsValidEntitlement(const FName skinId, const FEntitlement& entitlement) const {
	if (const auto* player = Cast<APlayerCharacter>(GetOwner())) {
		if (const auto playerId = player->GetOnlineUserId()) {
			return !USkinsUtil::IsEntitlementTamperedWith(GetOwner(), skinId, entitlement, playerId.GetValue());
		}		
	}
	return true; // Not a player or not online, always valid.
}
