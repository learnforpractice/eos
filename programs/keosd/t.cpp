//
// timer.cpp
// ~~~~~~~~~
//
// Copyright (c) 2003-2017 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include <iostream>
#include <boost/asio.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

static boost::asio::io_context io_serv;

void print(const boost::system::error_code& /*e*/)
{
  std::cout << "Hello, world!" << std::endl;
//  io_serv.stop();
}

int main()
{
//  execution_priority_queue                  pri_queue;

  boost::asio::io_service::work work(io_serv);
  (void)work;

  boost::asio::deadline_timer t(io_serv, boost::posix_time::seconds(1));
  t.async_wait(&print);

  t.async_wait(&print);

  while( true ) {
    io_serv.poll_one();
  }
  return 0;

  bool more = true;
  while( io_serv.run_one() ) {
    std::cout << "run_one!" << std::endl;

    // while( io_serv.poll_one() ) {
    //   std::cout << "poll_one!" << std::endl;
    // }

    // execute the highest priority item
//    more = pri_queue.execute_highest();
//    std::cout << "execute_highest!" << std::endl;
  }
//  io_serv.run();

  return 0;
}
