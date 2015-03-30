#include <pebble.h>

/*
TODO
  Maximize real estate
    25x4 for good and same for bad.
    4 per line. 011020
    16 total values can be fitted
  Store current time.
    Update this after every minute.
  Good and Bad are almost similar. Refactor code and make it DRY
*/

// view
static Window *window;
static TextLayer *text_layer;
static TextLayer *text_layer_good_times;
static TextLayer *text_layer_bad_times;

static GRect view_grect[3];

#define CHAR_PER_LINE 25
#define MAX_LINES 5
#define WORDS_PER_LINE 4
#define HEIGHT_NOW 12

#define BUFFER_SIZE (CHAR_PER_LINE*MAX_LINES + WORDS_PER_LINE)
static char buffer_good[BUFFER_SIZE];
static char buffer_bad[BUFFER_SIZE];

// Model
#define SIZE (WORDS_PER_LINE*MAX_LINES + 1)
static time_t model_good_times[SIZE];
static int model_good_size = 0;
static time_t model_bad_times[SIZE];
static int model_bad_size = 0;

// TODO rotating log. Use modular maths to avoid shifiting
static void model_add(time_t* model, time_t value, int* size) {
  if (*size + 1 >= SIZE)
    *size = SIZE - 1;

  for(int i = *size; i > 0; i--) {
    model[i] = model[i-1];
  }

  model[0] = value;
  if (*size + 1 < SIZE)
    (*size)++;
}

static void view_render_model(TextLayer *text_layer_view, time_t* model, int size, char* buffer) {
  static char tm_buffer[10];
  struct tm *tick_time;
  buffer[0] = '\0';

  for (int i = 0; i < size; i++) {
    tick_time = localtime(&model[i]);
    strftime(tm_buffer, 10, "%H%M%S", tick_time);
    strcat(buffer, tm_buffer);
    if ((i + 1) % WORDS_PER_LINE == 0) {
      strcat(buffer, "\n");
    }
  }

  text_layer_set_text(text_layer_view, buffer);
}

static void view_set_grect(GRect bounds) {
  // height. goes into 3. Good | Now | Bad
  int height_good_or_bad = (bounds.size.h - HEIGHT_NOW)/2;

  view_grect[0] = (GRect) { .origin = { 0, 0 }, .size = { bounds.size.w, height_good_or_bad } };
  view_grect[1] = (GRect) { .origin = { 0, height_good_or_bad }, .size = { bounds.size.w, HEIGHT_NOW } };
  view_grect[2] = (GRect) { .origin = { 0, height_good_or_bad + HEIGHT_NOW }, .size = { bounds.size.w, height_good_or_bad } };
}

static void update_time() {
  static char buffer[30];
  time_t temp = time(NULL);
  struct tm *tick_time = localtime(&temp);
  strftime(buffer, 30, "%d %b %Y, %a %H:%M:%S", tick_time);
  text_layer_set_text(text_layer, buffer);
}

static void select_click_handler(ClickRecognizerRef recognizer, void *context) {
  update_time();
}

static void up_click_handler(ClickRecognizerRef recognizer, void *context) {
  model_add(model_good_times, time(NULL), &model_good_size);
  view_render_model(text_layer_good_times, model_good_times, model_good_size, buffer_good);
  update_time();
}

static void down_click_handler(ClickRecognizerRef recognizer, void *context) {
  model_add(model_bad_times, time(NULL), &model_bad_size);
  view_render_model(text_layer_bad_times, model_bad_times, model_bad_size, buffer_bad);
  update_time();
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