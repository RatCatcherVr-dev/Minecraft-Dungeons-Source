#include "Dungeons.h"

#include "game/SaveConstants.h"
#include "PlayerControllerBase.h"
#include "DungeonsGameInstance.h"
#include "save/GlobalSaveData.h"
#include "save/CharacterSaveData.h"
#include "game/component/CharacterSerializeComponent.h"
#include "CoreOnline.h"
#include "GameFramework/PlayerController.h"
#include "online/sessions/OnlineUtil.h"
#include <Engine/LocalPlayer.h>
#include "game/util/ValueFormat.h"
#include "DungeonsLocalPlayer.h"
#include "game/avatar/AvatarInfo.h"
#include "game/skins/SkinsUtil.h"
#include "GameSettingsFunctionLibrary.h"
#include "DungeonsUserManagement.h"

#ifdef ENABLE_BOTAUTOMATION
#include "BotAutomationPCH.h"
#endif

APlayerControllerBase::APlayerControllerBase(const FObjectInitializer& ObjectInitializer /* = FObjectInitializer::Get() */) 
	: Super(ObjectInitializer)
{
	mCharacterSerializeComponent = CreateDefaultSubobject<UCharacterSerializeComponent>(TEXT("CharacterSerialize"));
}

const TSharedPtr<const FUniqueNetId> APlayerControllerBase::GetUniqueNetId() const {

	//D11.KS Lets check whether this player has a valid uniqueNetID, if so we can load their own data.
	if(auto localPlayer = GetLocalPlayer())
	{
		TSharedPtr<const FUniqueNetId> netId = localPlayer->GetPreferredUniqueNetId().GetUniqueNetId();

		if (netId.IsValid() && netId->IsValid()) {
			return localPlayer->GetCachedUniqueNetId().GetUniqueNetId();
		}
	}
	
	//D11.KS - Get the initial players netid if we can't get a valid one.
	auto userManagement = GetWorld()->GetGameInstance<UDungeonsGameInstance>()->GetUserManager();
	const ULocalPlayer* initialPlayer = userManagement->GetInitialLocalPlayer();

	if (initialPlayer) {
		//D11.KS - Local Players currently have no UniqueID. Use the main player for now.
		TSharedPtr<const FUniqueNetId> netId = initialPlayer->GetPreferredUniqueNetId().GetUniqueNetId();

		if (netId.IsValid() && netId->IsValid()) {
			return initialPlayer->GetCachedUniqueNetId().GetUniqueNetId();
		}
	}


	return nullptr;
}

FString APlayerControllerBase::GetRecentSaveDataKey() const {
	
	auto userManagement = GetWorld()->GetGameInstance<UDungeonsGameInstance>()->GetUserManager();
	const ULocalPlayer* initialPlayer = userManagement->GetInitialLocalPlayer();
	FString uniquePersistantString;

	if (initialPlayer)
	{
		if (online::usingNullSubsystem(GetWorld())) {
			int initialUser = userManagement->GetInitialUserSystemId();
			uniquePersistantString = userManagement->GetLocalUserName(initialUser);
		}
		else {
			TSharedPtr<const FUniqueNetId> netId = GetUniqueNetId();

			uniquePersistantString = ANSI_TO_TCHAR("TOFIX");
		}

		ULocalPlayer* localPlayer = GetLocalPlayer();
		if (localPlayer && localPlayer != initialPlayer)
		{
			return uniquePersistantString +"_" + FString::FromInt(localPlayer->GetControllerId());
		}
		else if(!localPlayer)
		{
			UE_LOG(LogTemp, Error, TEXT("Doesn't have a local player."));
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("No initial player."));
	}
	return uniquePersistantString;
}

void APlayerControllerBase::BeginPlay()
{
	Super::BeginPlay();
	UpdateControllerType();
	UpdateLocalPlayerDisplayIndex();
	InitialiseControllerTypeBinding();
	InitialiseMouseLockMode();
}


