#include "Dungeons.h"
#include "TitleNewsUtil.h"
#include "locale/LocalizationUtils.h"

float UTitleNewsUtil::getTitleNewsItemAutoNextDelay(const FTitleNews& newsItem) {
	constexpr float MINIMUM_AUTO_NEXT_DELAY = 4.0f;	
	constexpr float LOOK_AT_PICTURE_AUTO_NEXT_DELAY = 1.0f;	

	float delay = MINIMUM_AUTO_NEXT_DELAY; 
	delay += ULocalizationUtils::GetEstimatedReadingDurationForText(newsItem.Body);
	delay += ULocalizationUtils::GetEstimatedReadingDurationForText(newsItem.Title);
	delay += ULocalizationUtils::GetEstimatedReadingDurationForText(newsItem.Category);
	delay += newsItem.HeaderImage ? LOOK_AT_PICTURE_AUTO_NEXT_DELAY : 0.0f;
	return delay;
}