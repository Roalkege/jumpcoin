// Copyright (c) 2010 Satoshi Nakamoto
// Copyright (c) 2009-2015 The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include "base58.h"
#include "chainparams.h"
#include "consensus/merkle.h"
#include "dstencode.h"

#include "tinyformat.h"
#include "util.h"
#include "utilstrencodings.h"

#include <assert.h>

#include <boost/assign/list_of.hpp>

#include "chainparamsseeds.h"
#include "arith_uint256.h"

using namespace std;

static CBlock CreateGenesisBlock(const char* pszTimestamp, const CScript& genesisOutputScript, uint32_t nTime, uint32_t nNonce, uint32_t nBits, int32_t nVersion, const CAmount& genesisReward)
{

    CMutableTransaction txNew;
    txNew.nVersion = 1;
    txNew.nTime = nTime;
    txNew.vin.resize(1);
    txNew.vout.resize(1);
    txNew.vin[0].scriptSig = CScript() << 0 << CScriptNum(42) << vector<unsigned char>((const unsigned char*)pszTimestamp, (const unsigned char*)pszTimestamp + strlen(pszTimestamp));
    txNew.vout[0].nValue = genesisReward;

    CBlock genesis;
    genesis.nTime    = nTime;
    genesis.nBits    = nBits;
    genesis.nNonce   = nNonce;
    genesis.nVersion = nVersion;
    genesis.vtx.push_back(txNew);
    genesis.hashPrevBlock.SetNull();
    genesis.hashMerkleRoot = BlockMerkleRoot(genesis);
    return genesis;
}

/**
 * Build the genesis block. Note that the output of its generation
 * transaction cannot be spent since it did not originally exist in the
 * database.
 */
static CBlock CreateGenesisBlock(uint32_t nTime, uint32_t nNonce, uint32_t nBits, int32_t nVersion, const CAmount& genesisReward)
{
    const char* pszTimestamp = "So sad to code on Valentine's Day";
    const CScript genesisOutputScript = CScript() << ParseHex("048ceec1606a79da1de9dd1358d547521bd0975141331d7aa37d696e95d0a12c1bfa6ddca048b1503bc351d8cb198df249fe60a78307c5c475528ce28b76405624") << OP_CHECKSIG;
    return CreateGenesisBlock(pszTimestamp, genesisOutputScript, nTime, nNonce, nBits, nVersion, genesisReward);
}

/**
 * Main network
 */
/**
 * What makes a good checkpoint block?
 * + Is surrounded by blocks with reasonable timestamps
 *   (no blocks before with a timestamp after, none after with
 *    timestamp before)
 * + Contains no strange transactions
 */

