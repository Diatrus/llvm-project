//===-- SBListener.cpp ----------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "lldb/API/SBListener.h"
#include "SBReproducerPrivate.h"
#include "lldb/API/SBBroadcaster.h"
#include "lldb/API/SBDebugger.h"
#include "lldb/API/SBEvent.h"
#include "lldb/API/SBStream.h"
#include "lldb/Core/Debugger.h"
#include "lldb/Utility/Broadcaster.h"
#include "lldb/Utility/Listener.h"
#include "lldb/Utility/StreamString.h"

using namespace lldb;
using namespace lldb_private;

SBListener::SBListener() : m_opaque_sp(), m_unused_ptr(nullptr) {
  LLDB_RECORD_CONSTRUCTOR_NO_ARGS(SBListener);
}

SBListener::SBListener(const char *name)
    : m_opaque_sp(Listener::MakeListener(name)), m_unused_ptr(nullptr) {
  LLDB_RECORD_CONSTRUCTOR(SBListener, (const char *), name);
}

SBListener::SBListener(const SBListener &rhs)
    : m_opaque_sp(rhs.m_opaque_sp), m_unused_ptr(nullptr) {
  LLDB_RECORD_CONSTRUCTOR(SBListener, (const lldb::SBListener &), rhs);
}

const lldb::SBListener &SBListener::operator=(const lldb::SBListener &rhs) {
  LLDB_RECORD_METHOD(const lldb::SBListener &,
                     SBListener, operator=,(const lldb::SBListener &), rhs);

  if (this != &rhs) {
    m_opaque_sp = rhs.m_opaque_sp;
    m_unused_ptr = nullptr;
  }
  return LLDB_RECORD_RESULT(*this);
}

SBListener::SBListener(const lldb::ListenerSP &listener_sp)
    : m_opaque_sp(listener_sp), m_unused_ptr(nullptr) {}

SBListener::~SBListener() {}

bool SBListener::IsValid() const {
  LLDB_RECORD_METHOD_CONST_NO_ARGS(bool, SBListener, IsValid);
  return this->operator bool();
}
SBListener::operator bool() const {
  LLDB_RECORD_METHOD_CONST_NO_ARGS(bool, SBListener, operator bool);

  return m_opaque_sp != nullptr;
}

void SBListener::AddEvent(const SBEvent &event) {
  LLDB_RECORD_METHOD(void, SBListener, AddEvent, (const lldb::SBEvent &),
                     event);

  EventSP &event_sp = event.GetSP();
  if (event_sp)
    m_opaque_sp->AddEvent(event_sp);
}

void SBListener::Clear() {
  LLDB_RECORD_METHOD_NO_ARGS(void, SBListener, Clear);

  if (m_opaque_sp)
    m_opaque_sp->Clear();
}

uint32_t SBListener::StartListeningForEventClass(SBDebugger &debugger,
                                                 const char *broadcaster_class,
                                                 uint32_t event_mask) {
  LLDB_RECORD_METHOD(uint32_t, SBListener, StartListeningForEventClass,
                     (lldb::SBDebugger &, const char *, uint32_t), debugger,
                     broadcaster_class, event_mask);

  if (m_opaque_sp) {
    Debugger *lldb_debugger = debugger.get();
    if (!lldb_debugger)
      return 0;
    BroadcastEventSpec event_spec(ConstString(broadcaster_class), event_mask);
    return m_opaque_sp->StartListeningForEventSpec(
        lldb_debugger->GetBroadcasterManager(), event_spec);
  } else
    return 0;
}

