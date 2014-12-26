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

#ifndef cc__options_h
#define cc__options_h

#include <ccinternal>

#include <boost/program_options/options_description.hpp>
#include <boost/program_options/positional_options.hpp>
#include <boost/program_options/variables_map.hpp>
#include <boost/program_options/parsers.hpp>
#include <boost/exception/diagnostic_information.hpp>
#include <boost/exception/errinfo_file_name.hpp>
#include <boost/exception/errinfo_errno.hpp>
#include <boost/exception/info.hpp>
#include <boost/algorithm/string/join.hpp>
#include <boost/noncopyable.hpp>
#include <boost/tuple/tuple.hpp>
#include <boost/filesystem.hpp>
#include <boost/foreach.hpp>
#include <boost/format.hpp>
#include <iostream>
#include <fstream>

#include <ccerror>
#include <ccre>

// Core

#define OPT__CONFIG_FILE "config-file"
#define OPT__NO_DOWNLOAD "no-download"
#define OPT__BACKGROUND  "background"

// Core -- configurable

#define OPT__TIMESTAMP  "timestamp"
#define OPT__CONTINUE   "continue"
#define OPT__STREAM     "stream"
#define OPT__EXEC       "exec"

// Informative

#define OPT__PRINT_STREAMS "print-streams"
#define OPT__PRINT_CONFIG  "print-config"
#define OPT__SUPPORT       "support"
#define OPT__VERSION       "version"
#define OPT__HELP          "help"

// Output

#define OPT__VERBOSE_LIBCURL "verbose-libcurl"
#define OPT__OUTPUT_FILE     "output-file"
#define OPT__OVERWRITE       "overwrite"
#define OPT__LOG_FILE        "log-file"
#define OPT__QUIET           "quiet"

// Output -- configurable

#define OPT__FILENAME_FORMAT "filename-format"
#define OPT__UPDATE_INTERVAL "update-interval"
#define OPT__PROGRESSBAR     "progressbar"
#define OPT__OUTPUT_DIR      "output-dir"
#define OPT__TR              "tr"

// Network

#define OPT__NO_PROXY "no-proxy"

// Network -- configurable

#define OPT__DNS_CACHE_TIMEOUT "dns-cache-timeout"
#define OPT__TRANSFER_TIMEOUT  "transfer-timeout"
#define OPT__CONNECT_TIMEOUT   "connect-timeout"
#define OPT__MAX_RETRIES       "max-retries"
#define OPT__RETRY_WAIT        "retry-wait"
#define OPT__NO_RESOLVE        "no-resolve"
#define OPT__THROTTLE          "throttle"
#define OPT__AGENT             "agent"
#define OPT__PROXY             "proxy"

// Deprecated -- configurable

#define OPT__PREFER_FORMAT "prefer-format"

// Hidden (positional)

#define OPT__URL "url"

namespace cc
{

namespace po = boost::program_options;
namespace fs = boost::filesystem;

typedef std::vector<std::string> vs;
typedef std::ostream ostr;

struct tr
{
  inline friend ostr& operator<<(ostr& o, const tr& r) { return o<<r.str(); }
  explicit inline tr(const std::string& s)
  {
    try
      {
        std::string empty;
        re::tr(s, empty);
      }
    catch (const cc::error::tuple& x)
      {
        cc::error::type_tuple const *t =
          boost::get_error_info<cc::error::errinfo_tuple>(x);

        const std::string& w = boost::get<1>(*t);

        const std::string& m =
          (boost::format("invalid option value: %1%: %2%") % s % w).str();

        BOOST_THROW_EXCEPTION(cc::error::tuple()
          << cc::error::errinfo_tuple(boost::make_tuple(OPT__TR, m)));
      }
    _str = s;
  }
  inline const std::string& str() const { return _str; }
private:
  std::string _str;
};

typedef std::vector<tr> vtr;

inline static void validate(boost::any& v,
                            const std::vector<std::string>& values,
                            tr*, int)
{
  po::validators::check_first_occurrence(v);
  const std::string& s = po::validators::get_single_string(values);
  v = boost::any(tr(s));
}

struct progressbar_mode
{
  typedef enum {normal, simple, dotline} mode_type;

