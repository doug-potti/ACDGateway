//
// connection_manager.hpp
// ~~~~~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2013 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef HTTP_CONNECTION_MANAGER_HPP
#define HTTP_CONNECTION_MANAGER_HPP

#include <set>
#include <boost/noncopyable.hpp>
#include "connection.hpp"

namespace http 
{
	namespace server 
	{

		class connection_manager : private boost::noncopyable
		{
		public:
			void start(connection_ptr c);
			void stop(connection_ptr c);
			void stop_all();

		private:
			std::set<connection_ptr> connections_;
		};

	} 
} 

#endif 
