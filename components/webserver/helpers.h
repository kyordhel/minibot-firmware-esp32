#pragma once
#include <cstdint>
#include <cstdlib>

namespace minibot::webserver{

const char* ext_from_file(const char* file);
const char* mime_from_ext(const char* ext);

void write_ok_header(char* buffer, uint32_t buff_size, const char* mime_type, uint32_t offset=0, bool chunked=true);
void write_error_header(char* buffer, uint32_t buff_size,
	uint16_t    error_code,
	const char* error_code_label=NULL,
	const char* message=NULL,
	uint32_t offset=0);

}// End namespace
