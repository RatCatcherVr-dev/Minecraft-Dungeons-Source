#include "Dungeons.h"
#include "AnonymousTrialsProvider.h"
#include "IDungeonsAPIClient.h"

std::shared_ptr<TrialsClient> UAnonymousTrialsProvider::GetTrialsClient() const {
	if (IDungeonsAPIClient::IsAvailable()) {
		return IDungeonsAPIClient::Get().Trials()->Anonymous();
	}		
	return nullptr;
}
