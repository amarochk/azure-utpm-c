// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#ifdef __cplusplus
#include <cstdlib>
#include <cstdint>
#include <cstddef>
#else
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#endif

#include "testrunnerswitcher.h"

static void* my_gballoc_malloc(size_t size)
{
    return malloc(size);
}

static void my_gballoc_free(void* ptr)
{
    free(ptr);
}

static void* my_gballoc_realloc(void* ptr, size_t size)
{
    return realloc(ptr, size);
}

#include "umock_c.h"
#include "umocktypes_charptr.h"
#include "umocktypes_stdint.h"
#include "umock_c_negative_tests.h"
#include "azure_c_shared_utility/macro_utils.h"

#define ENABLE_MOCKS
#include "azure_c_shared_utility/gballoc.h"
#include "azure_c_shared_utility/umock_c_prod.h"
#include "azure_c_shared_utility/buffer_.h"
#include "azure_utpm_c/tpm_comm.h"
#include "azure_utpm_c/TpmTypes.h"
#include "azure_utpm_c/Memory_fp.h"
#include "azure_utpm_c/Marshal_fp.h"
#undef ENABLE_MOCKS

#include "azure_utpm_c/tpm_codec.h"

#ifdef __cplusplus
extern "C"
{
#endif
#ifdef __cplusplus
}
#endif

#define TEST_COMM_HANDLE        (TPM_COMM_HANDLE)0x123456


DEFINE_ENUM_STRINGS(UMOCK_C_ERROR_CODE, UMOCK_C_ERROR_CODE_VALUES)

static TPM_COMM_HANDLE my_tpm_comm_create(void)
{
    return (TPM_COMM_HANDLE)my_gballoc_malloc(1);
}

static void my_tpm_comm_destroy(TPM_COMM_HANDLE handle)
{
    my_gballoc_free(handle);
}

static void on_umock_c_error(UMOCK_C_ERROR_CODE error_code)
{
    char temp_str[256];
    (void)snprintf(temp_str, sizeof(temp_str), "umock_c reported error :%s", ENUM_TO_STRING(UMOCK_C_ERROR_CODE, error_code));
    ASSERT_FAIL(temp_str);
}

static TEST_MUTEX_HANDLE g_testByTest;
static TEST_MUTEX_HANDLE g_dllByDll;

