#pragma once
#include<pebble.h>

struct weather_window {
	time_t start, end;
	uint8_t precip_intensity;
};

typedef uint8_t minutely_forecast_t[60];