void APlayerControllerBase::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (PlayerInput) {
		// D11.DB - Unbind everything.
		TArray<FKeybinding> keybinds;
		UKeybindHelper::GetSavedKeybinds(this, keybinds, prevControllerType);
		UnbindKeys(keybinds);
	}
	if (auto gameInstance = Cast<UDungeonsGameInstance>(GetGameInstance()))
	{
		if (auto controllerTypeManager = gameInstance->GetControllerTypeManager())
		{
			controllerTypeManager->OnControllerTypeChanged.RemoveDynamic(this, &APlayerControllerBase::ControllerTypeChanged);
		}
	}
	if (auto world = GetWorld()) {
		world->GetTimerManager().ClearTimer(mControllerFlashHandle);
	}
}

void APlayerControllerBase::LoadSaveData(){
	TryLoadCharacterSaveData();	
}

TArray<UCharacterSaveData*> GetAllProfiles(int32 LocalUserNum, UWorld* world) {
	// the editor creates a controller and sets a pawn on it, not having a game instance.
	if (auto gi = Cast<UDungeonsGameInstance>(world->GetGameInstance())) {
		if (gi->IsSaveStateValid(LocalUserNum)) {
			return gi->EditGlobalSaveState(LocalUserNum)->GetAllProfiles();
		}
	}
	return {};
}

void APlayerControllerBase::TryLoadCharacterSaveData() {
	if (UDungeonsGameInstance* gi = Cast<UDungeonsGameInstance>(GetWorld()->GetGameInstance())) {

		if (HasSaveData()) {
			return;
		}

		// Happens because menu controller has a camera pawn weirdo character.
		// or if you play in editor without platform login
		if (!gi->IsSaveStateValid(GetSaveLocalUserNum())) {
#if WITH_EDITOR 
			gi->LoadPlayerControllerGlobalSaveDataInEditor();
#else
			UE_LOG(LogTemp, Error, TEXT("Failed to ensure save data."));
			return;
#endif
		}		

		// get most recent profile if there is one:
		int32 recentIndex = GetRecentSaveDataIndex();
		if (UCharacterSaveData* characterSaveData = GetAvailableSaveDataByIndex(recentIndex)) {
			AssignSaveData(characterSaveData);
			return;
		}
		// find first existing saveFile:
		else {
			int32 index = 0;
			for (UCharacterSaveData* fallback : GetAllProfiles(GetSaveLocalUserNum(), GetWorld())) {
				if (GetIsCharacterSlotIndexSelectable(index)) {
					AssignSaveData(fallback);
					return;
				}
				index++;
			}			
		}

		OnNoSaveDataFound.Broadcast();
	}
}

const FText& APlayerControllerBase::GetLocalPlayerDisplayText() const {
	return mLocalPlayerDisplayText;
}

void APlayerControllerBase::SetCoopGamepadLightColourIndex(int32 index)
{
	GetWorld()->GetTimerManager().ClearTimer(mControllerFlashHandle);

	if (GetGameInstance<UDungeonsGameInstance>()->IsLocalCoop())
	{
		if (index > -1 && index < 4)
		{
			SetControllerLightColor(PlayerGamepadColors[index]);
		}
	}
	else
	{
		ResetControllerLightColor();
	}
}

void APlayerControllerBase::SetGamepadLightColourForDuration(FColor colour, float duration) {
	if (!IsLocalController())
		return;

	SetControllerLightColor(colour);
	if (duration > 0) {
		int playerNum = 1;
		if (auto* playerPawn = GetPawn()) {
			if (auto* playerCharacter = Cast<APlayerCharacter>(playerPawn)) {
				playerNum = playerCharacter->GetDungeonsBasePlayerState()->GetPlayerNumber();
			}
		}
		
		GetWorld()->GetTimerManager().SetTimer(mControllerFlashHandle, FTimerDelegate::CreateUObject(this, &APlayerControllerBase::SetCoopGamepadLightColourIndex, playerNum), duration, false);
	}
}

