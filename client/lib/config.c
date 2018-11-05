/*
 * From PKGj
 *
 * Copyright 2018 Philippe Daouadi, 2018 baregl
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */
#include "config.h"

#include "constants.h"
#include "syncer.h"
#include <stdlib.h>
#include <strings.h>

char *skipnons(char *text, char *end)
{
	while (text < end && *text != ' ') {
		text++;
	}
	return text;
}

char *skipnonnr(char *text, char *end)
{
	while (text < end && *text != '\n' && *text != '\r') {
		text++;
	}
	return text;
}

char *skips(char *text, char *end)
{
	while (text < end && (*text == ' ')) {
		text++;
	}
	return text;
}

char *skipnr(char *text, char *end)
{
	while (text < end && (*text == '\n' || *text == '\r')) {
		text++;
	}
	return text;
}

void config_parse(char *file)
{
	LOG("Opening config\n");
	uint32_t size = clbk_file_size(file);
	if (size > config_max_size)
		clbk_show_error("Config file too large");
	if (clbk_open(file) != 0)
		clbk_show_error("Couldn't open config file");
	// For the final NULL Byte
	char *buffer = (char *)malloc(size + 1);
	char *text = (char *)buffer;
	LOG("Reading config\n");
	uint16_t read_len = clbk_read((uint8_t *)buffer, size);
	LOG("Read config\n");
	if (read_len != size)
		// Too long
		clbk_show_error("Unexpectedly the config file size changed");
	LOG("Writing final newline at %i\n", read_len);
	buffer[read_len] = '\n';
	LOG("Finding eof\n");
	char *end = (char *)buffer + read_len + 1;

	LOG("Parsing config\n");
	while (text < end) {
		char *key = text;

		text = skipnons(text, end);
		if (text == end)
			clbk_show_error("Config for key missing");

		*text++ = 0;

		text = skips(text, end);
		if (text == end)
			clbk_show_error("Config for key missing");

		char *value = text;

		text = skipnonnr(text, end);

		*text++ = 0;
		clbk_config_entry(key, value);
		if (text == (end + 1)) {
			break;
		}

		text = skipnr(text, end);
	}
	free(buffer);
}
