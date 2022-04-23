#include "app.h"

#include "driver.h"
#include "player.h"
#include <memory.h>
#include <stdlib.h>

#include "melody_1.h"
#include "melody_2.h"
#include "melody_3.h"
#include "melody_4.h"

#define BACKGROUND_COLOR lv_color_make(0x90, 0x90, 0x90)
#define MELODY_COUNT 4

typedef struct CheckBoxEventData {
	App* app;
	int id;
} CheckBoxEventData;

struct App {
	LVGLDriver* drv;

	lv_obj_t* screen;

	// Checkboxes
	lv_style_t cb_cont_style;
	lv_obj_t* cb_cont;
	CheckBoxEventData cb_event_data[MELODY_COUNT];
	int selected_melody;

	// Play/stop music
	lv_style_t btn_style;
	lv_style_t btn_style_clicked;
	lv_obj_t* btn;
	lv_obj_t* btn_label;
	int playing;
	Player player;


};

struct CheckBoxData {
	App* app;
	int id;
};

void play(App* app);
void stop(App* app);

void checkbox_event(lv_event_t* e) {
	CheckBoxEventData* data = lv_event_get_user_data(e);

	int touched_id = data->id;

	lv_obj_t* touch_cb = lv_obj_get_child(data->app->cb_cont, touched_id);
	lv_obj_add_state(touch_cb, LV_STATE_CHECKED);

	if (data->app->selected_melody == touched_id) {
		return;
	}

	lv_obj_t* prev_cb = lv_obj_get_child(data->app->cb_cont, data->app->selected_melody);
	data->app->selected_melody = touched_id;
	lv_obj_clear_state(prev_cb, LV_STATE_CHECKED);

	if (data->app->playing) {
		stop(data->app);
	}
}

void toggle_play(lv_event_t* e) {
	App* app = lv_event_get_user_data(e);

	if (app->playing) {
		stop(app);
	} else {
		play(app);
	}
}

void play(App* app) {
	app->playing = 1;
	lv_label_set_text(app->btn_label, "Stop");
	lv_obj_add_state(app->btn, LV_STATE_CHECKED);

	switch (app->selected_melody) {
	    case 0:
	    	app->player = MakeMusicPlayer((const NoteType *)MELODY_1_FREQUENCIES,
	                                 MELODY_1_DURATIONS, MELODY_1_LEN);
	      break;

	    case 1:
	    	app->player = MakeMusicPlayer((const NoteType *)MELODY_2_FREQUENCIES,
	                                 MELODY_2_DURATIONS, MELODY_2_LEN);
	      break;
	    case 2:
	    	app->player = MakeMusicPlayer((const NoteType *)MELODY_3_FREQUENCIES,
	                                 MELODY_3_DURATIONS, MELODY_3_LEN);
	      break;
	    case 3:
	    	app->player = MakeMusicPlayer((const NoteType *)MELODY_4_FREQUENCIES,
	                                 MELODY_4_DURATIONS, MELODY_4_LEN);
	      break;

	    default:
	    	app->player = MakeNoPlayer();
	      break;
	}

	Player_Init(&app->player);
}

void stop(App* app) {
	app->playing = 0;
	lv_label_set_text(app->btn_label, "Play");
	lv_obj_clear_state(app->btn, LV_STATE_CHECKED);

	Player_Destroy(&app->player);
}


App* App_new() {
	App* app = malloc(sizeof(App));

	app->drv = LVGLDriver_new();
	if (!app->drv) {
		App_destroy(app);
		return NULL;
	}

	app->screen = LVGLDriver_GetScreen(app->drv);
	lv_obj_set_style_bg_color(app->screen, BACKGROUND_COLOR, LV_PART_MAIN | LV_STATE_DEFAULT);

	// Checkboxes init
	lv_style_init(&app->cb_cont_style);
	lv_style_set_bg_color(&app->cb_cont_style, lv_color_make(0xA0, 0xA0, 0xA0));

	app->cb_cont = lv_obj_create(app->screen);
	lv_obj_set_flex_flow(app->cb_cont, LV_FLEX_FLOW_COLUMN);
	lv_obj_add_style(app->cb_cont, &app->cb_cont_style, LV_PART_ANY | LV_STATE_DEFAULT);
	lv_obj_align(app->cb_cont, LV_ALIGN_CENTER, 0, 0);
	lv_obj_set_size(app->cb_cont, 150, 150);

	for (int i = 0; i < MELODY_COUNT; i++) {
		lv_obj_t* cb = lv_checkbox_create(app->cb_cont);
		char title[] = "Melody 0";
		sprintf(title, "Melody %d", i+1);
		lv_checkbox_set_text(cb, title);

		app->cb_event_data[i].app = app;
		app->cb_event_data[i].id = i;
		lv_obj_add_event_cb(cb, checkbox_event, LV_EVENT_VALUE_CHANGED, &app->cb_event_data[i]);
	}

	app->selected_melody = 0;
	lv_obj_add_state(lv_obj_get_child(app->cb_cont, app->selected_melody), LV_STATE_CHECKED);


	// Player
	app->playing = 0;
	app->player = MakeNoPlayer();

	lv_style_init(&app->btn_style);
	lv_style_set_bg_color(&app->btn_style, lv_color_make(0x00, 0x90, 0x00));

	lv_style_init(&app->btn_style_clicked);
	lv_style_set_bg_color(&app->btn_style_clicked, lv_color_make(0x90, 0x00, 0x00));

	app->btn = lv_btn_create(app->screen);
	lv_obj_add_style(app->btn, &app->btn_style, LV_PART_MAIN | LV_STATE_DEFAULT);
	lv_obj_add_style(app->btn, &app->btn_style_clicked, LV_PART_MAIN | LV_STATE_CHECKED);
	lv_obj_align(app->btn, LV_ALIGN_BOTTOM_MID, 0, 0);
	lv_obj_set_size(app->btn, 80, 30);
	lv_obj_add_flag(app->btn, LV_OBJ_FLAG_CHECKABLE);
	lv_obj_add_event_cb(app->btn, toggle_play, LV_EVENT_CLICKED, app);
	app->btn_label = lv_label_create(app->btn);
	lv_label_set_text(app->btn_label, "Play");
	lv_obj_align(app->btn_label, LV_ALIGN_CENTER, 0, 0);

	return app;
}

void App_destroy(App* app) {

	if (app) {

		if (app->drv) {
			LVGLDriver_destroy(app->drv);
		}

		free(app);
	}

}

void App_update(App* app) {

	LVGLDriver_update(app->drv);

	if (Player_Ended(&app->player)) {
		stop(app);
	}

	if (app->playing) {
		Player_Update(&app->player);
	}

}
