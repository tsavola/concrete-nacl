/*
 * Copyright (c) 2011, 2012  Timo Savola
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 */

#ifndef NACL_INSTANCE_HPP
#define NACL_INSTANCE_HPP

#include <memory>

#include <ppapi/cpp/completion_callback.h>
#include <ppapi/cpp/instance.h>
#include <ppapi/cpp/var.h>

#include <concrete/context.hpp>
#include <concrete/util/noncopyable.hpp>

#include <nacl/event.hpp>

namespace concrete {

class NaClInstance: public pp::Instance {
public:
	static NaClInstance &Active() throw ();

	explicit NaClInstance(PP_Instance instance);
	virtual ~NaClInstance() {}

	virtual void HandleMessage(const pp::Var &var_message);

	Context &context() { return *m_context.get(); }
	void execute();

private:
	void run(const std::string &param);
	void do_execute(int32_t dummy);

	pp::CompletionCallbackFactory<NaClInstance> m_callback_factory;
	NaClEventLoop                               m_event_loop;
	std::unique_ptr<Context>                    m_context;
};

class ScopedInstance: Noncopyable {
public:
	explicit ScopedInstance(NaClInstance *instance) throw ();
	~ScopedInstance() throw ();

private:
	NaClInstance *m_instance;
};

} // namespace

#endif
