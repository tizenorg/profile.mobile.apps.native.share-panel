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

#include "share_panel_internal.h"
#include "conf.h"
#include "log.h"
#include "page.h"
#include "utils.h"

Evas_Object *_page_create(Evas_Object *scroller, int page_width, int page_height)
{
	Evas_Object *page = NULL;
	Evas_Object *page_bg = NULL;
	Evas *e = NULL;

	char *edj_path = utils_get_res_file_path("edje/page.edj");

	page = elm_layout_add(scroller);
	if(!page) {
		free(edj_path);
		return NULL;
	}
	elm_layout_file_set(page, edj_path, "page");

	e = evas_object_evas_get(scroller);
	goto_if(!e, ERROR);

	page_bg = evas_object_rectangle_add(e);
	goto_if(!page_bg, ERROR);
	evas_object_size_hint_weight_set(page_bg, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	evas_object_size_hint_min_set(page_bg, page_width, page_height);
	evas_object_size_hint_max_set(page_bg, page_width, page_height);
	evas_object_color_set(page_bg, 0, 0, 0, 0);
	evas_object_repeat_events_set(page_bg, EINA_TRUE);
	evas_object_show(page_bg);
	elm_object_part_content_set(page, "bg", page_bg);

	evas_object_show(page);
	free(edj_path);
	return page;

ERROR:
	free(edj_path);
	_page_destroy(page);
	return NULL;
}



void _page_destroy(Evas_Object *page)
{
	Evas_Object *bg = NULL;

	ret_if(!page);

	bg = elm_object_part_content_unset(page, "bg");
	if (bg) {
		evas_object_del(bg);
	}

	evas_object_del(page);
}



void _page_resize(Evas_Object *page, int width, int height)
{
	Evas_Object *bg = NULL;

	ret_if(!page);

	bg = elm_object_part_content_get(page, "bg");
	ret_if(!bg);

	evas_object_size_hint_min_set(bg, width, height);
	evas_object_size_hint_max_set(bg, width, height);
}
