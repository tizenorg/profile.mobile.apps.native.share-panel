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

#ifndef __TIZEN_SHARE_PANEL_LOG_H__
#define __TIZEN_SHARE_PANEL_LOG_H__

#include <dlog.h>

#define OK		(0)
#define FAIL	(-1)

#undef LOG_TAG
#define LOG_TAG "SHARE_PANEL"

#if defined(D_)
#undef D_
#endif
#define D_(str) dgettext(PACKAGE, str)

#if !defined(_D)
#define _D(fmt, arg...) dlog_print(DLOG_DEBUG, LOG_TAG, fmt"\n", ##arg)
#endif

#if !defined(_W)
#define _W(fmt, arg...) dlog_print(DLOG_WARN, LOG_TAG, fmt"\n", ##arg)
#endif

#if !defined(_E)
#define _E(fmt, arg...) dlog_print(DLOG_ERROR, LOG_TAG, fmt"\n", ##arg)
#endif

#define retvm_if(expr, val, fmt, arg...) do { \
	if(expr) { \
		_E(fmt, ##arg); \
		_E("(%s) -> %s() return", #expr, __FUNCTION__); \
		return val; \
	} \
} while (0)

#define retv_if(expr, val) do { \
	if(expr) { \
		_E("(%s) -> %s() return", #expr, __FUNCTION__); \
		return (val); \
	} \
} while (0)

#define ret_if(expr) do { \
	if(expr) { \
		_E("(%s) -> %s() return", #expr, __FUNCTION__); \
		return; \
	} \
} while (0)

#define goto_if(expr, val) do { \
	if(expr) { \
		_E("(%s) -> goto", #expr); \
		goto val; \
	} \
} while (0)

#define break_if(expr) { \
	if(expr) { \
		_E("(%s) -> break", #expr); \
		break; \
	} \
}

#define continue_if(expr) { \
	if(expr) { \
		_E("(%s) -> continue", #expr); \
		continue; \
	} \
}

#endif /* __TIZEN_SHARE_PANEL_LOG_H__ */
