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
#include <Elementary.h>
#include <notification.h>

#include "share_panel_internal.h"
#include "share_panel.h"

#include "log.h"
#include "grid.h"
#include "list.h"
#include "page.h"
#include "index.h"
#include "scroller.h"
#include "conf.h"
#include "ui_manager.h"
#include "utils.h"

#define GROUP_LAYOUT "layout"


static void __resize_cb(void *data, Evas *e, Evas_Object *obj, void *event_info)
{
	Evas_Object *ui_manager = obj;

	int x, y, w, h;

	ret_if(!ui_manager);

	evas_object_geometry_get(ui_manager, &x, &y, &w, &h);
	_D("%s resize(%d, %d, %d, %d)", data, x, y, w, h);
}


static void _scroll_cb(Evas_Object *scroller, int event_type, void *event_info, void *data)
{
	Evas_Object *index = data;
	int region_index = 0;

	ret_if(!index);

	region_index = _scroller_get_region_index(scroller);
	_index_bring_in(index, region_index);

	_D("Scroller region index is %d", region_index);
}


static void _append_page_cb(Evas_Object *scroller, int event_type, void *event_info, void *data)
{
	Evas_Object *index = data;
	unsigned int count = 0;

	ret_if(!index);

	count = _scroller_count(scroller);
	_index_update(index, count);

	_D("Index was updated as %d", count);
}


static void _remove_page_cb(Evas_Object *scroller, int event_type, void *event_info, void *data)
{
	Evas_Object *index = data;
	unsigned int count = 0;

	ret_if(!index);

	count = _scroller_count(scroller);
	_index_update(index, count);

	_D("Index was updated as %d", count);
}


static void __click_back_btn_cb(void *data, Evas_Object *obj, void *event_info)
{
	share_panel_s *share_panel = data;

	ret_if(!share_panel);

	_D("back button clicked, destroy the share-panel");
	share_panel_destroy(share_panel);
}