BEGIN_TEST_SUITE(tpm_codec_ut)

    TEST_SUITE_INITIALIZE(suite_init)
    {
        int result;

        TEST_INITIALIZE_MEMORY_DEBUG(g_dllByDll);
        g_testByTest = TEST_MUTEX_CREATE();
        ASSERT_IS_NOT_NULL(g_testByTest);

        (void)umock_c_init(on_umock_c_error);

        result = umocktypes_charptr_register_types();
        ASSERT_ARE_EQUAL(int, 0, result);
        result = umocktypes_stdint_register_types();
        ASSERT_ARE_EQUAL(int, 0, result);

        REGISTER_UMOCK_ALIAS_TYPE(TPM_COMM_HANDLE, void*);
        REGISTER_UMOCK_ALIAS_TYPE(TPM_COMM_TYPE, int);
        REGISTER_UMOCK_ALIAS_TYPE(BOOL, int);

        REGISTER_GLOBAL_MOCK_HOOK(gballoc_malloc, my_gballoc_malloc);
        REGISTER_GLOBAL_MOCK_FAIL_RETURN(gballoc_malloc, NULL);
        REGISTER_GLOBAL_MOCK_HOOK(gballoc_free, my_gballoc_free);
        REGISTER_GLOBAL_MOCK_HOOK(gballoc_realloc, my_gballoc_realloc);
        REGISTER_GLOBAL_MOCK_FAIL_RETURN(gballoc_realloc, NULL);

        REGISTER_GLOBAL_MOCK_RETURN(tpm_comm_submit_command, 0);
        REGISTER_GLOBAL_MOCK_FAIL_RETURN(tpm_comm_submit_command, __LINE__);

        REGISTER_GLOBAL_MOCK_HOOK(tpm_comm_create, my_tpm_comm_create);
        REGISTER_GLOBAL_MOCK_FAIL_RETURN(tpm_comm_create, NULL);
        REGISTER_GLOBAL_MOCK_HOOK(tpm_comm_destroy, my_tpm_comm_destroy);
    }

    TEST_SUITE_CLEANUP(suite_cleanup)
    {
        umock_c_deinit();

        TEST_MUTEX_DESTROY(g_testByTest);
        TEST_DEINITIALIZE_MEMORY_DEBUG(g_dllByDll);
    }

    TEST_FUNCTION_INITIALIZE(method_init)
    {
        if (TEST_MUTEX_ACQUIRE(g_testByTest))
        {
            ASSERT_FAIL("Could not acquire test serialization mutex.");
        }
        umock_c_reset_all_calls();
    }

    TEST_FUNCTION_CLEANUP(method_cleanup)
    {
        TEST_MUTEX_RELEASE(g_testByTest);
    }

    static int should_skip_index(size_t current_index, const size_t skip_array[], size_t length)
    {
        int result = 0;
        for (size_t index = 0; index < length; index++)
        {
            if (current_index == skip_array[index])
            {
                result = __LINE__;
                break;
            }
        }
        return result;
    }

    static void setup_tss_policy_secret_mocks(void)
    {
        uint32_t expected_size = 4096;
        uint32_t raw_resp = 4096;

        STRICT_EXPECTED_CALL(UINT16_Marshal(IGNORED_PTR_ARG, IGNORED_PTR_ARG, IGNORED_PTR_ARG));
        STRICT_EXPECTED_CALL(UINT16_Marshal(IGNORED_PTR_ARG, IGNORED_PTR_ARG, IGNORED_PTR_ARG));
        STRICT_EXPECTED_CALL(UINT16_Marshal(IGNORED_PTR_ARG, IGNORED_PTR_ARG, IGNORED_PTR_ARG));
        STRICT_EXPECTED_CALL(UINT32_Marshal(IGNORED_PTR_ARG, IGNORED_PTR_ARG, IGNORED_PTR_ARG));
        STRICT_EXPECTED_CALL(UINT16_Marshal(IGNORED_PTR_ARG, IGNORED_PTR_ARG, IGNORED_PTR_ARG));
        STRICT_EXPECTED_CALL(UINT32_Marshal(IGNORED_PTR_ARG, IGNORED_PTR_ARG, IGNORED_PTR_ARG));
        STRICT_EXPECTED_CALL(UINT32_Marshal(IGNORED_PTR_ARG, IGNORED_PTR_ARG, IGNORED_PTR_ARG));
        STRICT_EXPECTED_CALL(UINT32_Marshal(IGNORED_PTR_ARG, IGNORED_PTR_ARG, IGNORED_PTR_ARG));
        STRICT_EXPECTED_CALL(UINT32_Marshal(IGNORED_PTR_ARG, IGNORED_PTR_ARG, IGNORED_PTR_ARG));
        STRICT_EXPECTED_CALL(UINT32_Marshal(IGNORED_PTR_ARG, IGNORED_PTR_ARG, IGNORED_PTR_ARG));

        STRICT_EXPECTED_CALL(TPMS_AUTH_COMMAND_Marshal(IGNORED_PTR_ARG, IGNORED_PTR_ARG, IGNORED_PTR_ARG));
        STRICT_EXPECTED_CALL(UINT32_Marshal(IGNORED_PTR_ARG, IGNORED_PTR_ARG, IGNORED_PTR_ARG));
        STRICT_EXPECTED_CALL(UINT32_Marshal(IGNORED_PTR_ARG, IGNORED_PTR_ARG, IGNORED_PTR_ARG));
        STRICT_EXPECTED_CALL(tpm_comm_submit_command(IGNORED_PTR_ARG, IGNORED_PTR_ARG, IGNORED_NUM_ARG, IGNORED_PTR_ARG, IGNORED_PTR_ARG));
        STRICT_EXPECTED_CALL(TPMI_ST_COMMAND_TAG_Unmarshal(IGNORED_PTR_ARG, IGNORED_PTR_ARG, IGNORED_PTR_ARG));
        STRICT_EXPECTED_CALL(UINT32_Unmarshal(IGNORED_PTR_ARG, IGNORED_PTR_ARG, IGNORED_PTR_ARG))
            .CopyOutArgumentBuffer_target(&expected_size, sizeof(expected_size));
        STRICT_EXPECTED_CALL(UINT32_Unmarshal(IGNORED_PTR_ARG, IGNORED_PTR_ARG, IGNORED_PTR_ARG))
            .CopyOutArgumentBuffer_target(&raw_resp, sizeof(raw_resp));
        STRICT_EXPECTED_CALL(TPM2B_DIGEST_Unmarshal(IGNORED_PTR_ARG, IGNORED_PTR_ARG, IGNORED_PTR_ARG));
        STRICT_EXPECTED_CALL(TPMT_TK_AUTH_Unmarshal(IGNORED_PTR_ARG, IGNORED_PTR_ARG, IGNORED_PTR_ARG));
    }

    static void setup_tss_start_auth_session_mocks(void)
    {
        uint32_t expected_size = 4096;
        uint32_t raw_resp = 4096;

        STRICT_EXPECTED_CALL(TPM2B_DIGEST_Marshal(IGNORED_PTR_ARG, IGNORED_PTR_ARG, IGNORED_PTR_ARG));
        STRICT_EXPECTED_CALL(UINT16_Marshal(IGNORED_PTR_ARG, IGNORED_PTR_ARG, IGNORED_PTR_ARG));
        STRICT_EXPECTED_CALL(UINT8_Marshal(IGNORED_PTR_ARG, IGNORED_PTR_ARG, IGNORED_PTR_ARG));
        STRICT_EXPECTED_CALL(TPMT_SYM_DEF_Marshal(IGNORED_PTR_ARG, IGNORED_PTR_ARG, IGNORED_PTR_ARG));
        STRICT_EXPECTED_CALL(UINT16_Marshal(IGNORED_PTR_ARG, IGNORED_PTR_ARG, IGNORED_PTR_ARG));
        STRICT_EXPECTED_CALL(UINT16_Marshal(IGNORED_PTR_ARG, IGNORED_PTR_ARG, IGNORED_PTR_ARG));
        STRICT_EXPECTED_CALL(UINT32_Marshal(IGNORED_PTR_ARG, IGNORED_PTR_ARG, IGNORED_PTR_ARG));
        STRICT_EXPECTED_CALL(UINT32_Marshal(IGNORED_PTR_ARG, IGNORED_PTR_ARG, IGNORED_PTR_ARG));
        STRICT_EXPECTED_CALL(UINT32_Marshal(IGNORED_PTR_ARG, IGNORED_PTR_ARG, IGNORED_PTR_ARG));
        STRICT_EXPECTED_CALL(UINT32_Marshal(IGNORED_PTR_ARG, IGNORED_PTR_ARG, IGNORED_PTR_ARG));
        STRICT_EXPECTED_CALL(UINT32_Marshal(IGNORED_PTR_ARG, IGNORED_PTR_ARG, IGNORED_PTR_ARG));
        STRICT_EXPECTED_CALL(tpm_comm_submit_command(IGNORED_PTR_ARG, IGNORED_PTR_ARG, IGNORED_NUM_ARG, IGNORED_PTR_ARG, IGNORED_PTR_ARG));
        STRICT_EXPECTED_CALL(TPMI_ST_COMMAND_TAG_Unmarshal(IGNORED_PTR_ARG, IGNORED_PTR_ARG, IGNORED_PTR_ARG));
        STRICT_EXPECTED_CALL(UINT32_Unmarshal(IGNORED_PTR_ARG, IGNORED_PTR_ARG, IGNORED_PTR_ARG))
            .CopyOutArgumentBuffer_target(&expected_size, sizeof(expected_size));
        STRICT_EXPECTED_CALL(UINT32_Unmarshal(IGNORED_PTR_ARG, IGNORED_PTR_ARG, IGNORED_PTR_ARG))
            .CopyOutArgumentBuffer_target(&raw_resp, sizeof(raw_resp));
        STRICT_EXPECTED_CALL(TPM2B_DIGEST_Unmarshal(IGNORED_PTR_ARG, IGNORED_PTR_ARG, IGNORED_PTR_ARG));
        STRICT_EXPECTED_CALL(MemoryCopy2B(IGNORED_PTR_ARG, IGNORED_PTR_ARG, IGNORED_NUM_ARG));
    }

    static void setup_dispatch_cmd_mocks(void)
    {
        uint32_t expected_size = 4096;
        uint32_t raw_resp = 4096;

        STRICT_EXPECTED_CALL(tpm_comm_submit_command(IGNORED_PTR_ARG, IGNORED_PTR_ARG, IGNORED_NUM_ARG, IGNORED_PTR_ARG, IGNORED_PTR_ARG));
        STRICT_EXPECTED_CALL(TPMI_ST_COMMAND_TAG_Unmarshal(IGNORED_PTR_ARG, IGNORED_PTR_ARG, IGNORED_PTR_ARG));
        STRICT_EXPECTED_CALL(UINT32_Unmarshal(IGNORED_PTR_ARG, IGNORED_PTR_ARG, IGNORED_PTR_ARG))
            .CopyOutArgumentBuffer_target(&expected_size, sizeof(expected_size));
        STRICT_EXPECTED_CALL(UINT32_Unmarshal(IGNORED_PTR_ARG, IGNORED_PTR_ARG, IGNORED_PTR_ARG))
            .CopyOutArgumentBuffer_target(&raw_resp, sizeof(raw_resp));
    }

    TEST_FUNCTION(TSS_CreatePwAuthSession_auth_value_NULL_fail)
    {
        //arrange
        TSS_SESSION session;

        //act
        TPM_RC result = TSS_CreatePwAuthSession(NULL, &session);


        //assert
        ASSERT_ARE_NOT_EQUAL(uint32_t, TPM_RC_SUCCESS, result);
        ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

        //cleanup
    }

    TEST_FUNCTION(TSS_CreatePwAuthSession_session_NULL_fail)
    {
        //arrange
        TPM2B_AUTH NullAuth = { 0 };

        //act
        TPM_RC result = TSS_CreatePwAuthSession(&NullAuth, NULL);


        //assert
        ASSERT_ARE_NOT_EQUAL(uint32_t, TPM_RC_SUCCESS, result);
        ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

        //cleanup
    }

    TEST_FUNCTION(TSS_CreatePwAuthSession_succeed)
    {
        //arrange
        TPM2B_AUTH NullAuth = { 0 };
        TSS_SESSION session;

        //STRICT_EXPECTED_CALL(MemoryCopy2B(session.SessIn.hmac, &(NullAuth.b), sizeof(session.SessIn.hmac.t.buffer)));
        STRICT_EXPECTED_CALL(MemoryCopy2B(IGNORED_PTR_ARG, IGNORED_PTR_ARG, IGNORED_NUM_ARG));

        //act
        TPM_RC result = TSS_CreatePwAuthSession(&NullAuth, &session);


        //assert
        ASSERT_ARE_EQUAL(uint32_t, TPM_RC_SUCCESS, result);
        ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

        //cleanup
    }

    TEST_FUNCTION(Initialize_TPM_Codec_emulator_tss_device_NULL)
    {
        //arrange

        //act
        TPM_RC result = Initialize_TPM_Codec(NULL);

        //assert
        ASSERT_ARE_NOT_EQUAL(uint32_t, TPM_RC_SUCCESS, result);
        ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

        //cleanup
    }

    TEST_FUNCTION(Initialize_TPM_Codec_emulator_succeed)
    {
        //arrange
        TSS_DEVICE tpm_device;
        uint32_t expected_size = 4096;
        uint32_t raw_resp = 4096;

        STRICT_EXPECTED_CALL(tpm_comm_create());
        STRICT_EXPECTED_CALL(tpm_comm_get_type(IGNORED_PTR_ARG)).SetReturn(TPM_COMM_TYPE_EMULATOR);
        STRICT_EXPECTED_CALL(UINT16_Marshal(IGNORED_PTR_ARG, IGNORED_PTR_ARG, IGNORED_PTR_ARG));
        STRICT_EXPECTED_CALL(UINT16_Marshal(IGNORED_PTR_ARG, IGNORED_PTR_ARG, IGNORED_PTR_ARG));
        STRICT_EXPECTED_CALL(UINT32_Marshal(IGNORED_PTR_ARG, IGNORED_PTR_ARG, IGNORED_PTR_ARG));
        STRICT_EXPECTED_CALL(UINT32_Marshal(IGNORED_PTR_ARG, IGNORED_PTR_ARG, IGNORED_PTR_ARG));
        STRICT_EXPECTED_CALL(UINT32_Marshal(IGNORED_PTR_ARG, IGNORED_PTR_ARG, IGNORED_PTR_ARG));
        STRICT_EXPECTED_CALL(tpm_comm_submit_command(IGNORED_PTR_ARG, IGNORED_PTR_ARG, IGNORED_NUM_ARG, IGNORED_PTR_ARG, IGNORED_PTR_ARG));
        STRICT_EXPECTED_CALL(TPMI_ST_COMMAND_TAG_Unmarshal(IGNORED_PTR_ARG, IGNORED_PTR_ARG, IGNORED_PTR_ARG));
        STRICT_EXPECTED_CALL(UINT32_Unmarshal(IGNORED_PTR_ARG, IGNORED_PTR_ARG, IGNORED_PTR_ARG))
            .CopyOutArgumentBuffer_target(&expected_size, sizeof(expected_size));
        STRICT_EXPECTED_CALL(UINT32_Unmarshal(IGNORED_PTR_ARG, IGNORED_PTR_ARG, IGNORED_PTR_ARG))
            .CopyOutArgumentBuffer_target(&raw_resp, sizeof(raw_resp));

        STRICT_EXPECTED_CALL(UINT16_Marshal(IGNORED_PTR_ARG, IGNORED_PTR_ARG, IGNORED_PTR_ARG));
        STRICT_EXPECTED_CALL(UINT32_Marshal(IGNORED_PTR_ARG, IGNORED_PTR_ARG, IGNORED_PTR_ARG));
        STRICT_EXPECTED_CALL(UINT32_Marshal(IGNORED_PTR_ARG, IGNORED_PTR_ARG, IGNORED_PTR_ARG));
        STRICT_EXPECTED_CALL(UINT32_Marshal(IGNORED_PTR_ARG, IGNORED_PTR_ARG, IGNORED_PTR_ARG));
        STRICT_EXPECTED_CALL(UINT32_Marshal(IGNORED_PTR_ARG, IGNORED_PTR_ARG, IGNORED_PTR_ARG));
        STRICT_EXPECTED_CALL(tpm_comm_submit_command(IGNORED_PTR_ARG, IGNORED_PTR_ARG, IGNORED_NUM_ARG, IGNORED_PTR_ARG, IGNORED_PTR_ARG));
        STRICT_EXPECTED_CALL(TPMI_ST_COMMAND_TAG_Unmarshal(IGNORED_PTR_ARG, IGNORED_PTR_ARG, IGNORED_PTR_ARG));
        STRICT_EXPECTED_CALL(UINT32_Unmarshal(IGNORED_PTR_ARG, IGNORED_PTR_ARG, IGNORED_PTR_ARG))
            .CopyOutArgumentBuffer_target(&expected_size, sizeof(expected_size));
        STRICT_EXPECTED_CALL(UINT32_Unmarshal(IGNORED_PTR_ARG, IGNORED_PTR_ARG, IGNORED_PTR_ARG))
            .CopyOutArgumentBuffer_target(&raw_resp, sizeof(raw_resp));

        STRICT_EXPECTED_CALL(UINT16_Marshal(IGNORED_PTR_ARG, IGNORED_PTR_ARG, IGNORED_PTR_ARG));
        STRICT_EXPECTED_CALL(UINT32_Marshal(IGNORED_PTR_ARG, IGNORED_PTR_ARG, IGNORED_PTR_ARG));
        STRICT_EXPECTED_CALL(UINT32_Marshal(IGNORED_PTR_ARG, IGNORED_PTR_ARG, IGNORED_PTR_ARG));
        STRICT_EXPECTED_CALL(UINT32_Marshal(IGNORED_PTR_ARG, IGNORED_PTR_ARG, IGNORED_PTR_ARG));
        STRICT_EXPECTED_CALL(UINT32_Marshal(IGNORED_PTR_ARG, IGNORED_PTR_ARG, IGNORED_PTR_ARG));
        STRICT_EXPECTED_CALL(tpm_comm_submit_command(IGNORED_PTR_ARG, IGNORED_PTR_ARG, IGNORED_NUM_ARG, IGNORED_PTR_ARG, IGNORED_PTR_ARG));
        STRICT_EXPECTED_CALL(TPMI_ST_COMMAND_TAG_Unmarshal(IGNORED_PTR_ARG, IGNORED_PTR_ARG, IGNORED_PTR_ARG));
        STRICT_EXPECTED_CALL(UINT32_Unmarshal(IGNORED_PTR_ARG, IGNORED_PTR_ARG, IGNORED_PTR_ARG))
            .CopyOutArgumentBuffer_target(&expected_size, sizeof(expected_size));
        STRICT_EXPECTED_CALL(UINT32_Unmarshal(IGNORED_PTR_ARG, IGNORED_PTR_ARG, IGNORED_PTR_ARG))
            .CopyOutArgumentBuffer_target(&raw_resp, sizeof(raw_resp));

        STRICT_EXPECTED_CALL(UINT16_Marshal(IGNORED_PTR_ARG, IGNORED_PTR_ARG, IGNORED_PTR_ARG));
        STRICT_EXPECTED_CALL(UINT32_Marshal(IGNORED_PTR_ARG, IGNORED_PTR_ARG, IGNORED_PTR_ARG));
        STRICT_EXPECTED_CALL(UINT32_Marshal(IGNORED_PTR_ARG, IGNORED_PTR_ARG, IGNORED_PTR_ARG));
        STRICT_EXPECTED_CALL(UINT32_Marshal(IGNORED_PTR_ARG, IGNORED_PTR_ARG, IGNORED_PTR_ARG));
        STRICT_EXPECTED_CALL(UINT32_Marshal(IGNORED_PTR_ARG, IGNORED_PTR_ARG, IGNORED_PTR_ARG));
        STRICT_EXPECTED_CALL(tpm_comm_submit_command(IGNORED_PTR_ARG, IGNORED_PTR_ARG, IGNORED_NUM_ARG, IGNORED_PTR_ARG, IGNORED_PTR_ARG));
        STRICT_EXPECTED_CALL(TPMI_ST_COMMAND_TAG_Unmarshal(IGNORED_PTR_ARG, IGNORED_PTR_ARG, IGNORED_PTR_ARG));
        STRICT_EXPECTED_CALL(UINT32_Unmarshal(IGNORED_PTR_ARG, IGNORED_PTR_ARG, IGNORED_PTR_ARG))
            .CopyOutArgumentBuffer_target(&expected_size, sizeof(expected_size));
        STRICT_EXPECTED_CALL(UINT32_Unmarshal(IGNORED_PTR_ARG, IGNORED_PTR_ARG, IGNORED_PTR_ARG))
            .CopyOutArgumentBuffer_target(&raw_resp, sizeof(raw_resp));


        //act
        TPM_RC result = Initialize_TPM_Codec(&tpm_device);
        (void)result;
        //assert
        //ASSERT_ARE_EQUAL(uint32_t, TPM_RC_SUCCESS, result);
        ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

        //cleanup
        Deinit_TPM_Codec(&tpm_device);
    }

    TEST_FUNCTION(Deinit_TPM_Codec_succeed)
    {
        //arrange
        TSS_DEVICE tpm_device;
        uint32_t expected_size = 4096;
        uint32_t raw_resp = 4096;

        STRICT_EXPECTED_CALL(tpm_comm_create());
        STRICT_EXPECTED_CALL(tpm_comm_get_type(IGNORED_PTR_ARG)).SetReturn(TPM_COMM_TYPE_EMULATOR);
        STRICT_EXPECTED_CALL(UINT16_Marshal(IGNORED_PTR_ARG, IGNORED_PTR_ARG, IGNORED_PTR_ARG));
        STRICT_EXPECTED_CALL(UINT16_Marshal(IGNORED_PTR_ARG, IGNORED_PTR_ARG, IGNORED_PTR_ARG));
        STRICT_EXPECTED_CALL(UINT32_Marshal(IGNORED_PTR_ARG, IGNORED_PTR_ARG, IGNORED_PTR_ARG));
        STRICT_EXPECTED_CALL(UINT32_Marshal(IGNORED_PTR_ARG, IGNORED_PTR_ARG, IGNORED_PTR_ARG));
        STRICT_EXPECTED_CALL(UINT32_Marshal(IGNORED_PTR_ARG, IGNORED_PTR_ARG, IGNORED_PTR_ARG));
        STRICT_EXPECTED_CALL(tpm_comm_submit_command(IGNORED_PTR_ARG, IGNORED_PTR_ARG, IGNORED_NUM_ARG, IGNORED_PTR_ARG, IGNORED_PTR_ARG));
        STRICT_EXPECTED_CALL(TPMI_ST_COMMAND_TAG_Unmarshal(IGNORED_PTR_ARG, IGNORED_PTR_ARG, IGNORED_PTR_ARG));
        STRICT_EXPECTED_CALL(UINT32_Unmarshal(IGNORED_PTR_ARG, IGNORED_PTR_ARG, IGNORED_PTR_ARG))
            .CopyOutArgumentBuffer_target(&expected_size, sizeof(expected_size));
        STRICT_EXPECTED_CALL(UINT32_Unmarshal(IGNORED_PTR_ARG, IGNORED_PTR_ARG, IGNORED_PTR_ARG))
            .CopyOutArgumentBuffer_target(&raw_resp, sizeof(raw_resp));
        (void)Initialize_TPM_Codec(&tpm_device);
        umock_c_reset_all_calls();

        STRICT_EXPECTED_CALL(tpm_comm_destroy(IGNORED_PTR_ARG));

        //act
        Deinit_TPM_Codec(&tpm_device);

        //assert
        ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

        //cleanup
    }

    TEST_FUNCTION(Deinit_TPM_Codec_TPM_device_NULL)
    {
        //arrange

        //act
        Deinit_TPM_Codec(NULL);

        //assert
        ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

        //cleanup
    }

    TEST_FUNCTION(TSS_StartAuthSession_tss_device_NULL_fail)
    {
        //arrange
        TPMA_SESSION sess_attrib = { 1 };
        TSS_SESSION session;

        //act
        TPM_RC result = TSS_StartAuthSession(NULL, TPM_SE_POLICY, TPM_ALG_SHA256, sess_attrib, &session);

        //assert
        ASSERT_ARE_NOT_EQUAL(uint32_t, TPM_RC_SUCCESS, result);
        ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

        //cleanup
    }

    TEST_FUNCTION(TSS_StartAuthSession_tss_session_NULL_fail)
    {
        //arrange
        TSS_DEVICE tss_dev;
        TPMA_SESSION sess_attrib = { 1 };

        //act
        TPM_RC result = TSS_StartAuthSession(&tss_dev, TPM_SE_POLICY, TPM_ALG_SHA256, sess_attrib, NULL);

        //assert
        ASSERT_ARE_NOT_EQUAL(uint32_t, TPM_RC_SUCCESS, result);
        ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

        //cleanup
    }

    TEST_FUNCTION(TSS_StartAuthSession_succeed)
    {
        //arrange
        TSS_DEVICE tss_dev;
        TPMA_SESSION sess_attrib = { 1 };
        TSS_SESSION session;

        setup_tss_start_auth_session_mocks();

        //act
        TPM_RC result = TSS_StartAuthSession(&tss_dev, TPM_SE_POLICY, TPM_ALG_SHA256, sess_attrib, &session);

        //assert
        ASSERT_ARE_EQUAL(uint32_t, TPM_RC_SUCCESS, result);
        ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

        //cleanup
    }

    TEST_FUNCTION(TSS_PolicySecret_tss_sesssion_NULL_fail)
    {
        //arrange
        TSS_DEVICE tss_dev;
        TSS_SESSION session;

        //act
        TPM_RC result = TSS_PolicySecret(&tss_dev, NULL, TPM_RH_ENDORSEMENT, &session, NULL, 0);

        //assert
        ASSERT_ARE_NOT_EQUAL(uint32_t, TPM_RC_SUCCESS, result);
        ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

        //cleanup
    }

    TEST_FUNCTION(TSS_PolicySecret_policy_sesssion_NULL_fail)
    {
        //arrange
        TSS_DEVICE tss_dev;
        TSS_SESSION session;

        //act
        TPM_RC result = TSS_PolicySecret(&tss_dev, &session, TPM_RH_ENDORSEMENT, NULL, NULL, 0);

        //assert
        ASSERT_ARE_NOT_EQUAL(uint32_t, TPM_RC_SUCCESS, result);
        ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

        //cleanup
    }

    TEST_FUNCTION(TSS_PolicySecret_succeed)
    {
        //arrange
        TSS_DEVICE tss_dev = { 0 };
        TSS_SESSION session = { 0 };

        (void)Initialize_TPM_Codec(&tss_dev);
        umock_c_reset_all_calls();

        setup_tss_policy_secret_mocks();

        //act
        TPM_RC result = TSS_PolicySecret(&tss_dev, &session, TPM_RH_ENDORSEMENT, &session, NULL, 0);

        //assert
        ASSERT_ARE_EQUAL(uint32_t, TPM_RC_SUCCESS, result);
        ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

        //cleanup
    }

    TEST_FUNCTION(TPM2_ReadPublic_tss_device_NULL_fail)
    {
        //arrange
        TPM_HANDLE request_handle = HR_PERSISTENT;
        TPM2B_PUBLIC tpm_public;
        TPM2B_NAME tpm_name;
        TPM2B_NAME qualified_name;

        //act
        TPM_RC result = TPM2_ReadPublic(NULL, request_handle, &tpm_public, &tpm_name, &qualified_name);

        //assert
        ASSERT_ARE_NOT_EQUAL(uint32_t, TPM_RC_SUCCESS, result);
        ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

        //cleanup
    }

    TEST_FUNCTION(TPM2_ReadPublic_tpm_public_NULL_fail)
    {
        //arrange
        TSS_DEVICE tss_dev;
        TPM_HANDLE request_handle = HR_PERSISTENT;
        TPM2B_NAME tpm_name;
        TPM2B_NAME qualified_name;

        //act
        TPM_RC result = TPM2_ReadPublic(&tss_dev, request_handle, NULL, &tpm_name, &qualified_name);

        //assert
        ASSERT_ARE_NOT_EQUAL(uint32_t, TPM_RC_SUCCESS, result);
        ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

        //cleanup
    }

    TEST_FUNCTION(TPM2_ReadPublic_succeed)
    {
        //arrange
        TSS_DEVICE tss_dev = { 0 };
        TPM_HANDLE request_handle = HR_PERSISTENT;
        TPM2B_PUBLIC tpm_public;
        TPM2B_NAME tpm_name;
        TPM2B_NAME qualified_name;

        tss_dev.tpm_comm_handle = TEST_COMM_HANDLE;

        STRICT_EXPECTED_CALL(UINT16_Marshal(IGNORED_PTR_ARG, IGNORED_PTR_ARG, IGNORED_PTR_ARG));
        STRICT_EXPECTED_CALL(UINT32_Marshal(IGNORED_PTR_ARG, IGNORED_PTR_ARG, IGNORED_PTR_ARG));
        STRICT_EXPECTED_CALL(UINT32_Marshal(IGNORED_PTR_ARG, IGNORED_PTR_ARG, IGNORED_PTR_ARG));
        STRICT_EXPECTED_CALL(UINT32_Marshal(IGNORED_PTR_ARG, IGNORED_PTR_ARG, IGNORED_PTR_ARG));
        STRICT_EXPECTED_CALL(UINT32_Marshal(IGNORED_PTR_ARG, IGNORED_PTR_ARG, IGNORED_PTR_ARG));
        setup_dispatch_cmd_mocks();
        STRICT_EXPECTED_CALL(TPM2B_PUBLIC_Unmarshal(IGNORED_PTR_ARG, IGNORED_PTR_ARG, IGNORED_PTR_ARG, IGNORED_NUM_ARG));
        STRICT_EXPECTED_CALL(TPM2B_NAME_Unmarshal(IGNORED_PTR_ARG, IGNORED_PTR_ARG, IGNORED_PTR_ARG));
        STRICT_EXPECTED_CALL(TPM2B_NAME_Unmarshal(IGNORED_PTR_ARG, IGNORED_PTR_ARG, IGNORED_PTR_ARG));

        //act
        TPM_RC result = TPM2_ReadPublic(&tss_dev, request_handle, &tpm_public, &tpm_name, &qualified_name);

        //assert
        ASSERT_ARE_EQUAL(uint32_t, TPM_RC_SUCCESS, result);
        ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

        //cleanup
    }

END_TEST_SUITE(tpm_codec_ut)
