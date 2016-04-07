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

#ifndef __TIZEN_SHARE_PANEL_CONF_H__
#define __TIZEN_SHARE_PANEL_CONF_H__

#define BASE_WIDTH 480
#define BASE_HEIGHT 800

/* ITEM_ICON is changed from 74 to 63 about 87% decrease because of resolution issues. */
#define ITEM_ICON_WIDTH 63
#define ITEM_ICON_HEIGHT 63

/* ITEM_WIDTH is changed from 123 to 107 about 87% decrease because of resolution issues. */
#define ITEM_WIDTH 118
/* ITEM_HEIGHT is changed from 166 to 144 about 87% decrease because of resolution issues. */
#define ITEM_HEIGHT 144

#define INDEX_HEIGHT 50
#define TITLE_HEIGHT 54
#define SCROLLER_HEIGHT 300

#define SHARE_PANEL_LINE_MAX 4

/* PANEL_HEIGHT is changed from 368 to 320 about 87% decrease because of resolution issues. */
#define PANEL_HEIGHT (SCROLLER_HEIGHT+INDEX_HEIGHT+TITLE_HEIGHT)

#define PROJECT "share-panel"
#define PACKAGE "org.tizen.share-panel"

#endif /* __TIZEN_SHARE_PANEL_CONF_H__ */
