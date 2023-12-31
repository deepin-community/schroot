include(CheckCXXSourceRuns)

function(regex_test namespace header library outvar outlib)
  set(CMAKE_REQUIRED_LIBRARIES_SAVE ${CMAKE_REQUIRED_LIBRARIES})
  set(CMAKE_REQUIRED_LIBRARIES ${CMAKE_REQUIRED_LIBRARIES} ${library})

  if (CMAKE_CROSSCOMPILING)
    message(WARNING "Using simplified regex check for cross compilation. Assuming that regex implementations work correctly.")
    check_cxx_source_compiles(
"#include <${header}>

int main() {
  ${namespace} foo(\"x\");
  ${namespace} bar(\"x\", ${namespace}::extended);
  std::string test(\"x\");
  ${namespace}_search(test, foo);
  ${namespace}_match(test, foo);
  return 0;
}"
${outvar})
  else(CMAKE_CROSSCOMPILING)
    check_cxx_source_runs(
"#include <${header}>
#include <iostream>

int main() {
  ${namespace} foo(\"^foo[bar]\$\");
  ${namespace} bar(\"^foo[bar]\$\", ${namespace}::extended);
  ${namespace} chk(\"^[^:/,.][^:/,]*\$\", ${namespace}::extended);
  std::string test(\"foob\");
  std::string fail(\"fail:\");

  ${namespace} description_keys(\"^description\\\\\\\\[.*]$\", ${namespace}::extended);

  if (!${namespace}_search(test, foo)) return 1;
  if (!${namespace}_search(test, bar)) return 2;
  if (!${namespace}_search(test, chk)) return 3;
  if (${namespace}_search(fail, foo)) return 4;
  if (${namespace}_search(fail, bar)) return 5;
  if (${namespace}_search(fail, chk)) return 6;

  if (!${namespace}_match(test, foo)) return 7;
  if (!${namespace}_match(test, bar)) return 8;
  if (!${namespace}_match(test, chk)) return 9;
  if (${namespace}_match(fail, foo)) return 10;
  if (${namespace}_match(fail, bar)) return 11;
  if (${namespace}_match(fail, chk)) return 12;

  // Checks for broken support in GCC 4.9 and 5.1
  ${namespace} range1(\"^[a-z0-9][a-z0-9-]*\$\", ${namespace}::extended);
  ${namespace} range2(\"^[a-z0-9][-a-z0-9]*\$\", ${namespace}::extended);
  if (!${namespace}_match(test, range1)) return 13;
  if (!${namespace}_match(test, range2)) return 14;
  if (!${namespace}_match(\"a-\", range1)) return 15;
  if (!${namespace}_match(\"a-\", range2)) return 16;
  if (${namespace}_match(\"-a\", range1)) return 17;
  if (${namespace}_match(\"-a\", range2)) return 18;

  return 0;
}"
${outvar})
  endif(CMAKE_CROSSCOMPILING)

  set(CMAKE_REQUIRED_LIBRARIES ${CMAKE_REQUIRED_LIBRARIES_SAVE})

  set(${outvar} ${${outvar}} PARENT_SCOPE)
  if (${outvar})
    set(${outlib} ${library} PARENT_SCOPE)
  endif(${outvar})
endfunction(regex_test)

regex_test(std::regex regex "" HAVE_REGEX_REGEX REGEX_LIBRARY)
if(NOT HAVE_REGEX_REGEX)
  regex_test(std::tr1::regex tr1/regex "" HAVE_TR1_REGEX REGEX_LIBRARY)
  if(NOT HAVE_TR1_REGEX)
    regex_test(boost::regex boost/regex.hpp "${Boost_REGEX_LIBRARY_RELEASE}" HAVE_BOOST_REGEX REGEX_LIBRARY)
    if(NOT HAVE_BOOST_REGEX)
      message(FATAL_ERROR "No working regular expression implementation found")
    endif(NOT HAVE_BOOST_REGEX)
  endif(NOT HAVE_TR1_REGEX)
endif(NOT HAVE_REGEX_REGEX)
