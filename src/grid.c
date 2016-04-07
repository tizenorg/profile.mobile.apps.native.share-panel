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

#include <Elementary.h>
#include <app.h>
#include <app_manager.h>
#include <app_control.h>

#include "share_panel_internal.h"
#include "conf.h"
#include "grid.h"
#include "log.h"
#include "utils.h"

#define PRIVATE_DATA_KEY_ITEM_INFO "pdkii"

static struct {
	Elm_Gengrid_Item_Class *gic;
	char *default_icon;
	int index;
} grid_info = {
	.gic = NULL,
	.default_icon = "/usr/share/icons/A01-1_icon_Menu.png",
	.index = 0,
};

static char *__text_get(void *data, Evas_Object *obj, const char *part)
{
	item_s *info = data;
	retv_if(!info, NULL);

	retv_if(!info->name, NULL);
	if (!strcmp(part, "elm.text"))
		return strdup(D_(info->name));

	return NULL;
}


static Evas_Object *__add_icon(Evas_Object *parent, const char *file)
{
	const char *real_icon_file = NULL;
	Evas_Object *icon = NULL;
	Evas_Object *icon_layout = NULL;

	real_icon_file = file;
	if (access(real_icon_file, R_OK) != 0) {
		_E("Failed to access an icon(%s)", real_icon_file);
		real_icon_file = DEFAULT_ICON;
	}

	icon = elm_icon_add(parent);
	retv_if(!icon, NULL);

	if (elm_image_file_set(icon, real_icon_file, NULL) == EINA_FALSE) {
		_E("Icon file is not accessible (%s)", real_icon_file);
		evas_object_del(icon);
		return NULL;
	}

	evas_object_size_hint_min_set(icon, ELM_SCALE_SIZE(ITEM_ICON_WIDTH), ELM_SCALE_SIZE(ITEM_ICON_HEIGHT));

	elm_image_preload_disabled_set(icon, EINA_TRUE);
	elm_image_smooth_set(icon, EINA_TRUE);
	elm_image_no_scale_set(icon, EINA_FALSE);
	evas_object_show(icon);

	icon_layout = elm_layout_add(parent);
	retv_if(!icon_layout, NULL);

	char *edj_path = utils_get_res_file_path("edje/item.edj");

	elm_layout_file_set(icon_layout, edj_path, "grid,icon");
	evas_object_size_hint_weight_set(icon_layout, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	evas_object_size_hint_align_set(icon_layout, EVAS_HINT_FILL, EVAS_HINT_FILL);
	evas_object_show(icon_layout);

	elm_object_part_content_set(icon_layout, "icon", icon);

	free(edj_path);

	return icon_layout;
}

static Evas_Object *__content_get(void *data, Evas_Object *obj, const char *part)
{
	item_s *info = data;

	retv_if(!info, NULL);

	if (!strcmp(part, "elm.swallow.end")) {
		Evas_Object *bg = evas_object_rectangle_add(evas_object_evas_get(obj));
		retv_if(!bg, NULL);

		evas_object_size_hint_min_set(bg, ELM_SCALE_SIZE(ITEM_WIDTH), ELM_SCALE_SIZE(ITEM_HEIGHT));
		evas_object_color_set(bg, 0, 0, 0, 0);
		evas_object_show(bg);
		return bg;
	} else if (!strcmp(part, "elm.swallow.icon")) {
		retv_if(!info->icon, NULL);
		return __add_icon(obj, info->icon);
	} else if (!strcmp(part, "selected")) {

	}
	return NULL;
}

static void __del(void *data, Evas_Object *obj)
{
	ret_if(NULL == data);
	evas_object_data_del(obj, PRIVATE_DATA_KEY_ITEM_INFO);
}

void _app_reply_cb (app_control_h request, app_control_h reply,
						app_control_result_e result, void *user_data)
{
	if (result == APP_CONTROL_RESULT_APP_STARTED || result == APP_CONTROL_RESULT_SUCCEEDED)
	ui_app_exit();
}

int _app_control_launch(item_s *item)
{
	retv_if(!item->caller_control, APP_CONTROL_ERROR_INVALID_PARAMETER);

	int ret = APP_CONTROL_ERROR_NONE;

	ret = app_control_set_app_id(item->caller_control, item->appid);
	retv_if(ret != APP_CONTROL_ERROR_NONE, ret);

	ret = app_control_send_launch_request(item->caller_control, _app_reply_cb, NULL);
	retv_if(ret != APP_CONTROL_ERROR_NONE, ret);

	_D("app launched");

	return ret;
}

static void __item_selected(void *data, Evas_Object *obj, void *event_info)
{
	_D("__item_selected");
	item_s *item_info = (item_s *) data;

//	See explanation in another comment below
//	Elm_Object_Item *selected_item = NULL;

	int ret = 0;

	ret_if(!item_info);
	ret_if(!item_info->appid);
	ret_if(!item_info->share_panel);
	_D("item clicked, launch app : %s", item_info->appid);

//	TODO
//	Temporary commented to avoid double invoke. This is a bug. JIRA issue have been raised.
//
//	selected_item = elm_gengrid_selected_item_get(obj);
//	ret_if(!selected_item);
//	elm_gengrid_item_selected_set(selected_item, EINA_FALSE);


	ret = _app_control_launch(item_info);
	if (ret < 0)
		_E("Fail to launch app(%d)", ret);

	item_info->share_panel->after_launch = 1;

	elm_object_signal_emit(item_info->share_panel->ui_manager, "show", "blocker");
}

static void __lang_changed_cb(void *data, Evas_Object *grid, void *event_info)
{
	Elm_Object_Item *it = NULL;
	int ret = 0;
	ret_if(!grid);

	it = elm_gengrid_first_item_get(grid);
	while (it) {
		app_info_h app_info = NULL;
		item_s *item_info = NULL;
		char *name = NULL;

		item_info = evas_object_data_get(it, PRIVATE_DATA_KEY_ITEM_INFO);
		if (!item_info) {
			it = elm_gengrid_item_next_get(it);
			continue;
		}

		ret = app_info_create(item_info->appid, &app_info);
		if (ret != APP_MANAGER_ERROR_NONE && !app_info) {
			it = elm_gengrid_item_next_get(it);
			continue;
		}

		ret = app_info_get_label(app_info, &name);
		if (ret == APP_MANAGER_ERROR_NONE && name) {
			free(item_info->name);
			item_info->name = name;
			elm_object_item_part_text_set(it, "elm.text", item_info->name);
			elm_gengrid_item_update(it);
		} else {
			_E("fail to create the app_name from app_info");
		}

		app_info_destroy(app_info);
	}
}

Evas_Object *_grid_create(Evas_Object *page)
{
	Evas_Object *grid = NULL;

	retv_if(!page, NULL);

	grid = elm_gengrid_add(page);
	if (!grid) {
		_grid_destroy(grid);
		return NULL;
	}

	evas_object_size_hint_weight_set(grid, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	evas_object_size_hint_align_set(grid, EVAS_HINT_FILL, EVAS_HINT_FILL);

	elm_gengrid_item_size_set(grid, ELM_SCALE_SIZE(ITEM_WIDTH), ELM_SCALE_SIZE(ITEM_HEIGHT));
	elm_gengrid_align_set(grid, 0.0, 0.0);
	elm_gengrid_horizontal_set(grid, EINA_FALSE);
	elm_gengrid_multi_select_set(grid, EINA_FALSE);

	grid_info.gic = elm_gengrid_item_class_new();
	if (!grid_info.gic) {
		_grid_destroy(grid);
		return NULL;
	}

	grid_info.gic->func.text_get = __text_get;
	grid_info.gic->func.content_get = __content_get;
	grid_info.gic->func.state_get = NULL;
	grid_info.gic->func.del = __del;
	grid_info.gic->item_style = "type2";

	evas_object_smart_callback_add(grid, "language,changed", __lang_changed_cb, NULL);

	elm_scroller_movement_block_set(grid, ELM_SCROLLER_MOVEMENT_BLOCK_VERTICAL);
	evas_object_show(grid);

	return grid;
}

void _grid_destroy(Evas_Object *grid)
{
	ret_if(!grid);

	evas_object_del(grid);
}

Elm_Object_Item *_grid_append_item(Evas_Object *grid, item_s *item_info)
{
	Elm_Object_Item *item = NULL;

	retv_if(!grid, NULL);
	retv_if(!item_info, NULL);
	retv_if(!grid_info.gic, NULL);

	item = elm_gengrid_item_append(grid, grid_info.gic, item_info, __item_selected, item_info);
	retv_if(!item, NULL);
	evas_object_data_set(item, PRIVATE_DATA_KEY_ITEM_INFO, item_info);
	if (item_info->name)
		_D("grid append item : %s", item_info->name);

	item_info->grid_item = item;
	elm_gengrid_item_show(item, ELM_GENGRID_ITEM_SCROLLTO_NONE);
	elm_gengrid_item_update(item);

	return item;
}

void _grid_remove_item(Evas_Object *grid, item_s *item_info)
{
	Elm_Object_Item *item = NULL;

	ret_if(!grid);
	ret_if(!item_info);

	item = item_info->grid_item;

	ret_if(!item);
	evas_object_data_del(item, PRIVATE_DATA_KEY_ITEM_INFO);
	elm_object_item_del(item);
	item_info->grid_item = NULL;
}

int _grid_count_item(Evas_Object *grid)
{
	int count = 0;

	retv_if(!grid, 0);

	count = elm_gengrid_items_count(grid);

	return count;
}
