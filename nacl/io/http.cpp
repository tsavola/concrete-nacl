/*
 * Copyright (c) 2011  Timo Savola
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 */

#include "http.hpp"

#include <ppapi/c/pp_errors.h>
#include <ppapi/cpp/url_response_info.h>
#include <ppapi/cpp/var.h>

#include <concrete/context.hpp>

namespace concrete {

NaClHTTPTransaction::NaClHTTPTransaction(HTTP::Method method,
                                         const StringObject &url,
                                         Buffer *request_content):
	m_state(Open),
	m_response_length(-1),
	m_response_received(0),
	m_response_buffer(NULL)
{
	m_request.SetMethod(method == HTTP::GET ? "GET" : "POST");
	m_request.SetURL(url.string());
	m_request.SetAllowCrossOriginRequests(true);

	if (request_content) {
		m_request.AppendDataToBody(request_content->consumable_data(),
		                           request_content->consumable_size());
	}
}

void NaClHTTPTransaction::suspend_until(State objective)
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

NaClHTTPTransaction::State NaClHTTPTransaction::open()
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

NaClHTTPTransaction::State NaClHTTPTransaction::opening()
{
	if (m_completion.result() < 0)
		throw ResourceError();

	m_response = m_loader.GetResponseInfo();

	auto headers = m_response.GetHeaders();
	// TODO: set m_response_length

	return Opened;
}

NaClHTTPTransaction::State NaClHTTPTransaction::receive(bool recursion)
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

NaClHTTPTransaction::State NaClHTTPTransaction::receiving()
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
	Trace("NaClHTTPTransaction receiving result: %1%", len);
	assert(!"TODO");
#endif

	return Receiving;
}

void HTTPTransaction::reset_response_buffer(Buffer *buffer)
{
	static_cast<NaClHTTPTransaction *> (this)->m_response_buffer = buffer;
}

HTTP::Status HTTPTransaction::response_status() const
{
	return HTTP::Status(static_cast<const NaClHTTPTransaction *> (this)->m_response.GetStatusCode());
}

long HTTPTransaction::response_length() const
{
	return static_cast<const NaClHTTPTransaction *> (this)->m_response_length;
}

bool HTTPTransaction::headers_received()
{
	return static_cast<NaClHTTPTransaction *> (this)->m_state >= NaClHTTPTransaction::Opened;
}

bool HTTPTransaction::content_consumable()
{
	auto impl = static_cast<NaClHTTPTransaction *> (this);

	if (impl->m_state != NaClHTTPTransaction::Receiving)
		return false;

	assert(impl->m_response_buffer);

	return impl->m_response_buffer->consumable_size() > 0;
}

bool HTTPTransaction::content_received()
{
	return static_cast<NaClHTTPTransaction *> (this)->m_state >= NaClHTTPTransaction::Received;
}

void HTTPTransaction::suspend_until_headers_received()
{
	static_cast<NaClHTTPTransaction *> (this)->suspend_until(NaClHTTPTransaction::Opened);
}

void HTTPTransaction::suspend_until_content_consumable()
{
	suspend_until_content_received();
}

void HTTPTransaction::suspend_until_content_received()
{
	static_cast<NaClHTTPTransaction *> (this)->suspend_until(NaClHTTPTransaction::Received);
}

HTTPTransaction *ResourceCreate<HTTPTransaction>::New(HTTP::Method method,
                                                      const StringObject &url,
                                                      Buffer *request_content)
{
	return new NaClHTTPTransaction(method, url, request_content);
}

} // namespace
