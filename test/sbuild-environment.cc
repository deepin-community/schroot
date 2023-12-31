/* Copyright © 2006-2007  Roger Leigh <rleigh@codelibre.net>
 *
 * schroot is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * schroot is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see
 * <http://www.gnu.org/licenses/>.
 *
 *********************************************************************/

#include <sbuild/sbuild-environment.h>
#include <sbuild/sbuild-util.h>

#include <iostream>
#include <sstream>

#include <cppunit/extensions/HelperMacros.h>

using namespace CppUnit;

class test_environment : public TestFixture
{
  CPPUNIT_TEST_SUITE(test_environment);
  CPPUNIT_TEST(test_construction);
  CPPUNIT_TEST(test_add_strv);
  CPPUNIT_TEST(test_add_env);
  CPPUNIT_TEST(test_add_value);
  CPPUNIT_TEST(test_add_string_pair);
  CPPUNIT_TEST(test_add_template);
  CPPUNIT_TEST(test_add_string);
  CPPUNIT_TEST(test_add_empty);
  CPPUNIT_TEST(test_remove_strv);
  CPPUNIT_TEST(test_remove_env);
  CPPUNIT_TEST(test_remove_value);
  CPPUNIT_TEST(test_remove_string);
  CPPUNIT_TEST(test_get_value);
  CPPUNIT_TEST(test_get_strv);
  CPPUNIT_TEST(test_operator_plus);
  CPPUNIT_TEST(test_operator_plus_equals);
  CPPUNIT_TEST(test_operator_minus);
  CPPUNIT_TEST(test_operator_minus_equals);
  CPPUNIT_TEST(test_add_filter);
  CPPUNIT_TEST(test_filter);
  CPPUNIT_TEST(test_output);
  CPPUNIT_TEST_SUITE_END();

protected:
  sbuild::environment *env;
  sbuild::environment *half_env;

public:
  test_environment():
    TestFixture(),
    env()
  {}

  virtual ~test_environment()
  {}

  void setUp()
  {
    this->env = new sbuild::environment;
    this->env->add(std::make_pair("TERM", "wy50"));
    this->env->add(std::make_pair("SHELL", "/bin/sh"));
    this->env->add(std::make_pair("USER", "root"));
    this->env->add(std::make_pair("COLUMNS", "80"));
    this->env->add(std::make_pair("EMPTY", ""));

    this->half_env = new sbuild::environment;
    this->half_env->add(std::make_pair("TERM", "wy50"));
    this->half_env->add(std::make_pair("USER", "root"));
  }

  static void add_examples(sbuild::environment& env)
  {
    sbuild::environment::size_type size = env.size();
    env.add(std::make_pair("TERM", "wy50"));
    env.add(std::make_pair("SHELL", "/bin/sh"));
    env.add(std::make_pair("USER", "root"));
    env.add(std::make_pair("COLUMNS", "80"));
    CPPUNIT_ASSERT(env.size() == size + 4);
  }

  static void add_simple_examples(sbuild::environment& env)
  {
    sbuild::environment::size_type size = env.size();
    env.add(std::make_pair("TERM", "wy50"));
    env.add(std::make_pair("USER", "root"));
    CPPUNIT_ASSERT(env.size() == size + 2);
  }

  void tearDown()
  {
    delete this->env;
    delete this->half_env;
  }

  void
  test_construction()
  {
    const char *items[] = {"TERM=wy50", "SHELL=/bin/sh",
                           "USER=root", "COLUMNS=80", "EMPTY=", 0};
    sbuild::environment e(const_cast<char **>(&items[0]));

    CPPUNIT_ASSERT(e.size() == 5);

    CPPUNIT_ASSERT(e == *this->env);
  }

  void
  test_add_strv()
  {
    const char *items[] = {"TERM=wy50", "SHELL=/bin/sh",
                           "USER=root", "COLUMNS=80", "EMPTY=", 0};
    sbuild::environment e;
    e.add(const_cast<char **>(&items[0]));

    CPPUNIT_ASSERT(e.size() == 5);

    CPPUNIT_ASSERT(e == *this->env);
  }

  void
  test_add_env()
  {
    sbuild::environment e;
    e.add(*this->env);

    CPPUNIT_ASSERT(e == *this->env);
  }