Evas_Object *__create_back_btn(Evas_Object *ui_manager, share_panel_s *share_panel)
{
	Evas_Object *button = NULL;

	retv_if(!share_panel, NULL);
	retv_if(!ui_manager, NULL);

	button = elm_button_add(ui_manager);
	retv_if(!button, NULL);

	elm_object_style_set(button, "focus");
	elm_object_part_content_set(ui_manager, "event", button);
	evas_object_size_hint_weight_set(button, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	evas_object_show(button);
	evas_object_smart_callback_add(button, "clicked", __click_back_btn_cb, share_panel);

	return button;
}


void __destroy_back_btn(Evas_Object *button)
{
	ret_if(!button);

	evas_object_del(button);
}


Evas_Object *__create_bg_rect(Evas_Object *ui_manager, int width, int height)
{
	Evas_Object *ui_bg = NULL;

	ui_bg = evas_object_rectangle_add(evas_object_evas_get(ui_manager));
	retv_if(!ui_bg, NULL);

	evas_object_size_hint_weight_set(ui_bg, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	evas_object_size_hint_min_set(ui_bg, ELM_SCALE_SIZE(BASE_WIDTH), ELM_SCALE_SIZE(BASE_HEIGHT));
	evas_object_size_hint_max_set(ui_bg, ELM_SCALE_SIZE(BASE_WIDTH), ELM_SCALE_SIZE(BASE_HEIGHT));
	evas_object_color_set(ui_bg, 0, 0, 0, 0);
	evas_object_show(ui_bg);
	elm_object_part_content_set(ui_manager, "fixed_bg", ui_bg);

	return ui_bg;
}


void __destroy_bg_rect(Evas_Object *bg)
{
	ret_if(!bg);

	evas_object_del(bg);
}


Evas_Object *_ui_manager_create(share_panel_s *share_panel)
{
	Evas_Object *ui_manager = NULL;
	Evas_Object *edje = NULL;

	retv_if(!share_panel, NULL);

	ui_manager = elm_layout_add(share_panel->win);
	retv_if(!ui_manager, NULL);

	char *edj_path = utils_get_res_file_path("edje/layout.edj");

	elm_layout_file_set(ui_manager, edj_path, GROUP_LAYOUT);
	evas_object_size_hint_weight_set(ui_manager, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	evas_object_size_hint_align_set(ui_manager, EVAS_HINT_FILL, EVAS_HINT_FILL);
	evas_object_show(ui_manager);

	evas_object_event_callback_add(ui_manager, EVAS_CALLBACK_RESIZE, __resize_cb, "ui_manager");

	evas_object_data_set(ui_manager, DATA_KEY_SHARE_PANEL_INFO, share_panel);

	goto_if(!__create_bg_rect(ui_manager, share_panel->root_width, share_panel->root_height), ERROR);

	goto_if(!__create_back_btn(ui_manager, share_panel), ERROR);

	elm_object_part_text_set(ui_manager, "title", _("IDS_AM_HEADER_SHARE"));
	elm_object_domain_translatable_part_text_set(ui_manager, "title", PROJECT, "IDS_AM_HEADER_SHARE");

	share_panel->index = _index_create(ui_manager, 0);
	goto_if(!share_panel->index, ERROR);
	elm_object_part_content_set(ui_manager, "index", share_panel->index);

	share_panel->scroller = _scroller_create(ui_manager, share_panel);
	goto_if(!share_panel->scroller, ERROR);
	elm_object_part_content_set(ui_manager, "scroller", share_panel->scroller);

	if (SHARE_PANEL_ERROR_NONE != _scroller_register_event_cb(share_panel->scroller,
			SCROLLER_EVENT_TYPE_SCROLL, _scroll_cb, share_panel->index)) {
		_E("cannot register the scroller event");
	}
	if (SHARE_PANEL_ERROR_NONE != _scroller_register_event_cb(share_panel->scroller,
			SCROLLER_EVENT_TYPE_APPEND_PAGE, _append_page_cb, share_panel->index)) {
		_E("cannot register the scroller event");
	}
	if (SHARE_PANEL_ERROR_NONE != _scroller_register_event_cb(share_panel->scroller,
			SCROLLER_EVENT_TYPE_REMOVE_PAGE, _remove_page_cb, share_panel->index)) {
		_E("cannot register the scroller event");
	}

	share_panel->list = _list_create(share_panel);
	if (!share_panel->list) {
		notification_status_message_post(_("IDS_BT_POP_NO_APPLICATIONS_AVAILABLE_TO_OPEN_FILES"));
		ui_app_exit();
	}

	share_panel->list_count = eina_list_count(share_panel->list);

	if (1 == share_panel->list_count) {
		item_s *item_info = NULL;
		int ret = 0;

		_D("Only one item on the list. Launching.");

		item_info = eina_list_nth(share_panel->list, 0);
		goto_if(!item_info, ERROR);

		ret = _app_control_launch(item_info);

		if (ret < 0) {
			_E("Fail to launch app(%d)", ret);
		}

		ui_app_exit();
	}

	_scroller_append_list(share_panel->scroller
		, share_panel->list
		, share_panel->page_width
		, share_panel->page_height);

	edje = elm_layout_edje_get(ui_manager);
	goto_if(!edje, ERROR);

	if (share_panel->list_count <= SHARE_PANEL_LINE_MAX) {
		if (share_panel->root_width < share_panel->root_height) {
			edje_object_signal_emit(edje, "1line", "fixed_bg");
		} else {
			edje_object_signal_emit(edje, "1line,land", "fixed_bg");
		}
	}

	edje_object_message_signal_process(edje);

	free(edj_path);
	return ui_manager;

ERROR:
	free(edj_path);
	_ui_manager_destroy(ui_manager);
	return NULL;
}


void _ui_manager_destroy(Evas_Object *ui_manager)
{
	Evas_Object *button = NULL;
	Evas_Object *bg = NULL;
	share_panel_s *share_panel = NULL;
	ret_if(!ui_manager);

	share_panel = evas_object_data_del(ui_manager, DATA_KEY_SHARE_PANEL_INFO);
	ret_if(!share_panel);

	if (share_panel->list) {
		_scroller_remove_list(share_panel->scroller, share_panel->list);
		_list_destroy(share_panel->list);
	}

	if (share_panel->scroller) {
		_scroller_unregister_event_cb(share_panel->scroller, SCROLLER_EVENT_TYPE_SCROLL, _scroll_cb);
		_scroller_unregister_event_cb(share_panel->scroller, SCROLLER_EVENT_TYPE_APPEND_PAGE, _append_page_cb);
		_scroller_unregister_event_cb(share_panel->scroller, SCROLLER_EVENT_TYPE_REMOVE_PAGE, _remove_page_cb);
		_scroller_destroy(share_panel->scroller);
	}

	if (share_panel->index) {
		_index_destroy(share_panel->index);
	}

	button = elm_object_part_content_unset(ui_manager, "effect");
	if(button) {
		__destroy_back_btn(button);
	}

	bg = elm_object_part_content_unset(ui_manager, "fixed_bg");
	if(bg) {
		__destroy_bg_rect(bg);
	}

	evas_object_del(ui_manager);
}


void _ui_manager_resize(Evas_Object *ui_manager, int width, int height)
{
	ret_if(!ui_manager);

	evas_object_size_hint_min_set(ui_manager, width, height);
	evas_object_size_hint_max_set(ui_manager, width, height);
	evas_object_resize(ui_manager, width, height);
	evas_object_move(ui_manager, 0, 0);
	edje_object_message_signal_process(elm_layout_edje_get(ui_manager));
}


int _ui_manager_show(share_panel_h share_panel)
{
	retv_if(!share_panel, SHARE_PANEL_ERROR_INVALID_PARAMETER);
	retv_if(!share_panel->ui_manager, SHARE_PANEL_ERROR_INVALID_PARAMETER);

	evas_object_show(share_panel->ui_manager);

	share_panel->show_state = SHARE_PANEL_SHOW_STATE_HALF;

	return SHARE_PANEL_ERROR_NONE;
}


int _ui_manager_hide(share_panel_h share_panel)
{
	retv_if(!share_panel, SHARE_PANEL_ERROR_INVALID_PARAMETER);
	retv_if(!share_panel->ui_manager, SHARE_PANEL_ERROR_INVALID_PARAMETER);

	share_panel->show_state = SHARE_PANEL_SHOW_STATE_HIDE;
	evas_object_hide(share_panel->ui_manager);

	return SHARE_PANEL_ERROR_NONE;
}
