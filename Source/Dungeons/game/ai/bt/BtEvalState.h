#pragma once

class AMobCharacter;
class UCharacterAnimInstance;
class AGameBP;
class UWorld;
class AMobBtController;

namespace game { struct FDifficulty; }

struct FBtEvalTraversalState {
	class UBtRoot* currentRoot = nullptr;
};

struct FBtEvalState {
	int tick;
	AMobCharacter* owner;
	AMobBtController* controller;
	class UCharacterMovementComponent* movement;

	class UBtNode* root;

	mutable FBtEvalTraversalState traversalState;

	FBtEvalState() :tick(0), owner(nullptr), controller(nullptr), movement(nullptr), root(nullptr){}
	FBtEvalState(const int in_tick, AMobCharacter* in_owner, AMobBtController* in_controller, class UCharacterMovementComponent* in_movement, class UBtNode* in_root) :tick(in_tick), owner(in_owner), controller(in_controller), movement(in_movement), root(in_root){}
	FBtEvalState(const FBtEvalState& other) :tick(other.tick), owner(other.owner), controller(other.controller), movement(other.movement), root(other.root), traversalState(other.traversalState) {}
	
	UWorld& world() const;
	AGameBP& game() const;
	const game::FDifficulty& difficulty() const;
	struct FMobParams& params() const;
	class UCharacterAnimInstance* anim() const;
	class UMobAnimationsComponent* animPack() const;
};

namespace bt {
	using StateRef = const FBtEvalState&;

	FBtEvalState createState(AMobCharacter&, int tickId);

	namespace internal {
	extern int currentTickId;
	FBtEvalState copyStateWithTickId(StateRef, int tickId);
	FBtEvalState copyStateWithCurrentTickId(StateRef);
	}
}
