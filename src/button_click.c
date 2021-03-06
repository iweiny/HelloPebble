#include <pebble.h>

static Window *window;
static TextLayer *text_layer;

#define DATA_VERSION 1
#define DATA_OBJ_KEY 0xBEEF

/**
 * Weight handler code.
 */
struct ww_data
{
	uint8_t data_version;
	int8_t w2;
	uint16_t w1;
} ww_data;
char wstring[] = "xxxx.y";

/*
 * Initialize our global data and read persistent storage
 */
static void default_ww_data(void)
{
	ww_data.data_version = DATA_VERSION;
	ww_data.w1 = 150;
	ww_data.w2 = 0;
}
static void init_ww_data(void)
{
	struct ww_data dstore;
	default_ww_data();
	if (persist_exists(DATA_OBJ_KEY)) {
		int size = persist_read_data(DATA_OBJ_KEY, &dstore, sizeof(dstore));
		if (size == sizeof(dstore) && dstore.data_version == DATA_VERSION) {
			memcpy(&ww_data, &dstore, sizeof(ww_data));
		}
	}
}
static void save_ww_data(void)
{
	persist_write_data(DATA_OBJ_KEY, &ww_data, sizeof(ww_data));
}

static void display_weight(void)
{
	snprintf(wstring, sizeof(wstring), "%4d.%1d", ww_data.w1, ww_data.w2);
	text_layer_set_text(text_layer, wstring);
}

static void increment_weight(void)
{
	ww_data.w2++;
	if (ww_data.w2 >= 10) {
		ww_data.w1++;
		ww_data.w2 = 0;
	}
}

static void decriment_weight(void)
{
	ww_data.w2--;
	if (ww_data.w2 < 0) {
		ww_data.w1--;
		ww_data.w2 = 9;
	}
}
	

/**
 * Button Handler code
 */
static void select_click_handler(ClickRecognizerRef recognizer, void *context)
{
	display_weight();
}

static void up_click_handler(ClickRecognizerRef recognizer, void *context)
{
	increment_weight();
	display_weight();
}

static void down_click_handler(ClickRecognizerRef recognizer, void *context)
{
	decriment_weight();
	display_weight();
}

static void click_config_provider(void *context)
{
	window_single_click_subscribe(BUTTON_ID_SELECT, select_click_handler);
	window_single_repeating_click_subscribe(BUTTON_ID_UP, 30, up_click_handler);
	window_single_repeating_click_subscribe(BUTTON_ID_DOWN, 30, down_click_handler);
}

static void window_load(Window *window)
{
	Layer *window_layer = window_get_root_layer(window);
	GRect bounds = layer_get_bounds(window_layer);

	text_layer = text_layer_create((GRect) {
					.origin = { 0, 0 },
					.size = { bounds.size.w, bounds.size.h }
					});
	text_layer_set_font(text_layer, fonts_get_system_font(FONT_KEY_DROID_SERIF_28_BOLD));
	display_weight();
	text_layer_set_text_alignment(text_layer, GTextAlignmentCenter);
	layer_add_child(window_layer, text_layer_get_layer(text_layer));
}

static void window_unload(Window *window)
{
	text_layer_destroy(text_layer);
}

static void init(void)
{
	init_ww_data();
	window = window_create();
	window_set_click_config_provider(window, click_config_provider);
	window_set_window_handlers(window, (WindowHandlers) {
					.load = window_load,
					.unload = window_unload,
					});
	const bool animated = true;
	window_stack_push(window, animated);
}

static void deinit(void)
{
	window_destroy(window);
	save_ww_data();
}

int main(void)
{
	init();
	app_event_loop();
	deinit();
}
