#include <cstring>

#include <boost/spirit/home/qi.hpp>
#include <boost/spirit/home/phoenix/core.hpp>
#include <boost/spirit/home/phoenix/container.hpp>
#include <boost/spirit/home/phoenix/object.hpp>

#include "parse.hh"


dsh::CommandLine dsh::parse(const std::string& str) {
  namespace qi = boost::spirit::qi;
  namespace phoenix = boost::phoenix;
  using qi::alnum;
  using qi::char_;
  using qi::as_string;
  using qi::lexeme;
  using qi::omit;
  using qi::space;
  using qi::_1;
  using phoenix::push_back;
  using phoenix::construct;

  dsh::CommandLine cmdline = dsh::CommandLine();
  auto cmd = *space >>
    (as_string[+(alnum | char_("|-"))] % omit[+space])
    [push_back(phoenix::ref(cmdline.cmds), construct<Command>(_1))] >>
    *space;
  auto maybe_cmd = (cmd | *space);
  auto cmds = maybe_cmd >> *(char_(';') >> maybe_cmd);
  qi::parse(str.begin(), str.end(), cmds);

  //std::cout << cmdline->cmdCount() << ' ' << cmdline->cmds.at(0)->argc() << std::endl;
  return cmdline;
}