  inline friend ostr& operator<<(ostr& o, const progressbar_mode& r)
    { return o << r.str(); }
  explicit inline progressbar_mode(const std::string& s)
    : _mode(normal)
  {
    if (s == "normal")
      _mode = normal;
    else if (s == "dotline")
      _mode = dotline;
    else if (s == "simple")
     _mode = simple;
    else
      {
        const std::string& m =
          (boost::format("invalid option value (`%1%')") % s).str();

        BOOST_THROW_EXCEPTION(cc::error::tuple()
          << cc::error::errinfo_tuple(boost::make_tuple(OPT__PROGRESSBAR,m)));
      }
    _str = s;
  }
  inline const std::string& str() const { return _str; }
  inline mode_type value() const { return _mode; }
private:
  std::string _str;
  mode_type _mode;
};

inline static void validate(boost::any& v,
                            const std::vector<std::string>& values,
                            progressbar_mode*, int)
{
  po::validators::check_first_occurrence(v);
  const std::string& s = po::validators::get_single_string(values);
  v = boost::any(progressbar_mode(s));
}

struct integer
{
  inline friend ostr& operator<<(ostr& o, const integer& r)
    { return o << r._value; }
  inline int value() const { return _value; }
protected:
  explicit inline integer(): _value(0) { }
  inline static int cast(const std::string& optname, const std::string& s)
  {
    int r = 0;
    try
      {
        r = boost::lexical_cast<int>(s);
      }
    catch (const boost::bad_lexical_cast&)
      {
        const std::string& m = (boost::format("invalid option value: "
                                              "not an integer (`%1%')")
                                                % s).str();
        integer::failed(optname, m);
      }
    return r;
  }
  inline static void failed(const std::string& optname, const std::string& m)
  {
    BOOST_THROW_EXCEPTION(cc::error::tuple()
      << cc::error::errinfo_tuple(boost::make_tuple(optname, m)));
  }
  virtual void set(const std::string&, const std::string&) = 0;
  virtual void set(const int, const std::string&) = 0;
protected:
  int _value;
};

struct integer_ge0 : integer  // integer is greater than or equal to 0
{
protected:
  explicit inline integer_ge0(const std::string& optname,const std::string& s)
    : integer() { set(optname, s); }
  // _optname remains unset -- intended to be used by default_value only:
  explicit inline integer_ge0(const int n): integer() { set(n); }
private:
  inline void set(const std::string& optname, const std::string& s)
    { set(integer::cast(optname, s), optname); }
  inline void set(const int n, const std::string& optname="")
  {
    if (n <0)
      {
        const std::string& m =
          (boost::format("invalid option value (%1% < 0)") % n).str();

        integer::failed(optname, m);
      }
    _value = n;
  }
};

struct dns_cache_timeout : integer_ge0
{
  explicit inline dns_cache_timeout(int n): integer_ge0(n) { }
  explicit inline dns_cache_timeout(const std::string& s)
    : integer_ge0(OPT__DNS_CACHE_TIMEOUT, s) { }
};

struct transfer_timeout : integer_ge0
{
  explicit inline transfer_timeout(int n): integer_ge0(n) { }
  explicit inline transfer_timeout(const std::string& s)
    : integer_ge0(OPT__TRANSFER_TIMEOUT, s) { }
};

struct connect_timeout : integer_ge0
{
  explicit inline connect_timeout(int n): integer_ge0(n) { }
  explicit inline connect_timeout(const std::string& s)
    : integer_ge0(OPT__CONNECT_TIMEOUT, s) { }
};

struct max_retries : integer_ge0
{
  explicit inline max_retries(int n): integer_ge0(n) { }
  explicit inline max_retries(const std::string& s)
    : integer_ge0(OPT__MAX_RETRIES, s) { }
};

struct retry_wait : integer_ge0
{
  explicit inline retry_wait(int n): integer_ge0(n) { }
  explicit inline retry_wait(const std::string& s)
    : integer_ge0(OPT__RETRY_WAIT, s) { }
};

struct throttle : integer_ge0
{
  explicit inline throttle(int n): integer_ge0(n) { }
  explicit inline throttle(const std::string& s)
    : integer_ge0(OPT__THROTTLE, s) { }
};

template <class T>
inline static void integer_helper(boost::any& v,
                                  const std::vector<std::string>& values)
{
  po::validators::check_first_occurrence(v);
  const std::string& s = po::validators::get_single_string(values);
  v = boost::any(T(s));
}

inline static void validate(boost::any& v,
                            const std::vector<std::string>& values,
                            dns_cache_timeout*, int)
{
  integer_helper<dns_cache_timeout>(v, values);
}

inline static void validate(boost::any& v,
                            const std::vector<std::string>& values,
                            transfer_timeout*, int)
{
  integer_helper<transfer_timeout>(v, values);
}

inline static void validate(boost::any& v,
                            const std::vector<std::string>& values,
                            connect_timeout*, int)
{
  integer_helper<connect_timeout>(v, values);
}

inline static void validate(boost::any& v,
                            const std::vector<std::string>& values,
                            max_retries*, int)
{
  integer_helper<max_retries>(v, values);
}

inline static void validate(boost::any& v,
                            const std::vector<std::string>& values,
                            retry_wait*, int)
{
  integer_helper<retry_wait>(v, values);
}

inline static void validate(boost::any& v,
                            const std::vector<std::string>& values,
                            throttle*, int)
{
  integer_helper<throttle>(v, values);
}

struct integer_gt0 : integer  // integer is greater than 0
{
protected:
  explicit inline integer_gt0(const std::string& optname,const std::string& s)
    : integer() { set(optname, s); }
  // _optname remains unset -- intended to be used by default_value only:
  explicit inline integer_gt0(const int n): integer() { set(n); }
private:
  inline void set(const std::string& optname, const std::string& s)
    { set(integer::cast(optname, s), optname); }
  inline void set(const int n, const std::string& optname="")
  {
    if (n <=0)
      {
        const std::string& m =
          (boost::format("invalid option value (%1% < 1)") % n).str();

        integer::failed(optname, m);
      }
    _value = n;
  }
};

struct update_interval : integer_gt0
{
  explicit inline update_interval(int n): integer_gt0(n) { }
  explicit inline update_interval(const std::string& s)
    : integer_gt0(OPT__UPDATE_INTERVAL, s) { }
};

inline static void validate(boost::any& v,
                            const std::vector<std::string>& values,
                            update_interval*, int)
{
  integer_helper<update_interval>(v, values);
}

struct options : boost::noncopyable
{
  explicit inline options(int const argc, char const **argv)
  {
    fs::path cwd(fs::current_path());
    fs::path fpath(cwd);

    const char *h = getenv("HOME");
    if (h && strlen(h) >0)
      fpath = fs::system_complete(fs::path(h) / ".ccliverc");

    const std::string default_config = fpath.string();
    std::string config;

    po::options_description podc("Core");

    podc.add_options()
    (OPT__CONFIG_FILE",F",
      po::value<std::string>(&config)->default_value(fpath.string()),
      "Initialization file to read")
    (OPT__NO_DOWNLOAD",n",
      po::bool_switch(),
      "Skip download, print media details only")
#if defined(HAVE_WORKING_FORK) || defined(HAVE_WORKING_VFORK)
    (OPT__BACKGROUND",b",
      po::bool_switch(),
      "Go to background after startup")
#endif
    ;

    po::options_description podcc("Core -- Configurable");

    podcc.add_options()
    (OPT__TIMESTAMP",N",
      po::bool_switch(),
      "Try to preserve file modification time")
    (OPT__CONTINUE",c",
      po::bool_switch(),
      "Try to resume partially downloaded media")
    (OPT__STREAM",s",
      po::value<std::string>()->default_value("default"),
      "Select media stream to download")
    (OPT__EXEC",e",
      po::value<vs>(), "Execute child program after download")
    ;

    po::options_description podi("Informative");

    podi.add_options()
    (OPT__PRINT_STREAMS",S",
      po::bool_switch(),
      "Print available media streams for media URL")
    (OPT__PRINT_CONFIG",D",
      po::bool_switch(),
      "Print value of defined config options and exit")
    (OPT__SUPPORT",u",
      po::value<bool>()->zero_tokens()->implicit_value(true)
                       ->notifier(print_support),
      "Print supported media host domains and exit")
    (OPT__VERSION",v",
      po::value<bool>()->zero_tokens()->implicit_value(true)
                       ->notifier(print_version),
      "Print version information and exit")
    (OPT__HELP",h",
      po::bool_switch(),
      "Print help and exit")
    ;

    po::options_description podo("Output");

    podo.add_options()
    (OPT__VERBOSE_LIBCURL",B",
      po::bool_switch(),
      "Enable verbose output for libcurl")
    (OPT__OUTPUT_FILE",O",
      po::value<std::string>(),
      "Write downloaded media to file")
    (OPT__OVERWRITE",W",
      po::bool_switch(),
      "Overwrite existing files")
    (OPT__LOG_FILE",l",
      po::value<std::string>()->default_value("cclive_log"),
      "Write log output to file")
    (OPT__QUIET",q",
      po::bool_switch(),
      "Turn off all output but errors/libcurl")
    ;

    po::options_description podoc("Output -- Configurable");

    const vtr _default_tr = default_tr();

    podoc.add_options()
    (OPT__FILENAME_FORMAT",f",
      po::value<std::string>()->default_value("%t.%s"),
      "Format of media file name")
    (OPT__UPDATE_INTERVAL",i",
      po::value<update_interval>()->default_value(update_interval(1)),
      "Progressbar update interval in seconds")
    (OPT__PROGRESSBAR",R",
      po::value<progressbar_mode>()
        ->default_value(progressbar_mode("normal")),
      "Use progressbar mode")
    (OPT__OUTPUT_DIR",d",
      po::value<std::string>()->default_value(cwd.string()),
      "Write downloaded media to directory")
    (OPT__TR",t",
      po::value<vtr>()
        ->default_value(_default_tr, _default_tr[0].str()),
      "Translate characters in media title")
    ;

    po::options_description podn("Network");

    podn.add_options()
    (OPT__NO_PROXY",X",
      po::bool_switch(),
      "Disable use of HTTP proxy")
    ;

    po::options_description podnc("Network -- Configurable");

    podnc.add_options()
    (OPT__DNS_CACHE_TIMEOUT",A",
      po::value<dns_cache_timeout>()->default_value(dns_cache_timeout(60)),
      "Seconds DNS resolves kept in memory")
    (OPT__TRANSFER_TIMEOUT",T",
      po::value<transfer_timeout>()->default_value(transfer_timeout(0),"off"),
      "Seconds transfer allowed to take")
    (OPT__CONNECT_TIMEOUT",C",
      po::value<connect_timeout>()->default_value(connect_timeout(30)),
      "Seconds connecting allowed to take")
    (OPT__MAX_RETRIES",m",
      po::value<max_retries>()->default_value(max_retries(5)),
      "Max download attempts before giving up")
    (OPT__RETRY_WAIT",w",
      po::value<retry_wait>()->default_value(retry_wait(5)),
      "Time to wait before retrying")
    (OPT__NO_RESOLVE",r",
      po::bool_switch(),
      "Do not resolve HTTP URL redirections")
    (OPT__THROTTLE",H",
      po::value<throttle>()->default_value(throttle(0),"off"),
      "Do not exceed transfer rate (KB/s)")
    (OPT__AGENT",a",
      po::value<std::string>()->default_value("Mozilla/5.0"),
      "Identify as arg to HTTP servers")
    (OPT__PROXY",x",
      po::value<std::string>(),
      "Use proxy for HTTP connections")
    ;

    po::options_description poddc("Deprecated -- Configurable");

    poddc.add_options()
    (OPT__PREFER_FORMAT",p",
      po::value<vs>(),
      "Specify preferred stream for media hosts")
    ;

    // Visible.

    po::options_description podv;
    podv.add(podc).add(podcc).add(podi).add(podo).add(podoc)
        .add(podn).add(podnc).add(poddc);

    // Hidden.

    po::options_description podh;
    podh.add_options()(OPT__URL, po::value<vs>());

    // Command line.

    po::options_description podl;
    podl.add(podv).add(podh);

    // Config.

    po::options_description podf;
    podf.add(podcc).add(podoc).add(podnc).add(poddc);

    // URL.

    po::positional_options_description ppod;
    ppod.add(OPT__URL, -1);

    try
      {
        store(po::command_line_parser(argc, argv)
              .options(podl).positional(ppod).run(), _vm);
        notify(_vm);
      }
#ifdef HAVE_BUG__BOOST_PO__MULTIPLE_OCCURRENCES
    catch (const po::multiple_occurrences& x)
      {
        BOOST_THROW_EXCEPTION(cc::error::tuple()
          << cc::error::errinfo_tuple(
              boost::make_tuple(x.get_option_name(), x.what())));
      }
#endif
    catch (...)
      {
        throw;
      }

    if_optsw_given(_vm, OPT__HELP)
      {
        std::cout
          << boost::format("Usage: %1% [options] [args]") % PACKAGE_NAME
          << "\n" << podv << std::endl;
        BOOST_THROW_EXCEPTION(cc::exit_program());
      }

    fpath = fs::system_complete(config);

    std::ifstream ifs(fpath.string().c_str());
    if (ifs)
      {
        try
          {
            store(parse_config_file(ifs, podf), _vm);
            notify(_vm);
          }
#ifdef HAVE_BUG__BOOST_PO__MULTIPLE_OCCURRENCES
        catch (const po::multiple_occurrences& x)
          {
            BOOST_THROW_EXCEPTION(cc::error::tuple()
              << cc::error::errinfo_tuple(
                  boost::make_tuple(x.get_option_name(), x.what()))
              << boost::errinfo_file_name(fpath.string()));
          }
#endif
        catch (const boost::exception& x)
          {
            x << boost::errinfo_file_name(fpath.string());
            throw;
          }
      }
    else
      {
        // Unless this is the default config, raise an error if open failed.
        if (default_config != fpath)
          {
            BOOST_THROW_EXCEPTION(cc::error::config()
                                  << boost::errinfo_file_name(fpath.string())
                                  << boost::errinfo_errno(errno));
          }
      }

    if_optsw_given(_vm, OPT__PRINT_CONFIG)
      dump();

    chk_depr();
  }

