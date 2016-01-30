/*
 * Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
 *
 * Licensed under the Apache License, Version 2.0 (the License);
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an AS IS BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <app.h>
#include <app_control.h>
#include <app_control_internal.h>
#include <bundle.h>
#include <bundle_internal.h>
#include <Elementary.h>
#include <efl_util.h>
#include <system_settings.h>

#include "share_panel.h"
#include "share_panel_internal.h"
#include "conf.h"
#include "ui_manager.h"
#include "log.h"
#include "scroller.h"
#include "utils.h"

#ifdef EAPI
#undef EAPI
#define EAPI __attribute__ ((visibility("default")))
#endif

#define SHARE_PANEL_DOMAIN "share-panel"
#define TRANSIT_DURATION 0.5f


typedef struct custom_effect {
	Evas_Coord from_h;
	Evas_Coord to_h;
} custom_effect_s;


static Eina_Bool _back_key_pressed(void *data, Evas_Object *obj, Evas_Object *src, Evas_Callback_Type type, void *event_info)
{
	Evas_Event_Key_Down *ev = event_info;
	share_panel_h share_panel = data;

	retv_if(!share_panel, EINA_FALSE);

	_D("KEY PRESSED: %s", ev->key);
	if (type == EVAS_CALLBACK_KEY_DOWN && !strncmp(KEY_BACK, ev->key, strlen(KEY_BACK))) {
		_D("KEY PRESSED: %s", ev->key);

		ui_app_exit();
		return EINA_TRUE;
	} else {
		return EINA_FALSE;
	}
}


static void _rotate_cb(void *data, Evas_Object *obj, void *event)
{
	int angle = 0;
	share_panel_h share_panel = data;

	ret_if(!obj);
	ret_if(!share_panel);
	ret_if(!share_panel->list_count);
	ret_if(!share_panel->ui_manager);

	angle = elm_win_rotation_get(obj);
	_D("Angle is %d degree", angle);

	if (share_panel->list_count > SHARE_PANEL_LINE_MAX) {
		return;
	}

	switch(angle) {
	case 0:
	case 180:
		elm_object_signal_emit(share_panel->ui_manager, "1line", "fixed_bg");
		break;
	case 90:
	case 270:
		elm_object_signal_emit(share_panel->ui_manager, "1line,land", "fixed_bg");
		break;
	default:
		_E("cannot reach here");
	}
}


static Evas_Object *__create_win(share_panel_h share_panel)
{
	Evas_Object *win = NULL;
	Evas *e = NULL;
	Ecore_Evas *ee = NULL;

	win = elm_win_add(NULL, "Share Panel", ELM_WIN_BASIC);
	retv_if(!win, NULL);
	elm_win_alpha_set(win, EINA_TRUE);

	e = evas_object_evas_get(win);
	goto_if(!e, error);

	ee = ecore_evas_ecore_evas_get(e);
	goto_if(!ee, error);

	ecore_evas_name_class_set(ee, "SYSTEM_POPUP", "SYSTEM_POPUP");

	if (elm_win_wm_rotation_supported_get(win)) {
		int rots[4] = {0, 90, 180, 270};
		elm_win_wm_rotation_available_rotations_set(win, rots, (sizeof(rots) / sizeof(int)));
	}
	elm_win_screen_size_get(win, NULL, NULL, &share_panel->root_width, &share_panel->root_height);
	_D("screen size is (%d, %d)", share_panel->root_width, share_panel->root_height);

	elm_object_event_callback_add(win, _back_key_pressed, share_panel);
	evas_object_smart_callback_add(win, "wm,rotation,changed", _rotate_cb, share_panel);

	efl_util_set_window_opaque_state(win, 1);

	elm_win_indicator_mode_set(win, ELM_WIN_INDICATOR_HIDE);
	elm_win_borderless_set(win, EINA_TRUE);
	evas_object_show(win);

	share_panel->win = win;

	return win;

error:
	if (win)
		evas_object_del(win);

	return NULL;
}


static void __destroy_win(Evas_Object *win)
{
	evas_object_del(win);
}


EAPI int share_panel_create(app_control_h control, share_panel_h *share_panel)
{
	share_panel_h panel = NULL;

	retv_if(!share_panel, SHARE_PANEL_ERROR_INVALID_PARAMETER);
	retv_if(!control, SHARE_PANEL_ERROR_INVALID_PARAMETER);

	char * locale_path = get_res_file_path("locale");

	bindtextdomain(SHARE_PANEL_DOMAIN, locale_path);

	panel = calloc(1, sizeof(share_panel_s));
	retv_if(!panel, SHARE_PANEL_ERROR_NOT_INITIALIZED);

	app_control_export_as_bundle(control, &(panel->b));

	goto_if(!__create_win(panel), ERROR);

	if (panel->root_width > panel->root_height) {
		panel->page_width = panel->root_height;
	} else {
		panel->page_width = panel->root_width;
	}
	panel->page_height = ELM_SCALE_SIZE(SCROLLER_HEIGHT);

	panel->control = control;
	panel->after_launch = 0;

	panel->ui_manager = _ui_manager_create(panel);
	goto_if(!panel->ui_manager, ERROR);

	elm_win_resize_object_add(panel->win, panel->ui_manager);
	share_panel_hide(panel);

	*share_panel = panel;

	free(locale_path);

	return SHARE_PANEL_ERROR_NONE;

ERROR:
	if (panel->ui_manager) {
		_ui_manager_destroy(panel->ui_manager);
	}
	free(panel);
	free(locale_path);

	return SHARE_PANEL_ERROR_NOT_INITIALIZED;
}


EAPI int share_panel_destroy(share_panel_h share_panel)
{
	retv_if(!share_panel, SHARE_PANEL_ERROR_INVALID_PARAMETER);

	if (share_panel->b) {
		bundle_free(share_panel->b);
	}
	_ui_manager_destroy(share_panel->ui_manager);

	if (share_panel->win) {
		__destroy_win(share_panel->win);
	}

	app_control_destroy(share_panel->control);
	free(share_panel);

	return SHARE_PANEL_ERROR_NONE;
}


EAPI int share_panel_show(share_panel_h share_panel)
{
	retv_if(!share_panel, SHARE_PANEL_ERROR_INVALID_PARAMETER);

	return _ui_manager_show(share_panel);
}


EAPI int share_panel_hide(share_panel_h share_panel)
{
	retv_if(!share_panel, SHARE_PANEL_ERROR_INVALID_PARAMETER);

	return _ui_manager_hide(share_panel);
}


static bool _create_cb(void *data)
{
	elm_app_base_scale_set(1.7);

	return true;
}


static void _terminate_cb(void *data)
{
	share_panel_h *share_panel = data;

	ret_if(!share_panel);

	share_panel_hide(*share_panel);
	share_panel_destroy(*share_panel);
}


static void _app_control(app_control_h control, void *data)
{
	share_panel_h *share_panel = data;
	int ret = SHARE_PANEL_ERROR_NONE;

	ret_if(!share_panel);

	ret_if(!control);

	ret = share_panel_create(control, share_panel);
	if (SHARE_PANEL_ERROR_NONE != ret) {
		_E("Fail to create the share panel");
		return;
	}

	share_panel_show(*share_panel);
}

static void _pause_cb(void *data)
{
	share_panel_h *share_panel = data;
	_D("share_panel is paused");

	if ((*share_panel)->after_launch)
		ui_app_exit();
}

static void _language_changed(app_event_info_h event_info, void *data)
{
	char *lang = NULL;
	int ret = SYSTEM_SETTINGS_ERROR_NONE;

	_D("language changed");

	ret = system_settings_get_value_string(SYSTEM_SETTINGS_KEY_LOCALE_LANGUAGE, &lang);
	if (ret != SYSTEM_SETTINGS_ERROR_NONE) {
		_E("Fail to get string value (%d", ret);
		return;
	}
	ret_if(!lang);
	elm_language_set(lang);

	free(lang);

}


int main(int argc, char **argv)
{
	int ret;
	ui_app_lifecycle_callback_s lifecycle_callback = {0, };
	app_event_handler_h event_handlers[5] = {NULL, };
	share_panel_h share_panel = NULL;

	lifecycle_callback.create = _create_cb;
	lifecycle_callback.terminate = _terminate_cb;
	lifecycle_callback.pause = _pause_cb;
	lifecycle_callback.resume = NULL;
	lifecycle_callback.app_control = _app_control;

	ui_app_add_event_handler(&event_handlers[APP_EVENT_LOW_BATTERY], APP_EVENT_LOW_BATTERY, NULL, NULL);
	ui_app_add_event_handler(&event_handlers[APP_EVENT_LOW_MEMORY], APP_EVENT_LOW_MEMORY, NULL, NULL);
	ui_app_add_event_handler(&event_handlers[APP_EVENT_DEVICE_ORIENTATION_CHANGED], APP_EVENT_DEVICE_ORIENTATION_CHANGED, NULL, NULL);
	ui_app_add_event_handler(&event_handlers[APP_EVENT_LANGUAGE_CHANGED], APP_EVENT_LANGUAGE_CHANGED, _language_changed, NULL);
	ui_app_add_event_handler(&event_handlers[APP_EVENT_REGION_FORMAT_CHANGED], APP_EVENT_REGION_FORMAT_CHANGED, NULL, NULL);

	ret = ui_app_main(argc, argv, &lifecycle_callback, &share_panel);

	return ret;
}