class CMainParams : public CChainParams {
public:
    CMainParams() {
        strNetworkID = "main";
        consensus.nMaxReorganizationDepth = 500;
        consensus.nMajorityEnforceBlockUpgrade = 750;
        consensus.nMajorityRejectBlockOutdated = 950;
        consensus.nMajorityWindow = 1000;
        consensus.powLimit = uint256S("00000fffffffffffffffffffffffffffffffffffffffffffffffffffffffffff");
        consensus.posLimit = uint256S("00000fffffffffffffffffffffffffffffffffffffffffffffffffffffffffff");
        consensus.posLimitV2 = uint256S("000000000000ffffffffffffffffffffffffffffffffffffffffffffffffffff");
        consensus.nTargetTimespan = 16 * 60; // 16 mins
        consensus.nTargetSpacingV1 = 60;
        consensus.nTargetSpacing = 60;
        consensus.BIP34Height = -1;
        consensus.BIP34Hash = uint256();
        consensus.fPowAllowMinDifficultyBlocks = false;
        consensus.fPowNoRetargeting = false;
        consensus.fPoSNoRetargeting = false;
        consensus.nRuleChangeActivationThreshold = 1916; // 95% of 2016
        consensus.nMinerConfirmationWindow = 2016; // nTargetTimespan / nTargetSpacing
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].bit = 28;
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].nStartTime = 1199145601; // January 1, 2008
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].nTimeout = 1230767999; // December 31, 2008

        consensus.nProtocolV1RetargetingFixedTime = 1696771811;
        consensus.nProtocolV2Time = 1696771812;
        consensus.nProtocolV3Time = 1696771813;
        consensus.nProtocolV3_1Time = 4102437600;
        consensus.nLastPOWBlock = 500;
        consensus.nStakeTimestampMask = 0xf; // 15
        consensus.nCoinbaseMaturity = 100;

        // The best chain should have at least this much work.
        consensus.nMinimumChainWork = uint256S("0x0000000000000000000000000000000000000000000000155e140f294a1ba377");

        /**
         * The message start string is designed to be unlikely to occur in normal data.
         * The characters are rarely used upper ASCII, not valid as UTF-8, and produce
         * a large 32-bit integer with any alignment.
         */
        pchMessageStart[0] = 0xe7;
        pchMessageStart[1] = 0xe3;
        pchMessageStart[2] = 0xe2;
        pchMessageStart[3] = 0xe1;
        nDefaultPort = 31242;
        nPruneAfterHeight = 100000;

        genesis = CreateGenesisBlock(1696771811, 1010055, 0x1e0fffff, 1, 0);
        consensus.hashGenesisBlock = genesis.GetHash();
        assert(consensus.hashGenesisBlock == uint256S("0x00000fa4edca746cef5d3e903ca8691f3fa9602e6055e351a53be2e109f9a4e3"));
        assert(genesis.hashMerkleRoot == uint256S("0x9dd1cd781fbbf2739b0e430e956e808c9e7751aa4c5eeb84fb675af0c2ad14fa"));

        vSeeds.push_back(CDNSSeedData("jumpcoin.net", "seed1.jumpcoin.net"));
        vSeeds.push_back(CDNSSeedData("jumpcoin.net", "seed2.jumpcoin.net"));
        vSeeds.push_back(CDNSSeedData("jumpcoin.net", "seed3.jumpcoin.net"));
        vSeeds.push_back(CDNSSeedData("jumpcoin.net", "seed4.jumpcoin.net"));
        vSeeds.push_back(CDNSSeedData("jumpcoin.net", "seed5.jumpcoin.net"));
        vSeeds.push_back(CDNSSeedData("jumpcoin.net", "seed6.jumpcoin.net"));
        vSeeds.push_back(CDNSSeedData("jumpcoin.net", "seed7.jumpcoin.net"));
        vSeeds.push_back(CDNSSeedData("jumpcoin.net", "seed8.jumpcoin.net"));

        base58Prefixes[PUBKEY_ADDRESS] = std::vector<unsigned char>(1,43);
        base58Prefixes[SCRIPT_ADDRESS] = std::vector<unsigned char>(1,44);
        base58Prefixes[SECRET_KEY] = std::vector<unsigned char>(1,171);
        base58Prefixes[EXT_PUBLIC_KEY] = boost::assign::list_of(0x04)(0x88)(0xB2)(0x1E).convert_to_container<std::vector<unsigned char> >();
        base58Prefixes[EXT_SECRET_KEY] = boost::assign::list_of(0x04)(0x88)(0xAD)(0xE4).convert_to_container<std::vector<unsigned char> >();
        cashaddrPrefix = "jumpcoin";

        vFixedSeeds = std::vector<SeedSpec6>(pnSeed6_main, pnSeed6_main + ARRAYLEN(pnSeed6_main));

        fMiningRequiresPeers = false;
        fDefaultConsistencyChecks = false;
        fRequireStandard = true;
        fMineBlocksOnDemand = false;
        fTestnetToBeDeprecatedFieldRPC = false;

        checkpointData = (CCheckpointData) {
                    boost::assign::map_list_of
                    (      0, uint256S("0x00000fa4edca746cef5d3e903ca8691f3fa9602e6055e351a53be2e109f9a4e3"))
                    (    500, uint256S("0xb8cdca879671881ff5b30a873f6a5aec4fac38bd41d709a657a0f6dfa5089d09"))
                    (  10500, uint256S("0x6399e861fa66e0efe265c5399a7b97eda3fca2ab1a0c86c84e256dd6ba25e9dd"))
                    (  20500, uint256S("0xa243054f41aeacd867ab71f9f9487e2e7b16d48fe956ad6796770551677ebb28"))
                    (  30500, uint256S("0x02068d0104974f8423c4a72687a25a707b8c6acc9b0f89d9620157bec9cceca2"))
                    (  40500, uint256S("0x2235dcdc0edaeb9f6352d6155bcd7a55be2086fc015dfb11094c7a202b58631d"))
                    (  50500, uint256S("0x93e9652dd37c51406d69d76f7acf928791c1471896e7a85d8fd4657ed82071f0"))
                    (  60500, uint256S("0x11130b578a94c077d3f29e34d43ff7da89d2b4faa601ced14a54245a0e963ce8"))
                    (  70500, uint256S("0x3aa9cba5f007a0f37c317b4e164bc9d058eb8174a665295f6eca5d18e754797d"))
                    (  80500, uint256S("0x73e06d1a62b09fd8bfc26ebc8492945ced6e6fab547b5336921749438cb023d8"))
                    (  90500, uint256S("0xb99a6606888f5597d333b4d8b44b17053b417957a044a8fad308f8f0afcc5839"))
                    ( 100500, uint256S("0xf7d02af1f58da9d7a90be39bd17318484993b629c3926988fe5c243d20185431"))
                    ( 110500, uint256S("0xcd567d7411c98ba109ffc8ce566aa78c6eb26c04aaee44ce33f74def9b4cd2ca"))
                    ( 120500, uint256S("0x4d69d99e3e106f61c2192c7b83614f4bbccc13583f41eca21cb9c446dce0db87"))
                    ( 130500, uint256S("0x3aceaf77e5f3f75fb9ceabf6ebb9da3a711182800339504ab6c9b7b652a174cf"))
                    ( 140500, uint256S("0x62305ea41d86793b3040fbef79e73ccef2f1e80cdef349aad8599489ff9d7c3c"))
                    ( 150500, uint256S("0xbf1d124cd1820bcd40ab9d34648645e699858862af61c11853c416fb47d1952b"))
                    ( 160500, uint256S("0x5ecdf1db7e19edd3b497b573cfa7df3d4008ddcebe1c531ca854efdff60aaa91"))
                    ( 170500, uint256S("0x6d17e54dc1cf47a9c9f33862bedbe6098b7577f1269ed9a2767bac6d5cb7a841"))
                    ( 180500, uint256S("0x16c5f4be6e64263ad113de0ddfb59a4bb1bb1d791e767dd4e09cdbf94a143cb9"))
                    ( 190500, uint256S("0x095e6e0a25f6c0078e9a0bc7aa8b2b4a0becb745b8b9459d737671c0b4351cb3"))
                    ( 200500, uint256S("0x946dff09acc1c0bab6a9e0fcc48e65c9d953c16682b611d13c685cd5dd4318eb"))
                    ( 210500, uint256S("0x3457b8f13c2e589cc6792bc3c95fe729aa9913a189c0430d13278e12cca84d06"))
                    ( 220500, uint256S("0xf77c425c175decced84cce194544717a8c4f066923234d6ff43ae3d300e68e22"))
                    ( 230500, uint256S("0x08e2bbe92a3ca34cb91f937439d6d7f42daadf5da58f9d4b95f052bc4d07f094"))
                    ( 240500, uint256S("0x065ece21f24744ad5d4ca4cd8fe6a31a78f03649a0cf5b56e8a4d40dec24d12a"))
                    ( 250500, uint256S("0xa4ccedefd548d688d92598da02ef54a1144a9015a8e9c15f985d03653305db15"))
                    ( 260500, uint256S("0x7e38641db9bf55a1a0b5dc37316eee9e81a78a6731fbaad1cd2578f090b3f80f"))
                    ( 270500, uint256S("0xa7204631808007a88332f4761dce95aa6406e3b47db4f9cb9d1a0de280277a92"))
                    ( 280500, uint256S("0x993b74aaada04c65cb3d21ec4bfb99b26255f062d00be324609a0d6d01d99932"))
                    ( 290500, uint256S("0xbaf9ec823734695bb9515c9cc30e78c348e1115dcc1878facb9670c06f3b0235"))
                    ( 300500, uint256S("0x176dc9148322f02c06608a99467a31f71aa67ff0246969f5b0698f15ec219d60"))
                    ( 310500, uint256S("0x4b1f8fefdf819a512a8fc982f6b203a76f4cb4755d2ec9883234923f16ffea80"))
                    ( 320500, uint256S("0xa07990066d4f30601db6352482b4c61e42b93608ef6e1e954a6d012b3eeb8808"))
                    ( 330500, uint256S("0xf7bf0dfa77bd725e309b969e3e2ac5b67c369877368d0931318ded86379411cc"))
                    ( 340500, uint256S("0x30ced7647b2e2b9f7065573f5073530ac6b843f4f6da6849cfb9c97315bc6294"))
                    ( 350500, uint256S("0xa00240d6f530c3446b07c3e0a3bb7bdcc1f727ef13489e264b140e785789db27"))
                    ( 360500, uint256S("0xfa7a8fdfd205768c241c58831a39df4834d2ba144a4e69be24e1054b504bdc71"))
                    ( 370500, uint256S("0x11ec2f7625215a83b073a0cf19b5e5c366bf4272bec82344d5b6e1d91e9367cf"))
                    ( 380500, uint256S("0x5af8cff2b8a11fdcbb1b47369e538637c1f7d6b2a3f7fa7951d4e30f00f472ce"))
                    ( 390500, uint256S("0x270c83e3f48c74ae4c01d28e7ea9002957f030085c74783a54e81537fcd5eee8"))
                    ( 400500, uint256S("0x8a68f0e3e8faeee8dab05094c1f5445d57202f016f60ce85c0ccce04fda6e142"))
                    ( 410500, uint256S("0xd5d737d54bee4ee259054f39dc258ee269aefdacfb5c7e1d243fc4c84e9d7edf"))
                    ( 420500, uint256S("0xd9ff98b1263a53b2d207b40fa496676fdec2b157386c2ad6d46d6816649f8e8d"))
                    ( 430500, uint256S("0x54c3723e204325eeaf3fe300ba8401065c17efa7a76266f08f9509efc16c2a20"))
                    ( 440500, uint256S("0x69825f651e65e327249d4c57f1ed709dfbaf6d66a81c8c93d80108236e326836"))
                    ( 450500, uint256S("0xa9f7c783460033b822419c81372f4f1d21db9281ab6dbf358f507e5ba2cbcb26"))
                    ( 460500, uint256S("0x7fb27dc8facaada59ba9112641c9ed3a9611fe92736a698ab4434df21e1bad9c"))
                    ( 470500, uint256S("0x2f758ed068999bdb9c5422499c63c2643e7f4ba04ab4b9fa588ed7966a38c66d"))
                    ( 480500, uint256S("0xb505ba08835680275e4f46a8f0a50f02e70ad35fcb1138539a601b23b06ba953"))
                    ( 490500, uint256S("0x492a3f21d0c7a170f8ed9bd025e54cc256caf3b924ba7de3e94f0cd9a90414e6"))
                    ( 500500, uint256S("0x4f3068acda10ee8b05ab66b6b1482f5489c6ab27c64f71004fc55c4941fa205a"))
                    ( 510500, uint256S("0x87a119dccdbc79a85a55f4af78cf90d20f1582755a96ef58718887d79c806399"))
                    ( 520500, uint256S("0xc6ea7c03edc668feb21f5c6444a2eeb5b0151160ceabeb03afb9faaa7aa9ed32"))
                    ( 530500, uint256S("0x634988f4e5df14d49568dbc044ec73fd2f1b35ba4397b4f576bdf597040a4f94"))
                    ( 540500, uint256S("0xbd6f3fcd0b2f98bc9a3e71f5cb0af12fa8c9292cbca1aa94cd675c97115266f3"))
                    ( 550500, uint256S("0x08ccb48e2c69729c200c3592fe88f0d512be341aa63bba7080e14a12eb9872e8"))
                    ( 560500, uint256S("0x8b3893ca3c2ae885c29bc5ec9d7d2974672e7baf607ba7575ba0429065e03d89"))
                    ( 570500, uint256S("0x4d306d5f5de124ed6be29c6c096f995c82338671f7814495ad5c3b3d152fb37f"))
                    ( 580500, uint256S("0xd454a7369e4aaa31d5c997a8c02b46a4488d687fbb569c626475afc4d71cc611"))
                    ( 590500, uint256S("0x49085ad62963a8852e7ad7d5755dbd871bcfd54962f5664fac609534d9e8466b"))
                    ( 600500, uint256S("0x7393ea48b7c8da0fa7c4ab0b14b3651a0d3e2df0305253033867d2a271db9e54"))
                    ( 610500, uint256S("0x5a3d3d24b51432b2bd6febb962fe5ce547d9df2b60907278f0bf36efe3a50736"))
                    ( 620500, uint256S("0xd175e56f118f5135f88af815d44d7c5a5b272b5ac6d9210f63c5afa4703826d5"))
                    ( 630500, uint256S("0x210a46e51939f2b1d9be713563a7e89daa37adb585187339033c5080681fe816"))
                    ( 640500, uint256S("0x5ddd9768a106d73c979105c3bbdc917a9f29cca7293c06096e7bd13724e8ab8a"))
                    ( 650500, uint256S("0x0a2891fa864e3c1671984c4320335aa4fbb9febe8ea26a9fb556af22a897e869"))
                    ( 660500, uint256S("0xdeeea712263e09ea3314cdb4118605d5735423754c4acd10cd8f388371244941"))
                    ( 670500, uint256S("0xbacf321ce114c4d9fb32f31f9927f9fa3717634851a6a7807a6454249510036a"))
                    ( 680500, uint256S("0x559b25c6b402e98b793dc9574265fe76e545504e52a488441c34a8070b2af8c1"))
                    ( 690500, uint256S("0xeabe0dc5eadb3cf24123ebaee7048f287d9043e1fcfaeeda1d1bff74227c8557"))
                    ( 700500, uint256S("0xca685bc2fd7d1c8300ed9e71306d898f7ee643e685f3e4e9cc955e5f38e16be0"))
                    ( 710500, uint256S("0x0a20debcc63dd4b3273fc250bc3b51b196fa984d93c2d78f2a3beb338829832f"))
                    ( 720500, uint256S("0x4c56e4459b59f714ed70763a607f5fc15247dfc38b335e37b3b80804089f1b42"))
                    ( 730500, uint256S("0x176a5743262116ab8cc2d75273bd18019411d390636a3acd74f51a62965fd4d7"))
                    ( 740500, uint256S("0x5b92de922029cb5cb30b89e687cb853f247e29ef1bff0a593505715b501897fe"))
                    ( 750500, uint256S("0x164d77bb096136b65515c72971469e51dec39f9de2734868b1a07630de7b89a3"))
                    ( 760500, uint256S("0xc7260027a122505944462a55206940f2bc1b9b3df7c2d966c1e5e7951a0bd239"))
                    ( 770500, uint256S("0x8c1316f39bc9044da798faedc8f75d9c0d2fba80bc8dfb928f84f244665a5b02"))
                    ( 780500, uint256S("0x84f6eb9c208b9a34ca4109d30e57db57a067708dd5e7005ceff7ca3ab42b77c1"))
                    ( 790500, uint256S("0x3418ecf039e25cbaeb5efb3a588682ea88b543e02c8f8f304bb37558c14d6440"))
                    ( 800500, uint256S("0xa93990774f4e360ef9af1927a701dfcc0552364be959acac48cd9956740f37bf"))
                    ( 810500, uint256S("0x0e71daba1633c44e49bb70960970b08c5b171c41272195199f17b6dc18e47285"))
                    ( 820500, uint256S("0x0cad1d0f0985de72342b669b09f8459fedd0ec2eb3bd561e0af93496a2920ef6"))
                    ( 830500, uint256S("0x2974939501393fbf7dcc8a672b37e0885490ae0f91602660bdc51ef047f838e1"))
                    ( 840500, uint256S("0x2df69652ac17505994b0b20741b5d6fad89115573aadc3518a9526be3d8b0a52"))
                    ( 850500, uint256S("0xc2ca2ce8fe473c612a7a004e2c3a858b3e6607aa2219eb6af8687281df7ca9b2"))
                    ( 860500, uint256S("0x84ae01c53def11f6151ef86fa890a44c708d2f3009f7a2271d0319a6f0ba26fe"))
                    ( 870500, uint256S("0xb400df255f49033f0300de72e319118297e680049134e2d6b4c2c29de0be0cb9"))
                    ( 880500, uint256S("0x0e8e234c5445162cc62cb0eef8b5df6a74645f2446add7c0e67663c221681915"))
                    ( 890500, uint256S("0x2358b85012bee086f7f1b5b5c98346339158bc84d0d2a2c8e322745da0f56276"))
                    ( 900500, uint256S("0xa6567eb213a04171f9bc839858ce2bfe350897533a3ec67814752ae70ac977d8"))
                    ( 910500, uint256S("0xbb33e2e19df03675737987cf0f593f2e4db4a927bed54bc4f4e7f07fc8c261e2"))
                    ( 920500, uint256S("0xc1c15b151624d4d159bc599a927d16f81a73e7782a53ba30f8aa4a2773875dd4"))
                    ( 930500, uint256S("0x6bad46d6740055fe369abcc730e5ec19ca424ab321780218b1f657ece3e4799d"))
                    ( 940500, uint256S("0x866d329d14039fa9a525583989802e2ddf21f0328fb0a32d5431430098633b6e"))
                    ( 950500, uint256S("0x246da6ba7fe204a56cbdf7fb44dd3fdb7ccd0c115d9076836011e867ab10abf1"))
                    ( 960500, uint256S("0x38cc3775d04a6645a4028d94c2447d6a66694eb79eea5ad65e3dad7f7e43fec4"))
                    ( 970500, uint256S("0x4c71ca8918918ae2652f6005f749d250b251decb7df3dd30ffa36c263c66edad"))
                    ( 980500, uint256S("0x7dae1cd27c139c1bd879fa26d7646e4fb3f06ff5551156e03a4c749eb678e211"))
                    ( 990500, uint256S("0xe4515122c6903e4ce7e7360d9e44731f15593977445bd736c24b0960297658cd"))
                    (1000500, uint256S("0xeb6ab2970e87176c93ec62f99df61289d45f8fae5f6da61318b35bdfc7e78745"))
                    (1010500, uint256S("0xa0e8c384f758f5db65c8ca137525e31796719a891cede11f6a7c19bb8f47d23e"))
                    (1020500, uint256S("0xc7e8fcd1764bb68538a4bc2984995e03d9b0ebb466723e6fda09e604866b4c10")),
                    1780283600,
                    905363,
                    100.0
        };

        // A vector of p2sh addresses
        vDevFundAddress = { "Jg5zbGhHwBSZhm83Hju1em4EwHQv28QnFc" };
    }
};
static CMainParams mainParams;

