//
//  varint.c
//  CBForest
//
//  Created by Jens Alfke on 3/31/14.
//  Copyright (c) 2014 Couchbase. All rights reserved.
//

#include "varint.h"
#include <stdio.h>
#include "rev_tree.h" // just for sized_buf -- FIX


size_t SizeOfVarInt(uint64_t n) {
    size_t size = 1;
    while (n >= 0x80) {
        size++;
        n >>= 7;
    }
    return size;
}


size_t WriteUVarInt(void *buf, uint64_t n) {
    uint8_t* dst = buf;
    while (n >= 0x80) {
        *dst++ = (n & 0xFF) | 0x80;
        n >>= 7;
    }
    *dst++ = (uint8_t)n;
    return dst - (uint8_t*)buf;
}


size_t ReadUVarInt(sized_buf buf, uint64_t *n) {
    uint64_t result = 0;
    int shift = 0;
    for (int i = 0; i < buf.size; i++) {
        uint8_t byte = ((const uint8_t*)buf.buf)[i];
        result |= (uint64_t)(byte & 0x7f) << shift;
        if (byte >= 0x80) {
            shift += 7;
        } else {
            if (i > 9 || (i == 9 && byte > 1))
                return 0; // Numeric overflow
            *n = result;
            return i + 1;
        }
    }
    return 0; // buffer too short
}