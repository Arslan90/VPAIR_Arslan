/*
 * T_tlvTypeEnum.h
 *
 *  Created on: May 15, 2014
 *      Author: arslan
 */

#ifndef T_TLVTYPEENUM_H_
#define T_TLVTYPEENUM_H_

namespace t_tlvTypeEnum {
	enum {
		HELLO = 0x00,
		ERROR = 0x01,
		RIBD  = 0x02,
		RIB   = 0x03,
		Bundle_Offer = 0xA4,
		Bundle_Response = 0xA5,
		Bundle = 0xFF,
	};
};


#endif /* T_TLVTYPEENUM_H_ */
