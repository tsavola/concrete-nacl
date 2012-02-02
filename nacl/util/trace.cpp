/*
 * Copyright (c) 2012  Timo Savola
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 */

#include <string>

#include <concrete/util/trace.hpp>

#include <nacl/instance.hpp>

namespace concrete {

#ifdef CONCRETE_TRACE

void TraceOutput(const std::string &line)
{
	std::string s = "trace:";
	s += line;
	NaClInstance::Active().PostMessage(s);
}

#endif

} // namespace
