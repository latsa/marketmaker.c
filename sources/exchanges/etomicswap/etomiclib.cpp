//
// Created by artem on 24.01.18.
//
#include "etomiclib.h"
#include "etomiccurl.h"

#include <iostream>
#include <regex>
#include <cpp-ethereum/libethcore/Common.h>
#include <cpp-ethereum/libethcore/CommonJS.h>
#include <cpp-ethereum/libethcore/TransactionBase.h>
#include <inttypes.h>

using namespace dev;
using namespace dev::eth;

char *stringStreamToChar(std::stringstream& ss)
{
    const std::string tmp = ss.str();
    auto result = (char*)malloc(strlen(tmp.c_str()) + 1);
    strcpy(result, tmp.c_str());
    return result;
}

TransactionSkeleton txDataToSkeleton(BasicTxData txData)
{
    TransactionSkeleton tx;
    tx.from = jsToAddress(txData.from);
    tx.to = jsToAddress(txData.to);
    tx.value = jsToU256(txData.amount);
    tx.gas = 200000;
    tx.gasPrice = getGasPriceFromStation(1) * boost::multiprecision::pow(u256(10), 9);
    tx.nonce = getNonce(txData.from);
    return tx;
}

char *signTx(TransactionSkeleton& tx, char* secret)
{
    Secret secretKey(secret);
    TransactionBase baseTx(tx, secretKey);
    RLPStream rlpStream;
    baseTx.streamRLP(rlpStream);
    std::stringstream ss;
    ss << rlpStream.out();
    return stringStreamToChar(ss);
}

char *approveErc20(ApproveErc20Input input)
{
    TransactionSkeleton tx;
    tx.from = jsToAddress(input.owner);
    tx.to = jsToAddress(input.tokenAddress);
    tx.value = 0;
    tx.gas = 300000;
    tx.gasPrice = getGasPriceFromStation(1) * boost::multiprecision::pow(u256(10), 9);
    tx.nonce = getNonce(input.owner);
    std::stringstream ss;
    ss << "0x095ea7b3"
       << "000000000000000000000000"
       << toHex(jsToAddress(input.spender))
       << toHex(toBigEndian(jsToU256(input.amount)));
    tx.data = jsToBytes(ss.str());
    char* rawTx = signTx(tx, input.secret);
    char* result = sendRawTxWaitConfirm(rawTx);
    free(rawTx);
    return result;
}

std::stringstream aliceSendsEthPaymentData(AliceSendsEthPaymentInput input)
{
    std::stringstream ss;
    ss << "0x47c7b6e2"
       << toHex(jsToBytes(input.dealId))
       << "000000000000000000000000"
       << toHex(jsToAddress(input.bobAddress))
       << toHex(jsToBytes(input.aliceHash))
       << "000000000000000000000000"
       << toHex(jsToBytes(input.bobHash))
       << "000000000000000000000000";
    return ss;
}

char* aliceSendsEthPayment(AliceSendsEthPaymentInput input, BasicTxData txData)
{
    TransactionSkeleton tx = txDataToSkeleton(txData);
    std::stringstream ss = aliceSendsEthPaymentData(input);
    tx.data = jsToBytes(ss.str());
    char *rawTx = signTx(tx, txData.secretKey);
    char *result = sendRawTxWaitConfirm(rawTx);
    free(rawTx);
    return result;
}

uint8_t verifyAliceEthPaymentData(AliceSendsEthPaymentInput input, char *data)
{
    std::stringstream ss = aliceSendsEthPaymentData(input);
    if (strcmp(ss.str().c_str(), data) != 0) {
        printf("Alice ETH payment data %s does not match expected %s\n", data, ss.str().c_str());
        return 0;
    }
    return 1;
}

std::stringstream aliceSendsErc20PaymentData(AliceSendsErc20PaymentInput input)
{
    uint8_t decimals;
    if (input.decimals > 0) {
        decimals = input.decimals;
    } else {
        decimals = getErc20Decimals(input.tokenAddress);
    }
    u256 amount = jsToU256(input.amount);
    if (decimals < 18) {
        amount /= boost::multiprecision::pow(u256(10), 18 - decimals);
    }
    std::stringstream ss;
    ss << "0x184db3bf"
       << toHex(jsToBytes(input.dealId))
       << toHex(toBigEndian(amount))
       << "000000000000000000000000"
       << toHex(jsToAddress(input.bobAddress))
       << toHex(jsToBytes(input.aliceHash))
       << "000000000000000000000000"
       << toHex(jsToBytes(input.bobHash))
       << "000000000000000000000000"
       << "000000000000000000000000"
       << toHex(jsToAddress(input.tokenAddress));
    return ss;
}

