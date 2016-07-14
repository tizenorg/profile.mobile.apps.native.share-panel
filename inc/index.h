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

#ifndef __TIZEN_SHARE_PANEL_INDEX_H__
#define __TIZEN_SHARE_PANEL_INDEX_H__

#include <Evas.h>

/**
 * @ingroup View
 * @defgroup Index Index
 * @addtogroup Index
 * @{
 */

/**
 * @brief Creates share panel page index.
 *
 * @param ui_manager The parent layout
 * @param count Number of pages
 *
 * @return created index
 */
extern Evas_Object *_index_create(Evas_Object *ui_manager, unsigned int count);

/**
 * @brief Updates the index status.
 *
 * @param index The index to be updated
 * @param count Number of pages
 */
extern void _index_update(Evas_Object *index, unsigned int count);

/**
 * @brief Destroys index.
 *
 * @param index The index to be destroyed
 */
extern void _index_destroy(Evas_Object *index);

/**
 * @brief Sets index page.
 *
 * @param index The index object to be set
 * @param idx The index
 */
extern void _index_bring_in(Evas_Object *index, int idx);

/**
 * @}
 */

#endif //__TIZEN_SHARE_PANEL_INDEX_H__
