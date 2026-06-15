#include "TrialsClientManager.h"
#include "AnonymousTrialsClient.h"

std::shared_ptr<TrialsClient> TrialsClientManager::Authenticated() {
	if (!Client) {
		Client = make_shared<TrialsClient>();	
	}
	return Client;
}

std::shared_ptr<TrialsClient> TrialsClientManager::Anonymous() {
	if (!AnonymousClient) {
		AnonymousClient = make_shared<AnonymousTrialsClient>();
	}
	return AnonymousClient;
}