/**
 * Testnet
 */
class CTestNetParams : public CChainParams {
public:
    CTestNetParams() {
        strNetworkID = "test";
        consensus.nMaxReorganizationDepth = 500;
        consensus.nMajorityEnforceBlockUpgrade = 750;
        consensus.nMajorityRejectBlockOutdated = 950;
        consensus.nMajorityWindow = 1000;
        consensus.powLimit = uint256S("0000ffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff");
        consensus.posLimit = uint256S("00000fffffffffffffffffffffffffffffffffffffffffffffffffffffffffff");
        consensus.posLimitV2 = uint256S("000000000000ffffffffffffffffffffffffffffffffffffffffffffffffffff");
        consensus.nTargetTimespan = 16 * 60; // 16 mins
        consensus.nTargetSpacingV1 = 60;
        consensus.nTargetSpacing = 64;
        consensus.BIP34Height = -1;
        consensus.BIP34Hash = uint256();
        consensus.fPowAllowMinDifficultyBlocks = true;
        consensus.fPowNoRetargeting = false;
        consensus.fPoSNoRetargeting = false;
        consensus.nRuleChangeActivationThreshold = 1512; // 75% for testchains
        consensus.nMinerConfirmationWindow = 2016; // nPowTargetTimespan / nPowTargetSpacing
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].bit = 28;
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].nStartTime = 1199145601; // January 1, 2008
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].nTimeout = 1230767999; // December 31, 2008

        consensus.nProtocolV1RetargetingFixedTime = 1696771811;
        consensus.nProtocolV2Time = 1696771812;
        consensus.nProtocolV3Time = 1696771813;
        consensus.nProtocolV3_1Time = 1667779200;
        consensus.nLastPOWBlock = 0x7fffffff;
        consensus.nStakeTimestampMask = 0xf;
        consensus.nCoinbaseMaturity = 10;

        pchMessageStart[0] = 0xcd;
        pchMessageStart[1] = 0xf2;
        pchMessageStart[2] = 0xc0;
        pchMessageStart[3] = 0xef;
        nDefaultPort = 25714;

        // The best chain should have at least this much work.
        consensus.nMinimumChainWork = uint256S("0x00000000000000000000000000000000000000000000003aaf405e01eda716e7");

        nPruneAfterHeight = 1000;

        genesis = CreateGenesisBlock(1696771811, 73218, 0x1f00ffff, 1, 0);
        consensus.hashGenesisBlock = genesis.GetHash();
        assert(consensus.hashGenesisBlock == uint256S("0x000054c3da7d354fe66c0c06631682051282f40f8ee0b0815c5e120766112f6a"));
        assert(genesis.hashMerkleRoot == uint256S("0x9dd1cd781fbbf2739b0e430e956e808c9e7751aa4c5eeb84fb675af0c2ad14fa"));

        vFixedSeeds.clear();
        vSeeds.clear();

        base58Prefixes[PUBKEY_ADDRESS] = std::vector<unsigned char>(1,111);
        base58Prefixes[SCRIPT_ADDRESS] = std::vector<unsigned char>(1,196);
        base58Prefixes[SECRET_KEY] =     std::vector<unsigned char>(1,239);
        base58Prefixes[EXT_PUBLIC_KEY] = boost::assign::list_of(0x04)(0x35)(0x87)(0xCF).convert_to_container<std::vector<unsigned char> >();
        base58Prefixes[EXT_SECRET_KEY] = boost::assign::list_of(0x04)(0x35)(0x83)(0x94).convert_to_container<std::vector<unsigned char> >();
        cashaddrPrefix = "jumptest";

        vFixedSeeds = std::vector<SeedSpec6>(pnSeed6_test, pnSeed6_test + ARRAYLEN(pnSeed6_test));

        fMiningRequiresPeers = true;
        fDefaultConsistencyChecks = false;
        fRequireStandard = true;
        fMineBlocksOnDemand = false;
        fTestnetToBeDeprecatedFieldRPC = true;

        checkpointData = (CCheckpointData) {
            boost::assign::map_list_of
            (  0, uint256S("0x000054c3da7d354fe66c0c06631682051282f40f8ee0b0815c5e120766112f6a")), //start devfund
            1685643568,
            0,
            2.0
        };

        // A vector of p2sh addresses
        vDevFundAddress = { "Jg5zbGhHwBSZhm83Hju1em4EwHQv28QnFc" };

    }
};
static CTestNetParams testNetParams;

