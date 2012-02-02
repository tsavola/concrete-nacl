/*
 * Copyright (c) 2011, 2012  Timo Savola
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
#include <concrete/util/assert.hpp>
#include <concrete/util/trace.hpp>

#include <nacl/instance.hpp>

namespace concrete {

NaClURLOpener::NaClURLOpener(NaClInstance &instance, const StringObject &url, Buffer *response,
                             Buffer *request):
	m_state(Open),
	m_loader(instance),
	m_request(&instance),
	m_response_length(-1),
	m_response_received(0),
	m_response_buffer(response)
{
	Trace("NaClURLOpener \"%s\"", url.c_str());

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

#if 1
	if (m_state == Received) {
		Trace("NaClURLOpener: --- received content ---");
		Trace("NaClURLOpener: %s", std::string(m_response_buffer->consumable_data(), m_response_buffer->consumable_size()));
		Trace("NaClURLOpener: ---");
	}
#endif
}

NaClURLOpener::State NaClURLOpener::open()
{
	Trace("NaClURLOpener.open");

	auto callback = m_completion.new_callback();
	auto result = m_loader.Open(m_request, callback);

	if (result == PP_OK_COMPLETIONPENDING) {
		Context::Active().suspend_until(m_completion);
		return Opening;
	} else {
		// TODO: free callback?
		return opened(result);
	}
}

NaClURLOpener::State NaClURLOpener::opening()
{
	Trace("NaClURLOpener.opening");

	return opened(m_completion.result());
}

NaClURLOpener::State NaClURLOpener::opened(int32_t result)
{
	Trace("NaClURLOpener.opened (result = %d)", result);

	if (result < 0)
		throw ResourceError();

	m_response = m_loader.GetResponseInfo();
	// TODO: set m_response_length

	return Opened;
}

NaClURLOpener::State NaClURLOpener::receive(bool recursion)
{
	Trace("NaClURLOpener.receive");

	if (m_response_length >= 0 && m_response_received >= size_t(m_response_length))
		return Received;

	auto callback = m_completion.new_callback();
	auto result = m_loader.ReadResponseBody(
		m_response_buffer->production_ptr(),
		m_response_buffer->production_space(),
		callback);

	if (result == PP_OK_COMPLETIONPENDING) {
		Context::Active().suspend_until(m_completion);
		return Receiving;
	} else if (recursion) {
		// TODO: free callback?
		pp::Module::Get()->core()->CallOnMainThread(0, callback, result);
		return Receiving;
	} else {
		// TODO: free callback?
		return received_some(result);
	}
}

NaClURLOpener::State NaClURLOpener::receiving()
{
	Trace("NaClURLOpener.receiving");

	return received_some(m_completion.result());
}

NaClURLOpener::State NaClURLOpener::received_some(int32_t result)
{
	Trace("NaClURLOpener.received (result = %d)", result);

	assert(m_response_length != 0);

	if (result < 0)
		throw ResourceError();

	m_response_buffer->produced_length(result);
	m_response_received += result;

	if (result == 0) {
		// premature EOF
		if (m_response_length > 0)
			throw ResourceError();

		return Received;
	}

	return receive(true);
}

bool NaClURLOpener::content_consumable()
{
	if (m_state != Receiving)
		return false;

	return m_response_buffer->consumable_size() > 0;
}

URLOpener *URLOpener::New(const StringObject &url, Buffer *response, Buffer *request)
{
	return new NaClURLOpener(NaClInstance::Active(), url, response, request);
}

} // namespace
