/* cclive
 * Copyright (C) 2013  Toni Gundogdu <legatvs@gmail.com>
 *
 * This file is part of cclive <http://cclive.sourceforge.net/>.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <ccinternal>

#include <curl/curl.h>

#include <ccquvi>
#include <ccoptions>
#include <ccutil>

namespace cc
{

CURL *curl_new()
{
  curl_global_init(CURL_GLOBAL_ALL);
  return curl_easy_init();
}

void curl_free(CURL *c)
{
  if (c == NULL)
    return;

  curl_easy_cleanup(c);
  curl_global_cleanup();
}

namespace po = boost::program_options;

static void _set_proxy(CURL *c, const po::variables_map& map)
{
  if (map.count("proxy"))
    {
      curl_easy_setopt(c, CURLOPT_PROXY,
                       map["proxy"].as<std::string>().c_str());
    }

  if (opts.flags.no_proxy)
    curl_easy_setopt(c, CURLOPT_PROXY, "");
}

void curl_setup(CURL *c)
{
  const po::variables_map map = cc::opts.map();

  if (map.count("throttle"))
    {
      curl_off_t limit = map["throttle"].as<int>()*1024;
      curl_easy_setopt(c, CURLOPT_MAX_RECV_SPEED_LARGE, limit);
    }

  curl_easy_setopt(c, CURLOPT_FOLLOWLOCATION, 1L);

  if (cc::opts.flags.verbose_libcurl)
    curl_easy_setopt(c, CURLOPT_VERBOSE, 1L);

  curl_easy_setopt(c, CURLOPT_USERAGENT,
                   map["agent"].as<std::string>().c_str());

  curl_easy_setopt(c, CURLOPT_DNS_CACHE_TIMEOUT,
                   map["dns-cache-timeout"].as<int>());

  curl_easy_setopt(c, CURLOPT_CONNECTTIMEOUT,
                   map["connect-timeout"].as<int>());

  curl_easy_setopt(c, CURLOPT_TIMEOUT,
                   map["transfer-timeout"].as<int>());

  _set_proxy(c, map);
}

} // namespace cc

// vim: set ts=2 sw=2 tw=72 expandtab:
