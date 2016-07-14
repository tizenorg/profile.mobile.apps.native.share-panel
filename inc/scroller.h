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

#ifndef __TIZEN_SHARE_PANEL_SCROLLER_H__
#define __TIZEN_SHARE_PANEL_SCROLLER_H__

#include <Elementary.h>

/**
 * @ingroup View
 * @defgroup Scroller Scroller
 * @addtogroup Scroller
 * @{
 */

/**
 * @brief Enumerations for share panel event error.
 */
typedef enum {
	SCROLLER_EVENT_TYPE_INVALID = 0,
	SCROLLER_EVENT_TYPE_SCROLL,
	SCROLLER_EVENT_TYPE_APPEND_PAGE,
	SCROLLER_EVENT_TYPE_REMOVE_PAGE,
	SCROLLER_EVENT_TYPE_MAX,
} scroller_event_type_e;

/**
 * @brief Creates application page scroller.
 *
 * @param ui_manager The main parent layout
 * @param share_panel The share panel info structure
 *
 * @return created scroller
 */
extern Evas_Object *_scroller_create(Evas_Object *ui_manager, share_panel_s *share_panel);

/**
 * @brief Destroys application page scroller.
 *
 * @param scroller The scroller to be destroyed
 */
extern void _scroller_destroy(Evas_Object *scroller);

/**
 * @brief Appends new page to the scroller.
 *
 * @param scroller The scroller widget
 * @param page The page to be appended
 */
extern void _scroller_append_page(Evas_Object *scroller, Evas_Object *page);

/**
 * @brief Removes page form the scroller.
 *
 * @param scroller The scroller widget
 * @param page The page to be removed
 */
extern void _scroller_remove_page(Evas_Object *scroller, Evas_Object *page);

/**
 * @brief Appends list items into the scroller.
 *
 * @param scroller The scroller widget
 * @param list The list of applications to be appended
 * @param page_width Width of the pages to be appended
 * @param page_height Height of the pages to be appended
 */
extern void _scroller_append_list(Evas_Object *scroller, Eina_List *list, int page_width, int page_height);

/**
 * @brief Removes list items from the scroller.
 *
 * @param scroller The scroller widget
 * @param list The list of applications to be removed
 */
extern void _scroller_remove_list(Evas_Object *scroller, Eina_List *list);

/**
 * @brief Brings in scroller's page.
 *
 * @param scroller The scroller widget
 * @param page The page to be bringed
 */
extern void _scroller_bring_in_page(Evas_Object *scroller, Evas_Object *page);

/**
 * @brief Gets region of the scroller's index.
 *
 * @param scroller The scroller widget
 *
 * @return index's region
 */
extern int _scroller_get_region_index(Evas_Object *scroller);

/**
 * @brief Gets the number of pages in scroller's box.
 *
 * @param scroller The scroller widget
 *
 * @return number of pages
 */
extern unsigned int _scroller_count(Evas_Object *scroller);

/**
 * @brief Gets current scroller's scrolling status
 *
 * @param scroller The scroller widget
 *
 * @return scroller's scrolling status
 */
extern int _scroller_is_scrolling(Evas_Object *scroller);

/**
 * @brief Resizes share panel scroller.
 *
 * @param scroller The scroller widget
 * @param width The new width value to be set
 * @param height The new height value to be set
 */
extern void _scroller_resize(Evas_Object *scroller, int width, int height);

/**
 * @brief Registers scroller event callbacks.
 *
 * @param scroller The scroller widget
 * @param event_type The type of the event
 * @param event_cb The callback function to be called
 * @param data The data to be passed to the event callback function
 *
 * @return share_panel_error_e enumeration value
 */
extern int _scroller_register_event_cb(Evas_Object *scroller, int event_type, void (*event_cb)(Evas_Object *scroller, int event_type, void *event_info, void *data), void *data);

/**
 * @brief Unregister scroller event callbacks.
 *
 * @param scroller The scroller widget
 * @param event_type The type of the event
 * @param event_cb The scroller's event callback function to be unregistered
 *
 * @return share_panel_error_e enumeration value
 */
extern int _scroller_unregister_event_cb(Evas_Object *scroller, int event_type, void (*event_cb)(Evas_Object *scroller, int event_type, void *event_info, void *data));

/**
 * @}
 */

#endif /* __TIZEN_SHARE_PANEL_SCROLLER_H__ */
