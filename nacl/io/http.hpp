/*
 * Copyright (c) 2011  Timo Savola
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 */

#ifndef NACL_IO_HTTP_HPP
#define NACL_IO_HTTP_HPP

#include <cstddef>
#include <cstdint>

#include <ppapi/cpp/completion_callback.h>
#include <ppapi/cpp/url_loader.h>
#include <ppapi/cpp/url_request_info.h>
#include <ppapi/cpp/url_response_info.h>

#include <concrete/io/http.hpp>

#include <nacl/event.hpp>

namespace concrete {

class NaClHTTPTransaction: public HTTPTransaction {
	friend class HTTPTransaction;

public:
	NaClHTTPTransaction(HTTP::Method method, const StringObject &url, Buffer *request_content);
	virtual ~NaClHTTPTransaction() throw () {}

private:
	enum State {
		Open,
		Opening,
		Opened,
		Receiving,
		Received,
	};

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
