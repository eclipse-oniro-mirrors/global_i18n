/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#include "styled_number_format.h"

#include <unordered_map>
#include "unicode/formattedvalue.h"
#include "i18n_hilog.h"

namespace OHOS {
namespace Global {
namespace I18n {
StyledNumberFormat::StyledNumberFormat(bool isNumberFormat, std::shared_ptr<NumberFormat> numberFormat,
    std::shared_ptr<SimpleNumberFormat> simpleNumberFormat)
    : isNumberFormat(isNumberFormat), numberFormat(numberFormat), simpleNumberFormat(simpleNumberFormat)
{
}

std::string StyledNumberFormat::Format(double number)
{
    if (isNumberFormat && numberFormat) {
        return numberFormat->Format(number);
    }
    if (!isNumberFormat && simpleNumberFormat) {
        return simpleNumberFormat->Format(number);
    }
    return "";
}

std::vector<StyledNumberFormat::NumberPart> StyledNumberFormat::ParseToParts(double number)
{
    if (isNumberFormat && numberFormat) {
        if (numberFormat->IsRelativeTimeFormat(number)) {
            icu::FormattedRelativeDateTime formattedValue = numberFormat->FormatToFormattedRelativeDateTime(number);
            return ParseToParts(formattedValue);
        } else {
            icu::number::FormattedNumber formattedValue = numberFormat->FormatToFormattedNumber(number);
            return ParseToParts(formattedValue);
        }
    }
    if (!isNumberFormat && simpleNumberFormat) {
        icu::number::FormattedNumber formattedValue = simpleNumberFormat->FormatToFormattedNumber(number);
        return ParseToParts(formattedValue);
    }
    return {};
}

std::vector<StyledNumberFormat::NumberPart> StyledNumberFormat::ParseToParts(
    icu::number::FormattedNumber &formattedValue)
{
    std::vector<NumberPart> result;
    UErrorCode status = U_ZERO_ERROR;
    icu::ConstrainedFieldPosition constrainedPos;
    constrainedPos.constrainCategory(UFIELD_CATEGORY_NUMBER);
    while (formattedValue.nextPosition(constrainedPos, status)) {
        if (U_FAILURE(status)) {
            HILOG_ERROR_I18N("StyledNumberFormat::ParseToParts get nextPosition faild");
            break;
        }
        size_t startIndex = (size_t)constrainedPos.getStart();
        if (constrainedPos.getCategory() != UFIELD_CATEGORY_NUMBER) {
            continue;
        }
        switch (constrainedPos.getField()) {
            case UNUM_PERCENT_FIELD:
            case UNUM_PERMILL_FIELD:
            case UNUM_MEASURE_UNIT_FIELD:
            case UNUM_CURRENCY_FIELD: {
                result.push_back({"unit", startIndex,
                    static_cast<size_t>(constrainedPos.getLimit() - startIndex)});
                break;
            }
            case UNUM_DECIMAL_SEPARATOR_FIELD: {
                result.push_back({"decimal", startIndex,
                    static_cast<size_t>(constrainedPos.getLimit() - startIndex)});
                break;
            }
            case UNUM_EXPONENT_SYMBOL_FIELD:
            case UNUM_EXPONENT_SIGN_FIELD:
            case UNUM_EXPONENT_FIELD:
            case UNUM_SIGN_FIELD:
            case UNUM_COMPACT_FIELD:
            case UNUM_INTEGER_FIELD: {
                result.push_back({"integer", startIndex,
                    static_cast<size_t>(constrainedPos.getLimit() - startIndex)});
                break;
            }
            case UNUM_FRACTION_FIELD: {
                result.push_back({"fraction", startIndex,
                    static_cast<size_t>(constrainedPos.getLimit() - startIndex)});
                break;
            }
            default:
                break;
        }
    }
    return result;
}

std::vector<StyledNumberFormat::NumberPart> StyledNumberFormat::ParseToParts(
    icu::FormattedRelativeDateTime &formattedValue)
{
    std::vector<NumberPart> result;
    UErrorCode status = U_ZERO_ERROR;
    std::string numberStr;
    icu::UnicodeString formatResult = formattedValue.toString(status);
    if (U_FAILURE(status)) {
        HILOG_ERROR_I18N("StyledNumberFormat::ParseToParts toString faild");
        return result;
    }
    formatResult.toUTF8String(numberStr);
    icu::ConstrainedFieldPosition constrainedPos;
    constrainedPos.constrainCategory(UFIELD_CATEGORY_NUMBER);
    size_t prevIndex = 0;
    size_t length = numberStr.length();
    while (formattedValue.nextPosition(constrainedPos, status)) {
        if (U_FAILURE(status)) {
            HILOG_ERROR_I18N("StyledNumberFormat::ParseToParts get nextPosition faild");
            break;
        }
        size_t startIndex = (size_t)constrainedPos.getStart();
        if (constrainedPos.getCategory() != UFIELD_CATEGORY_NUMBER ||
            constrainedPos.getField() == UNUM_GROUPING_SEPARATOR_FIELD) {
            continue;
        }
        if (constrainedPos.getField() == UNUM_DECIMAL_SEPARATOR_FIELD) {
            result.push_back({"decimal", startIndex, static_cast<size_t>(constrainedPos.getLimit() - startIndex)});
        }
        if (constrainedPos.getField() == UNUM_INTEGER_FIELD) {
            result.push_back({"integer", startIndex, static_cast<size_t>(constrainedPos.getLimit() - startIndex)});
        }
        if (constrainedPos.getField() == UNUM_FRACTION_FIELD) {
            result.push_back({"fraction", startIndex, static_cast<size_t>(constrainedPos.getLimit() - startIndex)});
        }
        if (startIndex > prevIndex) {
            result.push_back({"unit", prevIndex, startIndex - prevIndex});
        }
        prevIndex = (size_t)constrainedPos.getLimit();
    }
    if (prevIndex < length) {
        result.push_back({"unit", prevIndex, length - prevIndex});
    }
    return result;
}
} // namespace I18n
} // namespace Global
} // namespace OHOS