char* aliceSendsErc20Payment(AliceSendsErc20PaymentInput input, BasicTxData txData)
{
    TransactionSkeleton tx = txDataToSkeleton(txData);
    std::stringstream ss = aliceSendsErc20PaymentData(input);
    tx.data = jsToBytes(ss.str());
    char* rawTx = signTx(tx, txData.secretKey);
    char* result = sendRawTxWaitConfirm(rawTx);
    free(rawTx);
    return result;
}

uint8_t verifyAliceErc20PaymentData(AliceSendsErc20PaymentInput input, char *data)
{
    std::stringstream ss = aliceSendsErc20PaymentData(input);
    if (strcmp(ss.str().c_str(), data) != 0) {
        printf("Alice ERC20 payment data %s is not equal to expected %s\n", data, ss.str().c_str());
        return 0;
    }
    return 1;
}

char* aliceReclaimsAlicePayment(AliceReclaimsAlicePaymentInput input, BasicTxData txData)
{
    TransactionSkeleton tx = txDataToSkeleton(txData);
    std::stringstream ss;
    u256 amount = jsToU256(input.amount);
    dev::Address tokenAddress = jsToAddress(input.tokenAddress);

    if (tokenAddress != ZeroAddress) {
        uint8_t decimals;
        if (input.decimals > 0) {
            decimals = input.decimals;
        } else {
            decimals = getErc20Decimals(input.tokenAddress);
        }

        if (decimals < 18) {
            amount /= boost::multiprecision::pow(u256(10), 18 - decimals);
        }
    }

    ss << "0x8b9a167a"
       << toHex(jsToBytes(input.dealId))
       << toHex(toBigEndian(amount))
       << "000000000000000000000000"
       << toHex(tokenAddress)
       << "000000000000000000000000"
       << toHex(jsToAddress(input.bobAddress))
       << toHex(jsToBytes(input.aliceHash))
       << "000000000000000000000000"
       << "00000000000000000000000000000000000000000000000000000000000000c0"
       << "0000000000000000000000000000000000000000000000000000000000000020"
       << toHex(jsToBytes(input.bobSecret));
    tx.data = jsToBytes(ss.str());
    char* rawTx = signTx(tx, txData.secretKey);
    char* result = sendRawTxWaitConfirm(rawTx);
    free(rawTx);
    return result;
}

char* bobSpendsAlicePayment(BobSpendsAlicePaymentInput input, BasicTxData txData)
{
    TransactionSkeleton tx = txDataToSkeleton(txData);
    std::stringstream ss;
    u256 amount = jsToU256(input.amount);
    dev::Address tokenAddress = jsToAddress(input.tokenAddress);

    if (tokenAddress != ZeroAddress) {
        uint8_t decimals;
        if (input.decimals > 0) {
            decimals = input.decimals;
        } else {
            decimals = getErc20Decimals(input.tokenAddress);
        }

        if (decimals < 18) {
            amount /= boost::multiprecision::pow(u256(10), 18 - decimals);
        }
    }

    ss << "0x392ec66b"
       << toHex(jsToBytes(input.dealId))
       << toHex(toBigEndian(amount))
       << "000000000000000000000000"
       << toHex(tokenAddress)
       << "000000000000000000000000"
       << toHex(jsToAddress(input.aliceAddress))
       << toHex(jsToBytes(input.bobHash))
       << "000000000000000000000000"
       << "00000000000000000000000000000000000000000000000000000000000000c0"
       << "0000000000000000000000000000000000000000000000000000000000000020"
       << toHex(jsToBytes(input.aliceSecret));
    tx.data = jsToBytes(ss.str());
    char* rawTx = signTx(tx, txData.secretKey);
    char* result = sendRawTxWaitConfirm(rawTx);
    free(rawTx);
    return result;
}

