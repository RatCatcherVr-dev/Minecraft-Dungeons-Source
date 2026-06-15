#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"
#include "IDungeonsHTTP.h"
#include "DungeonsHTTPClient.h"

using namespace minecraft::api;

class FDungeonsHTTP : public IDungeonsHTTP {
	std::shared_ptr<HttpClient> Client(const std::shared_ptr<DungeonsHTTPConfig>& config) override;
};

IMPLEMENT_MODULE( FDungeonsHTTP, DungeonsHTTP )

std::shared_ptr<HttpClient> FDungeonsHTTP::Client(const std::shared_ptr<DungeonsHTTPConfig>& config) {
	return std::static_pointer_cast<HttpClient>(std::make_shared<DungeonsHTTPClient>(config));
}