  inline const po::variables_map& values() const { return _vm; }

private:
  inline void dump()
  {
    for (po::variables_map::iterator i = _vm.begin(); i != _vm.end(); ++i)
    {
      const po::variable_value &v = i->second;

      if (v.empty())
        continue;

      const std::type_info &t = v.value().type();
      bool nl = true;

      if (t == typeid(bool))
        {
          std::cout << boost::format("%1% is %2%")
                        % i->first
                        % ((v.as<bool>() ? "set" : "unset"));
        }
      else if (t == typeid(vs))
        {
          const vs& values = v.as<vs>();
          BOOST_FOREACH(const std::string& s, values)
            std::cout << i->first << "=" << s << "\n";
          nl = false;
        }
      else if (t == typeid(vtr))
        {
          const vtr& values = v.as<vtr>();
          BOOST_FOREACH(const tr& r, values)
            std::cout << i->first << "=" << r << "\n";
          nl = false;
        }
      else
        {
          std::cout << i->first << "=";
          if (t == typeid(dns_cache_timeout))
            std::cout << v.as<dns_cache_timeout>();
          else if (t == typeid(progressbar_mode))
            std::cout << v.as<progressbar_mode>();
          else if (t == typeid(transfer_timeout))
            std::cout << v.as<transfer_timeout>();
          else if (t == typeid(update_interval))
            std::cout << v.as<update_interval>();
          else if (t == typeid(connect_timeout))
            std::cout << v.as<connect_timeout>();
          else if (t == typeid(max_retries))
            std::cout << v.as<max_retries>();
          else if (t == typeid(retry_wait))
            std::cout << v.as<retry_wait>();
          else if (t == typeid(std::string))
            std::cout << v.as<std::string>();
          else if (t == typeid(throttle))
            std::cout << v.as<throttle>();
          else if (t == typeid(double))
            std::cout << v.as<double>();
          else if (t == typeid(int))
            std::cout << v.as<int>();
          else if (t == typeid(tr))
            std::cout << v.as<tr>();
          else
            std::cout << "skipped: an unknown option type";
        }

      if (!nl)
        std::cout << std::flush;
      else
        std::cout << std::endl;
    }
    BOOST_THROW_EXCEPTION(cc::exit_program());
  }

