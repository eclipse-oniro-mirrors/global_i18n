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
#include "js_lifecycle_managers.h"
#include "i18n_hilog.h"

namespace OHOS {
namespace Global {
namespace I18n {
HandleScope::HandleScope(napi_env env, const std::string& location) : env_(env), location_(location)
{
    isOpen_ = true;
    napi_status status = napi_open_handle_scope(env_, &scope_);
    if (status != napi_ok) {
        isOpen_ = false;
        HILOG_ERROR_I18N("Failed to open handle scope at: %{public}s", location_.c_str());
    }
}

HandleScope::~HandleScope()
{
    if (!isOpen_) {
        return;
    }
    napi_status status = napi_close_handle_scope(env_, scope_);
    if (status != napi_ok) {
        HILOG_ERROR_I18N("Failed to close handle scope at: %{public}s", location_.c_str());
    }
}

bool HandleScope::IsOpen() const
{
    return isOpen_;
}

void ThreadReference::CleanupThreadReference(void* reference)
{
    if (reference) {
        auto nativeReference = reinterpret_cast<ThreadReference*>(reference);
        delete nativeReference;
        nativeReference = nullptr;
    }
}

ThreadReference* ThreadReference::CreateInstance(napi_env env, napi_value value, uint32_t initialRefcount)
{
    ThreadReference* threadReference = new (std::nothrow) ThreadReference(env, value, initialRefcount);
    if (!threadReference) {
        HILOG_ERROR_I18N("ThreadReference::CreateInstance: Failed to create ThreadReference pointer");
        return nullptr;
    }
    if (!threadReference->IsValid()) {
        HILOG_ERROR_I18N("ThreadReference::CreateInstance: ThreadReference is not valid");
        delete threadReference;
        threadReference = nullptr;
        return nullptr;
    }
    napi_status status = napi_add_env_cleanup_hook(env, ThreadReference::CleanupThreadReference, threadReference);
    if (status != napi_ok) {
        HILOG_ERROR_I18N("ThreadReference::CreateInstance: Add env cleanup hook failed when InitLocale");
        delete threadReference;
        threadReference = nullptr;
        return nullptr;
    }
    return threadReference;
}

ThreadReference::ThreadReference(napi_env env, napi_value value, uint32_t initialRefcount) : env_(env)
{
    napi_status status = napi_create_reference(env_, value, initialRefcount, &ref_);
    if (status != napi_ok) {
        HILOG_ERROR_I18N("ThreadReference: Failed to create reference");
        isValid_ = false;
        return;
    }
}

ThreadReference::~ThreadReference()
{
    if (!isValid_) {
        return;
    }
    napi_status status = napi_delete_reference(env_, ref_);
    if (status != napi_ok) {
        HILOG_ERROR_I18N("ThreadReference: Failed to delete reference");
    }
}

bool ThreadReference::IsValid() const
{
    return isValid_;
}

bool ThreadReference::GetReferenceValue(napi_env env, napi_value* result)
{
    if (!isValid_) {
        return false;
    }
    napi_status status = napi_get_reference_value(env, ref_, result);
    if (status != napi_ok) {
        HILOG_ERROR_I18N("ThreadReference: Failed to get reference value");
        return false;
    }
    return *result != nullptr;
}
} // namespace I18n
} // namespace Global
} // namespace OHOS
