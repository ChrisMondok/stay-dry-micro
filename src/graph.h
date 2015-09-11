#include <pebble.h>
#pragma once
	
#define GRAPH_COLUMNS 60
	
typedef struct Graph {
	Layer *layer;
	uint8_t values[GRAPH_COLUMNS];
} Graph;

Graph* graph_create(GRect frame);

void graph_destroy(Graph *g);

void graph_draw_layer(Layer *layer, GContext *ctx);

Layer* graph_get_layer(Graph *g);