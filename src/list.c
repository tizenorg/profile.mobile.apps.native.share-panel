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
#include <app_control.h>
#include <app_manager.h>
#include <glib.h>
#include <mime_type.h>
#include <bundle.h>

#include "share_panel_internal.h"
#include "log.h"


static bool __app_control_matched_cb(app_control_h service, const char *appid, void *user_data)
{
	Eina_List **list = user_data;
	char *caller = NULL;
	char *tmp = NULL;

	retv_if(!list, false);
	retv_if(!appid, false);

	app_control_get_caller(service, &caller);

	_SD("matched app : [%s]", appid);
	if (caller && !strcmp(appid, caller)) {
		_D("caller and callee is same");
		return true;
	}

	tmp = strdup(appid);
	retv_if(!tmp, false);
	*list = eina_list_append(*list, tmp);
	return true;
}


static void __create_single_share_list(app_control_h control, Eina_List **list)
{
	app_control_foreach_app_matched(control, __app_control_matched_cb, list);
}


static int __get_file_ext(const char *filepath, char **file_ext)
{
	const char *filename = NULL;
	char *pdot = NULL;

	retv_if(!filepath, -1);
	retv_if(!file_ext, -1);

	filename = ecore_file_file_get(filepath);

	retv_if(!filename, -1);

	pdot = strrchr(filename, '.');

	if (!pdot) {
		return -1;
	} else if (pdot != filepath) {
		*file_ext = pdot + 1;
		return 0;
	} else {
		return -1;
	}
}


static inline void __get_mime(const char *file_path, char **mime)
{
	char *ext = NULL;
	int res;

	ret_if(!file_path);

	if (strncmp(file_path, "/", 1) == 0) {
		res = __get_file_ext(file_path, &ext);
		if (0 != res) {
			_D("__get_file_ext error..");
		} else {
			mime_type_get_mime_type(ext, mime);
			_D("mime type: [%s]", *mime);
		}
	} else if (strncmp(file_path, "file:/", 6) == 0) {
		res = __get_file_ext(&file_path[5], &ext);
		if (0 != res) {
			_D("__get_file_ext error..");
		} else {
			mime_type_get_mime_type(ext, mime);
			_D("mime type: [%s]", *mime);
		}
	} else if (strncmp(file_path, "file:///", 8) == 0) {
		res = __get_file_ext(&file_path[7], &ext);
		if (0 != res) {
			_D("__get_file_ext error..");
		} else {
			mime_type_get_mime_type(ext, mime);
			_D("mime type: [%s]", *mime);
		}
	}
}


static bool __app_control_file_matched_cb(app_control_h service, const char *appid, void *user_data)
{
	Eina_List **sublist = user_data;

	*sublist = eina_list_append(*sublist, strdup(appid));
	return true;
}


static void __intersect_match_list_with_mime(Eina_List **matchlist, const char *mime)
{
	app_control_h control;
	Eina_List *l = NULL;
	Eina_List *ln = NULL;
	Eina_List *sublist = NULL;
	Eina_List *sub_ln = NULL;

	void *m_list_data = NULL;
	void *s_list_data = NULL;
	bool exist;

	app_control_create(&control);
	app_control_set_operation(control, TIZEN_MULTI_SHARE);
	app_control_set_mime(control, mime);
	app_control_foreach_app_matched(control, __app_control_file_matched_cb, &sublist);
	app_control_destroy(control);

	if (!*matchlist) {
		*matchlist = sublist;
		return;
	}

	EINA_LIST_FOREACH_SAFE(*matchlist, l, ln, m_list_data) {
		exist = false;
		EINA_LIST_FOREACH(sublist, sub_ln, s_list_data) {
			if (!strcmp(m_list_data, s_list_data)) {
				_SD("[%s] is inlcude!", m_list_data);
				exist = true;
				break;
			}
		}
		if (!exist) {
			_SD("[%s] is not include!", m_list_data);
			*matchlist = eina_list_remove(*matchlist, m_list_data);
		}
	}

	if (sublist) {
		EINA_LIST_FREE(sublist, m_list_data) {
			free(m_list_data);
		}
	}
}


static void __trim_uri(app_control_h control)
{
	char *uri = NULL;
	char *tmp = NULL;

	app_control_get_uri(control, &uri);
	_SD("uri : [[[%s]]]", uri);
	if (!uri) {
		return;
	}

	if (!strncmp(uri, "/", 1)) {
		tmp = uri;
	} else if (!strncmp(uri, "file:///", 8)) {
		tmp = &uri[7];
	} else {
		_E("file uri scheme is wrong");
		free(uri);
		return;
	}

	_D("set uri is [[[%s]]]", tmp);

	app_control_set_uri(control, tmp);
	free(uri);
}


