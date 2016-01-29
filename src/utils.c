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

#include <app_common.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "log.h"

#define _STRDUP(s) ((s) ? strdup(s) : NULL)

char *get_res_file_path(char *sub_path)
{
	char *res_path = app_get_resource_path();
	if(!res_path)
		return NULL;

	size_t absolute_path_size = strlen(res_path) + strlen(sub_path) + 2;

	char *absolute_path = malloc(absolute_path_size);
	if(!absolute_path)
		return NULL;

	snprintf(absolute_path, (int)absolute_path_size, "%s%s", res_path, sub_path);

	free(res_path);

	return _STRDUP(absolute_path);
}
