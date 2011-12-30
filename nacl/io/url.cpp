/*
 * Copyright (c) 2011  Timo Savola
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 */

#include "url.hpp"

#include <ppapi/c/pp_errors.h>
#include <ppapi/cpp/var.h>

#include <concrete/context.hpp>

namespace concrete {

NaClURLOpener::NaClURLOpener(const StringObject &url, Buffer *response, Buffer *request):
	m_state(Open),
	m_response_length(-1),
	m_response_received(0),
	m_response_buffer(response)
{
	m_request.SetMethod(request ? "POST" : "GET");
	m_request.SetURL(url.string());
	m_request.SetAllowCrossOriginRequests(true);

	if (request)
		m_request.AppendDataToBody(request->consumable_data(), request->consumable_size());
}

void NaClURLOpener::suspend_until(State objective)
{
	if (objective <= m_state) {
		assert(false);
		return;
	}

	switch (m_state) {
	case Open:      m_state = open();      break;
	case Opening:   m_state = opening();   break;
	case Opened:    m_state = receive();   break;
	case Receiving: m_state = receiving(); break;
	case Received:  assert(false);         break;
	}
}

NaClURLOpener::State NaClURLOpener::open()
{
	auto callback = m_completion.new_callback();
	auto result = m_loader.Open(m_request, callback);

	if (result == PP_OK_COMPLETIONPENDING) {
		Context::Active().suspend_until(m_completion);
		return Opening;
	} else {
		callback.Run(result);
		return opening();
	}
}

NaClURLOpener::State NaClURLOpener::opening()
{
	if (m_completion.result() < 0)
		throw ResourceError();

	m_response = m_loader.GetResponseInfo();

	auto headers = m_response.GetHeaders();
	// TODO: set m_response_length

	return Opened;
}

NaClURLOpener::State NaClURLOpener::receive(bool recursion)
{
	if (m_response_length >= 0) {
		if (m_response_received == size_t(m_response_length))
			return Received;

		assert(m_response_received < size_t(m_response_length));
	}

	assert(m_response_buffer);

	auto callback = m_completion.new_callback();
	auto result = m_loader.ReadResponseBody(
		m_response_buffer->production_ptr(),
		m_response_buffer->production_space(),
		callback);

	if (result == PP_OK_COMPLETIONPENDING) {
		Context::Active().suspend_until(m_completion);
		return Receiving;
	} else if (recursion) {
		pp::Module::Get()->core()->CallOnMainThread(0, callback, result);
		return Receiving;
	} else {
		callback.Run(result);
		return receiving();
	}
}

NaClURLOpener::State NaClURLOpener::receiving()
{
	assert(m_response_length != 0);
	assert(m_response_buffer);

	if (m_completion.result() < 0)
		throw ResourceError();

	size_t len = size_t(m_completion.result());

#if 0
	if (len == 0)
		/* TODO */ ;

	if (x)
		receive(true);
#else
	Trace("NaClURLOpener receiving result: %1%", len);
	assert(!"TODO");
#endif

	return Receiving;
}

bool NaClURLOpener::content_consumable()
{
	if (m_state != Receiving)
		return false;

	assert(m_response_buffer);
	return m_response_buffer->consumable_size() > 0;
}

URLOpener *URLOpener::New(const StringObject &url, Buffer *response, Buffer *request)
{
	return new NaClURLOpener(url, response, request);
}

} // namespace
