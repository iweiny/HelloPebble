#include <pebble.h>

static Window *window;
static TextLayer *text_layer;

/**
 * Weight handler code.
 */
uint16_t w1 = 150;
int8_t w2 = 0;
char wstring[] = "xxxx.y";

static void display_weight(void) {
	snprintf(wstring, sizeof(wstring), "%4d.%1d", w1, w2);
	text_layer_set_text(text_layer, wstring);
}

static void increment_weight(void) {
	w2++;
	if (w2 >= 10) {
		w1++;
		w2 = 0;
	}
}

static void decriment_weight(void) {
	w2--;
	if (w2 < 0) {
		w1--;
		w2 = 9;
	}
}
	

/**
 * Button Handler code
 */
static void select_click_handler(ClickRecognizerRef recognizer, void *context) {
	display_weight();
}

static void up_click_handler(ClickRecognizerRef recognizer, void *context) {
  //text_layer_set_text(text_layer, "Up");
	increment_weight();
	display_weight();
}

static void down_click_handler(ClickRecognizerRef recognizer, void *context) {
  //text_layer_set_text(text_layer, "Down");
	decriment_weight();
	display_weight();
}

static void click_config_provider(void *context) {
  window_single_click_subscribe(BUTTON_ID_SELECT, select_click_handler);
  window_single_click_subscribe(BUTTON_ID_UP, up_click_handler);
  window_single_click_subscribe(BUTTON_ID_DOWN, down_click_handler);
}

static void window_load(Window *window) {
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);

  text_layer = text_layer_create((GRect) { .origin = { 0, 0 }, .size = { bounds.size.w, bounds.size.h } });
	text_layer_set_font(text_layer, fonts_get_system_font(FONT_KEY_DROID_SERIF_28_BOLD));
  //text_layer_set_text(text_layer, "Press a button");
	display_weight();
  text_layer_set_text_alignment(text_layer, GTextAlignmentCenter);
  layer_add_child(window_layer, text_layer_get_layer(text_layer));
}

static void window_unload(Window *window) {
  text_layer_destroy(text_layer);
}

static void init(void) {
  window = window_create();
  window_set_click_config_provider(window, click_config_provider);
  window_set_window_handlers(window, (WindowHandlers) {
	.load = window_load,
    .unload = window_unload,
  });
  const bool animated = true;
  window_stack_push(window, animated);
}

static void deinit(void) {
  window_destroy(window);
}

int main(void) {
  init();
  app_event_loop();
  deinit();
}