bool SBListener::StopListeningForEventClass(SBDebugger &debugger,
                                            const char *broadcaster_class,
                                            uint32_t event_mask) {
  LLDB_RECORD_METHOD(bool, SBListener, StopListeningForEventClass,
                     (lldb::SBDebugger &, const char *, uint32_t), debugger,
                     broadcaster_class, event_mask);

  if (m_opaque_sp) {
    Debugger *lldb_debugger = debugger.get();
    if (!lldb_debugger)
      return false;
    BroadcastEventSpec event_spec(ConstString(broadcaster_class), event_mask);
    return m_opaque_sp->StopListeningForEventSpec(
        lldb_debugger->GetBroadcasterManager(), event_spec);
  } else
    return false;
}

uint32_t SBListener::StartListeningForEvents(const SBBroadcaster &broadcaster,
                                             uint32_t event_mask) {
  LLDB_RECORD_METHOD(uint32_t, SBListener, StartListeningForEvents,
                     (const lldb::SBBroadcaster &, uint32_t), broadcaster,
                     event_mask);

  uint32_t acquired_event_mask = 0;
  if (m_opaque_sp && broadcaster.IsValid()) {
    acquired_event_mask =
        m_opaque_sp->StartListeningForEvents(broadcaster.get(), event_mask);
  }

  return acquired_event_mask;
}

bool SBListener::StopListeningForEvents(const SBBroadcaster &broadcaster,
                                        uint32_t event_mask) {
  LLDB_RECORD_METHOD(bool, SBListener, StopListeningForEvents,
                     (const lldb::SBBroadcaster &, uint32_t), broadcaster,
                     event_mask);

  if (m_opaque_sp && broadcaster.IsValid()) {
    return m_opaque_sp->StopListeningForEvents(broadcaster.get(), event_mask);
  }
  return false;
}

bool SBListener::WaitForEvent(uint32_t timeout_secs, SBEvent &event) {
  LLDB_RECORD_METHOD(bool, SBListener, WaitForEvent,
                     (uint32_t, lldb::SBEvent &), timeout_secs, event);

  bool success = false;

  if (m_opaque_sp) {
    Timeout<std::micro> timeout(llvm::None);
    if (timeout_secs != UINT32_MAX) {
      assert(timeout_secs != 0); // Take this out after all calls with timeout
                                 // set to zero have been removed....
      timeout = std::chrono::seconds(timeout_secs);
    }
    EventSP event_sp;
    if (m_opaque_sp->GetEvent(event_sp, timeout)) {
      event.reset(event_sp);
      success = true;
    }
  }

  if (!success)
    event.reset(nullptr);
  return success;
}

bool SBListener::WaitForEventForBroadcaster(uint32_t num_seconds,
                                            const SBBroadcaster &broadcaster,
                                            SBEvent &event) {
  LLDB_RECORD_METHOD(bool, SBListener, WaitForEventForBroadcaster,
                     (uint32_t, const lldb::SBBroadcaster &, lldb::SBEvent &),
                     num_seconds, broadcaster, event);

  if (m_opaque_sp && broadcaster.IsValid()) {
    Timeout<std::micro> timeout(llvm::None);
    if (num_seconds != UINT32_MAX)
      timeout = std::chrono::seconds(num_seconds);
    EventSP event_sp;
    if (m_opaque_sp->GetEventForBroadcaster(broadcaster.get(), event_sp,
                                            timeout)) {
      event.reset(event_sp);
      return true;
    }
  }
  event.reset(nullptr);
  return false;
}

bool SBListener::WaitForEventForBroadcasterWithType(
    uint32_t num_seconds, const SBBroadcaster &broadcaster,
    uint32_t event_type_mask, SBEvent &event) {
  LLDB_RECORD_METHOD(
      bool, SBListener, WaitForEventForBroadcasterWithType,
      (uint32_t, const lldb::SBBroadcaster &, uint32_t, lldb::SBEvent &),
      num_seconds, broadcaster, event_type_mask, event);

  if (m_opaque_sp && broadcaster.IsValid()) {
    Timeout<std::micro> timeout(llvm::None);
    if (num_seconds != UINT32_MAX)
      timeout = std::chrono::seconds(num_seconds);
    EventSP event_sp;
    if (m_opaque_sp->GetEventForBroadcasterWithType(
            broadcaster.get(), event_type_mask, event_sp, timeout)) {
      event.reset(event_sp);
      return true;
    }
  }
  event.reset(nullptr);
  return false;
}

