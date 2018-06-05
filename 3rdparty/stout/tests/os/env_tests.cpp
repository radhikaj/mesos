// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License

#include <gtest/gtest.h>

#include <stout/os.hpp>

#include <stout/tests/utils.hpp>
#include <stout/os/environment.hpp>

using std::string;


class EnvTest : public TemporaryDirectoryTest {};


TEST(EnvTest, SimpleEnvTest)
{
  const string key = "key";
  const string value_1 = "value_1";

  // Key currently does not exist in process environment.
  EXPECT_NONE(os::getenv(key));

  // Set environment variable, check that it is there now.
  os::setenv(key, value_1, false);
  Option<string> result = os::getenv(key);
  ASSERT_SOME(result);
  EXPECT_EQ(value_1, result.get());

  // Verify that if we set the environment variable without the `overwrite`
  // flag set, the value of that variable was not updated.
  const string value_2 = "value_2";
  os::setenv(key, value_2, false);
  result = os::getenv(key);
  ASSERT_SOME(result);
  EXPECT_EQ(value_1, result.get());

  // Now set environment variable, and set `overwrite` flag.
  os::setenv(key, value_2, true);
  result = os::getenv(key);
  ASSERT_SOME(result);
  EXPECT_EQ(value_2, result.get());

  // Now verify that the default behavior sets the `overwrite` flag to true.
  const string env_value_3 = "even_newer_favorite_value";
  os::setenv(key, env_value_3);
  result = os::getenv(key);
  ASSERT_SOME(result);
  EXPECT_EQ(env_value_3, result.get());

  // Finally, unset the variable.
  os::unsetenv(key);
  result = os::getenv(key);
  EXPECT_NONE(result);
}



TEST(EnvTest, EraseEnv)
{
  os::setenv("key", "value");

  std::map<string, string> pre = os::environment();

  const string key = "key";
  const string value_1 = "value";

  Option<string> value = os::getenv("key");
  EXPECT_EQ("value", value.get());

  os::eraseenv("key");
  EXPECT_NONE(os::getenv("key"));

  // Verify that erasing "key" removed the environment variable without
  // damaging any other ernvironment variables.
  pre.erase("key");
  EXPECT_EQ(pre, os::environment());

}
