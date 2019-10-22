/*
 *
 * Copyright 2017 Asylo authors
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */

#ifndef ASYLO_CRYPTO_UTIL_TRIVIAL_OBJECT_UTIL_H_
#define ASYLO_CRYPTO_UTIL_TRIVIAL_OBJECT_UTIL_H_

#include <openssl/rand.h>
#include <string>
#include <type_traits>

#include "absl/strings/escaping.h"
#include "absl/strings/str_cat.h"
#include "absl/strings/string_view.h"
#include "asylo/util/status.h"

namespace asylo {

template <class T>
Status SetTrivialObjectFromHexString(absl::string_view view, T *obj) {
  static_assert(std::is_trivially_copy_assignable<T>::value,
                "Template parameter is not trivially copy-assignable.");
  // Make sure that the string has correct number of hex characters
  // to exactly fill obj, and that obj is not a nullptr.
  if (obj == nullptr) {
    return Status(error::GoogleError::INVALID_ARGUMENT,
                  "Output container must not be a nullptr");
  }
  if (view.size() != sizeof(T) * 2) {
    return Status(
        error::GoogleError::INVALID_ARGUMENT,
        absl::StrCat("The size of the output container: ", sizeof(T),
                     " must be the size of the string / 2: ", view.size() / 2));
  }
  for (auto ch : view) {
    if (std::isxdigit(ch) == 0) {
      return Status(
          error::GoogleError::INVALID_ARGUMENT,
          "The given string must be made of only valid hex characters");
    }
  }
  absl::HexStringToBytes(view).copy(reinterpret_cast<char *>(obj),
                                    sizeof(*obj));
  return Status::OkStatus();
}

template <class T>
std::string ConvertTrivialObjectToHexString(const T &obj) {
  static_assert(std::is_trivially_copy_assignable<T>::value,
                "Template parameter is not trivially copy-assignable.");
  return absl::BytesToHexString(
      absl::string_view(reinterpret_cast<const char *>(&obj), sizeof(obj)));
}

template <class T>
T TrivialRandomObject() {
  static_assert(std::is_trivially_copy_assignable<T>::value,
                "Template parameter is not trivially copy-assignable.");
  T tmp;
  RAND_bytes(reinterpret_cast<uint8_t *>(&tmp), sizeof(tmp));
  return tmp;
}

template <class T>
T TrivialZeroObject() {
  static_assert(std::is_trivially_copy_assignable<T>::value,
                "Template parameter is not trivially copy-assignable.");
  T tmp;
  memset(&tmp, 0, sizeof(tmp));
  return tmp;
}

template <class T>
T TrivialOnesObject() {
  static_assert(std::is_trivially_copy_assignable<T>::value,
                "Template parameter is not trivially copy-assignable.");
  T tmp;
  memset(&tmp, 0xff, sizeof(tmp));
  return tmp;
}

template <class T>
Status SetTrivialObjectFromBinaryString(absl::string_view view, T *obj) {
  static_assert(std::is_trivially_copy_assignable<T>::value,
                "Template parameter is not trivially copy-assignable.");
  if (view.size() != sizeof(*obj)) {
    return Status(error::GoogleError::INVALID_ARGUMENT,
                  absl::StrCat("Cannot set a ", sizeof(*obj), "byte object",
                               " from a string of size ", view.size()));
  }
  memcpy(obj, view.data(), view.size());
  return Status::OkStatus();
}

template <class T>
std::string ConvertTrivialObjectToBinaryString(const T &obj) {
  static_assert(std::is_trivially_copy_assignable<T>::value,
                "Template parameter is not trivially copy-assignable.");
  return std::string(reinterpret_cast<const char *>(&obj), sizeof(obj));
}

}  // namespace asylo

#endif  // ASYLO_CRYPTO_UTIL_TRIVIAL_OBJECT_UTIL_H_
