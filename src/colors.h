#include <pebble.h>

#ifdef PBL_COLOR
	#define GRAPH_FG GColorDukeBlue
	#define LOADING_BG GColorChromeYellow
	#define LOADING_FG GColorBlack
#else
	#define GRAPH_FG GColorBlack
	#define LOADING_FG GColorWhite
	#define LOADING_BG GColorBlack
#endif
	