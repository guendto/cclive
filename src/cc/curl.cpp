/* cclive
 * Copyright (C) 2013  Toni Gundogdu <legatvs@gmail.com>
 *
 * This file is part of cclive <http://cclive.sourceforge.net/>.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of the
 * License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public
 * License along with this program.  If not, see
 * <http://www.gnu.org/licenses/>.
 */

#include <ccinternal>

#include <curl/curl.h>

#include <ccquvi>
#include <ccoptions>
#include <ccutil>

namespace cc
{

namespace po = boost::program_options;

static void _set_proxy(CURL *c, const po::variables_map& vm)
{
  if (vm.count(OPT__PROXY))
    {
      curl_easy_setopt(c, CURLOPT_PROXY,
                       vm[OPT__PROXY].as<std::string>().c_str());
    }
  if_optsw_given(vm, OPT__NO_PROXY)
    curl_easy_setopt(c, CURLOPT_PROXY, "");
}

void curl_setup(CURL *c, const po::variables_map& vm)
{
  const int n = vm[OPT__THROTTLE].as<throttle>().value() * 1024;
  const curl_off_t t = static_cast<curl_off_t>(n);

  curl_easy_setopt(c, CURLOPT_MAX_RECV_SPEED_LARGE, t);
  curl_easy_setopt(c, CURLOPT_FOLLOWLOCATION, 1L);

  if_optsw_given(vm, OPT__VERBOSE_LIBCURL)
    curl_easy_setopt(c, CURLOPT_VERBOSE, 1L);

#ifndef HAVE_LIBQUVI_0_9
  curl_easy_setopt(c, CURLOPT_USERAGENT,
                   vm[OPT__AGENT].as<std::string>().c_str());
#endif

  curl_easy_setopt(c, CURLOPT_DNS_CACHE_TIMEOUT,
                  vm[OPT__DNS_CACHE_TIMEOUT].as<dns_cache_timeout>().value());

  curl_easy_setopt(c, CURLOPT_CONNECTTIMEOUT,
                   vm[OPT__CONNECT_TIMEOUT].as<connect_timeout>().value());

  curl_easy_setopt(c, CURLOPT_TIMEOUT,
                   vm[OPT__TRANSFER_TIMEOUT].as<transfer_timeout>().value());

  _set_proxy(c, vm);
}

} // namespace cc

// vim: set ts=2 sw=2 tw=72 expandtab:
