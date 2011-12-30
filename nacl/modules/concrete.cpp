/*
 * Copyright (c) 2011  Timo Savola
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 */

#include "concrete/modules/concrete.hpp"

namespace concrete {

Object ConcreteModule_Test(const TupleObject &args, const DictObject &kwargs)
{
	return args.get_item(0);
}

} // namespace