void APlayerControllerBase::SetPawn(APawn* InPawn) {
	if (InPawn && IsLocalController()) {
		if (UCharacterSerializeComponent* serializer = InPawn->FindComponentByClass<UCharacterSerializeComponent>()) {
			// only care about save data if this pawn has savedata (menu pawn doesnt)

			TryLoadCharacterSaveData();	

#if WITH_EDITOR	
			//When playing in the editor, you can skip menu and start right into lobby or ingame.
			//You might not have any characters created at this point, So we allow creating them as fallback.
			if (!HasSaveData()) {
				if (UGlobalStateData* saveData = EditGlobalSaveState()) {
					// create some save data.
					if (UCharacterSaveData* newProfile = saveData->CreateNewCharacterProfile(GetSaveLocalUserNum())) {
						AssignSaveData(newProfile);						
					}
				}
			}
#endif
			ensure(HasSaveData());

			serializer->AssignCharacter(GetSaveData());
		}
	}
	Super::SetPawn(InPawn);

	// #D11.CM - Flush any lingering gamepad binds if we are a local player
	if (GetLocalPlayer()) {
		TArray<FKeybinding> list;
		UKeybindHelper::GetSavedKeybinds(this, list, EDungeonsControllerType::Controller_Invalid);
		if (list.Num() == 0) {
			UKeybindHelper::GetDefaultConfig(EDungeonsControllerType::Controller_Invalid, list);
		}
		UnbindKeys(list);
	}

	//D11.PS
	UpdateControllerType();
	InitialiseKeybinds();
}

void APlayerControllerBase::InitialiseControllerTypeBinding()
{
	if (auto gameInstance = Cast<UDungeonsGameInstance>(GetGameInstance()))
	{
		if (auto controllerTypeManager = gameInstance->GetControllerTypeManager())
		{
			if (auto localPlayer = GetLocalPlayer())
			{
				// D11.DJB Try to remove an existing binding just in case, so we don't bind twice.
				controllerTypeManager->OnControllerTypeChanged.RemoveDynamic(this, &APlayerControllerBase::ControllerTypeChanged);

				controllerTypeManager->OnControllerTypeChanged.AddDynamic(this, &APlayerControllerBase::ControllerTypeChanged);
				prevControllerType = controllerTypeManager->GetControllerType(localPlayer->GetControllerId());
			}
		}
	}
}

void APlayerControllerBase::ControllerTypeChanged()
{
	if (auto gameInstance = Cast<UDungeonsGameInstance>(GetGameInstance()))
	{
		if (auto controllerTypeManager = gameInstance->GetControllerTypeManager())
		{
			if (auto localPlayer = GetLocalPlayer())
			{
				auto controllerType = gameInstance->GetControllerTypeManager()->GetControllerType(localPlayer->GetControllerId());

#if PLATFORM_SWITCH
				// D11.DJB Handle controller disconnects on Switch
				int localUserIndex = gameInstance->GetUserManager()->GetLocalPlayerIndex(localPlayer);
				if (controllerType == EDungeonsControllerType::Controller_Invalid)
				{
					// No controller present for this PlayerController, this will trigger the local MP player to begin the drop-out process.
					gameInstance->GetControllerTypeManager()->OnControllerConnectionChangedDelegate.Broadcast(false, localUserIndex);
				}
				else
				{
					// This will allow a controller to reconnect during the grace period.
					gameInstance->GetControllerTypeManager()->OnControllerConnectionChangedDelegate.Broadcast(true, localUserIndex);
				}
#endif
				if (prevControllerType != controllerType)
				{
					TArray<FKeybinding> list;
					UKeybindHelper::GetSavedKeybinds(this, list, prevControllerType);
					if (list.Num() == 0) {
						UKeybindHelper::GetDefaultConfig(prevControllerType, list);
					}
					UnbindKeys(list);

					list.Empty();

					if (controllerType != EDungeonsControllerType::Controller_Invalid)
					{
						UKeybindHelper::GetSavedKeybinds(this, list, controllerType);
						if (list.Num() == 0) {
							UKeybindHelper::GetDefaultConfig(controllerType, list);
						}
						else {
							// D11.SSN - make sure to unbind any leftover default binds for this controller type.
							TArray<FKeybinding> defaultList;
							UKeybindHelper::GetDefaultConfig(controllerType, defaultList);
							UnbindKeys(defaultList);
						}
						BindKeys(list);
					}

					prevControllerType = controllerType;
				}
			}
		}
	}
}