std::stringstream bobSendsEthDepositData(BobSendsEthDepositInput input)
{
    u256 lockTime = input.lockTime;
    std::stringstream ss;
    ss << "0xdd23795f"
       << toHex(jsToBytes(input.depositId))
       << "000000000000000000000000"
       << toHex(jsToAddress(input.aliceAddress))
       << toHex(jsToBytes(input.bobHash))
       << "000000000000000000000000"
       << toHex(toBigEndian(lockTime));
    return ss;
}

char* bobSendsEthDeposit(BobSendsEthDepositInput input, BasicTxData txData)
{
    TransactionSkeleton tx = txDataToSkeleton(txData);
    std::stringstream ss = bobSendsEthDepositData(input);
    tx.data = jsToBytes(ss.str());
    char* rawTx = signTx(tx, txData.secretKey);
    char* result = sendRawTx(rawTx);
    free(rawTx);
    return result;
}

uint8_t verifyBobEthDepositData(BobSendsEthDepositInput input, char *data)
{
    std::stringstream ss = bobSendsEthDepositData(input);
    if (strcmp(ss.str().c_str(), data) != 0) {
        printf("Bob deposit data %s != expected %s\n", data, ss.str().c_str());
        return 0;
    }
    return 1;
}

std::stringstream bobSendsErc20DepositData(BobSendsErc20DepositInput input)
{
    uint8_t decimals;
    if (input.decimals > 0) {
        decimals = input.decimals;
    } else {
        decimals = getErc20Decimals(input.tokenAddress);
    }

    u256 amount = jsToU256(input.amount);
    u256 lockTime = input.lockTime;
    if (decimals < 18) {
        amount /= boost::multiprecision::pow(u256(10), 18 - decimals);
    }
    std::stringstream ss;
    ss << "0x5d567259"
       << toHex(jsToBytes(input.depositId))
       << toHex(toBigEndian(amount))
       << "000000000000000000000000"
       << toHex(jsToAddress(input.aliceAddress))
       << toHex(jsToBytes(input.bobHash))
       << "000000000000000000000000"
       << "000000000000000000000000"
       << toHex(jsToAddress(input.tokenAddress))
       << toHex(toBigEndian(lockTime));
    return ss;
}

char* bobSendsErc20Deposit(BobSendsErc20DepositInput input, BasicTxData txData)
{
    TransactionSkeleton tx = txDataToSkeleton(txData);
    std::stringstream ss = bobSendsErc20DepositData(input);
    tx.data = jsToBytes(ss.str());
    char* rawTx = signTx(tx, txData.secretKey);
    char* result = sendRawTxWaitConfirm(rawTx);
    free(rawTx);
    return result;
}

uint8_t verifyBobErc20DepositData(BobSendsErc20DepositInput input, char *data)
{
    std::stringstream ss = bobSendsErc20DepositData(input);
    if (strcmp(ss.str().c_str(), data) != 0) {
        printf("Bob deposit data %s != expected %s\n", data, ss.str().c_str());
        return 0;
    }
    return 1;
}

char* bobRefundsDeposit(BobRefundsDepositInput input, BasicTxData txData)
{
    TransactionSkeleton tx = txDataToSkeleton(txData);
    std::stringstream ss;
    u256 amount = jsToU256(input.amount);
    dev::Address tokenAddress = jsToAddress(input.tokenAddress);
    if (tokenAddress != ZeroAddress) {
        uint8_t decimals;
        if (input.decimals > 0) {
            decimals = input.decimals;
        } else {
            decimals = getErc20Decimals(input.tokenAddress);
        }

        if (decimals < 18) {
            amount /= boost::multiprecision::pow(u256(10), 18 - decimals);
        }
    }
    ss << "0x1f7a72f7"
       << toHex(jsToBytes(input.depositId))
       << toHex(toBigEndian(amount))
       << toHex(jsToBytes(input.bobSecret))
       << "000000000000000000000000"
       << toHex(jsToAddress(input.aliceAddress))
       << "000000000000000000000000"
       << toHex(tokenAddress);
    tx.data = jsToBytes(ss.str());
    char* rawTx = signTx(tx, txData.secretKey);
    char* result = sendRawTxWaitConfirm(rawTx);
    free(rawTx);
    return result;
}

