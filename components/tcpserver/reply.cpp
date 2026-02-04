#include "reply.h"
#include <cstring>

namespace minibot::tcpserver{

reply_t reply_from_request(const request_t& req, bool success){
	return (reply_t) {
		.size    = 13,
		.id      = req.id,
		.cmd     = req.cmd,
		.success = (uint8_t)(success ? 1 : 0),
		.args    = {
			0, 0, 0, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 0, 0, 0,

			0, 0, 0, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 0, 0, 0,
		}
	};
}

uint16_t get_reply_payload_size(const reply_t& rpl){
	uint16_t ds_offset = 0;
	uint16_t ds_count  = 0;
	if( !rpl.success ) return 13;

	switch(rpl.cmd){
		// Zero args replies, 13 bytes

		case Command::STOP:
		case Command::SetPWM2:
		case Command::SetSpeed2:
		case Command::SetPWM4:
		case Command::SetSpeed4:
			return 13;

		// Float+uint8_t replies, 18 bytes
		case Command::ReadBatt:
			return 13 + sizeof(sbatteryr_t);

		// Three floats replies, 25 bytes
		case Command::MV:
			return 13 + 3*sizeof(float);

		// Four floats replies, 29 bytes
		case Command::GetPWM:
		case Command::GetSpeed:
			return 13 + 4*sizeof(float);

		case Command::ReadFloor:
			return 13 + sizeof(sfloorr_t);

		// Sixteen floats replies, 269 bytes
		case Command::ReadLight:
			return 13 + sizeof(slightr_t);

		// Variable length replies
		case Command::ReadDist:
			// H+n×ff dist
			return 13 + sizeof(uint16_t) + rpl.args.Hargs[0] * sizeof(float);

		case Command::ReadAll:
			// f+b battery, 4×ff floor, 8×ff light, H+n×ff dist
			ds_offset = sizeof(sbatteryr_t) + sizeof(sfloorr_t) + sizeof(slightr_t);
			ds_count  = *( (uint16_t*)(rpl.args.cargs + ds_offset) );
			return 13 + ds_offset + sizeof(uint16_t) + ds_count * sizeof(float);

		default:
			return 13;
	}
	return 13;
}

} // End namespace
