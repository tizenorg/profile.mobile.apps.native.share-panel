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

#ifndef __TIZEN_SHARE_PANEL_INTERNAL_H__
#define __TIZEN_SHARE_PANEL_INTERNAL_H__

#include <Elementary.h>
#include <app_control.h>

#define DATA_KEY_SHARE_PANEL "__dksp__"
#define DATA_KEY_SHARE_PANEL_INFO "__dkspi__"
#define TABBAR_NAME_MORE "More"
#define BUF_SIZE 128
#define TIZEN_DATA_PATH "http://tizen.org/appcontrol/data/path"
#define TIZEN_SHARE "http://tizen.org/appcontrol/operation/share"
#define TIZEN_MULTI_SHARE "http://tizen.org/appcontrol/operation/multi_share"
#define TIZEN_SHARE_TEXT "http://tizen.org/appcontrol/operation/share_text"
#define DEFAULT_ICON "/usr/share/icons/A01-1_icon_Menu.png"
#define KEY_BACK "XF86Back"



enum {
	SHARE_PANEL_SHOW_STATE_HIDE,
	SHARE_PANEL_SHOW_STATE_HALF,
};



struct _share_panel {
	Evas_Object *win;
	Evas_Object *ui_manager;
	Evas_Object *scroller;
	Evas_Object *index;

	Eina_List *list;
	Ecore_Idler *append_idler;
	Elm_Transit *transit;
	app_control_h control;
	bundle *b;

	int root_width;
	int root_height;
	int page_width;
	int page_height;
	int show_state;
	int list_count;
	int after_launch;
};
typedef struct _share_panel share_panel_s;



struct _item {
	app_control_h caller_control;
	share_panel_s *share_panel;

	char *appid;
	char *name;
	char *icon;
	Elm_Object_Item *grid_item;
};
typedef struct _item item_s;



#endif // __TIZEN_SHARE_PANEL_INTERNAL_H__
