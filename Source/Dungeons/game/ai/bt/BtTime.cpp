#include "Dungeons.h"
#include "BtTime.h"

bt::Duration operator""_ticks(unsigned long long int t) {
	return bt::Duration(bt::Ticks(t));
}