char* aliceClaimsBobDeposit(AliceClaimsBobDepositInput input, BasicTxData txData)
{
    TransactionSkeleton tx = txDataToSkeleton(txData);
    std::stringstream ss;
    u256 amount = jsToU256(input.amount);
    dev::Address tokenAddress = jsToAddress(input.tokenAddress);
    if (tokenAddress != ZeroAddress) {
        uint8_t decimals;
        if (input.decimals > 0) {
            decimals = input.decimals;
        } else {
            decimals = getErc20Decimals(input.tokenAddress);
        }

        if (decimals < 18) {
            amount /= boost::multiprecision::pow(u256(10), 18 - decimals);
        }
    }
    ss << "0x4b915a68"
       << toHex(jsToBytes(input.depositId))
       << toHex(toBigEndian(amount))
       << "000000000000000000000000"
       << toHex(jsToAddress(input.bobAddress))
       << "000000000000000000000000"
       << toHex(tokenAddress)
       << toHex(jsToBytes(input.bobHash))
       << "000000000000000000000000";
    tx.data = jsToBytes(ss.str());
    char* rawTx = signTx(tx, txData.secretKey);
    char* result = sendRawTxWaitConfirm(rawTx);
    free(rawTx);
    return result;
}

std::stringstream bobSendsEthPaymentData(BobSendsEthPaymentInput input)
{
    u256 lockTime = input.lockTime;
    std::stringstream ss;
    ss << "0x5ab30d95"
       << toHex(jsToBytes(input.paymentId))
       << "000000000000000000000000"
       << toHex(jsToAddress(input.aliceAddress))
       << toHex(jsToBytes(input.aliceHash))
       << "000000000000000000000000"
       << toHex(toBigEndian(lockTime));
    return ss;
}

char* bobSendsEthPayment(BobSendsEthPaymentInput input, BasicTxData txData)
{
    TransactionSkeleton tx = txDataToSkeleton(txData);
    std::stringstream ss = bobSendsEthPaymentData(input);
    tx.data = jsToBytes(ss.str());
    char* rawTx = signTx(tx, txData.secretKey);
    char* result = sendRawTxWaitConfirm(rawTx);
    free(rawTx);
    return result;
}

uint8_t verifyBobEthPaymentData(BobSendsEthPaymentInput input, char *data)
{
    std::stringstream ss = bobSendsEthPaymentData(input);
    if (strcmp(ss.str().c_str(), data) != 0) {
        printf("Bob payment data %s != expected %s\n", data, ss.str().c_str());
        return 0;
    }
    return 1;
}

std::stringstream bobSendsErc20PaymentData(BobSendsErc20PaymentInput input)
{
    u256 amount = jsToU256(input.amount);
    u256 lockTime = input.lockTime;
    uint8_t decimals;
    if (input.decimals > 0) {
        decimals = input.decimals;
    } else {
        decimals = getErc20Decimals(input.tokenAddress);
    }

    if (decimals < 18) {
        amount /= boost::multiprecision::pow(u256(10), 18 - decimals);
    }
    std::stringstream ss;
    ss << "0xb8a15b1d"
       << toHex(jsToBytes(input.paymentId))
       << toHex(toBigEndian(amount))
       << "000000000000000000000000"
       << toHex(jsToAddress(input.aliceAddress))
       << toHex(jsToBytes(input.aliceHash))
       << "000000000000000000000000"
       << "000000000000000000000000"
       << toHex(jsToAddress(input.tokenAddress))
       << toHex(toBigEndian(lockTime));
    return ss;
}

char* bobSendsErc20Payment(BobSendsErc20PaymentInput input, BasicTxData txData)
{
    TransactionSkeleton tx = txDataToSkeleton(txData);
    std::stringstream ss = bobSendsErc20PaymentData(input);
    tx.data = jsToBytes(ss.str());
    char* rawTx = signTx(tx, txData.secretKey);
    char* result = sendRawTxWaitConfirm(rawTx);
    free(rawTx);
    return result;
}

uint8_t verifyBobErc20PaymentData(BobSendsErc20PaymentInput input, char *data)
{
    std::stringstream ss = bobSendsErc20PaymentData(input);
    if (strcmp(ss.str().c_str(), data) != 0) {
        printf("Bob payment data %s != expected %s\n", data, ss.str().c_str());
        return 0;
    }
    return 1;
}

