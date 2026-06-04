// Copyright (c) 2014-2018 The BlackCoin Developers
// Copyright (c) 2011-2013 The PPCoin developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include "chain.h"
#include "chainparams.h"
#include "consensus/params.h"
#include "main.h"
#include "pos.h"
#include "primitives/transaction.h"
#include "uint256.h"

#include "test/test_bitcoin.h"

#include <boost/test/unit_test.hpp>

BOOST_FIXTURE_TEST_SUITE(pos_tests, TestingSetup)

BOOST_AUTO_TEST_CASE(coinstake_timestamp_pre_v2)
{
    SelectParams(CBaseChainParams::REGTEST);
    const Consensus::Params& params = Params().GetConsensus();

    int64_t nPreV2 = params.nProtocolV2Time - 1;
    BOOST_CHECK(params.IsProtocolV2(nPreV2) == false);
    BOOST_CHECK(CheckCoinStakeTimestamp(1000, 1000) == true);
    BOOST_CHECK(CheckCoinStakeTimestamp(1000, 999) == false);
    BOOST_CHECK(CheckCoinStakeTimestamp(1000, 1001) == false);
}

BOOST_AUTO_TEST_CASE(coinstake_timestamp_post_v2)
{
    SelectParams(CBaseChainParams::REGTEST);
    const Consensus::Params& params = Params().GetConsensus();

    int64_t nPostV2 = params.nProtocolV2Time + 1;
    BOOST_CHECK(params.IsProtocolV2(nPostV2) == true);
    BOOST_CHECK(CheckCoinStakeTimestamp(16, 16) == true);
    BOOST_CHECK(CheckCoinStakeTimestamp(32, 32) == true);
    BOOST_CHECK(CheckCoinStakeTimestamp(17, 17) == false);
    BOOST_CHECK(CheckCoinStakeTimestamp(16, 32) == false);
    BOOST_CHECK(CheckCoinStakeTimestamp(32, 16) == false);
}

BOOST_AUTO_TEST_CASE(coinstake_timestamp_post_v2_special_case)
{
    SelectParams(CBaseChainParams::REGTEST);
    int64_t nTime = 1407053678;
    BOOST_CHECK(Params().GetConsensus().IsProtocolV2(nTime) == false);
}

BOOST_AUTO_TEST_CASE(stake_block_timestamp)
{
    SelectParams(CBaseChainParams::REGTEST);
    const Consensus::Params& params = Params().GetConsensus();

    int64_t nPostV2 = params.nProtocolV2Time + 1;
    BOOST_CHECK(params.IsProtocolV2(nPostV2) == true);
    BOOST_CHECK(CheckStakeBlockTimestamp(16) == true);
    BOOST_CHECK(CheckStakeBlockTimestamp(32) == true);
    BOOST_CHECK(CheckStakeBlockTimestamp(17) == false);
    BOOST_CHECK(CheckStakeBlockTimestamp(20) == false);
}

BOOST_AUTO_TEST_CASE(compute_stake_modifier_genesis)
{
    BOOST_CHECK(ComputeStakeModifier(NULL, uint256()) == uint256());
}

BOOST_AUTO_TEST_CASE(compute_stake_modifier_nonzero)
{
    CBlockIndex pindexPrev;
    pindexPrev.nStakeModifier = uint256S("0xdeadbeefdeadbeefdeadbeefdeadbeefdeadbeefdeadbeefdeadbeefdeadbeef");
    uint256 kernel = uint256S("0xcafebabecafebabecafebabecafebabecafebabecafebabecafebabecafebabe");
    uint256 mod = ComputeStakeModifier(&pindexPrev, kernel);
    BOOST_CHECK(mod != uint256());
    BOOST_CHECK(mod != pindexPrev.nStakeModifier);
    BOOST_CHECK(mod != kernel);
}

BOOST_AUTO_TEST_CASE(consensus_nCoinbaseMaturity_regtest)
{
    SelectParams(CBaseChainParams::REGTEST);
    const Consensus::Params& params = Params().GetConsensus();
    BOOST_CHECK_EQUAL(params.nCoinbaseMaturity, 10);
    BOOST_CHECK(params.nStakeTimestampMask == 0xf);
}

BOOST_AUTO_TEST_CASE(consensus_nCoinbaseMaturity_main)
{
    SelectParams(CBaseChainParams::MAIN);
    const Consensus::Params& params = Params().GetConsensus();
    BOOST_CHECK_EQUAL(params.nCoinbaseMaturity, 100);
    BOOST_CHECK(params.nStakeTimestampMask == 0xf);
}

BOOST_AUTO_TEST_CASE(consensus_nMaxReorganizationDepth)
{
    SelectParams(CBaseChainParams::MAIN);
    BOOST_CHECK_EQUAL(Params().GetConsensus().nMaxReorganizationDepth, 500);

    SelectParams(CBaseChainParams::TESTNET);
    BOOST_CHECK_EQUAL(Params().GetConsensus().nMaxReorganizationDepth, 500);

    SelectParams(CBaseChainParams::REGTEST);
    BOOST_CHECK_EQUAL(Params().GetConsensus().nMaxReorganizationDepth, 50);
}

