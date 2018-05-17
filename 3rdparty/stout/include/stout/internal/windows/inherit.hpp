// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//  http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef __STOUT_INTERNAL_WINDOWS_INHERIT_HPP__
#define __STOUT_INTERNAL_WINDOWS_INHERIT_HPP__

#include <stout/error.hpp>
#include <stout/nothing.hpp>
#include <stout/try.hpp>
#include <stout/windows.hpp>

#include <stout/os/int_fd.hpp>

#include <processthreadsapi.h>

namespace internal {
namespace windows {

// This function creates LPPROC_THREAD_ATTRIBUTE_LIST.
// LPPROC_THREAD_ATTRIBUTE_LIST is used to whitelist handles sent to
// a child process.
typedef _PROC_THREAD_ATTRIBUTE_LIST AttributeList;
inline Result<std::shared_ptr<AttributeList>>
create_attributes_list_for_handles(std::vector<HANDLE>& handles)
{
  if (handles.empty()) {
    return None();
  }

  SIZE_T size = 0;
  BOOL result = ::InitializeProcThreadAttributeList(nullptr, 1, 0, &size);
  if (result == FALSE) {
    if (GetLastError() != ERROR_INSUFFICIENT_BUFFER) {
      return WindowsError();
    }
  }

  std::shared_ptr<AttributeList>
  attribute_list(
    reinterpret_cast<AttributeList*>(std::malloc(size)), [](AttributeList* p) {
      ::DeleteProcThreadAttributeList(p);
      std::free(p);
    });

  if (attribute_list == nullptr) {
    return WindowsError(ERROR_OUTOFMEMORY);
  }

  result =
    ::InitializeProcThreadAttributeList(attribute_list.get(), 1, 0, &size);
  if (result == FALSE) {
    return WindowsError();
  }

  result = ::UpdateProcThreadAttribute(
    attribute_list.get(),
    0,
    PROC_THREAD_ATTRIBUTE_HANDLE_LIST,
    handles.data(),
    (handles.size() * sizeof(HANDLE)),
    nullptr,
    nullptr);
  if (result == FALSE) {
    return WindowsError();
  }

  return attribute_list;
}

// This function enables or disables inheritance for a Windows file handle.
//
// NOTE: By default, handles on Windows are not inheritable, so this is
// primarily used to enable inheritance when passing handles to child processes,
// and subsequently disable inheritance.
inline Try<Nothing> set_inherit(const int_fd& fd, const bool inherit)
{
  const BOOL result = ::SetHandleInformation(
      fd, HANDLE_FLAG_INHERIT, inherit ? HANDLE_FLAG_INHERIT : 0);

  if (result == FALSE) {
    return WindowsError();
  }

  return Nothing();
}

} // namespace windows {
} // namespace internal {

#endif // __STOUT_INTERNAL_WINDOWS_INHERIT_HPP__
