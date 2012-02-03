/*
 * Copyright (c) 2012  Timo Savola
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 */

#include "concrete/modules/concrete.hpp"

#include <nacl/instance.hpp>

namespace concrete {

void BuiltinsPrintOutput(const std::string &s)
{
	std::string msg = "stdout:";
	msg += s;

	NaClInstance::Active().PostMessage(msg);
}

void BuiltinsPrintFlush()
{
}

} // namespace
