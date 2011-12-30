/*
 * Copyright (c) 2011  Timo Savola
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 */

#ifndef NACL_IO_URL_HPP
#define NACL_IO_URL_HPP

#include <cstddef>
#include <cstdint>

#include <ppapi/cpp/completion_callback.h>
#include <ppapi/cpp/url_loader.h>
#include <ppapi/cpp/url_request_info.h>
#include <ppapi/cpp/url_response_info.h>

#include <concrete/io/url.hpp>

#include <nacl/event.hpp>

namespace concrete {

class NaClURLOpener: public URLOpener {
	enum State {
		Open,
		Opening,
		Opened,
		Receiving,
		Received,
	};

public:
	NaClURLOpener(const StringObject &url, Buffer *response, Buffer *request);
	virtual ~NaClURLOpener() throw () {}

	virtual bool headers_received() { return m_state >= Opened; }
	virtual bool content_consumable();
	virtual bool content_received() { return m_state >= Received; }

	virtual void suspend_until_headers_received() { suspend_until(Opened); }
	virtual void suspend_until_content_consumable() { suspend_until_content_received(); }
	virtual void suspend_until_content_received() { suspend_until(Received); }

	virtual int  response_status() const { return m_response.GetStatusCode(); }
	virtual long response_length() const { return m_response_length; }

private:
	void suspend_until(State objective);

	State open();
	State opening();
	State receive(bool recursion = false);
	State receiving();

	State                m_state;
	EventCompletion      m_completion;
	pp::URLLoader        m_loader;
	pp::URLRequestInfo   m_request;
	pp::URLResponseInfo  m_response;
	long                 m_response_length;
	size_t               m_response_received;
	Buffer              *m_response_buffer;
};

} // namespace

#endif
