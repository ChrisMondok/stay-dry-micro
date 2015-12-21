#pragma once
#include<pebble.h>

struct weather_window {
	time_t start, end;
	uint8_t precip_intensity;
};