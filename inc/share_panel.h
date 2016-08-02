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

#ifndef __TIZEN_SHARE_PANEL_H__
#define __TIZEN_SHARE_PANEL_H__

#include <Elementary.h>
#include <tizen_error.h>
#include <app_control.h>

#ifdef __cplusplus
extern "C" {
#endif

/** This definition will be removed */
#ifndef TIZEN_ERROR_PANEL
#define TIZEN_ERROR_PANEL		-0x02850000
#endif

/**
 * @mainpage Share-Panel documentation
 *
 * Share-panel application is for sharing content(files, texts, contacts, etc.) using apps that are set for handling particular sharing option.
 *
 * There are three types of share:
 * - APP_CONTROL_OPERATION_SHARE
 * - APP_CONTROL_OPERATION_MULTI_SHARE
 * - APP_CONTROL_OPERATION_SHARE_TEXT
 *
 * Share-panel shows you which applications can handle your share request:
 * @image html screenshot.png
 * \n
 * <HR>
 * \n
 *
 * @section overview Detailed Description
 *
 * @ref share_content \n
 * @ref how_share_set_up
 *
 * @section share_content How to share content?
 * \n
 * To share content via share-panel you need to use App Control API. Below you can find example of how to share text:
 * @code
 * app_control_h control;
 * app_control_create(&control);

 * app_control_set_operation(control, APP_CONTROL_OPERATION_SHARE_TEXT);
 * app_control_add_extra_data(control, APP_CONTROL_DATA_TEXT, "TEST_TEXT");
 *
 * app_control_send_launch_request(control, NULL, NULL);
 * @endcode
 *
 * To share one file use APP_CONTROL_OPERATION_SHARE instead of APP_CONTROL_OPERATION_SHARE_TEXT and add file path as uri:
 * @code
 * app_control_set_uri(control, "file_path");
 * @endcode
 * To share more than one file use APP_CONTROL_OPERATION_MULTI_SHARE instead of APP_CONTROL_OPERATION_SHARE_TEXT and add files paths as extra data:
 * @code
 * app_control_add_extra_data_array(control, APP_CONTROL_DATA_PATH, { "first_path", "second_path" }, count_of_files_to_share);
 * @endcode
 *
 * Share-panel app will display all apps that are registered to share particular content basing on files extension.
 * \n\n
 * <HR>
 *
 * @section how_share_set_up How to set up app to be registered as sharing content app and might be displayed in share-panel?
 *
 * Go to tizen-manifest.xml file of your app.
 * Go to 'Advanced' tab and then to 'Application Control' section.
 * Click button 'Add...' and edit entry on the buttons left side
 * Chose operation from drop-down list as one of:
 *  - http://tizen.org/appcontrol/operation/multi_share
 *  - http://tizen.org/appcontrol/operation/share
 *  - http://tizen.org/appcontrol/operation/share_text
 *
 * Chose 'Mime' from drop-down list or left empty if your app is able to share all files extension.
 *
 *
 */

/**
 * @defgroup View View
 * @defgroup SPanel Share Panel
 * @addtogroup SPanel
 * @{
 * 	@defgroup SPInternal Share Panel Internal
 */

/**
 * @brief Share panel handle.
 * @since_tizen 2.4
 */
typedef struct _share_panel *share_panel_h;

/**
 * @brief Enumeration for values of share-panel response types.
 * @since_tizen 2.4
 */
typedef enum share_panel_error {
	SHARE_PANEL_ERROR_NONE = TIZEN_ERROR_NONE,				/**< Successfully handled */
	SHARE_PANEL_ERROR_INVALID_PARAMETER = TIZEN_ERROR_INVALID_PARAMETER,	/**< Request is not valid, invalid parameter or invalid argument value */
	SHARE_PANEL_ERROR_OUT_OF_MEMORY = TIZEN_ERROR_OUT_OF_MEMORY,	/**< Memory is not enough to handle a new request */
	SHARE_PANEL_ERROR_PERMISSION_DENIED = TIZEN_ERROR_PERMISSION_DENIED,	/**< Has no permission to share with */
	SHARE_PANEL_ERROR_ALREADY_EXISTS = TIZEN_ERROR_PANEL | 0x01, /**< There is already a panel in the conformant */
	SHARE_PANEL_ERROR_NOT_INITIALIZED = TIZEN_ERROR_PANEL | 0x02, /**< The panel is not initialized yet */
	SHARE_PANEL_ERROR_INVALID_OPERATION = TIZEN_ERROR_PANEL | 0x04, /**< Invalid operation */
} share_panel_error_e;

/**
 * @brief Creates a share panel.
 * @since_tizen 2.4
 * @privlevel public
 * @privilege %http://tizen.org/privilege/packagemanager.info
 * @privilege %http://tizen.org/privilege/appmanager.launch
 * @remarks The caller app has to check the return value of this function.\n
 *
 * @param[in] conformant The caller's conformant
 * @param[in] control The app control handler.\n
 *                    This control needs a share operation among below operations.\n
 *                    http://tizen.org/appcontrol/operation/share\n
 *                    http://tizen.org/appcontrol/operation/multi_share\n
 *                    http://tizen.org/appcontrol/operation/share_text\n
 *                    These operations are defined in @ref CAPI_APP_CONTROL_MODULE.
 * @param[out] share_panel The share panel handler
 * @return @c SHARE_PANEL_ERROR_NONE on success,
 *         otherwise a negative error value
 * @retval #SHARE_PANEL_ERROR_NONE Successful
 * @retval #SHARE_PANEL_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval #SHARE_PANEL_ERROR_OUT_OF_MEMORY Out of memory
 * @retval #SHARE_PANEL_ERROR_PERMISSION_DENIED permission denied
 * @retval #SHARE_PANEL_ERROR_INVALID_OPERATION Invalid operation
 *
 * @see share_panel_destroy()
 * @see share_panel_show()
 * @see share_panel_hide()
 *
 * @par Example
 * @code
 * #include <share_panel.h>
 *
 * static int app_control(void *data)
 * {
 *   struct appdata *ad = data;
 *   share_panel_h share_panel = NULL;
 *   app_control_h control = NULL;
 *   int ret = APP_CONTROL_ERROR_NONE;
 *
 *   if (!ad) {
 *     // Error handling
 *   }
 *
 *   if (!ad->conformant) {
 *     // Error handling
 *   }
 *
 *   ret = app_control_create(&control);
 *   if (APP_CONTROL_ERROR_NONE != ret) {
 *     // Error handling
 *   }
 *   app_control_set_operation(control, "http://tizen.org/appcontrol/operation/share");
 *
 *   ret = share_panel_create(ad->conformant, control, &share_panel);
 *   if (SHARE_PANEL_ERROR_NONE != ret) {
 *      // Error handling
 *   }
 *
 *   ret = share_panel_show(share_panel);
 *   if (SHARE_PANEL_ERROR_NONE != ret) {
 *     // Error handling
 *   }
 *
 *   ret = app_control_destroy(control);
 *   if (APP_CONTROL_ERROR_NONE != ret) {
 *     // Error handling
 *   }
 *
 * 	 return 0;
 * }
 *
 * @endcode
 */
extern int share_panel_create(app_control_h control, share_panel_h *share_panel);

/**
 * @brief Destroys the share panel.
 * @since_tizen 2.4
 * @remarks The caller app has to check the return value of this function.\n
 *
 * @param[in] share_panel_h Share panel handler
 * @return @c SHARE_PANEL_ERROR_NONE on success,
 *         otherwise a negative error value
 * @retval #SHARE_PANEL_ERROR_NONE Successful
 * @retval #SHARE_PANEL_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval #SHARE_PANEL_ERROR_NOT_INITIALIZED The panel is not initialized yet
 *
 * @see share_panel_create()
 * @see share_panel_show()
 * @see share_panel_hide()
 *
 * @par Example
 * @code
 * #include <share_panel.h>
 *
 * static int app_terminate(void *data)
 * {
 *   struct appdata *ad = data;
 *   int ret = APP_CONTROL_ERROR_NONE;
 *
 *   if (!ad) {
 *     // Error handling
 *   }
 *
 *   if (!ad->share_panel) {
 *     // Error handling
 *   }
 *
 *   ret = share_panel_hide(ad->share_panel);
 *   if (APP_CONTROL_ERROR_NONE != ret) {
 *     // Error handling
 *   }
 *
 *   ret = share_panel_destroy(ad->share_panel);
 *   if (APP_CONTROL_ERROR_NONE != ret) {
 *     // Error handling
 *   }
 *   ad->share_panel = NULL;
 *
 * 	 return 0;
 * }
 *
 * @endcode
 */
extern int share_panel_destroy(share_panel_h share_panel);

/**
 * @brief Shows the share panel, asynchronously.
 * @since_tizen 2.4
 * @remarks The caller app has to check the return value of this function.\n
 *          share_panel_show will activate an animation to display the panel.
 *
 * @param[in] share_panel_h Share panel handler
 * @return @c SHARE_PANEL_ERROR_NONE on success,
 *         otherwise a negative error value
 * @retval #SHARE_PANEL_ERROR_NONE Successful
 * @retval #SHARE_PANEL_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval #SHARE_PANEL_ERROR_NOT_INITIALIZED The panel is not initialized yet
 *
 * @see share_panel_create()
 * @see share_panel_destroy()
 * @see share_panel_hide()
 *
 * @par Example
 * @code
 * #include <share_panel.h>
 *
 * static int app_control(void *data)
 * {
 *   struct appdata *ad = data;
 *   share_panel_h share_panel = NULL;
 *   app_control_h control = NULL;
 *   int ret = APP_CONTROL_ERROR_NONE;
 *
 *   if (!ad) {
 *     // Error handling
 *   }
 *
 *   if (!ad->conformant) {
 *     // Error handling
 *   }
 *
 *   ret = app_control_create(&control);
 *   if (APP_CONTROL_ERROR_NONE != ret) {
 *     // Error handling
 *   }
 *   app_control_set_operation(control, "http://tizen.org/appcontrol/operation/share");
 *
 *   ret = share_panel_create(ad->conformant, control, &share_panel);
 *   if (APP_CONTROL_ERROR_NONE != ret) {
 *      // Error handling
 *   }
 *   ad->share_panel = share_panel;
 *
 *   ret = share_panel_show(share_panel);
 *   if (APP_CONTROL_ERROR_NONE != ret) {
 *     // Error handling
 *   }
 *
 *   ret = app_control_destroy(control);
 *   if (APP_CONTROL_ERROR_NONE != ret) {
 *     // Error handling
 *   }
 *
 * 	 return 0;
 * }
 * @endcode
 */
extern int share_panel_show(share_panel_h share_panel);

/**
 * @brief Hides the share panel, asynchronously.
 * @since_tizen 2.4
 * @remarks The caller app has to check the return value of this function.\n
 *          share_panel_hide will activate an animation to disappear the panel.
 *
 * @param[in] share_panel_h Share panel handler
 * @return @c SHARE_PANEL_ERROR_NONE on success,
 *         otherwise a negative error value
 * @retval #SHARE_PANEL_ERROR_NONE Successful
 * @retval #SHARE_PANEL_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval #SHARE_PANEL_ERROR_NOT_INITIALIZED The panel is not initialized yet
 *
 * @see share_panel_create()
 * @see share_panel_destroy()
 * @see share_panel_show()
 *
 * @par Example
 * @code
 * #include <share_panel.h>
 *
 * static int app_terminate(void *data)
 * {
 *   struct appdata *ad = data;
 *   int ret = APP_CONTROL_ERROR_NONE;
 *
 *   if (!ad) {
 *     // Error handling
 *   }
 *
 *   if (!ad->share_panel) {
 *     // Error handling
 *   }
 *
 *   ret = share_panel_hide(ad->share_panel);
 *   if (APP_CONTROL_ERROR_NONE != ret) {
 *     // Error handling
 *   }
 *
 *   ret = share_panel_destroy(ad->share_panel);
 *   if (APP_CONTROL_ERROR_NONE != ret) {
 *     // Error handling
 *   }
 *   ad->share_panel = NULL;
 *
 * 	 return 0;
 * }
 *
 * @endcode
 */
extern int share_panel_hide(share_panel_h share_panel);

/**
 * @brief Reply caller of share-panel application about sharing operation
 * result.
 *
 * @param share_panel share panel handle
 * @param result sharing result, true on success, false otherwise
 *
 * @return SHARE_PANEL_ERROR_NONE on succes, other value on failure.
 */
extern int share_panel_reply(share_panel_h share_panel, bool result);

/**
 * @}
 */

#ifdef __cplusplus
}
#endif
#endif // __TIZEN_SHARE_PANEL_H__

