/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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
 */
#ifndef JS_LIFECYCLE_MANAGERS_H
#define JS_LIFECYCLE_MANAGERS_H

#include <cstdint>
#include <string>
#include "napi/native_api.h"
#include "napi/native_node_api.h"

namespace OHOS {
namespace Global {
namespace I18n {
class HandleScope {
public:
    /**
     * @brief Constructs a new napi_handle_scope for managing JavaScript object lifetimes.
     *
     * @param env Node-API environment object.
     * @param location Debug location information identifying where the HandleScope was created.
     */
    HandleScope(napi_env env, const std::string& location);

    // Delete copy constructor and assignment operator to prevent accidental copying
    HandleScope(const HandleScope&) = delete;
    HandleScope& operator=(const HandleScope&) = delete;

    /**
     * @brief Destructor that automatically closes the HandleScope.
     */
    ~HandleScope();

    /**
     * @brief Checks if the HandleScope is currently open.
     *
     * @return bool Returns true if the HandleScope is open, false if closed.
     */
    bool IsOpen() const;

private:
    napi_env env_;
    napi_handle_scope scope_;
    std::string location_;
    bool isOpen_;
};

// Strong reference cleanup
class ThreadReference {
public:
    static void CleanupThreadReference(void* reference);
    static ThreadReference* CreateInstance(napi_env env, napi_value value, uint32_t initialRefcount);

    ThreadReference(napi_env env, napi_value value, uint32_t initialRefcount);
    ~ThreadReference();

    ThreadReference(const ThreadReference&) = delete;
    ThreadReference& operator=(const ThreadReference&) = delete;

    bool GetReferenceValue(napi_env env, napi_value* result);
    bool IsValid() const;

private:
    napi_env env_ = nullptr;
    napi_ref ref_ = nullptr;
    bool isValid_ = true;
};
} // namespace I18n
} // namespace Global
} // namespace OHOS
#endif