/**
 * Regression test
 */
class CRegTestParams : public CChainParams {
public:
    CRegTestParams() {
        strNetworkID = "regtest";
        consensus.nMaxReorganizationDepth = 50;
        consensus.nMajorityEnforceBlockUpgrade = 51;
        consensus.nMajorityRejectBlockOutdated = 75;
        consensus.nMajorityWindow = 100;
        consensus.powLimit = uint256S("0000ffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff");
        consensus.posLimit = uint256S("00000fffffffffffffffffffffffffffffffffffffffffffffffffffffffffff");
        consensus.posLimitV2 = uint256S("000000000000ffffffffffffffffffffffffffffffffffffffffffffffffffff");
        consensus.nTargetTimespan = 16 * 60; // 16 mins
        consensus.nTargetSpacingV1 = 64;
        consensus.nTargetSpacing = 64;
        consensus.BIP34Height = -1; // BIP34 has not necessarily activated on regtest
        consensus.BIP34Hash = uint256();
        consensus.fPowAllowMinDifficultyBlocks = true;
        consensus.fPowNoRetargeting = true;
        consensus.fPoSNoRetargeting = true;
        consensus.nRuleChangeActivationThreshold = 108;// 75% for regtest
        consensus.nMinerConfirmationWindow = 144; // Faster than normal for regtest (144 instead of 2016)
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].bit = 28;
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].nStartTime = 1199145601; // January 1, 2008
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].nTimeout = 1230767999; // December 31, 2008

        // The best chain should have at least this much work.
        consensus.nMinimumChainWork = uint256S("0x00");

        consensus.nProtocolV1RetargetingFixedTime = 1395631999;
        consensus.nProtocolV2Time = 1407053625;
        consensus.nProtocolV3Time = 1444028400;
        consensus.nProtocolV3_1Time = 4102437600;
        consensus.nLastPOWBlock = 1000;
        consensus.nStakeTimestampMask = 0xf;
        consensus.nCoinbaseMaturity = 10;

        pchMessageStart[0] = 0x70;
        pchMessageStart[1] = 0x35;
        pchMessageStart[2] = 0x22;
        pchMessageStart[3] = 0x06;
        nDefaultPort = 35714;
        nPruneAfterHeight = 100000;

        genesis = CreateGenesisBlock(1393221600, 216178, 0x1f00ffff, 1, 0);
        consensus.hashGenesisBlock = genesis.GetHash();
        assert(consensus.hashGenesisBlock == uint256S("0x0000724595fb3b9609d441cbfb9577615c292abf07d996d3edabc48de843642d"));
        assert(genesis.hashMerkleRoot == uint256S("0x12630d16a97f24b287c8c2594dda5fb98c9e6c70fc61d44191931ea2aa08dc90"));

        vFixedSeeds.clear(); //!< Regtest mode doesn't have any fixed seeds.
        vSeeds.clear();      //!< Regtest mode doesn't have any DNS seeds.

        base58Prefixes[PUBKEY_ADDRESS] = std::vector<unsigned char>(1,111);
        base58Prefixes[SCRIPT_ADDRESS] = std::vector<unsigned char>(1,196);
        base58Prefixes[SECRET_KEY] = std::vector<unsigned char>(1,239);
        base58Prefixes[EXT_PUBLIC_KEY] = boost::assign::list_of(0x04)(0x88)(0xB2)(0x1E).convert_to_container<std::vector<unsigned char> >();
        base58Prefixes[EXT_SECRET_KEY] = boost::assign::list_of(0x04)(0x88)(0xAD)(0xE4).convert_to_container<std::vector<unsigned char> >();
        cashaddrPrefix = "jumpreg";

        fMiningRequiresPeers = false;
        fDefaultConsistencyChecks = true;
        fRequireStandard = false;
        fMineBlocksOnDemand = true;
        fTestnetToBeDeprecatedFieldRPC = false;

    }

    void UpdateBIP9Parameters(Consensus::DeploymentPos d, int64_t nStartTime, int64_t nTimeout)
    {
        consensus.vDeployments[d].nStartTime = nStartTime;
        consensus.vDeployments[d].nTimeout = nTimeout;
    }
};
static CRegTestParams regTestParams;