  inline static void depr_msg(const std::string& depr_optname,
                              const std::string& repl_optname)
  {
    std::clog << boost::format("[!] Option `--%1%' is deprecated and "
                               "will be removed later") % depr_optname;
    if (repl_optname.length() >0)
      {
        std::clog << boost::format("\n[!] Consider using the option "
                                   "`--%1%', instead") % repl_optname;
      }
    std::clog << std::endl;
  }

#define if_given(o)           if (_vm.count(o))
#define was_given_norepl(o)   if_given(o)  depr_msg(o,"")
#define was_given(o,r)        if_given(o)  depr_msg(o,r)
  inline void chk_depr()
  {
#ifdef HAVE_LIBQUVI_0_9
    was_given("prefer-format", "stream");
#endif
  }
#undef was_given_norepl
#undef was_given
#undef if_given

  inline static void print_version(const bool&)
  {
    std::cout << PACKAGE_NAME" "
#ifdef VN
              << VN
#else
              << PACKAGE_VERSION
#endif
              << "\n  built on " << BUILD_TIME
              << " for "         << CANONICAL_TARGET
              << "\n    with "   << CXX", "CXXFLAGS
              << "\n  libquvi "  << quvi::version()
              << "\n  libquvi-scripts "
              << quvi_version(QUVI_VERSION_SCRIPTS)
              << std::endl;

    static const char copyr[] =
      "\nCopyright (C) 2010-2013  Toni Gundogdu <legatvs@gmail.com>\n"
      PACKAGE_NAME
      " comes with ABSOLUTELY NO WARRANTY. You may redistribute copies of\n"
      PACKAGE_NAME
      " under the terms of the GNU Affero General Public License version\n"
      "3 or later. For more information, see "
      "<http://www.gnu.org/licenses/agpl.html>.\n\n"
      "To contact the developers, please mail to "
      "<cclive-devel@lists.sourceforge.net>";

    std::cerr << copyr << std::endl;
    BOOST_THROW_EXCEPTION(cc::exit_program());
  }

  inline static void print_support(const int&)
  {
    quvi::query q;
    std::cout << quvi::support_to_s(q.support()) << std::flush;
    BOOST_THROW_EXCEPTION(cc::exit_program());
  }

  static vtr default_tr()
  {
    vtr r;
    r.push_back(tr("/(\\w|\\pL|\\s)/g"));
    return r;
  }

private:
  po::variables_map _vm;
};

} // namespace cc

#endif // cc__options_h

// vim: set ts=2 sw=2 tw=72 expandtab:
