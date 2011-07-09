/*
 * Copyright (c) 2011  Timo Savola
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 */

#include <ppapi/cpp/instance.h>
#include <ppapi/cpp/module.h>

#include <nacl/instance.hpp>

namespace concrete {
	class NaClModule: public pp::Module {
	public:
		virtual ~NaClModule() {}

		virtual pp::Instance *CreateInstance(PP_Instance instance)
		{
			return new NaClInstance(instance);
		}
	};
}

namespace pp {
	Module *CreateModule()
	{
		return new concrete::NaClModule();
	}
}
