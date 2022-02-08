//
// Created by 20123460 on 2022/2/8.
//

#include "e_io_readbuf.h"
#include "e_io_unpack.h"
#include "e_io_cb.h"
#include "../e_include.h"
#include <event/e_io.h>

void e_io_set_unpack(e_io_t *io, unpack_setting_t *setting) {
  e_io_unset_unpack(io);
  if (setting == NULL)
    return;

  io->unpack_setting = setting;
  if (io->unpack_setting->package_max_length == 0) {
    io->unpack_setting->package_max_length = DEFAULT_PACKAGE_MAX_LENGTH;
  }
  if (io->unpack_setting->mode == UNPACK_BY_FIXED_LENGTH) {
    assert(io->unpack_setting->fixed_length != 0 &&
           io->unpack_setting->fixed_length <=
               io->unpack_setting->package_max_length);
  } else if (io->unpack_setting->mode == UNPACK_BY_DELIMITER) {
    if (io->unpack_setting->delimiter_bytes == 0) {
      io->unpack_setting->delimiter_bytes =
          strlen((char *)io->unpack_setting->delimiter);
    }
  } else if (io->unpack_setting->mode == UNPACK_BY_LENGTH_FIELD) {
    assert(io->unpack_setting->body_offset >=
           io->unpack_setting->length_field_offset +
               io->unpack_setting->length_field_bytes);
  }

  // NOTE: unpack must have own readbuf
  if (io->unpack_setting->mode == UNPACK_BY_FIXED_LENGTH) {
    io->readbuf.len = io->unpack_setting->fixed_length;
  } else {
    io->readbuf.len = EVENT_LOOP_READ_BUFSIZE;
  }
  e_io_alloc_readbuf(io, io->readbuf.len);
}

void e_io_unset_unpack(e_io_t *io) {
  if (io->unpack_setting) {
    io->unpack_setting = NULL;
    // NOTE: unpack has own readbuf
    e_io_free_readbuf(io);
  }
}


int e_io_unpack(e_io_t* io, void* buf, int readbytes) {
  unpack_setting_t* setting = io->unpack_setting;
  switch(setting->mode) {
  case UNPACK_BY_FIXED_LENGTH:
    return e_io_unpack_by_fixed_length(io, buf, readbytes);
  case UNPACK_BY_DELIMITER:
    return e_io_unpack_by_delimiter(io, buf, readbytes);
  case UNPACK_BY_LENGTH_FIELD:
    return e_io_unpack_by_length_field(io, buf, readbytes);
  default:
    e_io_read_cb(io, buf, readbytes);
    return readbytes;
  }
}

int e_io_unpack_by_fixed_length(e_io_t* io, void* buf, int readbytes) {
  const unsigned char* sp = (const unsigned char*)io->readbuf.base + io->readbuf.head;
  const unsigned char* ep = (const unsigned char*)buf + readbytes;
  unpack_setting_t* setting = io->unpack_setting;

  int fixed_length = setting->fixed_length;
  assert(io->readbuf.len >= fixed_length);

  const unsigned char* p = sp;
  int remain = ep - p;
  int handled = 0;
  while (remain >= fixed_length) {
    e_io_read_cb(io, (void*)p, fixed_length);
    handled += fixed_length;
    p += fixed_length;
    remain -= fixed_length;
  }

  io->readbuf.head = 0;
  io->readbuf.tail = remain;
  if (remain) {
    // [p, p+remain] => [base, base+remain]
    if (p != (unsigned char*)io->readbuf.base) {
      memmove(io->readbuf.base, p, remain);
    }
  }

  return handled;
}

