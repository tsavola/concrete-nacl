/*
 * Copyright (c) 2011  Timo Savola
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 */

#ifndef NACL_EVENT_HPP
#define NACL_EVENT_HPP

#include <cstddef>
#include <cstdint>

#include <ppapi/cpp/completion_callback.h>
#include <ppapi/cpp/instance.h>

#include <concrete/event.hpp>
#include <concrete/util/noncopyable.hpp>

namespace concrete {

class NaClInstance;

class NaClEventLoop: public EventLoop {
public:
	explicit NaClEventLoop(NaClInstance &instance) throw (): m_instance(instance) {}
	virtual ~NaClEventLoop() throw () {}

	virtual void wait(const EventTrigger &trigger, EventCallback *callback);

private:
	NaClInstance &m_instance;
};

class EventCompletion: Noncopyable {
	friend class NaClEventLoop;

public:
	EventCompletion() throw (): m_factory(this), m_instance(NULL), m_event_callback(NULL) {}

	pp::CompletionCallback new_callback();
	int32_t result() const throw () { return m_result; }

private:
	void reset(NaClInstance *instance, EventCallback *callback) throw ();
	void complete(int32_t result);

	pp::CompletionCallbackFactory<EventCompletion>  m_factory;
	NaClInstance                                   *m_instance;
	EventCallback                                  *m_event_callback;
	int32_t                                         m_result;
};

class EventTrigger {
public:
	EventTrigger(EventCompletion &completion) throw (): completion(completion) {}

	EventCompletion &completion;
};

} // namespace

#endif
