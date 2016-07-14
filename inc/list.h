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

#ifndef __TIZEN_SHARE_PANEL_LIST_H__
#define __TIZEN_SHARE_PANEL_LIST_H__

#include <Elementary.h>

/**
 * @defgroup Model Model
 */

/**
 * @ingroup Model
 * @defgroup List List
 * @addtogroup List
 * @{
 */

/**
 * @brief Creates list of the sharing data applications.
 *
 * @param share_panel Share panel info handle
 *
 * @return created list
 */
Eina_List *_list_create(share_panel_h share_panel);

/**
 * @brief Destroys list of the sharing data applications.
 *
 * @param list The list to be destroyed.
 */
void _list_destroy(Eina_List *list);

/**
 * @}
 */

#endif /* __TIZEN_SHARE_PANEL_LIST_H__ */