  void
  test_add_value()
  {
    sbuild::environment e;
    e.add(sbuild::environment::value_type("TERM", "wy50"));
    e.add(sbuild::environment::value_type("SHELL", "/bin/sh"));
    e.add(sbuild::environment::value_type("USER", "root"));
    e.add(sbuild::environment::value_type("COLUMNS", "80"));
    e.add(sbuild::environment::value_type("EMPTY", ""));

    CPPUNIT_ASSERT(e == *this->env);
  }

  void
  test_add_string_pair()
  {
    sbuild::environment e;
    e.add("TERM", "wy50");
    e.add("SHELL", "/bin/sh");
    e.add("USER", "root");
    e.add("COLUMNS", "80");
    e.add("EMPTY", "");

    CPPUNIT_ASSERT(e == *this->env);
  }

  void
  test_add_template()
  {
    sbuild::environment e;
    e.add("TERM", "wy50");
    e.add("SHELL", "/bin/sh");
    e.add("USER", std::string("root"));
    e.add("COLUMNS", 80);
    e.add("EMPTY", "");

    CPPUNIT_ASSERT(e == *this->env);
  }

  void
  test_add_string()
  {
    sbuild::environment e;
    e.add("TERM=wy50");
    e.add("SHELL=/bin/sh");
    e.add("USER=root");
    e.add("COLUMNS=80");
    e.add("EMPTY=");

    CPPUNIT_ASSERT(e == *this->env);
  }

  void
  test_add_empty()
  {
    sbuild::environment e;
    e.add("TERM=wy50");
    e.add("USER=root");
    e.add("COLUMNS=");
    e.add("EMPTY=");
    e.add("SHELL=");

    this->env->add("COLUMNS=");
    this->env->add(sbuild::environment::value_type("SHELL", ""));

    CPPUNIT_ASSERT(this->env->size() == 5);
    CPPUNIT_ASSERT(e == *this->env);
  }

  void
  test_remove_strv()
  {
    const char *items[] = {"SHELL=/bin/bash",
                           "COLUMNS=160",
                           "EMPTY=", 0};
    this->env->remove(const_cast<char **>(&items[0]));

    CPPUNIT_ASSERT(this->env->size() == 2);
    CPPUNIT_ASSERT(*this->env == *this->half_env);
  }

  void
  test_remove_env()
  {
    sbuild::environment e;
    e.add("SHELL=/bin/bash");
    e.add("COLUMNS=160");
    e.add("EMPTY=");

    this->env->remove(e);

    CPPUNIT_ASSERT(*this->env == *this->half_env);
  }

  void
  test_remove_value()
  {
    this->env->remove(sbuild::environment::value_type("SHELL", "/bin/bash"));
    this->env->remove(sbuild::environment::value_type("COLUMNS", "160"));
    this->env->remove(sbuild::environment::value_type("EMPTY", ""));

    CPPUNIT_ASSERT(*this->env == *this->half_env);
  }

  void
  test_remove_string()
  {
    this->env->remove("SHELL=/bin/bash");
    this->env->remove("COLUMNS=160");
    this->env->remove("EMPTY=");

    CPPUNIT_ASSERT(*this->env == *this->half_env);
  }

  void test_get_value()
  {
    std::string value;
    CPPUNIT_ASSERT(this->env->get("TERM", value) && value == "wy50");
    CPPUNIT_ASSERT(this->env->get("SHELL", value) && value == "/bin/sh");
    CPPUNIT_ASSERT(this->env->get("USER", value) && value == "root");
    CPPUNIT_ASSERT(this->env->get("COLUMNS", value) && value == "80");
    // Check failure doesn't overwrite value.
    CPPUNIT_ASSERT(!this->env->get("MUSTFAIL", value) && value == "80");
    CPPUNIT_ASSERT(this->env->get("EMPTY", value) && value == "");

    // Check getting templated types.
    int tval;
    CPPUNIT_ASSERT(this->env->get("COLUMNS", tval) && tval == 80);
  }