void APlayerControllerBase::UpdateControllerType()
{
	if (auto gameInstance = Cast<UDungeonsGameInstance>(GetGameInstance()))
	{
		ULocalPlayer* localplayer = GetLocalPlayer();

		int InitialUser = gameInstance->GetUserManager()->GetInitialUserSystemId();

		if (localplayer && InitialUser == localplayer->GetControllerId())
		{
			auto controllerTypeManager = gameInstance->GetControllerTypeManager();
			if (controllerTypeManager->GetControllerType(GetLocalPlayer()->GetControllerId()) == EDungeonsControllerType::Controller_MouseAndKeyboard)
			{
				OnGamepadActiveChanged(false);
			}
			else
			{
				OnGamepadActiveChanged(true);
			}

			controllerTypeManager->SetFirstPlayerController(this);
		}
		else
		{
			OnGamepadActiveChanged(true);
		}
	}
}

void APlayerControllerBase::InitialiseKeybinds()
{
	ULocalPlayer* localPlayer = GetLocalPlayer();
	UDungeonsGameInstance* gameInstance = GetGameInstance<UDungeonsGameInstance>();

	if (!PlayerInput || !localPlayer || !gameInstance)
	{
		return;
	}

	if (auto controllerTypeManager = gameInstance->GetControllerTypeManager())
	{
		auto controllerType = controllerTypeManager->GetControllerType(localPlayer->GetControllerId());

		// D11.DB - Initialise keybinds.
		TArray<FKeybinding> keybinds;
		UKeybindHelper::GetSavedKeybinds(this, keybinds, controllerType);
		if (keybinds.Num() == 0)
		{
			// D11.DB - If we don't have any binds for the current controller type then we need
			//			to initialize the keybinds (first boot scenario).
			UKeybindHelper::InitializeKeybinds(this);
		}
		else
		{
			UKeybindHelper::SetSavedKeybinds(this, keybinds, controllerType);
		}
	}
}

void APlayerControllerBase::ReceivedPlayer()
{
	Super::ReceivedPlayer();

	UpdateControllerType();
	UpdateLocalPlayerDisplayIndex();
	InitialiseControllerTypeBinding();
}

void APlayerControllerBase::InitialiseMouseLockMode()
{
	mUserMouseLockMode = UGameSettingsFunctionLibrary::GetUserMouseLockMode(this);
}

void APlayerControllerBase::SetInputMode(const FInputModeDataBase& InData)
{
	Super::SetInputMode(InData);

	if (UGameViewportClient* GameViewportClient = GetWorld()->GetGameViewport())
	{
		if (mUserMouseLockMode == EMouseLockMode::LockOnCapture /*'Smart Cursor' setting*/)
		{
			if (const ABasePlayerController* BasePlayerController = Cast<ABasePlayerController>(this))
			{
				GameViewportClient->SetMouseLockMode(BasePlayerController->IsInputCapturedByUI() ? EMouseLockMode::DoNotLock : EMouseLockMode::LockAlways);
				return;
			}
			/*
			 * If this cast fails then we are likely in the main menu where we do not have a ABasePlayerController.
			 * Therefore we are in a UI and in the spirit of 'smart cursor' we should not lock to the viewport.
			 */
			GameViewportClient->SetMouseLockMode(EMouseLockMode::DoNotLock);
			return;
		}
		GameViewportClient->SetMouseLockMode(mUserMouseLockMode);
	}
}

// D11.DJB
void APlayerControllerBase::GetInputAnalogStickStateRadialDeadzone(EControllerAnalogStick::Type WhichStick, float& StickX, float& StickY) const
{
	GetInputAnalogStickRawState(WhichStick, StickX, StickY);
	FInputAxisProperties properties;
	if (PlayerInput)
	{
		switch (WhichStick)
		{
		case EControllerAnalogStick::CAS_LeftStick:
			PlayerInput->GetAxisProperties(EKeys::Gamepad_LeftX, properties);
			break;
		case EControllerAnalogStick::CAS_RightStick:
			PlayerInput->GetAxisProperties(EKeys::Gamepad_RightX, properties);
			break;
		default:
			checkNoEntry();
		}
	}
	CalculateScaledRadialDeadzone(StickX, StickY, properties.DeadZone);
}