char* bobReclaimsBobPayment(BobReclaimsBobPaymentInput input, BasicTxData txData)
{
    TransactionSkeleton tx = txDataToSkeleton(txData);
    std::stringstream ss;
    u256 amount = jsToU256(input.amount);
    dev::Address tokenAddress = jsToAddress(input.tokenAddress);
    if (tokenAddress != ZeroAddress) {
        uint8_t decimals;
        if (input.decimals > 0) {
            decimals = input.decimals;
        } else {
            decimals = getErc20Decimals(input.tokenAddress);
        }

        if (decimals < 18) {
            amount /= boost::multiprecision::pow(u256(10), 18 - decimals);
        }
    }
    ss << "0xe45ef4ad"
       << toHex(jsToBytes(input.paymentId))
       << toHex(toBigEndian(amount))
       << "000000000000000000000000"
       << toHex(jsToAddress(input.aliceAddress))
       << "000000000000000000000000"
       << toHex(tokenAddress)
       << toHex(jsToBytes(input.aliceHash))
       << "000000000000000000000000";
    tx.data = jsToBytes(ss.str());
    char* rawTx = signTx(tx, txData.secretKey);
    char* result = sendRawTxWaitConfirm(rawTx);
    free(rawTx);
    return result;
}

char* aliceSpendsBobPayment(AliceSpendsBobPaymentInput input, BasicTxData txData)
{
    TransactionSkeleton tx = txDataToSkeleton(txData);
    std::stringstream ss;
    u256 amount = jsToU256(input.amount);
    dev::Address tokenAddress = jsToAddress(input.tokenAddress);
    if (tokenAddress != ZeroAddress) {
        uint8_t decimals;
        if (input.decimals > 0) {
            decimals = input.decimals;
        } else {
            decimals = getErc20Decimals(input.tokenAddress);
        }

        if (decimals < 18) {
            amount /= boost::multiprecision::pow(u256(10), 18 - decimals);
        }
    }
    ss << "0x113ee583"
       << toHex(jsToBytes(input.paymentId))
       << toHex(toBigEndian(amount))
       << toHex(jsToBytes(input.aliceSecret))
       << "000000000000000000000000"
       << toHex(jsToAddress(input.bobAddress))
       << "000000000000000000000000"
       << toHex(tokenAddress);
    tx.data = jsToBytes(ss.str());
    char* rawTx = signTx(tx, txData.secretKey);
    char* result = sendRawTxWaitConfirm(rawTx);
    free(rawTx);
    return result;
}

char* privKey2Addr(char* privKey)
{
    Secret secretKey(privKey);
    std::stringstream ss;
    ss << "0x" << toAddress(secretKey);
    return stringStreamToChar(ss);
};

char* pubKey2Addr(char* pubKey)
{
    Public publicKey(pubKey);
    std::stringstream ss;
    ss << "0x" << toAddress(publicKey);
    return stringStreamToChar(ss);
};

char* getPubKeyFromPriv(char *privKey)
{
    Public publicKey = toPublic(Secret(privKey));
    std::stringstream ss;
    ss << "0x" << publicKey;
    return stringStreamToChar(ss);
}

uint64_t getEthBalance(char *address, int *error)
{
    char* hexBalance = getEthBalanceRequest(address);
    if (hexBalance != NULL) {
        // convert wei to satoshi
        u256 balance = jsToU256(hexBalance) / boost::multiprecision::pow(u256(10), 10);
        free(hexBalance);
        return static_cast<uint64_t>(balance);
    } else {
        if (error) *error = 1;
        return 0;
    }
}

uint64_t getErc20BalanceSatoshi(char *address, char *tokenAddress, uint8_t setDecimals, int *error)
{
    std::stringstream ss;
    ss << "0x70a08231"
       << "000000000000000000000000"
       << toHex(jsToAddress(address));
    char* hexBalance = ethCall(tokenAddress, ss.str().c_str());
    // convert wei to satoshi
    uint8_t decimals;
    if (hexBalance != NULL) {
        if (setDecimals > 0) {
            decimals = setDecimals;
        } else {
            decimals = getErc20Decimals(tokenAddress);
        }

        u256 balance = jsToU256(hexBalance);
        if (decimals < 18) {
            balance *= boost::multiprecision::pow(u256(10), 18 - decimals);
        }
        balance /= boost::multiprecision::pow(u256(10), 10);
        free(hexBalance);
        return static_cast<uint64_t>(balance);
    } else {
        if (error) *error = 1;
        return 0;
    }
}

