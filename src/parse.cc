#include <cstring>

#include <boost/spirit/home/qi.hpp>
#include <boost/spirit/home/phoenix/core.hpp>
#include <boost/spirit/home/phoenix/core/argument.hpp>
#include <boost/spirit/home/phoenix/container.hpp>
#include <boost/spirit/home/phoenix/object.hpp>

#include "parse.hh"

namespace qi = boost::spirit::qi;
namespace phoenix = boost::phoenix;

dsh::CommandLine construct_pipe(const std::vector<dsh::CommandLine>& xs) {
  assert(xs.size() == 2);
  dsh::CommandLine piped, left = xs[0], right = xs[1];
  dsh::Redirection out(&(right.cmds[0]));
  dsh::Redirection in(&(left.cmds.back()));

  std::copy(left.cmds.begin(), left.cmds.end(), std::back_inserter(piped.cmds));
  piped.cmds.back().redirections[1].push_back(out);
  std::copy(right.cmds.begin(), right.cmds.end(), std::back_inserter(piped.cmds));
  piped.cmds.back().redirections[0].push_back(in);

  return piped;
}

dsh::CommandLine concat_commandlines(std::vector<dsh::CommandLine>& xs) {
  dsh::CommandLine concated;
  BOOST_FOREACH(dsh::CommandLine c, xs) {
    std::copy(c.cmds.begin(), c.cmds.end(), std::back_inserter(concated.cmds));
  }
  return concated;
}

dsh::CommandLine dsh::parse(const std::string& str) {
  using qi::alnum;
  using qi::char_;
  using qi::as_string;
  using qi::lexeme;
  using qi::omit;
  using qi::space;
  using qi::_1;
  using qi::_2;
  using phoenix::push_back;
  using phoenix::construct;
  using phoenix::at;

  dsh::CommandLine cmdline;
  std::vector<dsh::CommandLine> cmdlines;
  auto cmd =
    (
     omit[*space] >>
     (as_string[+(alnum | char_("|-"))] % omit[+space]) >>
     omit[*space]
     )[construct<CommandLine>(_1)];
  auto pipe = (cmd >> omit[*space] >> omit[char_('|')] >> omit[*space] >> cmd);
  auto maybe_cmd = (cmd | omit[*space]);
  auto cmds = (maybe_cmd % omit[char_(';')]);
  qi::parse(str.begin(), str.end(), cmds, cmdlines);

  //std::cout << cmdline->cmdCount() << ' ' << cmdline->cmds.at(0)->argc() << std::endl;
  //return cmdline;
  return concat_commandlines(cmdlines);
}