// D11.JPhoenix
void APlayerControllerBase::GetInputDirectionalPadState(float& ValueX, float& ValueY) const
{
	if (PlayerInput)
	{
		ValueX -= PlayerInput->GetRawKeyValue(EKeys::Gamepad_DPad_Left);
		ValueX += PlayerInput->GetRawKeyValue(EKeys::Gamepad_DPad_Right);

		ValueY -= PlayerInput->GetRawKeyValue(EKeys::Gamepad_DPad_Up);
		ValueY += PlayerInput->GetRawKeyValue(EKeys::Gamepad_DPad_Down);
	}
	else
	{
		ValueX = ValueY = 0.0f;
	}
}

void APlayerControllerBase::GetInputAnalogStickRawState(EControllerAnalogStick::Type WhichStick, float& StickX, float& StickY) const
{
	if (PlayerInput)
	{
		switch (WhichStick)
		{
		case EControllerAnalogStick::CAS_LeftStick:
			
			StickX = PlayerInput->GetRawKeyValue(EKeys::Gamepad_LeftX);
			StickY = PlayerInput->GetRawKeyValue(EKeys::Gamepad_LeftY);
			break;

		case EControllerAnalogStick::CAS_RightStick:
			
			StickX = PlayerInput->GetRawKeyValue(EKeys::Gamepad_RightX);
			StickY = PlayerInput->GetRawKeyValue(EKeys::Gamepad_RightY);
			break;

		default:
			StickX = StickY = 0.f;
		}
	}
	else
	{
		StickX = StickY = 0.f;
	}
}

void APlayerControllerBase::CalculateScaledRadialDeadzone(float& AxisX, float& AxisY, const float& AxisDeadzone) const
{
	FVector2D AxisInput = FVector2D(AxisX, AxisY);
	float inputAxisMagnitudeSq = AxisInput.SizeSquared(); 
	if (inputAxisMagnitudeSq < FMath::Square(AxisDeadzone))
	{
		AxisX = AxisY = 0.f;
	}
	else
	{
		AxisInput.Normalize();
		AxisInput *= ((FMath::Sqrt(inputAxisMagnitudeSq) - AxisDeadzone) / (1 - AxisDeadzone));

		AxisX = AxisInput.X;
		AxisY = AxisInput.Y;
	}
}

void APlayerControllerBase::SetUserMouseLockMode(const EMouseLockMode InMode)
{
	mUserMouseLockMode = InMode;
}

bool APlayerControllerBase::IsInitialized() const {
	return bInitialized;
}

void APlayerControllerBase::SetInitialized(const bool initialized) {
	bInitialized = initialized;
}

// D11.DB - Binds all of the provided keybinds.
void APlayerControllerBase::BindKeys( const TArray<FKeybinding>& InKeybindList )
{
	if( !PlayerInput ) return;

	for( auto& bind : InKeybindList )
	{
		if( bind.Key.IsFloatAxis() )
		{
			FInputAxisKeyMapping Axis( ConvertBindingName(bind), bind.Key );
			PlayerInput->AddAxisMapping( Axis );
		}
		else
		{
			FInputActionKeyMapping Action( ConvertBindingName(bind), bind.Key );
			PlayerInput->AddActionMapping( Action );
			// D11.SSN - on mouse & keyboard, change the movement button to be the same as the main attack button.
			if (bind.Name == "MainAttack" && bind.Type == EKeybindPlatform::KeyboardMouse) {
				FKeybinding bind2("SetDestination", bind.Key, bind.Type);
				FInputActionKeyMapping Action2(ConvertBindingName(bind2), bind2.Key);
				PlayerInput->AddActionMapping(Action2);
			}
		}
	}
}

// D11.DB - Unbinds all of the provided keybinds.
void APlayerControllerBase::UnbindKeys( const TArray<FKeybinding>& InKeybindList )
{
	if( !PlayerInput ) return;

	for( auto& bind : InKeybindList )
	{
		if( bind.Key.IsFloatAxis() )
		{
			FInputAxisKeyMapping Axis( ConvertBindingName(bind), bind.Key );
			PlayerInput->RemoveAxisMapping( Axis );
		}
		else
		{
			FInputActionKeyMapping Action( ConvertBindingName(bind), bind.Key );
			PlayerInput->RemoveActionMapping( Action );
			// D11.SSN - on mouse & keyboard, change the movement button to be the same as the main attack button.
			if (bind.Name == "MainAttack" && bind.Type == EKeybindPlatform::KeyboardMouse) {
				FKeybinding bind2("SetDestination", bind.Key, bind.Type);
				FInputActionKeyMapping Action2(ConvertBindingName(bind2), bind2.Key);
				PlayerInput->RemoveActionMapping(Action2);
			}
		}
	}
}