char *getErc20BalanceHexWei(char *address, char *tokenAddress)
{
    std::stringstream ss;
    ss << "0x70a08231"
       << "000000000000000000000000"
       << toHex(jsToAddress(address));
    char *hexBalance = ethCall(tokenAddress, ss.str().c_str());
    return hexBalance;
}

uint64_t getErc20Allowance(char *owner, char *spender, char *tokenAddress, uint8_t set_decimals)
{
    std::stringstream ss;
    ss << "0xdd62ed3e"
       << "000000000000000000000000"
       << toHex(jsToAddress(owner))
       << "000000000000000000000000"
       << toHex(jsToAddress(spender));
    char* hexAllowance = ethCall(tokenAddress, ss.str().c_str());
    uint8_t decimals;
    if (set_decimals > 0) {
        decimals = set_decimals;
    } else {
        decimals = getErc20Decimals(tokenAddress);
    }
    u256 allowance = jsToU256(hexAllowance);
    if (decimals < 18) {
        allowance *= boost::multiprecision::pow(u256(10), 18 - decimals);
    }
    // convert wei to satoshi
    allowance /= boost::multiprecision::pow(u256(10), 10);
    free(hexAllowance);
    return static_cast<uint64_t>(allowance);
}

uint8_t getErc20Decimals(char *tokenAddress)
{
    char* hexDecimals = ethCall(tokenAddress, "0x313ce567");
    auto decimals = (uint8_t) strtol(hexDecimals, NULL, 0);
    free(hexDecimals);
    return decimals;
}

uint8_t getErc20DecimalsZeroOnError(char *tokenAddress)
{
    char* hexDecimals = ethCall(tokenAddress, "0x313ce567");
    if (hexDecimals != NULL) {
        auto decimals = (uint8_t) strtol(hexDecimals, NULL, 0);
        free(hexDecimals);
        return decimals;
    } else {
        return 0;
    }
}

void uint8arrayToHex(char *dest, uint8_t *input, int len)
{
    strcpy(dest, "0x");
    for (int i = 0; i < len; i++)
    {
        sprintf(dest + (i + 1) * 2, "%02x", input[i]);
    }
    dest[(len + 1) * 2] = '\0';
}

void satoshisToWei(char *dest, uint64_t input)
{
    sprintf(dest, "%" PRIu64, input);
    strcat(dest, "0000000000");
}

uint64_t weiToSatoshi(char *wei)
{
    u256 satoshi = jsToU256(wei) / boost::multiprecision::pow(u256(10), 10);
    return static_cast<uint64_t>(satoshi);
}

char *sendEth(char *to, char *amount, char *privKey, uint8_t waitConfirm, int64_t gas, int64_t gasPrice, uint8_t defaultGasOnErr)
{
    TransactionSkeleton tx;
    char *from = privKey2Addr(privKey), *result;
    tx.from = jsToAddress(from);
    tx.to = jsToAddress(to);
    tx.value = jsToU256(amount);
    tx.nonce = getNonce(from);
    free(from);
    if (gas > 0) {
        tx.gas = gas;
    } else {
        tx.gas = 21000;
    }
    if (gasPrice > 0) {
        tx.gasPrice = gasPrice * boost::multiprecision::pow(u256(10), 9);
    } else {
        tx.gasPrice = getGasPriceFromStation(defaultGasOnErr) * boost::multiprecision::pow(u256(10), 9);
        if (tx.gasPrice == 0 && !defaultGasOnErr) {
            printf("Could not get gas price from station!\n");
            unlock_send_tx_mutex();
            return NULL;
        }
    }

    char *rawTx = signTx(tx, privKey);
    if (waitConfirm == 0) {
        result = sendRawTx(rawTx);
    } else {
        result = sendRawTxWaitConfirm(rawTx);
    }
    free(rawTx);
    return result;
}

std::stringstream getErc20TransferData(char *tokenAddress, char *to, char *amount, uint8_t setDecimals)
{
    u256 amountWei = jsToU256(amount);
    uint8_t decimals;
    if (setDecimals > 0) {
        decimals = setDecimals;
    } else {
        decimals = getErc20Decimals(tokenAddress);
    }
    if (decimals < 18) {
        amountWei /= boost::multiprecision::pow(u256(10), 18 - decimals);
    }
    // convert wei to satoshi
    std::stringstream ss;
    ss << "0xa9059cbb"
       << "000000000000000000000000"
       << toHex(jsToAddress(to))
       << toHex(toBigEndian(amountWei));
    return ss;
}

