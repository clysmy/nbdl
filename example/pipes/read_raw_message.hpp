//
// Copyright Jason Rice 2016
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef NBDL_EXAMPLE_RECEIVE_RAW_MESSAGE_HPP
#define NBDL_EXAMPLE_RECEIVE_RAW_MESSAGE_HPP

#include <asio.hpp>
#include <cstdint>
#include <nbdl.hpp>

namespace example
{
  namespace detail
  {
    struct read_length_fn
    {
      using tcp = asio::ip::tcp;
      tcp::socket& socket;
      unsigned char buffer[4];

      read_length_fn(tcp::socket& socket)
        : socket(socket)
      { }

      template <typename Resolver, typename ...Args>
      void operator()(Resolver&& resolver, Args&&...)
      {
        asio::async_read(socket, asio::buffer(buffer, 4),
          [&](std::error_code ec, std::size_t)
          {
            if (ec)
            {
              resolver.reject(ec);
            }
            else
            {
              uint32_t length =
                  buffer[0] << 24
                | buffer[1] << 16
                | buffer[2] << 8
                | buffer[3]
                ;
              resolver.resolve(length);
            }
          });
      }
    };

    struct read_body_fn
    {
      using tcp = asio::ip::tcp;
      tcp::socket& socket;
      std::string body;

      read_body_fn(tcp::socket& socket)
        : socket(socket)
      { }

      template <typename Resolver>
      void operator()(Resolver&& resolver, uint32_t length)
      {
        body = std::string{};
        body.resize(length);

        asio::async_read(socket, asio::buffer(body, length),
          [&](std::error_code error, std::size_t)
          {
            if (!error)
            {
              resolver.resolve(std::move(body));
            }
            else
            {
              resolver.reject(error);
            }
          });
      }
    };
  };

  auto read_raw_message = [](asio::ip::tcp::socket& socket)
  { 
    return nbdl::pipe(
      nbdl::promise(detail::read_length_fn(socket))
    , nbdl::promise(detail::read_body_fn(socket))
    );
  };
}

#endif
