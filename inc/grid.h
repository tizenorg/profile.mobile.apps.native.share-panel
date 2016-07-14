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

#ifndef __TIZEN_SHARE_PANEL_GRID_H__
#define __TIZEN_SHARE_PANEL_GRID_H__

#include <Elementary.h>

/**
 * @defgroup Grid Grid
 */

/**
 * @addtogroup Grid
 * @{
 */

/**
 * @brief Creates grid of applications that meet sharing requirements.
 *
 * @param page The page widget
 *
 * @return Created grid
 */
extern Evas_Object *_grid_create(Evas_Object *page);

/**
 * @brief Destroys the applications grid.
 *
 * @param grid The grid to be destroyed
 */
extern void _grid_destroy(Evas_Object *grid);

/**
 * @brief Appends item to the applications grid.
 *
 * @param grid The grid widget
 * @param item_info The info structure of the item to be appended
 *
 * @return Elm_Object_Item of the appended item
 */
extern Elm_Object_Item *_grid_append_item(Evas_Object *grid, item_s *item_info);

/**
 * @brief Removes item from the application grid.
 *
 * @param grid The grid from where item is removed
 * @param item_info Info structure of the item to be removed
 */
extern void _grid_remove_item(Evas_Object *grid, item_s *item_info);

/**
 * @brief Returns number of items in application grid.
 * 
 * @param grid The grid widget
 */
extern int _grid_count_item(Evas_Object *grid);

/**
 * @brief Sends app control launch request
 *
 * @param item The info structure of the item related with app to be launched
 * 
 * @return app_control_error_e type value
 */
int _app_control_launch(item_s *item);

/**
 * @}
 */

#endif /* __TIZEN_SHARE_PANEL_GRID_H__ */
