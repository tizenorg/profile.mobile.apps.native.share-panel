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

#ifndef __TIZEN_SHARE_PANEL_UI_MANAGER_H__
#define __TIZEN_SHARE_PANEL_UI_MANAGER_H__

#include <Elementary.h>

/**
 * @ingroup View
 * @defgroup Man UI Manager
 * @addtogroup Man
 * @{
 */

/**
 * @brief Creates share panel ui manager.
 *
 * @param share_panel The share panel main info structure
 *
 * @return created ui manager
 */
extern Evas_Object *_ui_manager_create(share_panel_s *share_panel);

/**
 * @brief Destroys share panel ui manager.
 *
 * @param ui_manager The UI to be destroyed
 */
extern void _ui_manager_destroy(Evas_Object *ui_manager);

/**
 * @brief Resizes the share panel's UI.
 *
 * @param ui_manager The UI widget
 * @param width The new width size to be set
 * @param height The new height size to be set
 */
extern void _ui_manager_resize(Evas_Object *ui_manager, int width, int height);

/**
 * @brief Shows share panel's UI.
 *
 * @param share_panel The share panel info structure
 */
extern int _ui_manager_show(share_panel_h share_panel);

/**
 * @brief Hides share panel's UI.
 *
 * @param share_panel The share panel info structure
 *
 * @return share_panel_error_e enumeration
 */
extern int _ui_manager_hide(share_panel_h share_panel);

/**
 * @}
 */

#endif /* __TIZEN_SHARE_PANEL_UI_MANAGER_H__ */