static CChainParams *pCurrentParams = 0;

const CChainParams &Params() {
    assert(pCurrentParams);
    return *pCurrentParams;
}

CChainParams& Params(const std::string& chain)
{
    if (chain == CBaseChainParams::MAIN)
            return mainParams;
    else if (chain == CBaseChainParams::TESTNET)
            return testNetParams;
    else if (chain == CBaseChainParams::REGTEST)
            return regTestParams;
    else
        throw std::runtime_error(strprintf("%s: Unknown chain %s.", __func__, chain));
}

void SelectParams(const std::string& network)
{
    SelectBaseParams(network);
    pCurrentParams = &Params(network);
}

void UpdateRegtestBIP9Parameters(Consensus::DeploymentPos d, int64_t nStartTime, int64_t nTimeout)
{
    regTestParams.UpdateBIP9Parameters(d, nStartTime, nTimeout);
}

// Jumpcoin: Donations to dev fund 
std::string CChainParams::GetDevFundAddress() const
{
    return !vDevFundAddress.empty() ? vDevFundAddress[0] : "";
}

CScript CChainParams::GetDevRewardScript() const
{
    CTxDestination dest = DecodeDestination(GetDevFundAddress());
    CScript scriptPubKey = GetScriptForDestination(dest);
    return scriptPubKey;
}
