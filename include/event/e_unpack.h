//
// Created by 20123460 on 2022/2/8.
//

#ifndef EVENT_UNPACK_H
#define EVENT_UNPACK_H

#include "e_event.h"

typedef enum {
  UNPACK_BY_FIXED_LENGTH  = 1,    // Not recommended
  UNPACK_BY_DELIMITER     = 2,    // Suitable for text protocol
  UNPACK_BY_LENGTH_FIELD  = 3,    // Suitable for binary protocol
} unpack_mode_e;

#define DEFAULT_PACKAGE_MAX_LENGTH  (1 << 21)   // 2M

// UNPACK_BY_DELIMITER
#define PACKAGE_MAX_DELIMITER_BYTES 8

// UNPACK_BY_LENGTH_FIELD
typedef enum {
  ENCODE_BY_VARINT        = 17,               // 1 MSB + 7 bits
  ENCODE_BY_LITTEL_ENDIAN = LITTLE_ENDIAN,    // 1234
  ENCODE_BY_BIG_ENDIAN    = BIG_ENDIAN,       // 4321
} unpack_coding_e;


typedef struct unpack_setting_s {
  unpack_mode_e   mode;
  unsigned int    package_max_length;
  union {
    // UNPACK_BY_FIXED_LENGTH
    struct {
      unsigned int    fixed_length;
    };
    // UNPACK_BY_DELIMITER
    struct {
      unsigned char   delimiter[PACKAGE_MAX_DELIMITER_BYTES];
      unsigned short  delimiter_bytes;
    };
    // UNPACK_BY_LENGTH_FIELD
    /* package_len = head_len + body_len + length_adjustment
         *
         * if (length_field_coding == ENCODE_BY_VARINT) head_len = body_offset + varint_bytes - length_field_bytes;
         * else head_len = body_offset;
         *
         * body_len calc by length_field
         *
     */
    struct {
      unsigned short  body_offset;
      unsigned short  length_field_offset;
      unsigned short  length_field_bytes;
      short  length_adjustment;
      unpack_coding_e length_field_coding;
    };
  };
#ifdef __cplusplus
  unpack_setting_s() {
    // Recommended setting:
    // head = flags:1byte + length:4bytes = 5bytes
    mode = UNPACK_BY_LENGTH_FIELD;
    package_max_length = DEFAULT_PACKAGE_MAX_LENGTH;
    fixed_length = 0;
    delimiter_bytes = 0;
    body_offset = 5;
    length_field_offset = 1;
    length_field_bytes = 4;
    length_field_coding = ENCODE_BY_BIG_ENDIAN;
    length_adjustment = 0;
  }
#endif
} unpack_setting_t;

#endif // EVENT_UNPACK_H