static void __create_multi_share_list(app_control_h control, Eina_List **matchlist)
{
	char **data_array = NULL;
	char *mime = NULL;
	Eina_List *mime_list = NULL;

	int i;
	int length = 0;
	int res;

	res = app_control_get_extra_data_array(control, TIZEN_DATA_PATH, &data_array, &length);
	if (res != APP_CONTROL_ERROR_NONE) {
		_E("app_control_get_extra_data_array failed..[%d]", res);
	}
	for (i = 0; i < length; i++) {
		Eina_List *l = NULL;
		char *tmp = NULL;

		_SD("data_array[%d]= %s", i, data_array[i]);

		__get_mime(data_array[i], &mime);
		if (!mime) {
			_E("no mime");
			goto out;
		}

		EINA_LIST_FOREACH(mime_list, l, tmp) {
			if (!strcmp(tmp, mime)) {
				free(mime);
				goto out;
			}
		}

		mime_list = eina_list_prepend(mime_list, mime);
		_SD("data_array[%d]= %s", i, data_array[i]);

out:
		free(data_array[i]);
	}

	free(data_array);

	EINA_LIST_FREE(mime_list, mime) {
		__intersect_match_list_with_mime(matchlist, mime);
		free(mime);
		mime = NULL;
	}
}


static int __applist_compare(const void *com1, const void *com2)
{
	item_s *c1 = (item_s *)com1, *c2 = (item_s *)com2;
	int res = 0;

	res = strcasecmp(c1->name, c2->name);

	return res;
}


static void __make_applist(bundle *b, Eina_List *matchlist, Eina_List **applist)
{
	Eina_List *l;
	char *data = NULL;
	int ret = 0;

	ret_if(!b);

	EINA_LIST_FOREACH(matchlist, l, data) {
		item_s *app_s = NULL;
		app_info_h app_info = NULL;

		app_s = (item_s *) malloc(sizeof(item_s));
		continue_if(!app_s);

		app_s->appid = strdup(data);
		if (!app_s->appid) {
			_E("strdup error");
			free(app_s);
			continue;
		}
		app_s->b = b;

		ret = app_info_create(data, &app_info);
		if (ret != APP_MANAGER_ERROR_NONE || !app_info) {
			_E("fail to create the app_info");
			free(app_s->appid);
			free(app_s);
			continue;
		}

		ret = app_info_get_label(app_info, &app_s->name);
		if (ret != APP_MANAGER_ERROR_NONE || !app_s->name) {
			_E("fail to create the app_name from app_info");
			free(app_s->appid);
			free(app_s);
			app_info_destroy(app_info);
			continue;
		}
		_SD("icon_name : [%s]", app_s->name);

		ret = app_info_get_icon(app_info, &app_s->icon);
		if (ret != APP_MANAGER_ERROR_NONE) {
			free(app_s->appid);
			free(app_s->name);
			free(app_s);
			app_info_destroy(app_info);
			continue;
		}

		app_info_destroy(app_info);

		if (!app_s->icon) {
			app_s->icon = strdup(DEFAULT_ICON);
		}
		_SD("icon_path : [%s]", app_s->icon);

		*applist = eina_list_sorted_insert(*applist, __applist_compare, app_s);
	}
}


Eina_List *_list_create(app_control_h control, bundle *b)
{
	Eina_List *matchlist = NULL;
	Eina_List *applist  = NULL;
	char *operation_type = NULL;
	char *data = NULL;

	retv_if(!control, NULL);
	retv_if(!b, NULL);

	app_control_get_operation(control, &operation_type);
	retv_if(!operation_type, NULL);

	__trim_uri(control);

	if (!strcmp(operation_type, TIZEN_SHARE)
		|| !strcmp(operation_type, TIZEN_SHARE_TEXT)) {
		_D("single share operation");
		__create_single_share_list(control, &matchlist);
	} else if (!strcmp(operation_type, TIZEN_MULTI_SHARE)) {
		_D("multi share operation");
		__create_multi_share_list(control, &matchlist);
	} else {
		_D("Wrong operation");
		return NULL;
	}

	__make_applist(b, matchlist, &applist);

	EINA_LIST_FREE(matchlist, data) {
		free(data);
	}

	return applist;
}


void _list_destroy(Eina_List *list)
{
	ret_if(!list);
	eina_list_free(list);
}

