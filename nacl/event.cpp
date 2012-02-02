/*
 * Copyright (c) 2011, 2012  Timo Savola
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 */

#include "event.hpp"

#include <ppapi/c/pp_errors.h>

#include <concrete/context.hpp>
#include <concrete/util/trace.hpp>

#include <nacl/instance.hpp>

namespace concrete {

void NaClEventLoop::wait(const EventTrigger &trigger, EventCallback *callback)
{
	Trace("NaClEventLoop.wait: callback=%p", callback);

	trigger.completion.reset(&m_instance, callback);
}

pp::CompletionCallback EventCompletion::new_callback()
{
	return m_factory.NewCallback(&EventCompletion::complete);
}

void EventCompletion::reset(NaClInstance *instance, EventCallback *callback) throw ()
{
	m_result = PP_OK_COMPLETIONPENDING;
	m_instance = instance;
	m_event_callback = callback;
}

void EventCompletion::complete(int32_t result)
{
	auto instance = m_instance;
	auto callback = m_event_callback;

	ScopedInstance instance_activation(instance);

	Trace("EventCompletion.complete: callback=%p", callback);

	ScopedContext context_activation(instance->context());

	m_result = result;
	m_instance = NULL;
	m_event_callback = NULL;

	callback->resume();
	instance->execute();
}

} // namespace
