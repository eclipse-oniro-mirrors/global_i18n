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

#include <gtest/gtest.h>
#include <map>
#include <vector>
#include "translit_test.h"
#include <unistd.h>
#include "unicode/utypes.h"
#include "unicode/translit.h"
#include "ohos/init_data.h"

using namespace OHOS::Global::I18n;
using testing::ext::TestSize;
using namespace std;
using namespace testing;

namespace OHOS {
namespace Global {
namespace I18n {

std::map<std::string, std::string> TranslitTest::translitTestCases {
    {"阿胶", "ē jiāo"},
    {"挨打", "ái dǎ"},
    {"扒手", "pá shǒu"},
    {"扒草", "pá cǎo"},
    {"刀把", "dāo bà"},
    {"话把儿", "huà bà er"},
    {"单薄", "dān bó"},
    {"稀薄", "xī bó"},
    {"背包", "bēi bāo"},
    {"背枪", "bēi qiāng"},
    {"剥皮", "bāo pí"},
    {"称心", "chèn xīn"},
    {"对称", "duì chèn"},
    {"冲床", "chòng chuáng"},
    {"冲子", "chòng zi"},
    {"处罚", "chǔ fá"},
    {"处置", "chǔ zhì"},
    {"朝阳", "zhāo yáng"},
    {"朝霞", "zhāo xiá"},
    {"朝气", "zhāo qì"},
    {"重复", "chóng fù"},
    {"重新", "chóng xīn"},
    {"答理", "dā lǐ"},
    {"答应", "dā yìng"},
    {"山大王", "shān dài wáng"},
    {"当天", "dàng tiān"},
    {"颠倒", "diān dǎo"},
    {"倒下", "dǎo xià"},
    {"得喝水了", "děi hē shuǐ le"},
    {"好得很", "hǎo de hěn"},
    {"的确", "dí què"},
    {"目的", "mù dì"},
    {"一语中的", "yī yǔ zhòng dì"},
    {"都市", "dū shì"},
    {"首都", "shǒu dū"},
    {"弹琴", "tán qín"},
    {"弹力", "tán lì"},
    {"理发", "lǐ fà"},
    {"头发", "tóu fà"},
    {"身分", "shēn fèn"},
    {"一分子", "yī fèn zi"},
    {"可汗", "kè hán"},
    {"大汗", "dà hán"},
    {"呼号", "hū háo"},
    {"号叫", "háo jiào"},
    {"应和", "yìng hè"},
    {"和诗", "hè shī"},
    {"和面", "huó miàn"},
    {"和泥", "huó ní"},
    {"搀和", "chān huo"},
    {"搅和", "jiǎo huo"},
    {"喝令", "hè lìng"},
    {"喝彩", "hè cǎi"},
    {"好奇", "hào qí"},
    {"好学", "hào xué"},
    {"还原", "huán yuán"},
    {"还书", "huán shū"},
    {"茶几", "chá jī"},
    {"几乎", "jī hū"},
    {"假期", "jià qī"},
    {"假日", "jià rì"},
    {"间断", "jiàn duàn"},
    {"相间", "xiāng jiàn"},
    {"将士", "jiàng shì"},
    {"虾兵蟹将", "xiā bīng xiè jiàng"},
    {"角色", "jué sè"},
    {"角斗", "jué dòu"},
    {"教书", "jiāo shū"},
    {"教给", "jiāo gěi"},
    {"结果", "jié guǒ"},
    {"结实", "jiē shí"},
    {"睡觉", "shuì jiào"},
    {"午觉", "wǔ jiào"},
    {"关卡", "guān qiǎ"},
    {"卡子", "qiǎ zi"},
    {"看守", "kān shǒu"},
    {"看管", "kān guǎn"},
    {"空白", "kòng bái"},
    {"空地", "kòng dì"},
    {"空闲", "kòng xián"},
    {"露头", "lòu tóu"},
    {"露马脚", "lòu mǎ jiǎo"},
    {"丢三落四", "diū sān là sì"},
    {"落下", "là xià"},
    {"了解", "liǎo jiě"},
    {"不了了之", "bù liǎo liǎo zhī"},
    {"音乐", "yīn yuè"},
    {"乐曲", "yuè qǔ"},
    {"埋怨", "mán yuàn"},
    {"没收", "mò shōu"},
    {"淹没", "yān mò"},
    {"难民", "nàn mín"},
    {"难兄难弟", "nàn xiōng nàn dì"},
    {"宁可", "nìng kě"},
    {"宁折不弯", "nìng zhé bù wān"},
    {"勉强", "miǎn qiǎng"},
    {"强迫", "qiǎng pò"},
    {"强词夺理", "qiǎng cí duó lǐ"},
    {"倔强", "jué jiàng"},
    {"曲调", "qǔ diào"},
    {"曲艺", "qǔ yì"},
    {"歌曲", "gē qǔ"},
    {"数落", "shǔ luò"},
    {"数数", "shǔ shù"},
    {"少年", "shào nián"},
    {"少先队", "shào xiān duì"},
    {"调皮", "tiáo pí"},
    {"调和", "tiáo hé"},
    {"挑战", "tiǎo zhàn"},
    {"挑拨", "tiǎo bō"},
    {"威吓", "wēi hè"},
    {"恐吓", "kǒng hè"},
    {"朝鲜", "cháo xiǎn"},
    {"鲜为人知", "xiǎn wéi rén zhī"},
    {"行市", "háng shì"},
    {"银行", "yín háng"},
    {"道行", "dào héng"},
    {"鸡血", "jī xiě"},
    {"狗血", "gǒu xiě"},
    {"馒头", "mán tou"},
    {"流了点血", "liú le diǎn xiě"},
    {"相声", "xiàng shēng"},
    {"面相", "miàn xiàng"},
    {"应付", "yìng fù"},
    {"应对", "yìng duì"},
    {"要求", "yāo qiú"},
    {"着急", "zháo jí"},
    {"着迷", "zháo mí"},
    {"着凉", "zháo liáng"},
    {"着落", "zhuó luò"},
    {"着重", "zhuó zhòng"},
    {"着手", "zhuó shǒu"},
    {"正月", "zhēng yuè"},
    {"中奖", "zhòng jiǎng"},
    {"中靶", "zhòng bǎ"},
    {"耕种", "gēng zhòng"},
    {"种植", "zhòng zhí"},
    {"转动", "zhuàn dòng"},
    {"转速", "zhuàn sù"},
    {"只身", "zhī shēn"},
    {"一只", "yī zhī"},
    {"西藏", "xī zàng"},
    {"藏民", "zàng mín"},
    {"青藏高原", "qīng zàng gāo yuán"},
    {"散文", "sǎn wén"},
    {"参差", "cēn cī"},
    {"缝纫", "féng rèn"},
    {"劲敌", "jìng dí"},
    {"扇风", "shān fēng"},
    {"畜牧", "xù mù"},
    {"弄堂", "lòng táng"},
    {"伎俩", "jì liǎng"},
    {"草率", "cǎo shuài"},
    {"漂泊", "piāo bó"},
    {"泊车", "bó chē"},
    {"膀胱", "páng guāng"},
    {"复辟", "fù bì"},
    {"单于", "chán yú"},
    {"折本", "shé běn"},
    {"一沓纸", "yì dá zhǐ"},
    {"引吭高歌", "yǐn háng gāo gē"},
    {"出差", "chū chāi"},
    {"差不多", "chà bù duō"},
    {"差错", "chā cuò"},
    {"盛饭", "chéng fàn"},
    {"创伤", "chuāng shāng"},
    {"雪茄", "xuě jiā"},
    {"番茄", "fān qié"},
    {"茄子", "qié zi"},
    {"发疟子", "fā yào zǐ"},
    {"自怨自艾", "zì yuàn zì yì"},
    {"传记", "zhuàn jì"},
    {"荷枪实弹", "hè qiāng shí dàn"},
    {"涨红了脸", "zhàng hóng le liǎn"},
    {"校对", "jiào duì"},
    {"奇数", "jī shù"},
    {"炮制", "páo zhì"},
    {"供给", "gōng jǐ"},
    {"冠冕堂皇", "guān miǎn táng huáng"},
    {"干活", "gàn huó"},
    {"巷道", "hàng dào"},
    {"薄荷", "bò hé"},
    {"可恶", "kě wù"},
    {"大腹便便", "dà fù pián pián"},
    {"星宿", "xīng xiù"},
    {"号啕大哭", "háo táo dà kū"},
    {"倾轧", "qīng yà"},
    {"模样", "mú yàng"},
    {"宿舍", "sù shè"},
    {"殷红", "yān hóng"},
    {"还要", "hái yào"},
    {"系领带", "jì lǐng dài"},
    {"放假", "fàng jià"},
    {"空降", "kōng jiàng"},
    {"塞外", "sài wài"},
    {"屏气凝神", "bǐng qì níng shén"},
    {"考卷", "kǎo juàn"},
    {"乐亭", "lào tíng"},
    {"好为人师", "hào wéi rén shī"},
    {"窥伺", "kuī sì"},
    {"处理", "chǔ lǐ"},
};

void TranslitTest::SetUpTestCase(void)
{
    SetHwIcuDirectory();
}

void TranslitTest::TearDownTestCase(void)
{
}

void TranslitTest::SetUp(void)
{}

void TranslitTest::TearDown(void)
{}

/**
 * @tc.name: TranslitTest0001
 * @tc.desc: Test icu::Transliterator
 * @tc.type: FUNC
 */
HWTEST_F(TranslitTest, TranslitTest0001, TestSize.Level1)
{
    const std::string transId = "Any-Latn";
    UErrorCode status = U_ZERO_ERROR;
    icu::UnicodeString unistr = icu::UnicodeString::fromUTF8(transId);
    icu::Transliterator *trans = icu::Transliterator::createInstance(unistr, UTransDirection::UTRANS_FORWARD, status);
    ASSERT_TRUE(U_SUCCESS(status));
    ASSERT_TRUE(trans != nullptr);
    for (auto iter = translitTestCases.begin(); iter != translitTestCases.end(); ++iter) {
        std::string word = iter->first;
        std::string expect = iter->second;
        icu::UnicodeString uniWord = icu::UnicodeString::fromUTF8(word.data());
        trans->transliterate(uniWord);
        std::string result;
        uniWord.toUTF8String(result);
        EXPECT_EQ(result, expect);
    }
    delete trans;
}
} // namespace I18n
} // namespace Global
} // namespace OHOS