#ifdef ENABLE_BOTAUTOMATION
void APlayerControllerBase::PreProcessInput(const float DeltaTime, const bool bGamePaused)
{
	if (InputAutomator.IsValid())
	{
		InputAutomator->UpdateAutomation(DeltaTime);
	}
}
#endif

int32 APlayerControllerBase::GetSaveLocalUserNum() const
{
	//This only gets the initial user for now, but we should be able to change this function to allow saves to all signed in profiles
	if (UDungeonsGameInstance* gi = Cast<UDungeonsGameInstance>(GetWorld()->GetGameInstance()))
	{
		auto* dungeonsLocalPlayer = Cast<UDungeonsLocalPlayer>(GetLocalPlayer());

#if PLATFORM_WINDOWS || PLATFORM_SWITCH
		return 0;
#elif PLATFORM_XBOXONE || PLATFORM_PS4
		auto const Identity = online::getIdentityInterface();

		//Check if we are a guest
		if (Identity && dungeonsLocalPlayer && Identity->GetSponsorUniquePlayerId(dungeonsLocalPlayer->GetSystemUserId()))
		{
			return Cast<UDungeonsGameInstance>(GetGameInstance())->GetUserManager()->GetInitialUserSystemId();
		}
#endif
		if (dungeonsLocalPlayer) {
			return dungeonsLocalPlayer->GetSystemUserId();
		}
	}

	return INVALID_LOCAL_USER_NUM;
}

bool APlayerControllerBase::HasSaveData() const {
	return mCharacterSerializeComponent->GetSaveData() != nullptr;
}

void APlayerControllerBase::AssignSaveData(UCharacterSaveData* inSaveData) {
	mCharacterSerializeComponent->AssignCharacter(inSaveData);
	if (auto saveData = GetSaveData()) {
		SetRecentSaveDataFilename(GetSaveLocalUserNum(), saveData->GetBaseFilename());

		/*	1 player dedicated seems to already have a pawn when we assign the pawn ingame.
			Because of this, we can't assume we have no pawn when we load data.
			Which in turn means we have the handle this case as well.
		*/
		if (auto pawn = GetPawn()) {
			if (UCharacterSerializeComponent* serializer = pawn->FindComponentByClass<UCharacterSerializeComponent>()) {
				check(!serializer->HasProfile() && "pawn already had save data assign, code path not supported");
				serializer->AssignCharacter(saveData);
			}
		}
		OnSelectedSkinId(saveData->mRecordedData.skin);
	}
	OnSaveDataChanged.Broadcast();
}


void APlayerControllerBase::SetRecentSaveDataFilename(int32 LocalUserNum, const FString& inFilename) {
	if (UGlobalStateData* saveData = EditGlobalSaveState()) {
		FString key = GetRecentSaveDataKey();
		saveData->SetRecentSaveData(key, inFilename);
	}

	OnRecentSaveDataIndexChanged.Broadcast();
}

UCharacterSaveData* APlayerControllerBase::GetSaveData() const {
	return mCharacterSerializeComponent->GetSaveData();
}


UCharacterSerializeComponent* APlayerControllerBase::GetCharacterSerializeComponent() const {
	return mCharacterSerializeComponent;
}

void APlayerControllerBase::SaveCharacterData()
{
	mCharacterSerializeComponent->GetSaveData()->Save(GetSaveLocalUserNum());
}

void APlayerControllerBase::SaveCharacterData_OnlyIfValidSaveDataNum() {
	if (!HasSaveData()) {
		return;
	}
	const auto saveLocalUserNum = GetSaveLocalUserNum();
	if (saveLocalUserNum == INVALID_LOCAL_USER_NUM) {
		return;
	}

	mCharacterSerializeComponent->GetSaveData()->Save(saveLocalUserNum);
}