uint64_t estimate_erc20_gas(
    char *tokenAddress,
    char *to,
    char *amount,
    char *privKey,
    uint8_t decimals
)
{
    std::stringstream ss = getErc20TransferData(tokenAddress, to, amount, decimals);
    char *from = privKey2Addr(privKey);
    uint64_t result = estimateGas(from, tokenAddress, ss.str().c_str());
    free(from);
    return result;
}

char *sendErc20(
        char *tokenAddress,
        char *to,
        char *amount,
        char *privKey,
        uint8_t waitConfirm,
        int64_t gas,
        int64_t gasPrice,
        uint8_t defaultGasOnErr,
        uint8_t decimals
)
{
    TransactionSkeleton tx;
    char *from = privKey2Addr(privKey), *result;
    std::stringstream ss = getErc20TransferData(tokenAddress, to, amount, decimals);

    tx.from = jsToAddress(from);
    tx.to = jsToAddress(tokenAddress);
    tx.value = 0;
    tx.nonce = getNonce(from);

    if (gas > 0) {
        tx.gas = gas;
    } else {
        uint64_t gasEstimation = estimateGas(from, tokenAddress, ss.str().c_str());
        if (gasEstimation > 0) {
            tx.gas = gasEstimation;
        } else {
            tx.gas = 150000;
        }
    }
    free(from);
    if (gasPrice > 0) {
        tx.gasPrice = gasPrice * boost::multiprecision::pow(u256(10), 9);
    } else {
        tx.gasPrice = getGasPriceFromStation(defaultGasOnErr) * boost::multiprecision::pow(u256(10), 9);
        if (tx.gasPrice == 0 && !defaultGasOnErr) {
            printf("Could not get gas price from station!\n");
            unlock_send_tx_mutex();
            return NULL;
        }
    }

    tx.data = jsToBytes(ss.str());

    char *rawTx = signTx(tx, privKey);
    if (waitConfirm == 0) {
        result = sendRawTx(rawTx);
    } else {
        result = sendRawTxWaitConfirm(rawTx);
    }
    free(rawTx);
    return result;
}

uint8_t verifyAliceErc20FeeData(char *tokenAddress, char *to, char *amount, char *data, uint8_t decimals)
{
    std::stringstream ss = getErc20TransferData(tokenAddress, to, amount, decimals);
    if (strcmp(ss.str().c_str(), data) != 0) {
        printf("Alice ERC20 fee data %s is not equal to expected %s\n", data, ss.str().c_str());
        return 0;
    }
    return 1;
}

uint8_t alicePaymentStatus(char *paymentId)
{
    char buffer[100];
    memset(buffer, 0, sizeof(buffer));
    strcpy(buffer, "0x81cd872a");
    strcat(buffer, paymentId);
    char *hexStatus = ethCall(ETOMIC_ALICECONTRACT, buffer);
    auto status = (uint8_t) strtol(hexStatus + 66, NULL, 0);
    free(hexStatus);
    return status;
}

uint8_t bobDepositStatus(char *depositId)
{
    char buffer[100];
    memset(buffer, 0, sizeof(buffer));
    strcpy(buffer, "0x3d4dff7b");
    strcat(buffer, depositId);
    char *hexStatus = ethCall(ETOMIC_BOBCONTRACT, buffer);
    auto status = (uint8_t) strtol(hexStatus + 130, NULL, 0);
    free(hexStatus);
    return status;
}

uint8_t bobPaymentStatus(char *paymentId)
{
    char buffer[100];
    memset(buffer, 0, sizeof(buffer));
    strcpy(buffer, "0x0716326d");
    strcat(buffer, paymentId);
    char *hexStatus = ethCall(ETOMIC_BOBCONTRACT, buffer);
    auto status = (uint8_t) strtol(hexStatus + 130, NULL, 0);
    free(hexStatus);
    return status;
}

uint8_t compareAddresses(char *address1, char *address2)
{
    auto addr_bytes_1 = jsToAddress(address1);
    auto addr_bytes_2 = jsToAddress(address2);
    return static_cast<uint8_t>(addr_bytes_1 == addr_bytes_2);
}

uint8_t isValidAddress(char *address)
{
    std::regex r("^(0x|0X)?[a-fA-F0-9]{40}$");
    return static_cast<uint8_t>(std::regex_match(address, r));
}