  void test_get_strv()
  {
    char **strv = this->env->get_strv();

    int size = 0;
    for (char **ev = strv; ev != 0 && *ev != 0; ++ev, ++size);

    CPPUNIT_ASSERT(size == 5);
    CPPUNIT_ASSERT(std::string(strv[0]) == "COLUMNS=80");
    CPPUNIT_ASSERT(std::string(strv[1]) == "EMPTY=");
    CPPUNIT_ASSERT(std::string(strv[2]) == "SHELL=/bin/sh");
    CPPUNIT_ASSERT(std::string(strv[3]) == "TERM=wy50");
    CPPUNIT_ASSERT(std::string(strv[4]) == "USER=root");

    sbuild::strv_delete(strv);
  }

  void test_operator_plus()
  {
    sbuild::environment e;
    e.add("SHELL=/bin/sh");
    e.add("COLUMNS=80");
    e.add("EMPTY=");

    sbuild::environment result;
    result = *this->half_env + e;
    CPPUNIT_ASSERT(result == *this->env);

    sbuild::environment e2;
    e2 = *this->half_env + "SHELL=/bin/sh";
    e2 = e2 + sbuild::environment::value_type("COLUMNS", "80");
    e2 = e2 + sbuild::environment::value_type("EMPTY", "");
    CPPUNIT_ASSERT(e2 == *this->env);
  }

  void test_operator_plus_equals()
  {
    sbuild::environment e;
    e.add("SHELL=/bin/sh");
    e.add("COLUMNS=80");
    e.add("EMPTY=");

    sbuild::environment result(*this->half_env);
    result += e;
    CPPUNIT_ASSERT(result == *this->env);

    sbuild::environment e2(*this->half_env);
    e2 += "SHELL=/bin/sh";
    // TODO: Why does calling direct fail?
    sbuild::environment::value_type val("COLUMNS", "80");
    e2 += val;
    sbuild::environment::value_type val2("EMPTY", "");
    e2 += val2;
    CPPUNIT_ASSERT(e2 == *this->env);
  }

  void test_operator_minus()
  {
    sbuild::environment e;
    e.add("SHELL=/bin/sh");
    e.add("COLUMNS=80");
    e.add("EMPTY=");

    sbuild::environment result;
    result = *this->env - e;
    CPPUNIT_ASSERT(result == *this->half_env);

    sbuild::environment e2;
    e2 = *this->env - "SHELL=/bin/sh";
    e2 = e2 - sbuild::environment::value_type("COLUMNS", "80");
    e2 = e2 - sbuild::environment::value_type("EMPTY", "");
    CPPUNIT_ASSERT(e2 == *this->half_env);
  }

  void test_operator_minus_equals()
  {
    sbuild::environment e;
    e.add("SHELL=/bin/sh");
    e.add("COLUMNS=80");
    e.add("EMPTY=");

    sbuild::environment result(*this->env);
    result -= e;
    CPPUNIT_ASSERT(result == *this->half_env);

    sbuild::environment e2(*this->env);
    e2 -= "SHELL=/bin/sh";
    // TODO: Why does calling direct fail?
    sbuild::environment::value_type val("COLUMNS", "80");
    e2 -= val;
    sbuild::environment::value_type val2("EMPTY", "");
    e2 -= val2;
    CPPUNIT_ASSERT(e2 == *this->half_env);
  }

  void test_add_filter()
  {
    sbuild::regex f("^FOO|BAR$");

    sbuild::environment e;
    e.set_filter(f);

    CPPUNIT_ASSERT(f.compare(e.get_filter()) == 0);
  }

  void test_filter()
  {
    sbuild::regex f("^FOO|BAR$");

    sbuild::environment e;
    e.set_filter(f);

    e.add("FOO=bar");
    e.add("BAR=baz");
    e.add("BAZ=bat");
    e.add("BAT=bah");

    std::string value;
    CPPUNIT_ASSERT(e.get("FOO", value) == false);
    CPPUNIT_ASSERT(e.get("BAR", value) == false);
    CPPUNIT_ASSERT(e.get("BAZ", value) && value == "bat");
    CPPUNIT_ASSERT(e.get("BAT", value) && value == "bah");
  }

  void test_output()
  {
    std::ostringstream os;
    os << *this->env;

    CPPUNIT_ASSERT(os.str() ==
                   "COLUMNS=80\n"
                   "EMPTY=\n"
                   "SHELL=/bin/sh\n"
                   "TERM=wy50\n"
                   "USER=root\n");
  }
};

CPPUNIT_TEST_SUITE_REGISTRATION(test_environment);