bool APlayerControllerBase::CreateNewCharacter() {
	if (UGlobalStateData* saveData = EditGlobalSaveState()) {
		if (!CanCreateNewCharacter()) {
			return false;
		}

		UCharacterSaveData* characterSaveData = saveData->CreateNewCharacterProfile(GetSaveLocalUserNum());
		AssignSaveData(characterSaveData);
		return true;
	}

	return false;
}

bool APlayerControllerBase::CloneCharacter(int32 inIndex) {
	if (UGlobalStateData* saveData = EditGlobalSaveState()) {
		if (!CanCreateNewCharacter()) {
			return false;
		}

		if (UCharacterSaveData* characterSaveData = GetAvailableSaveDataByIndex(inIndex)) {
			UCharacterSaveData* newCharacterSaveData = saveData->CloneCharacterProfile(*characterSaveData, GetSaveLocalUserNum());
			AssignSaveData(newCharacterSaveData);
			return true;
		}
		
	}

	return false;
}


bool APlayerControllerBase::DeepCloneCharacter(UCharacterSaveData* characterSaveData) 
{
	if (UGlobalStateData* saveData = EditGlobalSaveState()) 
	{
		if (!CanCreateNewCharacter()) 
			return false;
		UCharacterSaveData* newCharacterSaveData = saveData->CloneCharacterProfile(*characterSaveData, GetSaveLocalUserNum(), ECharacterCloneType::Deep);
		AssignSaveData(newCharacterSaveData);
		return true;
	}

	return false;
}



bool APlayerControllerBase::CloneCharacter(UCharacterSaveData* characterSaveData)
{
	if (UGlobalStateData* saveData = EditGlobalSaveState())
	{
		if (!CanCreateNewCharacter())
			return false;
		UCharacterSaveData* newCharacterSaveData = saveData->CloneCharacterProfile(*characterSaveData, GetSaveLocalUserNum());
		AssignSaveData(newCharacterSaveData);
		return true;
	}
	return false;
}


bool APlayerControllerBase::DeleteCharacterByIndex(int32 inIndex) {
	if (UGlobalStateData* saveData = EditGlobalSaveState()) {
		if (!CanDeleteCharacterByIndex(inIndex)) {
			return false;
		}

		if (saveData->DeleteByIndex(GetSaveLocalUserNum(), inIndex)) {

			// #D11.CM - Clear our save data after deleting our character.
			AssignSaveData(nullptr);

			if (GetNumProfiles() > 0) {
				// clamp index to profile count:
				int32 newIndex = Math::clamp(inIndex, 0, GetNumProfiles() - 1);
				if (UCharacterSaveData* characterSaveData = GetAvailableSaveDataByIndex(newIndex)) {
					AssignSaveData(characterSaveData);
					return true;
				}
			}

			TryLoadCharacterSaveData();
			return true;
		}
	}

	return false;
}

bool APlayerControllerBase::CanCreateNewCharacter() const {
	return GetNumProfiles() < GetMaxProfiles();
}

bool APlayerControllerBase::CanDeleteCharacterByIndex(int32 inIndex) const {
	// cant delete a character selected by someone else
	if (!GetIsCharacterSlotIndexSelectable(inIndex)) {
		return false;
	}
	return true;
}



UCharacterSaveData* APlayerControllerBase::GetAvailableSaveDataByIndex(int32 index) const {
	TArray<UCharacterSaveData*> allProfiles = GetAllProfiles(GetSaveLocalUserNum(), GetWorld());

	if (allProfiles.IsValidIndex(index)) {
		return allProfiles[index];
	}

	return nullptr;
}

APlayerCharacterSaveSlot* APlayerControllerBase::GetCharacterSlotByIndex(int32 index, bool forceRefreshSlot) {
	if (UCharacterSaveData* saveData = GetAvailableSaveDataByIndex(index)) {
		if (UDungeonsGameInstance* gi = Cast<UDungeonsGameInstance>(GetWorld()->GetGameInstance())) {
			return gi->EditGlobalSaveState(GetSaveLocalUserNum())->GetCharacterSlotFor(this, saveData, forceRefreshSlot);
		}
	}

	return nullptr;
}