bool SBListener::PeekAtNextEvent(SBEvent &event) {
  LLDB_RECORD_METHOD(bool, SBListener, PeekAtNextEvent, (lldb::SBEvent &),
                     event);

  if (m_opaque_sp) {
    event.reset(m_opaque_sp->PeekAtNextEvent());
    return event.IsValid();
  }
  event.reset(nullptr);
  return false;
}

bool SBListener::PeekAtNextEventForBroadcaster(const SBBroadcaster &broadcaster,
                                               SBEvent &event) {
  LLDB_RECORD_METHOD(bool, SBListener, PeekAtNextEventForBroadcaster,
                     (const lldb::SBBroadcaster &, lldb::SBEvent &),
                     broadcaster, event);

  if (m_opaque_sp && broadcaster.IsValid()) {
    event.reset(m_opaque_sp->PeekAtNextEventForBroadcaster(broadcaster.get()));
    return event.IsValid();
  }
  event.reset(nullptr);
  return false;
}

bool SBListener::PeekAtNextEventForBroadcasterWithType(
    const SBBroadcaster &broadcaster, uint32_t event_type_mask,
    SBEvent &event) {
  LLDB_RECORD_METHOD(bool, SBListener, PeekAtNextEventForBroadcasterWithType,
                     (const lldb::SBBroadcaster &, uint32_t, lldb::SBEvent &),
                     broadcaster, event_type_mask, event);

  if (m_opaque_sp && broadcaster.IsValid()) {
    event.reset(m_opaque_sp->PeekAtNextEventForBroadcasterWithType(
        broadcaster.get(), event_type_mask));
    return event.IsValid();
  }
  event.reset(nullptr);
  return false;
}

bool SBListener::GetNextEvent(SBEvent &event) {
  LLDB_RECORD_METHOD(bool, SBListener, GetNextEvent, (lldb::SBEvent &), event);

  if (m_opaque_sp) {
    EventSP event_sp;
    if (m_opaque_sp->GetEvent(event_sp, std::chrono::seconds(0))) {
      event.reset(event_sp);
      return true;
    }
  }
  event.reset(nullptr);
  return false;
}

bool SBListener::GetNextEventForBroadcaster(const SBBroadcaster &broadcaster,
                                            SBEvent &event) {
  LLDB_RECORD_METHOD(bool, SBListener, GetNextEventForBroadcaster,
                     (const lldb::SBBroadcaster &, lldb::SBEvent &),
                     broadcaster, event);

  if (m_opaque_sp && broadcaster.IsValid()) {
    EventSP event_sp;
    if (m_opaque_sp->GetEventForBroadcaster(broadcaster.get(), event_sp,
                                            std::chrono::seconds(0))) {
      event.reset(event_sp);
      return true;
    }
  }
  event.reset(nullptr);
  return false;
}

bool SBListener::GetNextEventForBroadcasterWithType(
    const SBBroadcaster &broadcaster, uint32_t event_type_mask,
    SBEvent &event) {
  LLDB_RECORD_METHOD(bool, SBListener, GetNextEventForBroadcasterWithType,
                     (const lldb::SBBroadcaster &, uint32_t, lldb::SBEvent &),
                     broadcaster, event_type_mask, event);

  if (m_opaque_sp && broadcaster.IsValid()) {
    EventSP event_sp;
    if (m_opaque_sp->GetEventForBroadcasterWithType(broadcaster.get(),
                                                    event_type_mask, event_sp,
                                                    std::chrono::seconds(0))) {
      event.reset(event_sp);
      return true;
    }
  }
  event.reset(nullptr);
  return false;
}

