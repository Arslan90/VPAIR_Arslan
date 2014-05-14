/*
 * Prophet_Enum.h
 *
 *  Created on: Apr 22, 2014
 *      Author: arslan
 */

#ifndef PROPHET_ENUM_H_
#define PROPHET_ENUM_H_

/*
 * Defined as described in RFC 6693,
 * the only difference is the Bundle Enum that is not defined in RFC
 * it value is taken from Experimental values (bounded between 0xD0 & 0xFF)
 */
enum t_tlv_type {
	HELLO = 0x00,
	ERROR = 0x01,
	RIBD  = 0x02,
	RIB   = 0x03,
	Bundle_Offer = 0xA4,
	Bundle_Response = 0xA5,
	Bundle = 0xFF,
};

enum t_flags_hello {
	SYN = 0b000,
	SYNACK = 0b001,
	ACK    = 0b010,
	RSTACK = 0b100,
};

enum t_flags_error {
	Dictionary_Conflict = 0x00,
	Bad_String_ID 		= 0x01,
};

enum t_flags_RIBD {
	Sent_by_Listener = 0,
};

enum t_flags_RIB {
	More_RIB_TLV = 0,
};

enum t_flags_Bundle_OfferResp {
	More_OfferResp_TLV = 0,
};

enum t_b_flags {
	Bndl_Accepted = 0,
	Bndl_is_a_Fragment = 1,
	Bndl_Payload_Length_Included = 2,
	PRoPHET_ACK = 7,
};

enum t_result_field {
	NoSuccessACK = 0x01,
	ACKAll,
	Success,
	Failure,
	ReturnReceipt,
};

enum t_SuccessCodes_field {
	Generic_Success = 0x01,
	Submessage_Received,
};

enum t_FailureCodes_field {
	Unspecified_Failure = 0x02,
	Error_TLV_in_message = 0xFF,
};

#endif /* PROPHET_ENUM_H_ */