bool APlayerControllerBase::GetIsCharacterSlotIndexSelectable(int32 index) const {
	UCharacterSaveData* saveData = GetAvailableSaveDataByIndex(index);
	for (APlayerControllerBase* otherController : TActorRange<APlayerControllerBase>(GetWorld(), APlayerControllerBase::StaticClass())) {
		if (otherController != this && otherController->IsLocalController()){			
			if (otherController->GetSaveData() == saveData) {
				return false;
			}
		}
	}
	return true;
}

UGlobalStateData* APlayerControllerBase::EditGlobalSaveState() {
	if (UDungeonsGameInstance* gi = GetWorld()->GetGameInstance<UDungeonsGameInstance>()) {				
		return gi->EditGlobalSaveState(GetSaveLocalUserNum());
	}
	return nullptr;
}

const UGlobalStateData* APlayerControllerBase::ReadGlobalSaveState() const {
	if (UDungeonsGameInstance* gi = GetWorld()->GetGameInstance<UDungeonsGameInstance>()) {		
		return gi->ReadGlobalSaveState(GetSaveLocalUserNum());
	}
	return nullptr;
}

bool APlayerControllerBase::HasEverSelectedSkinId(const FName& skinId) {
	if (auto* globalSave = ReadGlobalSaveState()) {
		return globalSave->HasSelectedSkinId(skinId);
	}
	return false;
}

void APlayerControllerBase::OnSelectedSkinId(const FName& skinId) {
	if (auto* globalSave = EditGlobalSaveState()) {
		if (!globalSave->HasSelectedSkinId(skinId)) {
			globalSave->AddSelectedSkinId(skinId);
			OnSkinEverSelectedChanged.Broadcast();
		}
	}
}

int32 APlayerControllerBase::GetRecentSaveDataIndex() const {
	if (UDungeonsGameInstance* gi = Cast<UDungeonsGameInstance>(GetWorld()->GetGameInstance())) {
		int userSystemID = GetSaveLocalUserNum();

		if (!gi->ReadGlobalSaveState(userSystemID)) {
			return -1;
		}

		FString filename = gi->ReadGlobalSaveState(userSystemID)->GetRecentSaveDataByKey(GetRecentSaveDataKey());
		if (!filename.IsEmpty()) {
			return gi->ReadGlobalSaveState(userSystemID)->GetIndexForFilename(filename);
		}
	}

	return -1;
}

int32 APlayerControllerBase::GetNumProfiles() const {
	TArray<UCharacterSaveData*> allProfiles = GetAllProfiles(GetSaveLocalUserNum(), GetWorld());
	return allProfiles.Num();
}

int32 APlayerControllerBase::GetMaxProfiles() const {
	return CHARACTER_PROFILE_MAX;
}

void APlayerControllerBase::OnGamepadActiveChanged(bool GamepadActive)
{
	bGamepadActive = GamepadActive;

	if (IsOwnedByInitialLocalPlayer() || bGamepadActive)
	{
		int shouldVibrate;
		if (USettingsBlueprintFunctionLibrary::GetSettingFromSave(FString("EVibration"), this, shouldVibrate))
		{
			bForceFeedbackEnabled = GamepadActive && shouldVibrate;
		}
		else
		{
			bForceFeedbackEnabled = GamepadActive;
		}
	}
}

void APlayerControllerBase::UpdateLocalPlayerDisplayIndex() {
	if (auto localPlayer = GetLocalPlayer()) {
		const auto localPlayers = GetGameInstance()->GetLocalPlayers();
		const int foundIndex = localPlayers.Find(localPlayer);
		if(foundIndex != INDEX_NONE){
			//We have a local player which exists in the local player array!
			mLocalPlayerDisplayIndex = foundIndex;
			//Local player display index changed!
			mLocalPlayerDisplayText = valueformat::asCompactPlayerNumber(mLocalPlayerDisplayIndex.GetValue());	
			return; //Success
		}
	} 

	//Otherwise always fallback to Defaults, as is the case with remote controllers on the server.
	mLocalPlayerDisplayIndex = {};
	mLocalPlayerDisplayText = FText::GetEmpty();
}

int32 APlayerControllerBase::GetFirstAvailableSaveIndex() const
{
	for(int i = 0; i < GetNumProfiles(); i++)
	{
		if(GetIsCharacterSlotIndexSelectable(i))
		{
			return i;
		}
	}

	return -1;
}
