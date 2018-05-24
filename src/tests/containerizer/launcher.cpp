// Licensed to the Apache Software Foundation (ASF) under one
// or more contributor license agreements.  See the NOTICE file
// distributed with this work for additional information
// regarding copyright ownership.  The ASF licenses this file
// to you under the Apache License, Version 2.0 (the
// "License"); you may not use this file except in compliance
// with the License.  You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "tests/containerizer/launcher.hpp"

namespace mesos {
namespace internal {
namespace tests {


ACTION_P(InvokeRecover, launcher)
{
  return launcher->real->recover(arg0);
}


ACTION_P(InvokeFork, launcher)
{
  return launcher->real->fork(
      arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8);
}


ACTION_P(InvokeDestroy, launcher)
{
  return launcher->real->destroy(arg0);
}


TestLauncher::TestLauncher(const process::Owned<slave::Launcher>& _real)
  : real(_real)
{
  using testing::_;
  using testing::DoDefault;

  ON_CALL(*this, recover(_))
    .WillByDefault(InvokeRecover(this));
  EXPECT_CALL(*this, recover(_))
    .WillRepeatedly(DoDefault());

  ON_CALL(*this, forkImpl(_, _, _, _, _, _, _, _, _))
    .WillByDefault(InvokeFork(this));
  EXPECT_CALL(*this, forkImpl(_, _, _, _, _, _, _, _, _))
    .WillRepeatedly(DoDefault());

  ON_CALL(*this, destroy(_))
    .WillByDefault(InvokeDestroy(this));
  EXPECT_CALL(*this, destroy(_))
    .WillRepeatedly(DoDefault());
}


TestLauncher::~TestLauncher() {}

Try<pid_t> TestLauncher::fork(
  const ContainerID& containerId,
  const std::string& path,
  const std::vector<std::string>& argv,
  const mesos::slave::ContainerIO& containerIO,
  const flags::FlagsBase* flags,
  const Option<std::map<std::string, std::string>>& environment,
  const Option<int>& enterNamespaces,
  const Option<int>& cloneNamespaces,
  const std::vector<int_fd>& whitelistFds)
{
  return forkImpl(
    containerId,
    path,
    argv,
    containerIO,
    flags,
    environment,
    enterNamespaces,
    cloneNamespaces,
    whitelistFds);
}

} // namespace tests {
} // namespace internal {
} // namespace mesos {
