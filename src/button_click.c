#include <pebble.h>

static Window *window;
static TextLayer *text_layer;
static TextLayer *text_layer_good_times;
static TextLayer *text_layer_bad_times;

static GRect view_grect[3];

// Model
#define SIZE 10
static int model_good_times[SIZE];
static int model_bad_times[SIZE];

static void model_add(int* model, int value) {
  for(int i = SIZE - 1; i > 0; i--) {
    model[i] = model[i-1];
  }

  model[0] = value;
}

static void view_render_model(TextLayer *text_layer_view, int* model) {
  // build dynamic string out of model
  if (model == model_good_times)
    text_layer_set_text(text_layer_view, "GOOD");
  else
    text_layer_set_text(text_layer_view, "BAD");
}

static void view_set_grect(GRect bounds) {
  // 20 for now. remaining between good and bad
  // height. goes into 3. Good | Now | Bad
  int height_now = 20;
  int height_good_or_bad = (bounds.size.h - height_now)/2;

  view_grect[0] = (GRect) { .origin = { 0, 0 }, .size = { bounds.size.w, height_good_or_bad } };
  view_grect[1] = (GRect) { .origin = { 0, height_good_or_bad }, .size = { bounds.size.w, height_now } };
  view_grect[2] = (GRect) { .origin = { 0, height_good_or_bad + height_now }, .size = { bounds.size.w, height_good_or_bad } };
}

static void select_click_handler(ClickRecognizerRef recognizer, void *context) {
  text_layer_set_text(text_layer, "Select");
}

static void up_click_handler(ClickRecognizerRef recognizer, void *context) {
  model_add(model_good_times, 1);
  view_render_model(text_layer_good_times, model_good_times);
  text_layer_set_text(text_layer, "Up");
}

static void down_click_handler(ClickRecognizerRef recognizer, void *context) {
  model_add(model_bad_times, 2);
  view_render_model(text_layer_bad_times, model_bad_times);
  text_layer_set_text(text_layer, "Down");
}

static void click_config_provider(void *context) {
  window_single_click_subscribe(BUTTON_ID_SELECT, select_click_handler);
  window_single_click_subscribe(BUTTON_ID_UP, up_click_handler);
  window_single_click_subscribe(BUTTON_ID_DOWN, down_click_handler);
}

static void window_load(Window *window) {
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);

  view_set_grect(bounds);

  text_layer = text_layer_create(view_grect[1]);
  text_layer_set_text(text_layer, "Press a button");
  text_layer_set_text_alignment(text_layer, GTextAlignmentCenter);
  layer_add_child(window_layer, text_layer_get_layer(text_layer));

  text_layer_good_times = text_layer_create(view_grect[0]);
  text_layer_set_text(text_layer_good_times, "Good goes here");
  text_layer_set_text_alignment(text_layer_good_times, GTextAlignmentCenter);
  layer_add_child(window_layer, text_layer_get_layer(text_layer_good_times));

  text_layer_bad_times = text_layer_create(view_grect[2]);
  text_layer_set_text(text_layer_bad_times, "Bad goes here");
  text_layer_set_text_alignment(text_layer_bad_times, GTextAlignmentCenter);
  layer_add_child(window_layer, text_layer_get_layer(text_layer_bad_times));
}

static void window_unload(Window *window) {
  text_layer_destroy(text_layer);
  text_layer_destroy(text_layer_good_times);
  text_layer_destroy(text_layer_bad_times);
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