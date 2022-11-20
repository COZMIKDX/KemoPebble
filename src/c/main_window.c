#include <pebble.h>
#include "main_window.h"

static Window *s_window;
// static Layer *canvas;
static TextLayer *time_layer;

static GBitmap *fox_bitmap = NULL;
static BitmapLayer *bitmap_layer;
static GBitmapSequence *fox_sequence = NULL;

static GBitmap * idle_bitmap;
static BitmapLayer *idle_bitmap_layer;

// Draw the background elements onto the canvas layer.
static void canvas_update_proc(Layer *layer, GContext *ctx) {
}

static void update_time() {
    // Get a tm structure.
    time_t temp = time(NULL);
    struct tm *tick_time = localtime(&temp);

    // format the time from the tm structure and stick it into a buffer
    static char buffer[8];
    strftime(buffer, sizeof(buffer), clock_is_24h_style() ? "%H:%M" : "%I:%M", tick_time);

    // Change the text layer to reflect the current time.
    text_layer_set_text(time_layer, buffer);
}

static void tick_handler(struct tm *tick_time, TimeUnits unit_changed) {
    update_time();
    // Do other things.
    // run animation.
}

static void timer_handler(void *context) {
    uint32_t next_delay;

    // Advance to the next APNG frame, and get the delay for this frame
    if (gbitmap_sequence_update_bitmap_next_frame(fox_sequence, fox_bitmap, &next_delay))
    {
        // Set the new frame into the BitmapLayer
        bitmap_layer_set_bitmap(bitmap_layer, fox_bitmap);
        layer_mark_dirty(bitmap_layer_get_layer(bitmap_layer));

        // Timer for that frame's delay
        app_timer_register(next_delay, timer_handler, NULL);
    }
}

static void window_load(Window *window) {
    // Boiler plate
    // window info. I'll use this for both the canvas and text.
    Layer *window_layer = window_get_root_layer(window);
    GRect window_bounds = layer_get_bounds(window_layer); // This canvas will cover the whole window

    // Drawing layer init.
    // canvas = layer_create(window_bounds);
    // layer_set_update_proc(canvas, canvas_update_proc);
    // layer_add_child(window_layer, canvas);
    // end boiler plate

    /*bitmap_layer = bitmap_layer_create(window_bounds);
    layer_add_child(window_layer, bitmap_layer_get_layer(bitmap_layer));

    fox_sequence = gbitmap_sequence_create_with_resource(RESOURCE_ID_FOX);
    GSize fox_frame_size = gbitmap_sequence_get_bitmap_size(fox_sequence);

    fox_bitmap = gbitmap_create_blank(fox_frame_size, GBitmapFormat8Bit);
    app_timer_register(1, timer_handler, NULL);*/

    idle_bitmap = gbitmap_create_with_resource(RESOURCE_ID_FOX_IDLE);
    GRect idle_bounds = layer_get_bounds(window_layer);
    idle_bitmap_layer = bitmap_layer_create(idle_bounds);
    bitmap_layer_set_compositing_mode(idle_bitmap_layer, GCompOpSet);
    bitmap_layer_set_bitmap(idle_bitmap_layer, idle_bitmap);
    layer_add_child(window_layer, bitmap_layer_get_layer(idle_bitmap_layer));

    time_layer = text_layer_create(GRect(0, 52, window_bounds.size.w, 50));
    text_layer_set_background_color(time_layer, GColorClear);
    text_layer_set_text_color(time_layer, GColorWhite);
    text_layer_set_text(time_layer, "00:00");
    text_layer_set_font(time_layer, fonts_get_system_font(FONT_KEY_BITHAM_42_BOLD));
    text_layer_set_text_alignment(time_layer, GTextAlignmentCenter);
    layer_add_child(window_layer, text_layer_get_layer(time_layer));
}

static void window_unload(Window *window) {
    //text_layer_destroy(whateverLayer);
    text_layer_destroy(time_layer);
}

// Ran in the app's init function.
void main_window_create() {
    s_window = window_create();
    window_set_window_handlers(s_window, (WindowHandlers){
        .load = window_load,
        .unload = window_unload,
    });

    window_set_background_color(s_window, GColorBlack);
    const bool animated = true;
    window_stack_push(s_window, animated);

    // Get the time every minute.
    // Can be placed before the window is created too. I think this first update_time() call could be placed in window_load() if you do.
    tick_timer_service_subscribe(MINUTE_UNIT, tick_handler);
    update_time();
}

// Ran in deinit.
void main_window_destroy() {
    //destroy stuff. Not sure if destroying the window must be done last.
    // gbitmap_sequence_destroy(fox_sequence);
    // gbitmap_destroy(fox_bitmap);
    // bitmap_layer_destroy(bitmap_layer);

    gbitmap_destroy(idle_bitmap);
    bitmap_layer_destroy(idle_bitmap_layer);

    window_destroy(s_window);
}