int e_io_unpack_by_delimiter(e_io_t* io, void* buf, int readbytes) {
  const unsigned char* sp = (const unsigned char*)io->readbuf.base + io->readbuf.head;
  const unsigned char* ep = (const unsigned char*)buf + readbytes;
  unpack_setting_t* setting = io->unpack_setting;

  unsigned char* delimiter = setting->delimiter;
  int delimiter_bytes = setting->delimiter_bytes;

  const unsigned char* p = (const unsigned char*)buf - delimiter_bytes + 1;
  if (p < sp) p = sp;
  int remain = ep - p;
  int handled = 0;
  int i = 0;
  while (remain >= delimiter_bytes) {
    for (i = 0; i < delimiter_bytes; ++i) {
      if (p[i] != delimiter[i]) {
        goto not_match;
      }
    }
  match:
    p += delimiter_bytes;
    remain -= delimiter_bytes;
    e_io_read_cb(io, (void*)sp, p - sp);
    handled += p - sp;
    sp = p;
    continue;
  not_match:
    ++p;
    --remain;
  }

  remain = ep - sp;
  io->readbuf.head = 0;
  io->readbuf.tail = remain;
  if (remain) {
    // [sp, sp+remain] => [base, base+remain]
    if (sp != (unsigned char*)io->readbuf.base) {
      memmove(io->readbuf.base, sp, remain);
    }
    if (io->readbuf.tail == io->readbuf.len) {
      if (io->readbuf.len >= setting->package_max_length) {
//        hloge("recv package over %d bytes!", (int)setting->package_max_length);
        io->error = ERR_OVER_LIMIT;
        e_io_close(io);
        return -1;
      }
      int newsize = MIN(io->readbuf.len * 2, setting->package_max_length);
      e_io_alloc_readbuf(io, newsize);
    }
  }

  return handled;
}

int e_io_unpack_by_length_field(e_io_t* io, void* buf, int readbytes) {
  const unsigned char* sp = (const unsigned char*)io->readbuf.base + io->readbuf.head;
  const unsigned char* ep = (const unsigned char*)buf + readbytes;
  unpack_setting_t* setting = io->unpack_setting;

  const unsigned char* p = sp;
  int remain = ep - p;
  int handled = 0;
  unsigned int head_len = setting->body_offset;
  unsigned int body_len = 0;
  unsigned int package_len = head_len;
  const unsigned char* lp = NULL;
  while (remain >= setting->body_offset) {
    body_len = 0;
    lp = p + setting->length_field_offset;
    if (setting->length_field_coding == BIG_ENDIAN) {
      for (int i = 0; i < setting->length_field_bytes; ++i) {
        body_len = (body_len << 8) | (unsigned int)*lp++;
      }
    }
    else if (setting->length_field_coding == LITTLE_ENDIAN) {
      for (int i = 0; i < setting->length_field_bytes; ++i) {
        body_len |= ((unsigned int)*lp++) << (i * 8);
      }
    }
    else if (setting->length_field_coding == ENCODE_BY_VARINT) {
      int varint_bytes = ep - lp;
      body_len = varint_decode(lp, &varint_bytes);
      if (varint_bytes == 0) break;
      if (varint_bytes == -1) {
//        hloge("varint is too big!");
        io->error = ERR_OVER_LIMIT;
        e_io_close(io);
        return -1;
      }
      head_len = setting->body_offset + varint_bytes - setting->length_field_bytes;
    }
    package_len = head_len + body_len + setting->length_adjustment;
    if (remain >= package_len) {
      e_io_read_cb(io, (void*)p, package_len);
      handled += package_len;
      p += package_len;
      remain -= package_len;
    } else {
      break;
    }
  }

  io->readbuf.head = 0;
  io->readbuf.tail = remain;
  if (remain) {
    // [p, p+remain] => [base, base+remain]
    if (p != (unsigned char*)io->readbuf.base) {
      memmove(io->readbuf.base, p, remain);
    }
    if (package_len > io->readbuf.len) {
      if (package_len > setting->package_max_length) {
//        hloge("package length over %d bytes!", (int)setting->package_max_length);
        io->error = ERR_OVER_LIMIT;
        e_io_close(io);
        return -1;
      }
      int newsize = LIMIT(package_len, io->readbuf.len * 2, setting->package_max_length);
      e_io_alloc_readbuf(io, newsize);
    }
  }

  return handled;
}
