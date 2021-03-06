//===----------------------------------------------------------------------===//
//
// This source file is part of the Swift.org open source project
//
// Copyright (c) 2014 - 2016 Apple Inc. and the Swift project authors
// Licensed under Apache License v2.0 with Runtime Library Exception
//
// See http://swift.org/LICENSE.txt for license information
// See http://swift.org/CONTRIBUTORS.txt for the list of Swift project authors
//
//===----------------------------------------------------------------------===//


#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wnullability-completeness"

#include <dispatch/dispatch.h>
#include <stdio.h>

#define DISPATCH_RUNTIME_STDLIB_INTERFACE __attribute__((__visibility__("default")))

// Replicate the SWIFT_CC(swift) calling convention macro from
// swift/include/swift/Runtime/Config.h because it is
// quite awkward to include Config.h and its recursive includes
// in dispatch. This define must be manually kept in synch
#define SWIFT_CC(CC) SWIFT_CC_##CC
#if SWIFT_USE_SWIFTCALL
#define SWIFT_CC_swift __attribute__((swiftcall))
#else
#define SWIFT_CC_swift
#endif

extern "C" dispatch_queue_attr_t _swift_dispatch_queue_concurrent(void);
extern "C" void _swift_dispatch_apply_current(size_t iterations, __attribute__((__noescape__)) void (^block)(size_t));
extern "C" dispatch_queue_t _swift_dispatch_get_main_queue(void);
extern "C" void _swift_dispatch_async(dispatch_queue_t queue, dispatch_block_t block);
extern "C" void _swift_dispatch_group_async(dispatch_group_t group, dispatch_queue_t queue, dispatch_block_t block);
extern "C" void _swift_dispatch_sync(dispatch_queue_t queue, dispatch_block_t block);
extern "C" void _swift_dispatch_release(dispatch_object_t obj);
extern "C" void _swift_dispatch_retain(dispatch_object_t obj);
#if !USE_OBJC
extern "C" void * objc_retainAutoreleasedReturnValue(void *obj);
#endif


SWIFT_CC(swift) DISPATCH_RUNTIME_STDLIB_INTERFACE
extern "C" dispatch_queue_attr_t
_swift_dispatch_queue_concurrent(void) {
  return DISPATCH_QUEUE_CONCURRENT;
}

SWIFT_CC(swift) DISPATCH_RUNTIME_STDLIB_INTERFACE
extern "C" void
_swift_dispatch_apply_current(size_t iterations, __attribute__((__noescape__)) void (^block)(size_t)) {
  dispatch_apply(iterations, (dispatch_queue_t _Nonnull)0, block);
}

SWIFT_CC(swift) DISPATCH_RUNTIME_STDLIB_INTERFACE
extern "C" dispatch_queue_t
_swift_dispatch_get_main_queue(void) {
  return dispatch_get_main_queue();
}

SWIFT_CC(swift) DISPATCH_RUNTIME_STDLIB_INTERFACE
extern "C" void
_swift_dispatch_async(dispatch_queue_t queue, dispatch_block_t block) {
  dispatch_async(queue, block);
}

SWIFT_CC(swift) DISPATCH_RUNTIME_STDLIB_INTERFACE
extern "C" void
_swift_dispatch_group_async(dispatch_group_t group, dispatch_queue_t queue, dispatch_block_t block) {
  dispatch_group_async(group, queue, block);
}

SWIFT_CC(swift) DISPATCH_RUNTIME_STDLIB_INTERFACE
extern "C" void
_swift_dispatch_sync(dispatch_queue_t queue, dispatch_block_t block) {
  dispatch_sync(queue, block);
}

SWIFT_CC(swift) DISPATCH_RUNTIME_STDLIB_INTERFACE
extern "C" void
_swift_dispatch_release(dispatch_object_t obj) {
  dispatch_release(obj);
}

SWIFT_CC(swift) DISPATCH_RUNTIME_STDLIB_INTERFACE
extern "C" void
_swift_dispatch_retain(dispatch_object_t obj) {
  dispatch_retain(obj);
}

#define SOURCE(t)                                                              \
  extern "C" dispatch_source_type_t _swift_dispatch_source_type_##t(void);     \
  SWIFT_CC(swift)                                                              \
  DISPATCH_RUNTIME_STDLIB_INTERFACE extern "C" dispatch_source_type_t  \
  _swift_dispatch_source_type_##t(void) {                                      \
    return DISPATCH_SOURCE_TYPE_##t;                                           \
  }


SOURCE(DATA_ADD)
SOURCE(DATA_OR)
// SOURCE(DATA_REPLACE)
#if HAVE_MACH
SOURCE(MACH_SEND)
SOURCE(MACH_RECV)
SOURCE(MEMORYPRESSURE)
#endif
#ifndef __linux__
SOURCE(PROC)
#endif
SOURCE(READ)
SOURCE(SIGNAL)
SOURCE(TIMER)
#ifndef __linux__
SOURCE(VNODE)
#endif
SOURCE(WRITE)

#if !USE_OBJC

// For CF functions with 'Get' semantics, the compiler currently assumes that
// the result is autoreleased and must be retained. It does so on all platforms
// by emitting a call to objc_retainAutoreleasedReturnValue. On Darwin, this is
// implemented by the ObjC runtime. On non-ObjC platforms, there is no runtime,
// and therefore we have to stub it out here ourselves. The compiler will
// eventually call swift_release to balance the retain below. This is a
// workaround until the compiler no longer emits this callout on non-ObjC
// platforms.
extern "C" void swift_retain(void *);

SWIFT_CC(swift) DISPATCH_RUNTIME_STDLIB_INTERFACE
extern "C" void * objc_retainAutoreleasedReturnValue(void *obj) {
    if (obj) {
        swift_retain(obj);
        return obj;
    }
    else return NULL;
}

#endif // !USE_OBJC

#pragma clang diagnostic pop