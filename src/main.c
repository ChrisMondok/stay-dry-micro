#include <pebble.h>
#include "graph.h"
#include "weather_window.h"
#include "colors.h"
	
#define NUM_WINDOWS 8
	
#define KEY_WINDOWS 3
#define KEY_MINUTELY 8

Graph *graph;
TextLayer *loading_layer;
Window* root_window;
struct weather_window windows[NUM_WINDOWS];

void window_loaded(Window* window) {
	Layer *window_layer = window_get_root_layer(root_window);
	graph = graph_create(layer_get_frame(window_layer));
	
	loading_layer = text_layer_create(layer_get_frame(window_layer));
	text_layer_set_text(loading_layer, "Hang tight...");
	text_layer_set_background_color(loading_layer, LOADING_BG);
	text_layer_set_text_color(loading_layer, LOADING_FG);
	text_layer_set_font(loading_layer, fonts_get_system_font(FONT_KEY_BITHAM_42_BOLD));
	
	layer_add_child(window_layer, graph_get_layer(graph));
	layer_add_child(window_layer, text_layer_get_layer(loading_layer));
}

void loaded() {
	GRect from = layer_get_frame(text_layer_get_layer(loading_layer));
	GRect to = GRect(from.origin.x, from.origin.y - from.size.h, from.size.w, from.size.h);
	
	static PropertyAnimation *s_anim_ptr = NULL;
	
	s_anim_ptr = property_animation_create_layer_frame(text_layer_get_layer(loading_layer), &from, &to);
	animation_schedule(property_animation_get_animation(s_anim_ptr));
}

void window_unloaded() {
	text_layer_destroy(loading_layer);
	graph_destroy(graph);
}

void create_main_window(void) {
  root_window = window_create();

	window_set_window_handlers(root_window, (WindowHandlers) {
		.load = window_loaded,
		.unload = window_unloaded
	});
	
  window_stack_push(root_window, true);
}

static void inbox_received_callback(DictionaryIterator *iterator, void *context) {
	loaded();
	APP_LOG(APP_LOG_LEVEL_INFO, "Message received");
	
 	Tuple *t = dict_read_first(iterator);
	
	bool graphIsDirty = false;
	
 	while(t != NULL) {
 		switch(t->key) {
 			case KEY_WINDOWS:
 				memcpy(&windows, t->value->data, sizeof(windows));
 				break;
			case KEY_MINUTELY:
				memcpy(graph->values, t->value->data, sizeof(graph->values));
				graphIsDirty = true;
				break;
 			default:
 				APP_LOG(APP_LOG_LEVEL_WARNING, "Received message of unknown key %d", (int) t->key);
 				break;
 		}
 		t = dict_read_next(iterator);
 	}
	
	if(graphIsDirty)
		layer_mark_dirty(graph_get_layer(graph));
	
	struct tm *start_time = localtime(&windows[0].start);
	
	static char buffer[] = "00:00";
	
	strftime(buffer, sizeof(buffer), "%H:%M", start_time);
}

static void inbox_dropped_callback(AppMessageResult reason, void *context) {
	APP_LOG(APP_LOG_LEVEL_ERROR, "Message dropped: %d", (int) reason);
	switch((int) reason) {
		case APP_MSG_SEND_REJECTED:
			APP_LOG(APP_LOG_LEVEL_ERROR, "Send rejected.");
			break;
		case APP_MSG_SEND_TIMEOUT:
			APP_LOG(APP_LOG_LEVEL_ERROR, "Send timed out");
			break;
		case APP_MSG_APP_NOT_RUNNING:
			APP_LOG(APP_LOG_LEVEL_ERROR, "App not running");
			break;
		case APP_MSG_BUFFER_OVERFLOW:
			APP_LOG(APP_LOG_LEVEL_ERROR, "Buffer overflow");
			break;
	}
}

static void outbox_sent_callback(DictionaryIterator* iterator, void *context) {
	APP_LOG(APP_LOG_LEVEL_INFO,"Outbox send success");
}

static void outbox_failed_callback(DictionaryIterator *iterator, AppMessageResult reason, void *context) {
	switch((int) reason) {
		case APP_MSG_SEND_REJECTED:
			APP_LOG(APP_LOG_LEVEL_ERROR, "Send rejected.");
			break;
		case APP_MSG_SEND_TIMEOUT:
			APP_LOG(APP_LOG_LEVEL_ERROR, "Send timed out");
			break;
		case APP_MSG_APP_NOT_RUNNING:
			APP_LOG(APP_LOG_LEVEL_ERROR, "App not running");
	}
}

void set_up_app_message() {
	app_message_register_inbox_received(inbox_received_callback);
	app_message_register_inbox_dropped(inbox_dropped_callback);
	app_message_register_outbox_sent(outbox_sent_callback);
	app_message_register_outbox_failed(outbox_failed_callback);
	
	app_message_open(app_message_inbox_size_maximum(), APP_MESSAGE_OUTBOX_SIZE_MINIMUM);
}

int main(void) {
  create_main_window();
	set_up_app_message();
  app_event_loop();
  window_destroy(root_window);
}