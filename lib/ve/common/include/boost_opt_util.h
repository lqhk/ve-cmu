/*
 * Copyright (c) 2011. Shohei NOBUHARA, Kyoto University and Carnegie
 * Mellon University. This code may be used, distributed, or modified
 * only for research purposes or under license from Kyoto University or
 * Carnegie Mellon University. This notice must be retained in all copies.
 */
#ifndef BOOST_OPT_UTIL_H
#define BOOST_OPT_UTIL_H

#include <string>
#include <iostream>
#include <sstream>
#include <boost/program_options.hpp>

inline std::string boost_opt_string(const boost::program_options::variables_map & parameter_map, const char * parameter_name) {
  if(parameter_map.count(parameter_name)) {
    return parameter_map[parameter_name].as<std::string>();
  } else {
    return std::string();
  }
}

// 問題無ければ NULL, 指定されていないものがあればその名前
inline const char * boost_opt_mandatory_check(const boost::program_options::variables_map & parameter_map, const char * optnames[]) {
  while( *optnames ) {
    if( 0 == parameter_map.count(*optnames) ) {
      return *optnames;
    }
    optnames++;
  }

  return NULL;
}

// 説明の先頭に [MANDATORY] とあるオプションが指定されているかどうかをチェック
inline void boost_opt_mandatory_check(const boost::program_options::options_description & cmdline,
                                      const boost::program_options::variables_map & parameter_map) {
  const std::vector< boost::shared_ptr<boost::program_options::option_description> > opts = cmdline.options();
  for(unsigned int i=0 ; i<opts.size() ; i++) {
    // オプションの定義が [MANDATORY] と指定されているか？
    if( opts[i]->description().substr(0, 12) != "[MANDATORY] " ) continue;

    // オプションが実行時に指定されたか？
    if( parameter_map.count(opts[i]->long_name()) ) continue;

    std::cerr << "\n" << cmdline << "\n\n  Error: option --" << opts[i]->long_name() << " is mandatory.\n\n\n";
    exit(0);
  }
}

inline boost::program_options::variables_map boost_opt_check(const boost::program_options::options_description & cmdline, int argc, char * argv[]) {
  boost::program_options::positional_options_description pos;
  boost::program_options::variables_map parameter_map;
  try {
    boost::program_options::store(boost::program_options::command_line_parser(argc, argv).options(cmdline).positional(pos).run(), parameter_map);
    boost::program_options::notify(parameter_map);
  } catch(std::exception &e) {
    std::cerr << "Error in the command line options: " << e.what() << "\n\n"
              << cmdline << "\n";
    exit(1);
  }

  if(parameter_map.count("help")) {
    std::cerr << cmdline;
    exit(1);
  }

  boost_opt_mandatory_check(cmdline, parameter_map);

  return parameter_map;
}

#define BOOST_ANY_OUTPUT_TMPL(X) if(v.type() == typeid(X)) { os << boost::any_cast<X>(v); return; }

inline void boost_any_output(const boost::any & v, std::ostream & os) {
  if(v.empty()) { os << "__EMPTY__"; return; }
  BOOST_ANY_OUTPUT_TMPL(unsigned int);
  BOOST_ANY_OUTPUT_TMPL(int);
  BOOST_ANY_OUTPUT_TMPL(char);
  BOOST_ANY_OUTPUT_TMPL(short);
  BOOST_ANY_OUTPUT_TMPL(float);
  BOOST_ANY_OUTPUT_TMPL(double);
  BOOST_ANY_OUTPUT_TMPL(const unsigned char *);

  // special case for std::string, as it can be used both
  // for "parameter with std::string arg" and "parameter
  // without arg".
  if(v.type() == typeid(std::string)) {
    std::string value = boost::any_cast<std::string>(v);
    if(value.empty()) {
      // this is a "parameter without arg"
      os << "(Specified)";
    } else {
      os << value;
    }
    return;
  }
  //BOOST_ANY_OUTPUT_TMPL(std::string);
  os << "__NOT_SUPPORTED__ (type='" << v.type().name() << "')";
}

inline std::string boost_opt_to_string(const boost::program_options::options_description & cmdline, const boost::program_options::variables_map & parameter_map, const char * line_header="") {
  std::ostringstream oss;
  const std::vector< boost::shared_ptr<boost::program_options::option_description> > opts = cmdline.options();
  for(unsigned int i=0 ; i<opts.size() ; i++) {
    oss << line_header << opts[i]->long_name() << " = ";
    if( parameter_map.count(opts[i]->long_name()) ) {
      boost_any_output(parameter_map[opts[i]->long_name()].value(), oss);
    } else {
      oss << "(Not specified)";
    }
    oss << "\n";
  }
  return oss.str();
}

#endif //BOOST_OPT_UTIL_H
