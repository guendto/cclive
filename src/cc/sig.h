/* cclive
 * Copyright (C) 2013  Toni Gundogdu <legatvs@gmail.com>
 *
 * This file is part of cclive <http://cclive.sourceforge.net/>
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

#ifndef cc__sig_h
#define cc__sig_h

#include <boost/noncopyable.hpp>
#include <boost/scoped_ptr.hpp>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <unistd.h>
#include <csignal>
#include <cstdio>

namespace cc
{

namespace var
{
extern volatile sig_atomic_t recv_sigwinch;
extern volatile sig_atomic_t recv_sigusr1;
extern size_t max_width;
}

namespace sig
{

namespace cv = cc::var;

static inline void default_handler(int n)
{
  switch (n)
    {
    case SIGWINCH:
      cv::recv_sigwinch = 1;
      break;
    case SIGUSR1:
      cv::recv_sigusr1 = 1;
      break;
    default:
      std::clog << __PRETTY_FUNCTION__ << ": warning: unhandled signal `"
                << n << "'" << std::endl;
    }
}

static inline size_t query_term_width()
{
  struct winsize w;
  if (ioctl(fileno(stderr), TIOCGWINSZ, &w) <0)
    return 0;
  return w.ws_col;
}

static inline size_t calc_term_spaceleft(const size_t len, size_t& curr_width)
{
  static const size_t default_term_width = 80;
  if (cv::max_width ==0 || cv::recv_sigwinch ==1)
    {
      cv::max_width = query_term_width();
      if (cv::max_width ==0)
        cv::max_width = default_term_width;
    }
  curr_width = cv::max_width;
  return (cv::max_width-len-1);
}

static inline size_t calc_term_spaceleft(const size_t len)
{
  size_t curr;
  return calc_term_spaceleft(len, curr);
}

} // namespace sig

template<int T>
struct sighandler : boost::noncopyable
{
  inline virtual ~sighandler() { reset(); }
  inline sighandler() { setup(); }
private:
  inline void setup()
  {
    ours.sa_handler = cc::sig::default_handler;
    sigemptyset(&ours.sa_mask);
    ours.sa_flags = 0;

    sigaction(T, NULL, &theirs);

    if (theirs.sa_handler != SIG_IGN)
      sigaction(T, &ours, NULL);
  }
  inline void reset() const
  {
    if (theirs.sa_handler != SIG_IGN)
      sigaction(T, &theirs, NULL);
  }
private:
  struct sigaction theirs;
  struct sigaction ours;
};

typedef struct sighandler<SIGWINCH> type_sigwinch;
typedef struct sighandler<SIGUSR1> type_sigusr1;

typedef boost::scoped_ptr<type_sigwinch> sigwinch_handler_scptr;
typedef boost::scoped_ptr<type_sigusr1> sigusr1_handler_scptr;

} // namespace cc

#endif //  cc__sigwinch_h

// vim: set ts=2 sw=2 tw=72 expandtab:
