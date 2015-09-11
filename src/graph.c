#include <pebble.h>
#include "graph.h"
	
Graph* graph_create(GRect frame) {
	Layer *layer = layer_create_with_data(frame, sizeof(Graph));
	Graph* g = layer_get_data(layer);
	g->layer = layer;
	for(uint8_t i = 0; i < GRAPH_COLUMNS; i++) 
		g->values[i] = 0;
	
	layer_set_update_proc(layer, graph_draw_layer);
	
	return g;
}

void graph_destroy(Graph* g) {
	APP_LOG(APP_LOG_LEVEL_DEBUG, "Freeing graph");
	free(g->layer);
}

void graph_draw_layer(Layer *layer, GContext *ctx) {
	Graph* graph = layer_get_data(layer);
	
	GSize size = layer_get_bounds(layer).size;
	
	const GPathInfo pathInfo = {
		.num_points = GRAPH_COLUMNS + 2,
		.points = calloc(GRAPH_COLUMNS + 2, sizeof(GPoint))
	};
	
	for(uint16_t i = 0; i < GRAPH_COLUMNS; i++) {
		pathInfo.points[i + 1].x = (i * size.w) / (GRAPH_COLUMNS - 1);
		pathInfo.points[i + 1].y = ((UINT8_MAX - graph->values[i]) * size.h) / UINT8_MAX;
	}
	
	pathInfo.points[0].x = 0;
	pathInfo.points[GRAPH_COLUMNS + 1].x = size.w;
	pathInfo.points[0].y = pathInfo.points[GRAPH_COLUMNS + 1].y = size.h;
	
	GPath *gpath = gpath_create(&pathInfo);
	
	gpath_draw_outline(ctx, gpath);
	
	APP_LOG(APP_LOG_LEVEL_DEBUG, "Freeing pathinfo points");
	free(pathInfo.points);
	APP_LOG(APP_LOG_LEVEL_DEBUG, "Freeing path");
	gpath_destroy(gpath);
}

Layer* graph_get_layer(Graph* g) {
	return g->layer;
}