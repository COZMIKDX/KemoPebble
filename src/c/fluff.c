#include <pebble.h>
#include "main_window.h"

static void init(void) {
  main_window_create();
}

static void deinit(void) {
  main_window_destroy();
}

int main(void) {
  init();
  //APP_LOG(APP_LOG_LEVEL_DEBUG, "Done initializing, pushed window: %p", s_window);
  app_event_loop();
  deinit();
}
