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
#include <bundle.h>

#include "share_panel_internal.h"
#include "conf.h"
#include "grid.h"
#include "log.h"

#define PRIVATE_DATA_KEY_ITEM_INFO "pdkii"

struct _launch_data {
	app_control_h app_control;
    app_control_h caller_control;
    char *uri;
    char *operation;
    char **data_array;
    int data_array_size;
};

typedef struct _launch_data launch_data_t;

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

static void _app_control_launch_release(launch_data_t launch_data)
{
	int i;

	if(launch_data.app_control)
		app_control_destroy(launch_data.app_control);

	if(launch_data.caller_control)
		app_control_destroy(launch_data.caller_control);

	free(launch_data.operation);
	free(launch_data.uri);

	if (!launch_data.data_array)
		return;

	for (i = 0; i < launch_data.data_array_size; i++)
		free(launch_data.data_array[i]);
}

static bool _app_control_error_handle(launch_data_t launch_data, int ret_value, const char *func_name)
{
	if (ret_value != APP_CONTROL_ERROR_NONE) {
		_app_control_launch_release(launch_data);
		_E("(ret != APP_CONTROL_ERROR_NONE) -> %s() return", func_name);
		return true;
	}

	return false;
}

static void __bundle_data_foreach_cb(const char *key, const int type, const bundle_keyval_t *kv, void *user_data)
{
	app_control_h app_control = (app_control_h)user_data;
	int ret;
	void *val = NULL;
	size_t size = 0;
	void **array_val = NULL;
	unsigned int array_len = 0;
	size_t *array_elem_size = NULL;

	if (bundle_keyval_type_is_array((bundle_keyval_t *)kv)) {
		ret = bundle_keyval_get_array_val((bundle_keyval_t *)kv, &array_val, &array_len, &array_elem_size);
		ret_if(ret != BUNDLE_ERROR_NONE);

		ret = app_control_add_extra_data_array(app_control, key, (const char **)array_val, array_len);
		ret_if(ret != APP_CONTROL_ERROR_NONE);
	} else {
		ret = bundle_keyval_get_basic_val((bundle_keyval_t *)kv, &val, &size);
		ret_if(ret != BUNDLE_ERROR_NONE);

		app_control_add_extra_data(app_control, key, (const char *)val);
		ret_if(ret != APP_CONTROL_ERROR_NONE);
	}
}

int _app_control_launch(item_s *item)
{
	int ret = APP_CONTROL_ERROR_NONE;
	launch_data_t launch_data = {0,};

	ret = app_control_create(&launch_data.app_control);
	retv_if(ret != APP_CONTROL_ERROR_NONE, ret);

	ret = app_control_create(&launch_data.caller_control);
	if (_app_control_error_handle(launch_data, ret, "app_control_create"))
		return ret;

	bundle_foreach(item->b, __bundle_data_foreach_cb, (void *)launch_data.caller_control);

	ret = app_control_get_operation(launch_data.caller_control, &launch_data.operation);
	if (_app_control_error_handle(launch_data, ret, "app_control_get_operation"))
		return ret;

	ret = app_control_get_uri(launch_data.caller_control, &launch_data.uri);
	if (_app_control_error_handle(launch_data, ret, "app_control_get_uri"))
		return ret;

	ret = app_control_get_extra_data_array(launch_data.caller_control, TIZEN_DATA_PATH, &launch_data.data_array, &launch_data.data_array_size);
	if (_app_control_error_handle(launch_data, ret, "app_control_get_extra_data_array"))
		return ret;

	_D("Operation: %s", launch_data.operation);

	ret = app_control_set_operation(launch_data.app_control, launch_data.operation);
	if (_app_control_error_handle(launch_data, ret, "app_control_set_operation"))
		return ret;

	ret = app_control_set_uri(launch_data.app_control, launch_data.uri);
	if (_app_control_error_handle(launch_data, ret, "app_control_set_uri"))
		return ret;

	ret = app_control_add_extra_data_array(launch_data.app_control, TIZEN_DATA_PATH, (const char **)launch_data.data_array, launch_data.data_array_size);
	if (_app_control_error_handle(launch_data, ret, "app_control_add_extra_data_array"))
		return ret;

	ret = app_control_set_app_id(launch_data.app_control, item->appid);
	if (_app_control_error_handle(launch_data, ret, "app_control_set_app_id"))
		return ret;

	ret = app_control_set_launch_mode(launch_data.app_control, APP_CONTROL_LAUNCH_MODE_GROUP);
	if (_app_control_error_handle(launch_data, ret, "app_control_set_launch_mode"))
		return ret;

	ret = app_control_send_launch_request(launch_data.app_control, NULL, NULL);
	if (_app_control_error_handle(launch_data, ret, "app_control_send_launch_request"))
		return ret;

	_D("app launched");

	_app_control_launch_release(launch_data);
	return ret;
}

static void __item_selected(void *data, Evas_Object *obj, void *event_info)
{
	item_s *item_info = data;
	Elm_Object_Item *selected_item = NULL;

	int ret = 0;

	ret_if(!item_info);
	ret_if(!item_info->appid);
	ret_if(!item_info->b);
	ret_if(!item_info->share_panel);
	_D("item clicked, launch app : %s", item_info->appid);

	selected_item = elm_gengrid_selected_item_get(obj);
	ret_if(!selected_item);
	elm_gengrid_item_selected_set(selected_item, EINA_FALSE);

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
