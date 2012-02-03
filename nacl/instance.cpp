/*
 * Copyright (c) 2011, 2012  Timo Savola
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 */

#include "instance.hpp"

#include <cstdint>
#include <cstring>
#include <string>

#include <ppapi/cpp/core.h>
#include <ppapi/cpp/module.h>

#include <concrete/context.hpp>
#include <concrete/execution.hpp>
#include <concrete/objects/code.hpp>
#include <concrete/util/assert.hpp>
#include <concrete/util/thread.hpp>

namespace concrete {

static CONCRETE_THREAD_LOCAL NaClInstance *active_instance;

static unsigned int decode_hex(char c)
{
	if (c >= '0' && c <= '9')
		return c - '0';

	if (c >= 'A' && c <= 'F')
		return c - 'A' + 10;

	return 0;
}

NaClInstance &NaClInstance::Active() throw ()
{
	assert(active_instance);
	return *active_instance;
}

NaClInstance::NaClInstance(PP_Instance instance):
	pp::Instance(instance),
	m_callback_factory(this),
	m_event_loop(*this)
{
}

void NaClInstance::HandleMessage(const pp::Var &var_message)
{
	if (var_message.is_string()) {
		std::string message = var_message.AsString();
		if (message.substr(0, 4) == "run:")
			run(message.substr(4));
	}
}

void NaClInstance::run(const std::string &param)
{
	if (m_context) {
		PostMessage("error:Context already running");
		return;
	}

	ScopedInstance instance_activation(this);

	char buf[param.length() / 2];
	for (size_t i = 0; i < sizeof (buf); i++)
		buf[i] = (decode_hex(param[i * 2]) << 4) | decode_hex(param[i * 2 + 1]);

	m_context.reset(new Context(m_event_loop));
	ScopedContext context_activation(*m_context.get());
	m_context->add_execution(Execution::New(CodeObject::Load(buf, sizeof (buf))));
	execute();
}

void NaClInstance::execute()
{
	auto callback = m_callback_factory.NewCallback(&NaClInstance::do_execute);
	pp::Module::Get()->core()->CallOnMainThread(0, callback);
}

void NaClInstance::do_execute(int32_t dummy)
{
	ScopedInstance instance_activation(this);

	bool executable;

	{
		ScopedContext context_activation(*m_context.get());

		bool runnable = m_context->execute();
		executable = m_context->executable();

		if (runnable)
			execute();
	}

	if (!executable) {
		m_context.reset();
		PostMessage("done:");
	}
}

ScopedInstance::ScopedInstance(NaClInstance *instance) throw ():
	m_instance(instance)
{
	assert(active_instance == NULL);
	active_instance = instance;
}

ScopedInstance::~ScopedInstance() throw ()
{
	assert(active_instance == m_instance);
	active_instance = NULL;
}

} // namespace
