//
//  c4Internal.hh
//  Couchbase Lite Core
//
//  Created by Jens Alfke on 9/15/15.
//  Copyright (c) 2015-2016 Couchbase. All rights reserved.
//

#pragma once

#include "Base.hh"
#include "Error.hh"
#include "RefCounted.hh"
#include "PlatformCompat.hh"
#include "function_ref.hh"
#include <functional>


struct C4DocEnumerator;

namespace litecore {
    class Record;
}

using namespace std;
using namespace litecore;


// SLICE STUFF:

typedef struct _C4Slice {
    const void *buf;
    size_t size;

    operator slice() const { return {buf, size}; }
    explicit operator std::string() const { return std::string((const char*)buf, size); }

    bool operator==(const struct _C4Slice &s) const {
        return size == s.size &&
            memcmp(buf, s.buf, size) == 0;
    }
    bool operator!=(const struct _C4Slice &s) const { return !(*this == s); }

    _C4Slice& operator= (slice s)   {buf = s.buf; size = s.size; return *this;}
} C4Slice;

static inline C4Slice toc4slice(slice s)
{
    return {s.buf, s.size};
}

typedef struct {
    const void *buf;
    size_t size;
} C4SliceResult;


constexpr C4Slice C4SliceNull = { nullptr, 0 };
#define kC4SliceNull C4SliceNull


#define C4_IMPL // This tells c4Base.h to skip its declaration of C4Slice
#include "c4Base.h"
#include "c4ExceptionUtils.hh"


namespace c4Internal {

    // ERRORS & EXCEPTIONS:

    const size_t kMaxErrorMessagesToSave = 10;

    void recordError(C4ErrorDomain, int code, std::string message, C4Error* outError) noexcept;
    void recordError(C4ErrorDomain, int code, C4Error* outError) noexcept;
    // SLICES:

    C4SliceResult sliceResult(slice s);
    C4SliceResult sliceResult(alloc_slice s);
    C4SliceResult sliceResult(const char *str);

    // DOC ENUMERATORS:

    typedef function<bool(const Record&, uint32_t/*C4DocumentFlags*/ documentFlags)> EnumFilter;

    void setEnumFilter(C4DocEnumerator*, EnumFilter);

}

using namespace c4Internal;