bool SBListener::HandleBroadcastEvent(const SBEvent &event) {
  LLDB_RECORD_METHOD(bool, SBListener, HandleBroadcastEvent,
                     (const lldb::SBEvent &), event);

  if (m_opaque_sp)
    return m_opaque_sp->HandleBroadcastEvent(event.GetSP());
  return false;
}

lldb::ListenerSP SBListener::GetSP() { return m_opaque_sp; }

Listener *SBListener::operator->() const { return m_opaque_sp.get(); }

Listener *SBListener::get() const { return m_opaque_sp.get(); }

void SBListener::reset(ListenerSP listener_sp) {
  m_opaque_sp = listener_sp;
  m_unused_ptr = nullptr;
}

namespace lldb_private {
namespace repro {

template <>
void RegisterMethods<SBListener>(Registry &R) {
  LLDB_REGISTER_CONSTRUCTOR(SBListener, ());
  LLDB_REGISTER_CONSTRUCTOR(SBListener, (const char *));
  LLDB_REGISTER_CONSTRUCTOR(SBListener, (const lldb::SBListener &));
  LLDB_REGISTER_METHOD(const lldb::SBListener &,
                       SBListener, operator=,(const lldb::SBListener &));
  LLDB_REGISTER_METHOD_CONST(bool, SBListener, IsValid, ());
  LLDB_REGISTER_METHOD_CONST(bool, SBListener, operator bool, ());
  LLDB_REGISTER_METHOD(void, SBListener, AddEvent, (const lldb::SBEvent &));
  LLDB_REGISTER_METHOD(void, SBListener, Clear, ());
  LLDB_REGISTER_METHOD(uint32_t, SBListener, StartListeningForEventClass,
                       (lldb::SBDebugger &, const char *, uint32_t));
  LLDB_REGISTER_METHOD(bool, SBListener, StopListeningForEventClass,
                       (lldb::SBDebugger &, const char *, uint32_t));
  LLDB_REGISTER_METHOD(uint32_t, SBListener, StartListeningForEvents,
                       (const lldb::SBBroadcaster &, uint32_t));
  LLDB_REGISTER_METHOD(bool, SBListener, StopListeningForEvents,
                       (const lldb::SBBroadcaster &, uint32_t));
  LLDB_REGISTER_METHOD(bool, SBListener, WaitForEvent,
                       (uint32_t, lldb::SBEvent &));
  LLDB_REGISTER_METHOD(
      bool, SBListener, WaitForEventForBroadcaster,
      (uint32_t, const lldb::SBBroadcaster &, lldb::SBEvent &));
  LLDB_REGISTER_METHOD(
      bool, SBListener, WaitForEventForBroadcasterWithType,
      (uint32_t, const lldb::SBBroadcaster &, uint32_t, lldb::SBEvent &));
  LLDB_REGISTER_METHOD(bool, SBListener, PeekAtNextEvent, (lldb::SBEvent &));
  LLDB_REGISTER_METHOD(bool, SBListener, PeekAtNextEventForBroadcaster,
                       (const lldb::SBBroadcaster &, lldb::SBEvent &));
  LLDB_REGISTER_METHOD(
      bool, SBListener, PeekAtNextEventForBroadcasterWithType,
      (const lldb::SBBroadcaster &, uint32_t, lldb::SBEvent &));
  LLDB_REGISTER_METHOD(bool, SBListener, GetNextEvent, (lldb::SBEvent &));
  LLDB_REGISTER_METHOD(bool, SBListener, GetNextEventForBroadcaster,
                       (const lldb::SBBroadcaster &, lldb::SBEvent &));
  LLDB_REGISTER_METHOD(
      bool, SBListener, GetNextEventForBroadcasterWithType,
      (const lldb::SBBroadcaster &, uint32_t, lldb::SBEvent &));
  LLDB_REGISTER_METHOD(bool, SBListener, HandleBroadcastEvent,
                       (const lldb::SBEvent &));
}

}
}