BOOST_AUTO_TEST_CASE(consensus_nLastPOWBlock)
{
    SelectParams(CBaseChainParams::MAIN);
    BOOST_CHECK_EQUAL(Params().GetConsensus().nLastPOWBlock, 500);

    SelectParams(CBaseChainParams::REGTEST);
    BOOST_CHECK_EQUAL(Params().GetConsensus().nLastPOWBlock, 1000);
}

BOOST_AUTO_TEST_CASE(dev_fund_address_main)
{
    SelectParams(CBaseChainParams::MAIN);
    BOOST_CHECK(!Params().GetDevFundAddress().empty());
    BOOST_CHECK_EQUAL(Params().GetDevFundAddress(), "Jg5zbGhHwBSZhm83Hju1em4EwHQv28QnFc");
}

BOOST_AUTO_TEST_CASE(dev_fund_address_testnet)
{
    SelectParams(CBaseChainParams::TESTNET);
    BOOST_CHECK(!Params().GetDevFundAddress().empty());
    BOOST_CHECK_EQUAL(Params().GetDevFundAddress(), "Jg5zbGhHwBSZhm83Hju1em4EwHQv28QnFc");
}

BOOST_AUTO_TEST_CASE(dev_fund_address_regtest)
{
    SelectParams(CBaseChainParams::REGTEST);
    BOOST_CHECK(Params().GetDevFundAddress().empty());
}

BOOST_AUTO_TEST_CASE(protocol_v1_2_3_consistency)
{
    SelectParams(CBaseChainParams::MAIN);
    const Consensus::Params& params = Params().GetConsensus();

    BOOST_CHECK(params.nProtocolV1RetargetingFixedTime < params.nProtocolV2Time);
    BOOST_CHECK(params.nProtocolV2Time < params.nProtocolV3Time);

    BOOST_CHECK(params.IsProtocolV1RetargetingFixed(0) == false);
    BOOST_CHECK(params.IsProtocolV1RetargetingFixed(params.nProtocolV1RetargetingFixedTime + 1) == true);

    BOOST_CHECK(params.IsProtocolV2(0) == false);
    BOOST_CHECK(params.IsProtocolV2(params.nProtocolV2Time + 1) == true);

    BOOST_CHECK(params.IsProtocolV3(0) == false);
    BOOST_CHECK(params.IsProtocolV3(params.nProtocolV3Time + 1) == true);
}

BOOST_AUTO_TEST_CASE(protocol_v3_1_mainnet_future)
{
    SelectParams(CBaseChainParams::MAIN);
    const Consensus::Params& params = Params().GetConsensus();
    BOOST_CHECK(params.nProtocolV3_1Time > 2000000000);
    BOOST_CHECK(params.IsProtocolV3_1(GetTime()) == false);
}

BOOST_AUTO_TEST_CASE(reward_two_jump)
{
    BOOST_CHECK_EQUAL(GetProofOfStakeSubsidy(), 2 * COIN);
}

BOOST_AUTO_TEST_CASE(verify_signature_rejects_mismatched_tx)
{
    CMutableTransaction txFrom;
    txFrom.vout.resize(1);
    txFrom.vout[0].scriptPubKey = CScript() << OP_TRUE;
    txFrom.vout[0].nValue = 1000;

    CMutableTransaction txTo;
    txTo.vin.resize(1);
    txTo.vin[0].prevout.hash = txFrom.GetHash();
    txTo.vin[0].prevout.n = 0;
    txTo.vout.resize(1);

    CMutableTransaction txWrong;
    txWrong.vout.resize(1);
    txWrong.vout[0].scriptPubKey = CScript() << OP_TRUE;
    txWrong.vout[0].nValue = 999;
    txWrong.vin.resize(1);
    txWrong.vin[0].prevout.hash = txFrom.GetHash();
    txWrong.vin[0].prevout.n = 0;

    BOOST_CHECK(VerifySignature(CTransaction(txFrom), CTransaction(txTo), 0, SCRIPT_VERIFY_NONE, 0) == true);
    BOOST_CHECK(VerifySignature(CTransaction(txWrong), CTransaction(txTo), 0, SCRIPT_VERIFY_NONE, 0) == false);
}

BOOST_AUTO_TEST_CASE(verify_signature_rejects_out_of_range_nIn)
{
    CMutableTransaction txFrom;
    txFrom.vout.resize(1);
    txFrom.vout[0].scriptPubKey = CScript() << OP_TRUE;
    txFrom.vout[0].nValue = 1000;

    CMutableTransaction txTo;
    txTo.vin.resize(0);

    BOOST_CHECK(VerifySignature(CTransaction(txFrom), CTransaction(txTo), 5, SCRIPT_VERIFY_NONE, 0) == false);
}

BOOST_AUTO_TEST_CASE(verify_signature_rejects_bad_prevout_n)
{
    CMutableTransaction txFrom;
    txFrom.vout.resize(1);
    txFrom.vout[0].scriptPubKey = CScript() << OP_TRUE;
    txFrom.vout[0].nValue = 1000;

    CMutableTransaction txTo;
    txTo.vin.resize(1);
    txTo.vin[0].prevout.hash = txFrom.GetHash();
    txTo.vin[0].prevout.n = 5;
    txTo.vout.resize(1);

    BOOST_CHECK(VerifySignature(CTransaction(txFrom), CTransaction(txTo), 0, SCRIPT_VERIFY_NONE, 0) == false);
}

BOOST_AUTO_TEST_SUITE_END()
