#include <iostream>

// Include the GTest headers
#include "gtest_headers.h"

// Include the SYSTEM MOCK and PFR headers
#include "ut_nios_wrapper.h"

class PFRSpdmDebugTest : public testing::Test
{
public:
    alt_u32* m_memory = nullptr;
    alt_u32* m_mctp_memory = nullptr;

    virtual void SetUp()
    {
        SYSTEM_MOCK::get()->reset();
        m_memory = U_MAILBOX_AVMM_BRIDGE_ADDR;
        m_mctp_memory = U_RFNVRAM_SMBUS_MASTER_ADDR;
    }

    virtual void TearDown() {}
};

TEST_F(PFRSpdmDebugTest, test_get_certificate_openspdm)
{
    // Initialise context
    // Move to separate function
    SPDM_CONTEXT spdm_context;
    SPDM_CONTEXT* spdm_context_ptr = &spdm_context;

    // Reset everything
    reset_buffer((alt_u8*)spdm_context_ptr, sizeof(SPDM_CONTEXT));

    MCTP_CONTEXT mctp_context;
    MCTP_CONTEXT* mctp_context_ptr = &mctp_context;

    reset_buffer((alt_u8*)mctp_context_ptr, sizeof(MCTP_CONTEXT));

    init_spdm_context(spdm_context_ptr);

    spdm_context_ptr->connection_info.algorithm.base_hash_algo = BASE_HASH_ALGO_TPM_ALG_ECDSA_ECC_NIST_P384;

    // Expect first message from host to request for version
    spdm_context_ptr->spdm_msg_state = SPDM_CERTIFICATE_FLAG;
    mctp_context_ptr->cached_addr = BMC_SLAVE_ADDRESS;
    mctp_context_ptr->msg_type = MCTP_MESSAGE_TYPE;

    // Cert chain obtained from OPENSPDM responder
	alt_u8 cert_chain_buffer[1544] = {
			//cert chain length and reserved bytes
			0x08, 0x06, 0x00, 0x00,
			//start of root hash
			0x5a, 0x64, 0xb3, 0x8b, 0x5d, 0x5f, 0x4d, 0xb3, 0x5f, 0xb2, 0xaa, 0x1d,
			0x46, 0x9f, 0x6a, 0xdc, 0xca, 0x7f, 0xac, 0x85, 0xbe, 0xf0, 0x84, 0x10,
			0x9c, 0xcd, 0x54, 0x09, 0xf0, 0xab, 0x38, 0x3a, 0xaa, 0xf7, 0xa6, 0x2e,
			0x3b, 0xd7, 0x81, 0x2c, 0xea, 0x24, 0x7e, 0x14, 0xa9, 0x56, 0x9d, 0x28,
			//start of cert chain
			0x30, 0x82, 0x01, 0xcf, 0x30, 0x82, 0x01, 0x56, 0xa0, 0x03, 0x02, 0x01,
			0x02, 0x02, 0x14, 0x20, 0x3a, 0xc2, 0x59, 0xcc, 0xda, 0xcb, 0xf6, 0x72,
			0xf1, 0xc0, 0x1a, 0x62, 0x1a, 0x45, 0x82, 0x90, 0x24, 0xb8, 0xaf, 0x30,
			0x0a, 0x06, 0x08, 0x2a, 0x86, 0x48, 0xce, 0x3d, 0x04, 0x03, 0x03, 0x30,
			0x1f, 0x31, 0x1d, 0x30, 0x1b, 0x06, 0x03, 0x55, 0x04, 0x03, 0x0c, 0x14,
			0x69, 0x6e, 0x74, 0x65, 0x6c, 0x20, 0x74, 0x65, 0x73, 0x74, 0x20, 0x45,
			0x43, 0x50, 0x32, 0x35, 0x36, 0x20, 0x43, 0x41, 0x30, 0x1e, 0x17, 0x0d,
			0x32, 0x31, 0x30, 0x32, 0x30, 0x39, 0x30, 0x30, 0x35, 0x30, 0x35, 0x38,
			0x5a, 0x17, 0x0d, 0x33, 0x31, 0x30, 0x32, 0x30, 0x37, 0x30, 0x30, 0x35,
			0x30, 0x35, 0x38, 0x5a, 0x30, 0x1f, 0x31, 0x1d, 0x30, 0x1b, 0x06, 0x03,
			0x55, 0x04, 0x03, 0x0c, 0x14, 0x69, 0x6e, 0x74, 0x65, 0x6c, 0x20, 0x74,
			0x65, 0x73, 0x74, 0x20, 0x45, 0x43, 0x50, 0x32, 0x35, 0x36, 0x20, 0x43,
			0x41, 0x30, 0x76, 0x30, 0x10, 0x06, 0x07, 0x2a, 0x86, 0x48, 0xce, 0x3d,
			0x02, 0x01, 0x06, 0x05, 0x2b, 0x81, 0x04, 0x00, 0x22, 0x03, 0x62, 0x00,
			0x04, 0x99, 0x8f, 0x81, 0x68, 0x9a, 0x83, 0x9b, 0x83, 0x39, 0xad, 0x0e,
			0x32, 0x8d, 0xb9, 0x42, 0x0d, 0xae, 0xcc, 0x91, 0xa9, 0xbc, 0x4a, 0xe1,
			0xbb, 0x79, 0x4c, 0x22, 0xfa, 0x3f, 0x0c, 0x9d, 0x93, 0x3c, 0x1a, 0x02,
			0x5c, 0xc2, 0x73, 0x05, 0xec, 0x43, 0x5d, 0x04, 0x02, 0xb1, 0x68, 0xb3,
			0xf4, 0xd8, 0xde, 0x0c, 0x8d, 0x53, 0xb7, 0x04, 0x8e, 0xa1, 0x43, 0x9a,
			0xeb, 0x31, 0x0d, 0xaa, 0xce, 0x89, 0x2d, 0xba, 0x73, 0xda, 0x4f, 0x1e,
			0x39, 0x5d, 0x92, 0x11, 0x21, 0x38, 0xb4, 0x00, 0xd4, 0xf5, 0x55, 0x8c,
			0xe8, 0x71, 0x30, 0x3d, 0x46, 0x83, 0xf4, 0xc4, 0x52, 0x50, 0xda, 0x12,
			0x5b, 0xa3, 0x53, 0x30, 0x51, 0x30, 0x1d, 0x06, 0x03, 0x55, 0x1d, 0x0e,
			0x04, 0x16, 0x04, 0x14, 0xcf, 0x09, 0xd4, 0x7a, 0xee, 0x08, 0x90, 0x62,
			0xbf, 0xe6, 0x9c, 0xb4, 0xb9, 0xdf, 0xe1, 0x41, 0x33, 0x1c, 0x03, 0xa5,
			0x30, 0x1f, 0x06, 0x03, 0x55, 0x1d, 0x23, 0x04, 0x18, 0x30, 0x16, 0x80,
			0x14, 0xcf, 0x09, 0xd4, 0x7a, 0xee, 0x08, 0x90, 0x62, 0xbf, 0xe6, 0x9c,
			0xb4, 0xb9, 0xdf, 0xe1, 0x41, 0x33, 0x1c, 0x03, 0xa5, 0x30, 0x0f, 0x06,
			0x03, 0x55, 0x1d, 0x13, 0x01, 0x01, 0xff, 0x04, 0x05, 0x30, 0x03, 0x01,
			0x01, 0xff, 0x30, 0x0a, 0x06, 0x08, 0x2a, 0x86, 0x48, 0xce, 0x3d, 0x04,
			0x03, 0x03, 0x03, 0x67, 0x00, 0x30, 0x64, 0x02, 0x30, 0x5a, 0xb4, 0xf5,
			0x95, 0x25, 0x82, 0xf6, 0x68, 0x3e, 0x49, 0xc7, 0xb4, 0xbb, 0x42, 0x81,
			0x91, 0x7e, 0x38, 0xd0, 0x2d, 0xac, 0x53, 0xae, 0x8e, 0xb0, 0x51, 0x50,
			0xaa, 0xf8, 0x7e, 0xff, 0xc0, 0x30, 0xab, 0xd5, 0x08, 0x5b, 0x06, 0xf7,
			0xe1, 0xbf, 0x39, 0xd2, 0x3e, 0xae, 0xbf, 0x8e, 0x48, 0x02, 0x30, 0x09,
			0x75, 0xa8, 0xc0, 0x6f, 0x4f, 0x3c, 0xad, 0x5d, 0x4e, 0x4f, 0xf8, 0x2c,
			0x3b, 0x39, 0x46, 0xa0, 0xdf, 0x83, 0x8e, 0xb5, 0xd3, 0x61, 0x61, 0x59,
			0xbc, 0x39, 0xd7, 0xad, 0x68, 0x5e, 0x0d, 0x4f, 0x3f, 0xe2, 0xca, 0xc1,
			0x74, 0x8f, 0x47, 0x37, 0x11, 0xc8, 0x22, 0x59, 0x6f, 0x64, 0x52, 0x30,
			0x82, 0x01, 0xd7, 0x30, 0x82, 0x01, 0x5d, 0xa0, 0x03, 0x02, 0x01, 0x02,
			0x02, 0x01, 0x01, 0x30, 0x0a, 0x06, 0x08, 0x2a, 0x86, 0x48, 0xce, 0x3d,
			0x04, 0x03, 0x03, 0x30, 0x1f, 0x31, 0x1d, 0x30, 0x1b, 0x06, 0x03, 0x55,
			0x04, 0x03, 0x0c, 0x14, 0x69, 0x6e, 0x74, 0x65, 0x6c, 0x20, 0x74, 0x65,
			0x73, 0x74, 0x20, 0x45, 0x43, 0x50, 0x32, 0x35, 0x36, 0x20, 0x43, 0x41,
			0x30, 0x1e, 0x17, 0x0d, 0x32, 0x31, 0x30, 0x32, 0x30, 0x39, 0x30, 0x30,
			0x35, 0x30, 0x35, 0x39, 0x5a, 0x17, 0x0d, 0x33,	0x31, 0x30, 0x32, 0x30,
			0x37, 0x30, 0x30, 0x35, 0x30, 0x35, 0x39, 0x5a, 0x30, 0x2e, 0x31, 0x2c,
			0x30, 0x2a, 0x06, 0x03, 0x55, 0x04, 0x03, 0x0c, 0x23, 0x69, 0x6e, 0x74,
			0x65, 0x6c, 0x20, 0x74, 0x65, 0x73, 0x74, 0x20, 0x45, 0x43, 0x50, 0x32,
			0x35, 0x36, 0x20, 0x69, 0x6e, 0x74, 0x65, 0x72, 0x6d, 0x65, 0x64, 0x69,
			0x61, 0x74, 0x65, 0x20, 0x63, 0x65, 0x72, 0x74, 0x30, 0x76, 0x30, 0x10,
			0x06, 0x07, 0x2a, 0x86, 0x48, 0xce, 0x3d, 0x02, 0x01, 0x06, 0x05, 0x2b,
			0x81, 0x04, 0x00, 0x22, 0x03, 0x62, 0x00, 0x04, 0x77, 0x1b, 0x24, 0xf6,
			0xc6, 0x76, 0x1f, 0xb8, 0x30, 0x07, 0x8b, 0xb8, 0xa3, 0x9e, 0xc0, 0x26,
			0xc1, 0xea, 0x7d, 0xfc, 0x29, 0x7d, 0xe0, 0x59, 0xb2, 0x64, 0x32, 0x75,
			0x4a, 0xe3, 0x02, 0x64, 0x3c, 0xbc, 0x85, 0x8e, 0xc6, 0xec, 0xef, 0xb0,
			0x79, 0xf4, 0xc1, 0xa4, 0xb9, 0xbb, 0x29, 0x6b, 0xae, 0xad, 0xf0, 0x7d,
			0x63, 0xc6, 0xaf, 0xb3, 0x73, 0x5e, 0x4f, 0x3f, 0xfe, 0x89, 0x8a, 0xbb,
			0x7d, 0x2b, 0x60, 0x3e, 0x16, 0xba, 0x82, 0xcf, 0xa4, 0x70, 0x04, 0x85,
			0xc3, 0xa3, 0x3c, 0x5e, 0x6a, 0xa0, 0xef, 0xda, 0xd5, 0x20, 0x30, 0x19,
			0xba, 0x79, 0x95, 0xb0, 0xc2, 0x7f, 0x4c, 0xdd, 0xa3, 0x5e, 0x30, 0x5c,
			0x30, 0x0c, 0x06, 0x03, 0x55, 0x1d, 0x13, 0x04, 0x05, 0x30, 0x03, 0x01,
			0x01, 0xff, 0x30, 0x0b,	0x06, 0x03, 0x55, 0x1d, 0x0f, 0x04, 0x04, 0x03,
			0x02, 0x01, 0xfe, 0x30, 0x1d, 0x06, 0x03, 0x55, 0x1d, 0x0e, 0x04, 0x16,
			0x04, 0x14, 0x12, 0xe0, 0x1a, 0x23, 0xc6, 0x23, 0xe4, 0x02, 0x58, 0x0b,
			0x06, 0xac, 0x90, 0xfa, 0x4b, 0x80, 0x3d, 0xc9, 0xf1, 0x1d, 0x30, 0x20,
			0x06, 0x03, 0x55, 0x1d, 0x25, 0x01, 0x01, 0xff, 0x04, 0x16, 0x30, 0x14,
			0x06, 0x08, 0x2b, 0x06, 0x01, 0x05, 0x05, 0x07, 0x03, 0x01, 0x06, 0x08,
			0x2b, 0x06, 0x01, 0x05, 0x05, 0x07, 0x03, 0x02, 0x30, 0x0a, 0x06, 0x08,
			0x2a, 0x86, 0x48, 0xce, 0x3d, 0x04, 0x03, 0x03, 0x03, 0x68, 0x00, 0x30,
			0x65, 0x02, 0x30, 0x03, 0x32, 0xb1, 0x8b, 0x20, 0xf4, 0x76, 0xda, 0x8c,
			0x83, 0x96, 0x87, 0x55, 0xd9, 0x12, 0x72, 0xbd, 0x58, 0x4d, 0x0a, 0x37,
			0xaf, 0x29, 0x95, 0x1d, 0x36, 0xc4, 0x9e, 0xa5, 0xcd, 0xe2, 0x3b, 0xf5,
			0xe0, 0x7a, 0x64, 0x36, 0x1e, 0xd4, 0xf1, 0xe1, 0xbb, 0x14, 0x57, 0x9e,
			0x86, 0x82, 0x72, 0x02, 0x31, 0x00, 0xc0, 0xd6, 0x02, 0x99, 0x50, 0x76,
			0x34, 0x16, 0xd6, 0x51, 0x9c, 0xc4, 0x86, 0x08, 0x68, 0x94, 0xbf, 0x3c,
			0x09, 0x7e, 0x10, 0xe5, 0x62, 0x8a, 0xba, 0x48, 0x0a, 0xa5, 0xed, 0x1a,
			0x6a, 0xf6, 0x3c, 0x2f, 0x4d, 0x38, 0x5d, 0x7d, 0x5c, 0x60, 0x63, 0x88,
			0x84, 0x5d, 0x49, 0x33, 0xe2, 0xa7, 0x30, 0x82, 0x02, 0x22, 0x30, 0x82,
			0x01, 0xa8, 0xa0, 0x03, 0x02, 0x01, 0x02, 0x02, 0x01, 0x03, 0x30, 0x0a,
			0x06, 0x08, 0x2a, 0x86, 0x48, 0xce, 0x3d, 0x04, 0x03, 0x03, 0x30, 0x2e,
			0x31, 0x2c, 0x30, 0x2a, 0x06, 0x03, 0x55, 0x04, 0x03, 0x0c, 0x23, 0x69,
			0x6e, 0x74, 0x65, 0x6c, 0x20, 0x74, 0x65, 0x73, 0x74, 0x20, 0x45, 0x43,
			0x50, 0x32, 0x35, 0x36, 0x20, 0x69, 0x6e, 0x74, 0x65, 0x72, 0x6d, 0x65,
			0x64, 0x69, 0x61, 0x74, 0x65, 0x20, 0x63, 0x65, 0x72, 0x74, 0x30, 0x1e,
			0x17, 0x0d, 0x32, 0x31, 0x30, 0x32, 0x30, 0x39, 0x30, 0x30, 0x35, 0x30,
			0x35, 0x39, 0x5a, 0x17, 0x0d, 0x32, 0x32, 0x30, 0x32, 0x30, 0x39, 0x30,
			0x30, 0x35, 0x30, 0x35, 0x39, 0x5a, 0x30, 0x2b, 0x31, 0x29, 0x30, 0x27,
			0x06, 0x03, 0x55, 0x04, 0x03, 0x0c, 0x20, 0x69, 0x6e, 0x74, 0x65, 0x6c,
			0x20, 0x74, 0x65, 0x73, 0x74, 0x20, 0x45, 0x43, 0x50, 0x32, 0x35, 0x36,
			0x20, 0x72, 0x65, 0x73, 0x70, 0x6f, 0x6e, 0x64, 0x65, 0x72, 0x20, 0x63,
			0x65, 0x72, 0x74, 0x30, 0x76, 0x30, 0x10, 0x06, 0x07, 0x2a, 0x86, 0x48,
			0xce, 0x3d, 0x02, 0x01, 0x06, 0x05, 0x2b, 0x81, 0x04, 0x00, 0x22, 0x03,
			0x62, 0x00, 0x04, 0x6c, 0x22, 0x41, 0xdf, 0xb7, 0xe4, 0xd6, 0x8d, 0x53,
			0x72, 0x4e, 0x4a, 0x1b, 0x99, 0x82, 0xe6, 0x56, 0xd2, 0x2d, 0x97, 0x4b,
			0x98, 0x40, 0xa9, 0x99, 0xd6, 0x0d, 0xd8, 0xe9,	0xa6, 0xfc, 0x74, 0xb9,
			0xce, 0x89, 0x48, 0xa7, 0xb5, 0x09, 0xb6, 0x24, 0x49, 0xd6, 0x23, 0xb3,
			0x5f, 0x3a, 0xf0, 0x99, 0xb0, 0xca, 0x63, 0x7d, 0x24, 0xfe, 0xe9, 0x12,
			0x19, 0x0f, 0xc2, 0x73, 0x1c, 0xe3, 0x76, 0x91, 0xec, 0x57, 0x6c, 0xcd,
			0x7b, 0xab, 0x32, 0xfd, 0x6d, 0x6e, 0x92, 0x7d, 0x37, 0x60, 0x01, 0xdb,
			0x13, 0x92, 0x3b, 0x77, 0xf7, 0x12, 0x97, 0x1d, 0x5e, 0xe3, 0xb9, 0x15,
			0x83, 0xaf, 0x89, 0xa3, 0x81, 0x9c, 0x30, 0x81, 0x99, 0x30, 0x0c, 0x06,
			0x03, 0x55, 0x1d, 0x13, 0x01, 0x01, 0xff, 0x04, 0x02, 0x30, 0x00, 0x30,
			0x0b, 0x06, 0x03, 0x55, 0x1d, 0x0f, 0x04, 0x04, 0x03, 0x02, 0x05, 0xe0,
			0x30, 0x1d, 0x06, 0x03, 0x55, 0x1d, 0x0e, 0x04, 0x16, 0x04, 0x14, 0x48,
			0x1f, 0x5d, 0x95, 0xce, 0x89, 0xd4, 0x7d, 0xa4, 0x4c, 0x21, 0x8f, 0x5b,
			0xd5, 0x50, 0x96, 0xff, 0xba, 0xe2, 0xee, 0x30, 0x31, 0x06, 0x03, 0x55,
			0x1d, 0x11, 0x04, 0x2a, 0x30, 0x28, 0xa0, 0x26, 0x06, 0x0a, 0x2b, 0x06,
			0x01, 0x04, 0x01, 0x83, 0x1c, 0x82, 0x12, 0x01, 0xa0, 0x18, 0x0c, 0x16,
			0x41, 0x43, 0x4d, 0x45, 0x3a, 0x57, 0x49, 0x44, 0x47, 0x45, 0x54, 0x3a,
			0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x30, 0x30, 0x2a,
			0x06, 0x03, 0x55, 0x1d, 0x25, 0x01, 0x01, 0xff, 0x04, 0x20, 0x30, 0x1e,
			0x06, 0x08, 0x2b, 0x06,	0x01, 0x05, 0x05, 0x07, 0x03, 0x01, 0x06, 0x08,
			0x2b, 0x06, 0x01, 0x05, 0x05, 0x07, 0x03, 0x02, 0x06, 0x08, 0x2b, 0x06,
			0x01, 0x05, 0x05, 0x07, 0x03, 0x09, 0x30, 0x0a, 0x06, 0x08, 0x2a, 0x86,
			0x48, 0xce, 0x3d, 0x04, 0x03, 0x03, 0x03, 0x68, 0x00, 0x30, 0x65, 0x02,
			0x30, 0x08, 0xe6, 0x1f, 0x0d, 0xdf, 0x18, 0xd3, 0x2f, 0x50, 0x49, 0x99,
			0xb0, 0xe2, 0x64, 0x95, 0x30, 0xa9, 0x5a, 0xbf, 0x83, 0x76, 0xae, 0x4a,
			0x39, 0xd8, 0xe2, 0x51, 0x12, 0x84, 0x9c, 0xbe, 0x11, 0x1d, 0x3b, 0x77,
			0x20, 0x6f, 0x05, 0x6c, 0xc7, 0x98, 0xb2, 0xba, 0xb8, 0x96, 0x75, 0x25,
			0xcf, 0x02, 0x31, 0x00, 0x93, 0x12, 0x5b, 0x66, 0x93, 0xc0, 0xe7, 0x56,
			0x1b, 0x68, 0x28, 0x27, 0xd8, 0x8e, 0x69, 0xaa, 0x30, 0x76, 0x05, 0x6f,
			0x4b, 0xd0, 0xce, 0x10, 0x0f, 0xf8, 0xdf, 0x4a, 0xab, 0x9b, 0x4d, 0xb1,
			0x47, 0xe4, 0xcd, 0xce, 0xce, 0x48, 0x0d, 0xf8, 0x35, 0x3d, 0xbc, 0x25,
			0xce, 0xec, 0xb9, 0xca
	};
    alt_u8 mctp_cert_packet_with_cert_chain[500] = {0};

    alt_u8 mctp_get_cert_pkt[15] = MCTP_SPDM_GET_CERTIFICATE_MSG;
    mctp_get_cert_pkt[UT_MCTP_SOM_EOM_BYTE_IDX] = FIRST_AND_LAST_PKT | TAG_OWNER;
    mctp_get_cert_pkt[UT_MCTP_BYTE_COUNT_IDX] = 0x0e;
    mctp_get_cert_pkt[UT_MCTP_SOURCE_ADDR] = SRC_SLAVE_ADDR_WR;

    alt_u16* mctp_get_cert_pkt_ptr = (alt_u16*)&mctp_get_cert_pkt[UT_MCTP_SPDM_GET_CERTIFICATE_LENGTH_MASK - 1];
    *mctp_get_cert_pkt_ptr = CERT_REQUEST_SIZE;
    alt_u16* mctp_get_cert_pkt_ptr_2 = (alt_u16*)&mctp_get_cert_pkt[UT_MCTP_SPDM_GET_CERTIFICATE_OFFSET_MASK - 1];
    *mctp_get_cert_pkt_ptr_2 = 0;

    alt_u8 mctp_cert_pkt[15] = MCTP_SPDM_CERTIFICATE_MSG;
    mctp_cert_pkt[UT_MCTP_SOM_EOM_BYTE_IDX] = FIRST_AND_LAST_PKT;
    mctp_cert_pkt[UT_MCTP_BYTE_COUNT_IDX] = 206 + 8;
    mctp_cert_pkt[UT_MCTP_SOURCE_ADDR] = (BMC_SLAVE_ADDRESS << 1);

    alt_u16* mctp_get_pkt_ptr = (alt_u16*)&mctp_cert_pkt[UT_MCTP_SPDM_CERTIFICATE_PORTION_LENGTH_MASK - 1];
    *mctp_get_pkt_ptr = 0xc8;
    alt_u16* mctp_get_pkt_ptr_2 = (alt_u16*)&mctp_cert_pkt[UT_MCTP_SPDM_CERTIFICATE_REMAINDER_LENGTH_MASK - 1];
    *mctp_get_pkt_ptr_2 = 0x540;

    for (alt_u32 i = 0; i < 15; i++)
    {
        mctp_cert_packet_with_cert_chain[i] = mctp_cert_pkt[i];
    }

    for (alt_u32 i = 15; i < 15 + 200; i++)
    {
        mctp_cert_packet_with_cert_chain[i] = cert_chain_buffer[i - 15];
    }

    spdm_context_ptr->local_context.cert_flag = 1;
    alt_u8* stored_digest_buf_ptr = (alt_u8*)&spdm_context_ptr->local_context.stored_digest_buf[0];

    const alt_u8 expected_hash[SHA384_LENGTH] = {
        0x28, 0xaf, 0x70, 0x27, 0xbc, 0x2d, 0x95, 0xb5, 0xa0, 0xe4, 0x26, 0x04,
		0xc5, 0x8c, 0x5c, 0x3c, 0xbf, 0xa2, 0xc8, 0x24, 0xa6, 0x30, 0xca, 0x2f,
		0x0f, 0x4a, 0x79, 0x35, 0x57, 0xfb, 0x39, 0x3b, 0xdd, 0x8a, 0xc8, 0x8a,
		0x92, 0xd8, 0xa3, 0x70, 0x17, 0x12, 0x83, 0x9b, 0x66, 0xe1, 0x3a, 0x3a
    };

    for (alt_u32 i = 0; i < SHA384_LENGTH; i++)
    {
        stored_digest_buf_ptr[i] = expected_hash[i];
    }

    // Send in the packet to RXFIFO
    // Need not stage the message as only one message exchange, hence set last param to 0
    ut_send_in_bmc_mctp_packet(m_memory, mctp_cert_packet_with_cert_chain, 207 + 8, 1, 0);

    // Set remainder
    *mctp_get_pkt_ptr_2 = 0x478;

    for (alt_u32 i = 0; i < 15; i++)
    {
        mctp_cert_packet_with_cert_chain[i] = mctp_cert_pkt[i];
    }

    for (alt_u32 i = 15; i < 15 + 200; i++)
    {
        mctp_cert_packet_with_cert_chain[i] = cert_chain_buffer[i - 15 + 200];
    }

    ut_send_in_bmc_mctp_packet(m_memory, mctp_cert_packet_with_cert_chain, 207 + 8, 1, 0);

    // Set remainder
    *mctp_get_pkt_ptr_2 = 0x3b0;

    for (alt_u32 i = 0; i < 15; i++)
    {
        mctp_cert_packet_with_cert_chain[i] = mctp_cert_pkt[i];
    }

    for (alt_u32 i = 15; i < 15 + 200; i++)
    {
        mctp_cert_packet_with_cert_chain[i] = cert_chain_buffer[i - 15 + 200 + 200];
    }

    ut_send_in_bmc_mctp_packet(m_memory, mctp_cert_packet_with_cert_chain, 207 + 8, 1, 0);

    // Set remainder
    *mctp_get_pkt_ptr_2 = 0x2e8;

    for (alt_u32 i = 0; i < 15; i++)
    {
        mctp_cert_packet_with_cert_chain[i] = mctp_cert_pkt[i];
    }

    for (alt_u32 i = 15; i < 15 + 200; i++)
    {
        mctp_cert_packet_with_cert_chain[i] = cert_chain_buffer[i - 15 + 200 + 200 + 200];
    }

    ut_send_in_bmc_mctp_packet(m_memory, mctp_cert_packet_with_cert_chain, 207 + 8, 1, 0);

    // Set remainder
    *mctp_get_pkt_ptr_2 = 0x220;

    for (alt_u32 i = 0; i < 15; i++)
    {
        mctp_cert_packet_with_cert_chain[i] = mctp_cert_pkt[i];
    }

    for (alt_u32 i = 15; i < 15 + 200; i++)
    {
        mctp_cert_packet_with_cert_chain[i] = cert_chain_buffer[i - 15 + 200 + 200 + 200 + 200];
    }

    ut_send_in_bmc_mctp_packet(m_memory, mctp_cert_packet_with_cert_chain, 207 + 8, 1, 0);

    // Set remainder
    *mctp_get_pkt_ptr_2 = 0x158;

    for (alt_u32 i = 0; i < 15; i++)
    {
        mctp_cert_packet_with_cert_chain[i] = mctp_cert_pkt[i];
    }

    for (alt_u32 i = 15; i < 15 + 200; i++)
    {
        mctp_cert_packet_with_cert_chain[i] = cert_chain_buffer[i - 15 + 200 + 200 + 200 + 200 + 200];
    }

    ut_send_in_bmc_mctp_packet(m_memory, mctp_cert_packet_with_cert_chain, 207 + 8, 1, 0);

    // Set remainder
    *mctp_get_pkt_ptr_2 = 0x90;

    for (alt_u32 i = 0; i < 15; i++)
    {
        mctp_cert_packet_with_cert_chain[i] = mctp_cert_pkt[i];
    }

    for (alt_u32 i = 15; i < 15 + 200; i++)
    {
        mctp_cert_packet_with_cert_chain[i] = cert_chain_buffer[i - 15 + 200 + 200 + 200 + 200 + 200 + 200];
    }

    ut_send_in_bmc_mctp_packet(m_memory, mctp_cert_packet_with_cert_chain, 207 + 8, 1, 0);

    // Set remainder
    *mctp_get_pkt_ptr_2 = 0x00;
    *mctp_get_pkt_ptr = 0x90;
    mctp_cert_pkt[UT_MCTP_BYTE_COUNT_IDX] = 206 + 8 - 56;

    for (alt_u32 i = 0; i < 15; i++)
    {
        mctp_cert_packet_with_cert_chain[i] = mctp_cert_pkt[i];
    }

    for (alt_u32 i = 15; i < 15 + 200 - 56; i++)
    {
        mctp_cert_packet_with_cert_chain[i] = cert_chain_buffer[i - 15 + 200 + 200 + 200 + 200 + 200 + 200 + 200];
    }

    ut_send_in_bmc_mctp_packet(m_memory, mctp_cert_packet_with_cert_chain, 207 + 8 - 56, 0, 0);

    EXPECT_EQ(requester_cpld_get_certificate(spdm_context_ptr, mctp_context_ptr, BMC), alt_u32(0));

    EXPECT_EQ((alt_u32)(BMC_SLAVE_ADDRESS << 1), IORD(m_mctp_memory, 0));
    EXPECT_EQ((alt_u32)MCTP_COMMAND_CODE, IORD(m_mctp_memory, 0));
    for (alt_u32 i = 0; i < 15; i++)
    {
        EXPECT_EQ(mctp_get_cert_pkt[i], IORD(m_mctp_memory, 0));
    }

    // Expect CPLD to obtain the offset from previous cert chain
    *mctp_get_cert_pkt_ptr_2 = 0xc8;

    EXPECT_EQ((alt_u32)(BMC_SLAVE_ADDRESS << 1), IORD(m_mctp_memory, 0));
    EXPECT_EQ((alt_u32)MCTP_COMMAND_CODE, IORD(m_mctp_memory, 0));
    for (alt_u32 i = 0; i < 15; i++)
    {
        EXPECT_EQ(mctp_get_cert_pkt[i], IORD(m_mctp_memory, 0));
    }

    // Expect that CPLD request the remaining portion of the certificate
    *mctp_get_cert_pkt_ptr = 0xc8;
    // Expect CPLD to obtain the offset from previous cert chain
    *mctp_get_cert_pkt_ptr_2 = 0x190;

    EXPECT_EQ((alt_u32)(BMC_SLAVE_ADDRESS << 1), IORD(m_mctp_memory, 0));
    EXPECT_EQ((alt_u32)MCTP_COMMAND_CODE, IORD(m_mctp_memory, 0));
    for (alt_u32 i = 0; i < 15; i++)
    {
        EXPECT_EQ(mctp_get_cert_pkt[i], IORD(m_mctp_memory, 0));
    }

    // Expect that CPLD request the remaining portion of the certificate
    *mctp_get_cert_pkt_ptr = 0xc8;
    // Expect CPLD to obtain the offset from previous cert chain
    *mctp_get_cert_pkt_ptr_2 = 0x258;

    EXPECT_EQ((alt_u32)(BMC_SLAVE_ADDRESS << 1), IORD(m_mctp_memory, 0));
    EXPECT_EQ((alt_u32)MCTP_COMMAND_CODE, IORD(m_mctp_memory, 0));
    for (alt_u32 i = 0; i < 15; i++)
    {
        EXPECT_EQ(mctp_get_cert_pkt[i], IORD(m_mctp_memory, 0));
    }

    // Expect that CPLD request the remaining portion of the certificate
    *mctp_get_cert_pkt_ptr = 0xc8;
    // Expect CPLD to obtain the offset from previous cert chain
    *mctp_get_cert_pkt_ptr_2 = 0x320;

    EXPECT_EQ((alt_u32)(BMC_SLAVE_ADDRESS << 1), IORD(m_mctp_memory, 0));
    EXPECT_EQ((alt_u32)MCTP_COMMAND_CODE, IORD(m_mctp_memory, 0));
    for (alt_u32 i = 0; i < 15; i++)
    {
        EXPECT_EQ(mctp_get_cert_pkt[i], IORD(m_mctp_memory, 0));
    }

    // Expect that CPLD request the remaining portion of the certificate
    *mctp_get_cert_pkt_ptr = 0xc8;
    // Expect CPLD to obtain the offset from previous cert chain
    *mctp_get_cert_pkt_ptr_2 = 0x3e8;

    EXPECT_EQ((alt_u32)(BMC_SLAVE_ADDRESS << 1), IORD(m_mctp_memory, 0));
    EXPECT_EQ((alt_u32)MCTP_COMMAND_CODE, IORD(m_mctp_memory, 0));
    for (alt_u32 i = 0; i < 15; i++)
    {
        EXPECT_EQ(mctp_get_cert_pkt[i], IORD(m_mctp_memory, 0));
    }

    // Expect that CPLD request the remaining portion of the certificate
    *mctp_get_cert_pkt_ptr = 0xc8;
    // Expect CPLD to obtain the offset from previous cert chain
    *mctp_get_cert_pkt_ptr_2 = 0x4b0;

    EXPECT_EQ((alt_u32)(BMC_SLAVE_ADDRESS << 1), IORD(m_mctp_memory, 0));
    EXPECT_EQ((alt_u32)MCTP_COMMAND_CODE, IORD(m_mctp_memory, 0));
    for (alt_u32 i = 0; i < 15; i++)
    {
        EXPECT_EQ(mctp_get_cert_pkt[i], IORD(m_mctp_memory, 0));
    }

    // Expect that CPLD request the remaining portion of the certificate
    *mctp_get_cert_pkt_ptr = 0x90;
    // Expect CPLD to obtain the offset from previous cert chain
    *mctp_get_cert_pkt_ptr_2 = 0x578;

    EXPECT_EQ((alt_u32)(BMC_SLAVE_ADDRESS << 1), IORD(m_mctp_memory, 0));
    EXPECT_EQ((alt_u32)MCTP_COMMAND_CODE, IORD(m_mctp_memory, 0));
    for (alt_u32 i = 0; i < 15; i++)
    {
        EXPECT_EQ(mctp_get_cert_pkt[i], IORD(m_mctp_memory, 0));
    }
}

TEST_F(PFRSpdmDebugTest, test_responder_certificate_openspdm)
{
    // Reset Nios firmware
    ut_reset_nios_fw();

    // Exit after 50 iterations in the T0 loop
    SYSTEM_MOCK::get()->insert_code_block(SYSTEM_MOCK::CODE_BLOCK_TYPES::T0_OPERATIONS_END_AFTER_50_ITERS);

    // Set asserts to throw as opposed to abort
    SYSTEM_MOCK::get()->set_assert_to_throw();
    // Throw after performing CFM switch
    SYSTEM_MOCK::get()->insert_code_block(SYSTEM_MOCK::CODE_BLOCK_TYPES::THROW_AFTER_CFM_SWITCH);

    // Run Nios FW through PFR/Recovery Main to generate keys
    ut_run_main(CPLD_CFM0, true);

    // Initialise context
    // Move to separate function
    SPDM_CONTEXT spdm_context;
    SPDM_CONTEXT* spdm_context_ptr = &spdm_context;

    // Reset everything
    reset_buffer((alt_u8*)spdm_context_ptr, sizeof(SPDM_CONTEXT));

    MCTP_CONTEXT mctp_context;
    MCTP_CONTEXT* mctp_context_ptr = &mctp_context;

    reset_buffer((alt_u8*)mctp_context_ptr, sizeof(MCTP_CONTEXT));

    init_spdm_context(spdm_context_ptr);
    mctp_context_ptr->cached_addr = BMC_SLAVE_ADDRESS;
    mctp_context_ptr->msg_type = MCTP_MESSAGE_TYPE;

    alt_u8 mctp_cert_packet_with_cert_chain[500] = {0};

    alt_u8* pkt = (alt_u8*)&mctp_context_ptr->processed_message.buffer;
    alt_u8 mctp_get_cert_pkt[15] = MCTP_SPDM_GET_CERTIFICATE_MSG;
    mctp_get_cert_pkt[UT_MCTP_SOM_EOM_BYTE_IDX] = FIRST_AND_LAST_PKT | TAG_OWNER;
    mctp_get_cert_pkt[UT_MCTP_BYTE_COUNT_IDX] = 0x0e;
    mctp_get_cert_pkt[UT_MCTP_SOURCE_ADDR] = ((BMC_SLAVE_ADDRESS << 1) | 0b1);
    mctp_get_cert_pkt[UT_MCTP_SPDM_GET_CERTIFICATE_OFFSET_MASK] = 0;
    //mctp_get_cert_pkt[UT_MCTP_SPDM_GET_CERTIFICATE_LENGTH_MASK - 1] = (LARGE_SPDM_MESSAGE_MAX_BUFFER_SIZE/4) >> 8;
    alt_u16* mctp_get_cert_pkt_ptr = (alt_u16*)&mctp_get_cert_pkt[UT_MCTP_SPDM_GET_CERTIFICATE_LENGTH_MASK - 1];
    *mctp_get_cert_pkt_ptr = 0x400;
    mctp_get_cert_pkt_ptr = (alt_u16*)&mctp_get_cert_pkt[UT_MCTP_SPDM_GET_CERTIFICATE_OFFSET_MASK - 1];
    *mctp_get_cert_pkt_ptr = 0;

    alt_u8_memcpy(pkt, (alt_u8*)&mctp_get_cert_pkt[7], 15 - 7);

    alt_u8 mctp_cert_pkt[15] = MCTP_SPDM_CERTIFICATE_MSG;
    mctp_cert_pkt[UT_MCTP_SOM_EOM_BYTE_IDX] = FIRST_AND_LAST_PKT;
    mctp_cert_pkt[UT_MCTP_BYTE_COUNT_IDX] = 0xd6;
    mctp_cert_pkt[UT_MCTP_SOURCE_ADDR] = SRC_SLAVE_ADDR_WR;
    mctp_cert_pkt[UT_MCTP_SPDM_CERTIFICATE_PORTION_LENGTH_MASK] = 0;
    mctp_cert_pkt[UT_MCTP_SPDM_CERTIFICATE_REMAINDER_LENGTH_MASK] = 0;
    alt_u16* mctp_get_pkt_ptr = (alt_u16*)&mctp_cert_pkt[UT_MCTP_SPDM_CERTIFICATE_PORTION_LENGTH_MASK - 1];
    *mctp_get_pkt_ptr = 0xc8;
    mctp_get_pkt_ptr = (alt_u16*)&mctp_cert_pkt[UT_MCTP_SPDM_CERTIFICATE_REMAINDER_LENGTH_MASK - 1];
    alt_u16 cert_length = *((alt_u16*)get_ufm_cpld_cert_ptr());
    *mctp_get_pkt_ptr = cert_length - 0xc8;

    //alt_u8* stored_cert_chain = (alt_u8*) get_ufm_cpld_cert_ptr();
    for (alt_u8 i = 0; i < 15; i++)
    {
        mctp_cert_packet_with_cert_chain[i] = mctp_cert_pkt[i];
    }

    alt_u8* cert = (alt_u8*) get_ufm_cpld_cert_ptr();
    for (alt_u8 i = 15; i < 15 + 0xc8; i++)
    {
        mctp_cert_packet_with_cert_chain[i] = cert[i - 15];
    }

    EXPECT_EQ(responder_cpld_respond_certificate(spdm_context_ptr, mctp_context_ptr, sizeof(GET_CERTIFICATE), BMC), alt_u32(0));

    EXPECT_EQ((alt_u32)(BMC_SLAVE_ADDRESS << 1), IORD(m_mctp_memory, 0));
    EXPECT_EQ((alt_u32)MCTP_COMMAND_CODE, IORD(m_mctp_memory, 0));
    for (alt_u32 i = 0; i < 15 + 0xc8; i++)
    {
        EXPECT_EQ(mctp_cert_packet_with_cert_chain[i], IORD(m_mctp_memory, 0));
    }


    mctp_get_cert_pkt[UT_MCTP_SPDM_GET_CERTIFICATE_OFFSET_MASK] = 0;
    //mctp_get_cert_pkt[UT_MCTP_SPDM_GET_CERTIFICATE_LENGTH_MASK - 1] = (LARGE_SPDM_MESSAGE_MAX_BUFFER_SIZE/4) >> 8;
    mctp_get_cert_pkt_ptr = (alt_u16*)&mctp_get_cert_pkt[UT_MCTP_SPDM_GET_CERTIFICATE_LENGTH_MASK - 1];
    *mctp_get_cert_pkt_ptr = 0x400;
    mctp_get_cert_pkt_ptr = (alt_u16*)&mctp_get_cert_pkt[UT_MCTP_SPDM_GET_CERTIFICATE_OFFSET_MASK - 1];
    *mctp_get_cert_pkt_ptr = 0xc8;

    alt_u8_memcpy(pkt, (alt_u8*)&mctp_get_cert_pkt[7], 15 - 7);

    mctp_cert_pkt[UT_MCTP_SPDM_CERTIFICATE_PORTION_LENGTH_MASK] = 0;
    mctp_cert_pkt[UT_MCTP_SPDM_CERTIFICATE_REMAINDER_LENGTH_MASK] = 0;
    mctp_get_pkt_ptr = (alt_u16*)&mctp_cert_pkt[UT_MCTP_SPDM_CERTIFICATE_PORTION_LENGTH_MASK - 1];
    *mctp_get_pkt_ptr = 0xc8;
    mctp_get_pkt_ptr = (alt_u16*)&mctp_cert_pkt[UT_MCTP_SPDM_CERTIFICATE_REMAINDER_LENGTH_MASK - 1];
    *mctp_get_pkt_ptr = cert_length - 0xc8 - 0xc8;

    //alt_u8* stored_cert_chain = (alt_u8*) get_ufm_cpld_cert_ptr();
    for (alt_u8 i = 0; i < 15; i++)
    {
        mctp_cert_packet_with_cert_chain[i] = mctp_cert_pkt[i];
    }

    //cert = (alt_u8*) get_ufm_cpld_cert_ptr();
    cert += 0xc8;

    for (alt_u8 i = 15; i < 15 + 0xc8; i++)
    {
        mctp_cert_packet_with_cert_chain[i] = cert[i - 15];
    }

    EXPECT_EQ(responder_cpld_respond_certificate(spdm_context_ptr, mctp_context_ptr, sizeof(GET_CERTIFICATE), BMC), alt_u32(0));

    EXPECT_EQ((alt_u32)(BMC_SLAVE_ADDRESS << 1), IORD(m_mctp_memory, 0));
    EXPECT_EQ((alt_u32)MCTP_COMMAND_CODE, IORD(m_mctp_memory, 0));
    for (alt_u32 i = 0; i < 15 + 0xc8; i++)
    {
        EXPECT_EQ(mctp_cert_packet_with_cert_chain[i], IORD(m_mctp_memory, 0));
    }

    mctp_get_cert_pkt[UT_MCTP_SPDM_GET_CERTIFICATE_OFFSET_MASK] = 0;
    //mctp_get_cert_pkt[UT_MCTP_SPDM_GET_CERTIFICATE_LENGTH_MASK - 1] = (LARGE_SPDM_MESSAGE_MAX_BUFFER_SIZE/4) >> 8;
    mctp_get_cert_pkt_ptr = (alt_u16*)&mctp_get_cert_pkt[UT_MCTP_SPDM_GET_CERTIFICATE_LENGTH_MASK - 1];
    *mctp_get_cert_pkt_ptr = 0x400;
    mctp_get_cert_pkt_ptr = (alt_u16*)&mctp_get_cert_pkt[UT_MCTP_SPDM_GET_CERTIFICATE_OFFSET_MASK - 1];
    *mctp_get_cert_pkt_ptr = 0x190;

    alt_u8_memcpy(pkt, (alt_u8*)&mctp_get_cert_pkt[7], 15 - 7);

    mctp_cert_pkt[UT_MCTP_SPDM_CERTIFICATE_PORTION_LENGTH_MASK] = 0;
    mctp_cert_pkt[UT_MCTP_SPDM_CERTIFICATE_REMAINDER_LENGTH_MASK] = 0;
    mctp_get_pkt_ptr = (alt_u16*)&mctp_cert_pkt[UT_MCTP_SPDM_CERTIFICATE_PORTION_LENGTH_MASK - 1];
    *mctp_get_pkt_ptr = 0xc8;
    mctp_get_pkt_ptr = (alt_u16*)&mctp_cert_pkt[UT_MCTP_SPDM_CERTIFICATE_REMAINDER_LENGTH_MASK - 1];
    *mctp_get_pkt_ptr = cert_length - 0xc8 - 0xc8 - 0xc8;

    //alt_u8* stored_cert_chain = (alt_u8*) get_ufm_cpld_cert_ptr();
    for (alt_u8 i = 0; i < 15; i++)
    {
        mctp_cert_packet_with_cert_chain[i] = mctp_cert_pkt[i];
    }

    //cert = (alt_u8*) get_ufm_cpld_cert_ptr();
    cert += 0xc8;

    for (alt_u8 i = 15; i < 15 + 0xc8; i++)
    {
        mctp_cert_packet_with_cert_chain[i] = cert[i - 15];
    }

    EXPECT_EQ(responder_cpld_respond_certificate(spdm_context_ptr, mctp_context_ptr, sizeof(GET_CERTIFICATE), BMC), alt_u32(0));

    EXPECT_EQ((alt_u32)(BMC_SLAVE_ADDRESS << 1), IORD(m_mctp_memory, 0));
    EXPECT_EQ((alt_u32)MCTP_COMMAND_CODE, IORD(m_mctp_memory, 0));
    for (alt_u32 i = 0; i < 15 + 0xc8; i++)
    {
        EXPECT_EQ(mctp_cert_packet_with_cert_chain[i], IORD(m_mctp_memory, 0));
    }

    mctp_get_cert_pkt[UT_MCTP_SPDM_GET_CERTIFICATE_OFFSET_MASK] = 0;
    //mctp_get_cert_pkt[UT_MCTP_SPDM_GET_CERTIFICATE_LENGTH_MASK - 1] = (LARGE_SPDM_MESSAGE_MAX_BUFFER_SIZE/4) >> 8;
    mctp_get_cert_pkt_ptr = (alt_u16*)&mctp_get_cert_pkt[UT_MCTP_SPDM_GET_CERTIFICATE_LENGTH_MASK - 1];
    *mctp_get_cert_pkt_ptr = 0x400;
    mctp_get_cert_pkt_ptr = (alt_u16*)&mctp_get_cert_pkt[UT_MCTP_SPDM_GET_CERTIFICATE_OFFSET_MASK - 1];
    *mctp_get_cert_pkt_ptr = 0x258;

    alt_u8_memcpy(pkt, (alt_u8*)&mctp_get_cert_pkt[7], 15 - 7);

    mctp_cert_pkt[UT_MCTP_SPDM_CERTIFICATE_PORTION_LENGTH_MASK] = 0;
    mctp_cert_pkt[UT_MCTP_SPDM_CERTIFICATE_REMAINDER_LENGTH_MASK] = 0;
    mctp_get_pkt_ptr = (alt_u16*)&mctp_cert_pkt[UT_MCTP_SPDM_CERTIFICATE_PORTION_LENGTH_MASK - 1];
    *mctp_get_pkt_ptr = 0xc8;
    mctp_get_pkt_ptr = (alt_u16*)&mctp_cert_pkt[UT_MCTP_SPDM_CERTIFICATE_REMAINDER_LENGTH_MASK - 1];
    *mctp_get_pkt_ptr = cert_length - 0xc8 - 0xc8 - 0xc8 - 0xc8;

    for (alt_u8 i = 0; i < 15; i++)
    {
        mctp_cert_packet_with_cert_chain[i] = mctp_cert_pkt[i];
    }

    cert += 0xc8;

    for (alt_u8 i = 15; i < 15 + 0xc8; i++)
    {
        mctp_cert_packet_with_cert_chain[i] = cert[i - 15];
    }

    EXPECT_EQ(responder_cpld_respond_certificate(spdm_context_ptr, mctp_context_ptr, sizeof(GET_CERTIFICATE), BMC), alt_u32(0));

    EXPECT_EQ((alt_u32)(BMC_SLAVE_ADDRESS << 1), IORD(m_mctp_memory, 0));
    EXPECT_EQ((alt_u32)MCTP_COMMAND_CODE, IORD(m_mctp_memory, 0));
    for (alt_u32 i = 0; i < 15 + 0xc8; i++)
    {
        EXPECT_EQ(mctp_cert_packet_with_cert_chain[i], IORD(m_mctp_memory, 0));
    }

    mctp_get_cert_pkt[UT_MCTP_SPDM_GET_CERTIFICATE_OFFSET_MASK] = 0;
    //mctp_get_cert_pkt[UT_MCTP_SPDM_GET_CERTIFICATE_LENGTH_MASK - 1] = (LARGE_SPDM_MESSAGE_MAX_BUFFER_SIZE/4) >> 8;
    mctp_get_cert_pkt_ptr = (alt_u16*)&mctp_get_cert_pkt[UT_MCTP_SPDM_GET_CERTIFICATE_LENGTH_MASK - 1];
    *mctp_get_cert_pkt_ptr = 0x400;
    mctp_get_cert_pkt_ptr = (alt_u16*)&mctp_get_cert_pkt[UT_MCTP_SPDM_GET_CERTIFICATE_OFFSET_MASK - 1];
    *mctp_get_cert_pkt_ptr = 0x320;

    alt_u8_memcpy(pkt, (alt_u8*)&mctp_get_cert_pkt[7], 15 - 7);

    mctp_cert_pkt[UT_MCTP_SPDM_CERTIFICATE_PORTION_LENGTH_MASK] = 0;
    mctp_cert_pkt[UT_MCTP_SPDM_CERTIFICATE_REMAINDER_LENGTH_MASK] = 0;
    mctp_get_pkt_ptr = (alt_u16*)&mctp_cert_pkt[UT_MCTP_SPDM_CERTIFICATE_PORTION_LENGTH_MASK - 1];
    *mctp_get_pkt_ptr = 0xc8;
    mctp_get_pkt_ptr = (alt_u16*)&mctp_cert_pkt[UT_MCTP_SPDM_CERTIFICATE_REMAINDER_LENGTH_MASK - 1];
    *mctp_get_pkt_ptr = cert_length - 0xc8 - 0xc8 - 0xc8 - 0xc8 - 0xc8;

    for (alt_u8 i = 0; i < 15; i++)
    {
        mctp_cert_packet_with_cert_chain[i] = mctp_cert_pkt[i];
    }

    cert += 0xc8;

    for (alt_u8 i = 15; i < 15 + 0xc8; i++)
    {
        mctp_cert_packet_with_cert_chain[i] = cert[i - 15];
    }

    EXPECT_EQ(responder_cpld_respond_certificate(spdm_context_ptr, mctp_context_ptr, sizeof(GET_CERTIFICATE), BMC), alt_u32(0));

    EXPECT_EQ((alt_u32)(BMC_SLAVE_ADDRESS << 1), IORD(m_mctp_memory, 0));
    EXPECT_EQ((alt_u32)MCTP_COMMAND_CODE, IORD(m_mctp_memory, 0));
    for (alt_u32 i = 0; i < 15 + 0xc8; i++)
    {
        EXPECT_EQ(mctp_cert_packet_with_cert_chain[i], IORD(m_mctp_memory, 0));
    }

    mctp_get_cert_pkt[UT_MCTP_SPDM_GET_CERTIFICATE_OFFSET_MASK] = 0;
    //mctp_get_cert_pkt[UT_MCTP_SPDM_GET_CERTIFICATE_LENGTH_MASK - 1] = (LARGE_SPDM_MESSAGE_MAX_BUFFER_SIZE/4) >> 8;
    mctp_get_cert_pkt_ptr = (alt_u16*)&mctp_get_cert_pkt[UT_MCTP_SPDM_GET_CERTIFICATE_LENGTH_MASK - 1];
    *mctp_get_cert_pkt_ptr = 0x400;
    mctp_get_cert_pkt_ptr = (alt_u16*)&mctp_get_cert_pkt[UT_MCTP_SPDM_GET_CERTIFICATE_OFFSET_MASK - 1];
    *mctp_get_cert_pkt_ptr = 0x3e8;

    alt_u8_memcpy(pkt, (alt_u8*)&mctp_get_cert_pkt[7], 15 - 7);

    mctp_cert_pkt[UT_MCTP_SPDM_CERTIFICATE_PORTION_LENGTH_MASK] = 0;
    mctp_cert_pkt[UT_MCTP_SPDM_CERTIFICATE_REMAINDER_LENGTH_MASK] = 0;
    mctp_get_pkt_ptr = (alt_u16*)&mctp_cert_pkt[UT_MCTP_SPDM_CERTIFICATE_PORTION_LENGTH_MASK - 1];
    *mctp_get_pkt_ptr = 0xc8;
    mctp_get_pkt_ptr = (alt_u16*)&mctp_cert_pkt[UT_MCTP_SPDM_CERTIFICATE_REMAINDER_LENGTH_MASK - 1];
    *mctp_get_pkt_ptr = cert_length - 0xc8 - 0xc8 - 0xc8 - 0xc8 - 0xc8 - 0xc8;

    //alt_u8* stored_cert_chain = (alt_u8*) get_ufm_cpld_cert_ptr();
    for (alt_u8 i = 0; i < 15; i++)
    {
        mctp_cert_packet_with_cert_chain[i] = mctp_cert_pkt[i];
    }

    //cert = (alt_u8*) get_ufm_cpld_cert_ptr();
    cert += 0xc8;

    for (alt_u8 i = 15; i < 15 + 0xc8; i++)
    {
        mctp_cert_packet_with_cert_chain[i] = cert[i - 15];
    }

    EXPECT_EQ(responder_cpld_respond_certificate(spdm_context_ptr, mctp_context_ptr, sizeof(GET_CERTIFICATE), BMC), alt_u32(0));

    EXPECT_EQ((alt_u32)(BMC_SLAVE_ADDRESS << 1), IORD(m_mctp_memory, 0));
    EXPECT_EQ((alt_u32)MCTP_COMMAND_CODE, IORD(m_mctp_memory, 0));
    for (alt_u32 i = 0; i < 15 + 0xc8; i++)
    {
        EXPECT_EQ(mctp_cert_packet_with_cert_chain[i], IORD(m_mctp_memory, 0));
    }

    mctp_get_cert_pkt[UT_MCTP_SPDM_GET_CERTIFICATE_OFFSET_MASK] = 0;
    //mctp_get_cert_pkt[UT_MCTP_SPDM_GET_CERTIFICATE_LENGTH_MASK - 1] = (LARGE_SPDM_MESSAGE_MAX_BUFFER_SIZE/4) >> 8;
    mctp_get_cert_pkt_ptr = (alt_u16*)&mctp_get_cert_pkt[UT_MCTP_SPDM_GET_CERTIFICATE_LENGTH_MASK - 1];
    *mctp_get_cert_pkt_ptr = 0x400;
    mctp_get_cert_pkt_ptr = (alt_u16*)&mctp_get_cert_pkt[UT_MCTP_SPDM_GET_CERTIFICATE_OFFSET_MASK - 1];
    *mctp_get_cert_pkt_ptr = 0x4b0;

    alt_u8_memcpy(pkt, (alt_u8*)&mctp_get_cert_pkt[7], 15 - 7);

    mctp_cert_pkt[UT_MCTP_SPDM_CERTIFICATE_PORTION_LENGTH_MASK] = 0;
    mctp_cert_pkt[UT_MCTP_SPDM_CERTIFICATE_REMAINDER_LENGTH_MASK] = 0;
    mctp_get_pkt_ptr = (alt_u16*)&mctp_cert_pkt[UT_MCTP_SPDM_CERTIFICATE_PORTION_LENGTH_MASK - 1];
    *mctp_get_pkt_ptr = cert_length - 0xc8 - 0xc8 - 0xc8 - 0xc8 - 0xc8 - 0xc8;
    mctp_get_pkt_ptr = (alt_u16*)&mctp_cert_pkt[UT_MCTP_SPDM_CERTIFICATE_REMAINDER_LENGTH_MASK - 1];
    *mctp_get_pkt_ptr = 0;

    mctp_get_pkt_ptr = (alt_u16*)&mctp_cert_pkt[UT_MCTP_SPDM_CERTIFICATE_PORTION_LENGTH_MASK - 1];

    mctp_cert_pkt[UT_MCTP_BYTE_COUNT_IDX] = (alt_u8)(*mctp_get_pkt_ptr) + 0x0e;
    //alt_u8* stored_cert_chain = (alt_u8*) get_ufm_cpld_cert_ptr();
    for (alt_u8 i = 0; i < 15; i++)
    {
        mctp_cert_packet_with_cert_chain[i] = mctp_cert_pkt[i];
    }

    //cert = (alt_u8*) get_ufm_cpld_cert_ptr();
    cert += 0xc8;

    for (alt_u8 i = 15; i < 15 + (alt_u32)(*mctp_get_pkt_ptr); i++)
    {
        mctp_cert_packet_with_cert_chain[i] = cert[i - 15];
    }

    EXPECT_EQ(responder_cpld_respond_certificate(spdm_context_ptr, mctp_context_ptr, sizeof(GET_CERTIFICATE), BMC), alt_u32(0));

    EXPECT_EQ((alt_u32)(BMC_SLAVE_ADDRESS << 1), IORD(m_mctp_memory, 0));
    EXPECT_EQ((alt_u32)MCTP_COMMAND_CODE, IORD(m_mctp_memory, 0));
    for (alt_u32 i = 0; i < 15 + (alt_u32)(*mctp_get_pkt_ptr); i++)
    {
        EXPECT_EQ(mctp_cert_packet_with_cert_chain[i], IORD(m_mctp_memory, 0));
    }
}

TEST_F(PFRSpdmDebugTest, test_challenge_openspdm)
{
    // Initialise context
    // Move to separate function
    SPDM_CONTEXT spdm_context;
    SPDM_CONTEXT* spdm_context_ptr = &spdm_context;

    // Reset everything
    reset_buffer((alt_u8*)spdm_context_ptr, sizeof(SPDM_CONTEXT));

    MCTP_CONTEXT mctp_context;
    MCTP_CONTEXT* mctp_context_ptr = &mctp_context;

    reset_buffer((alt_u8*)mctp_context_ptr, sizeof(MCTP_CONTEXT));

    init_spdm_context(spdm_context_ptr);

    spdm_context_ptr->connection_info.algorithm.base_hash_algo = BASE_HASH_ALGO_TPM_ALG_ECDSA_ECC_NIST_P384;
    spdm_context_ptr->connection_info.algorithm.base_asym_algo = BASE_ASYM_ALGO_TPM_ALG_ECDSA_ECC_NIST_P384;
    spdm_context_ptr->capability.flags |= SUPPORTS_MEAS_RESPONSE_WITH_SIG_GEN_MSG_MASK;

    // Set uuid 2
    spdm_context_ptr->uuid = 2;

    // Expect first message from host to request for version
    spdm_context_ptr->spdm_msg_state = SPDM_CHALLENGE_FLAG;
    mctp_context_ptr->cached_addr = BMC_SLAVE_ADDRESS;
    mctp_context_ptr->msg_type = MCTP_MESSAGE_TYPE;

    alt_u8 mctp_pkt_challenge[43] = MCTP_SPDM_CHALLENGE_MSG;
    mctp_pkt_challenge[UT_CHALLENGE_SPDM_SLOT_NUMBER_IDX] = 0;
    mctp_pkt_challenge[UT_CHALLENGE_SPDM_MEAS_SUMMARY_HASH_TYPE_IDX] = 0xff;
    mctp_pkt_challenge[UT_MCTP_SOM_EOM_BYTE_IDX] = FIRST_AND_LAST_PKT | TAG_OWNER;
    mctp_pkt_challenge[UT_MCTP_BYTE_COUNT_IDX] = 42;
    mctp_pkt_challenge[UT_MCTP_SOURCE_ADDR] = SRC_SLAVE_ADDR_WR;

    alt_u8 mctp_pkt_challenge_auth[239] = MCTP_SPDM_CHALLENGE_AUTH_MSG;
    mctp_pkt_challenge_auth[UT_CHALLENGE_AUTH_SPDM_SLOT_NUMBER_IDX] = 0x0;
    mctp_pkt_challenge_auth[UT_CHALLENGE_AUTH_SPDM_SLOT_MASK_IDX] = 0x01;
    // TODO: Might need to fix endianness
    //mctp_pkt_challenge_auth[UT_CHALLENGE_AUTH_SPDM_OPAQUE_LENGTH_IDX] = 0;
    alt_u16* mctp_pkt_challenge_auth_ptr = (alt_u16*)&mctp_pkt_challenge_auth[UT_CHALLENGE_AUTH_SPDM_OPAQUE_LENGTH_IDX];
    *mctp_pkt_challenge_auth_ptr = 0x02;
    //mctp_pkt_challenge_auth[UT_CHALLENGE_AUTH_SPDM_OPAQUE_LENGTH_IDX + 1] = 0x02;
    mctp_pkt_challenge_auth[UT_MCTP_SOM_EOM_BYTE_IDX] = FIRST_AND_LAST_PKT;
    mctp_pkt_challenge_auth[UT_MCTP_BYTE_COUNT_IDX] = 238;
    mctp_pkt_challenge_auth[UT_MCTP_SOURCE_ADDR] = (BMC_SLAVE_ADDRESS << 1);

    alt_u8 cert_chain_hash[SHA384_LENGTH] = {
    0x8d, 0x72, 0xb6, 0x20, 0xe1, 0x59, 0xb8, 0x52, 0xa3, 0x89, 0x31, 0x0e,
    0x03, 0x7c, 0x7e, 0x15, 0x93, 0x69, 0x87, 0xc4, 0xab, 0x82, 0x43, 0xfb,
    0xb3, 0xbb, 0x4b, 0xfd, 0x8b, 0x5a, 0xa7, 0x8d, 0x4b, 0xa2, 0xca, 0x36,
    0x3c, 0xe7, 0x29, 0x9c, 0x61, 0x0e, 0xb0, 0x2a, 0x75, 0x4e, 0x5a, 0x65
    };

    alt_u32* ptr = (alt_u32*) cert_chain_hash;
    alt_u32* pkt_ptr = (alt_u32*)&mctp_pkt_challenge_auth[UT_CHALLENGE_AUTH_SPDM_CERT_CHAIN_HASH_IDX];

    for (alt_u32 i = 0; i < SHA384_LENGTH/4; i++)
    {
        pkt_ptr[i] = ptr[i];
    }

    alt_u8 priv_key[48] = {
      	  0xc9, 0x9a, 0x06, 0xda, 0x8b, 0xad, 0x33, 0xd6, 0xee, 0x2d, 0x62, 0x36,
      	  0x7b, 0x9c, 0x5a, 0x4e, 0xf7, 0x0d, 0xc8, 0x6f, 0x54, 0xd8, 0x82, 0x27,
      	  0xa5, 0x1d, 0x5a, 0x71, 0xe3, 0x95, 0x9a, 0xdf, 0xc3, 0xd1, 0x52, 0x71,
      	  0x43, 0x95, 0xe3, 0x63, 0xc7, 0xe0, 0xc0, 0xd5, 0x49, 0x14, 0xf7, 0xbc
    };

    CRYPTO_MODE sha_mode = CRYPTO_384_MODE;
    alt_u32 sha_size = SHA384_LENGTH;

    // Need to make sure hash match so will inject data to transcript manually
    const alt_u8 test_data[128] = {
    	  0xc9, 0x9a, 0x06, 0xda, 0x8b, 0xad, 0x33, 0xd6, 0xee, 0x2d, 0x62, 0x36,
    	  0x7b, 0x9c, 0x5a, 0x4e, 0xf7, 0x0d, 0xc8, 0x6f, 0x54, 0xd8, 0x82, 0x27,
    	  0xa5, 0x1d, 0x5a, 0x71, 0xe3, 0x95, 0x9a, 0xdf, 0xc3, 0xd1, 0x52, 0x71,
    	  0x43, 0x95, 0xe3, 0x63, 0xc7, 0xe0, 0xc0, 0xd5, 0x49, 0x14, 0xf7, 0xbc,
    	  0xa2, 0x2a, 0x80, 0xb6, 0x69, 0x38, 0x61, 0xad, 0x4b, 0xf8, 0x42, 0x6d,
    	  0xc8, 0x3e, 0x2a, 0x7d, 0xb1, 0x21, 0x00, 0x0b, 0x09, 0x56, 0x01, 0x9f,
    	  0xd1, 0xd2, 0x83, 0x92, 0x5a, 0x0b, 0xd9, 0x97, 0xba, 0x20, 0x47, 0x3e,
    	  0xa1, 0xec, 0xfd, 0xe6, 0xe8, 0x3b, 0x81, 0x9e, 0x31, 0x9e, 0x05, 0x57,
    	  0x89, 0xa2, 0xa2, 0xe9, 0x20, 0xd4, 0xce, 0xc1, 0x1f, 0x3e, 0xb7, 0xd4,
    	  0xe2, 0x97, 0xc4, 0xc6, 0x56, 0x6a, 0xd3, 0x66, 0xe0, 0x2f, 0x3f, 0xd5,
    	  0x95, 0xd2, 0xf3, 0x3c, 0x8b, 0xb7, 0x60, 0xd7
    };

    alt_u8* test_data_ptr = (alt_u8*)test_data;

    append_large_buffer(&spdm_context_ptr->transcript.m1_m2,
                        test_data_ptr, 64);
    test_data_ptr += 64;
    append_large_buffer(&spdm_context_ptr->transcript.m1_m2,
                        test_data_ptr, 64);
    append_large_buffer(&spdm_context_ptr->transcript.m1_m2,
                        (alt_u8*)&mctp_pkt_challenge[7], 36);
    append_large_buffer(&spdm_context_ptr->transcript.m1_m2,
                        (alt_u8*)&mctp_pkt_challenge_auth[7], 232 - 96);

    alt_u8 test_pubkey_cx[SHA384_LENGTH] = {0};
    alt_u8 test_pubkey_cy[SHA384_LENGTH] = {0};
    alt_u8 sig_r[SHA384_LENGTH] = {0};
    alt_u8 sig_s[SHA384_LENGTH] = {0};

    alt_u8* msg_to_hash = (alt_u8*)&spdm_context_ptr->transcript.m1_m2.buffer[0];

    // Manually generate the public key here
    generate_pubkey((alt_u32*)test_pubkey_cx, (alt_u32*)test_pubkey_cy, (alt_u32*)priv_key, sha_mode);

    // Manually generate signature
    generate_ecdsa_signature((alt_u32*)sig_r, (alt_u32*)sig_s, (alt_u32*)msg_to_hash, spdm_context_ptr->transcript.m1_m2.buffer_size,
    	               (alt_u32*)test_pubkey_cx, (alt_u32*)priv_key, sha_mode);

    pkt_ptr = (alt_u32*)&mctp_pkt_challenge_auth[UT_CHALLENGE_AUTH_SPDM_SIGNATURE_IDX];

    ptr = (alt_u32*) sig_r;
    for (alt_u32 i = 0; i < (sha_size >> 2); i++)
    {
        pkt_ptr[i] = ptr[i];
    }

    pkt_ptr += (SHA384_LENGTH >> 2);
    ptr = (alt_u32*) sig_s;
    for (alt_u32 i = 0; i < (sha_size >> 2); i++)
    {
        pkt_ptr[i] = ptr[i];
    }

    alt_u8* leaf_cert_key = (alt_u8*)&spdm_context_ptr->local_context.stored_chal_meas_signing_key[0];
    for (alt_u32 i = 0; i < SHA384_LENGTH; i++)
    {
        leaf_cert_key[i] = test_pubkey_cx[i];
    }

    for (alt_u32 i = 0; i < SHA384_LENGTH; i++)
    {
        leaf_cert_key[i + SHA384_LENGTH] = test_pubkey_cy[i];
    }

    // Need to inject the public key obtained from leaf cert

    alt_u32 msg_a_and_msg_b_size = spdm_context_ptr->transcript.m1_m2.buffer_size - (36 + 232 - 96);

    // Clear buffer m1m2 and msg c as this will be appended by the function internally as well
    reset_buffer((alt_u8*)&spdm_context_ptr->transcript.m1_m2.buffer[msg_a_and_msg_b_size], 36 + 232 - 96);
    spdm_context_ptr->transcript.m1_m2.buffer_size -= (36 + 232 - 96);
    //reset_buffer((alt_u8*)&spdm_context_ptr->transcript.message_c, sizeof(SMALL_APPENDED_BUFFER));

    // Send in the packet
    ut_send_in_bmc_mctp_packet(m_memory, mctp_pkt_challenge_auth, 239, 0, 0);

    EXPECT_EQ(requester_cpld_challenge(spdm_context_ptr, mctp_context_ptr, BMC), (alt_u32)SPDM_RESPONSE_SUCCESS);

    EXPECT_EQ((alt_u32)(BMC_SLAVE_ADDRESS << 1), IORD(m_mctp_memory, 0));
    EXPECT_EQ((alt_u32)MCTP_COMMAND_CODE, IORD(m_mctp_memory, 0));
    for (alt_u32 i = 0; i < 43; i++)
    {
        EXPECT_EQ(mctp_pkt_challenge[i], IORD(m_mctp_memory, 0));
    }
}

TEST_F(PFRSpdmDebugTest, test_cpld_get_measurement_openspdm)
{
    // Initialise context
    // Move to separate function
    SPDM_CONTEXT spdm_context;
    SPDM_CONTEXT* spdm_context_ptr = &spdm_context;

    // Reset everything
    reset_buffer((alt_u8*)spdm_context_ptr, sizeof(SPDM_CONTEXT));

    MCTP_CONTEXT mctp_context;
    MCTP_CONTEXT* mctp_context_ptr = &mctp_context;

    reset_buffer((alt_u8*)mctp_context_ptr, sizeof(MCTP_CONTEXT));

    init_spdm_context(spdm_context_ptr);

    spdm_context_ptr->connection_info.algorithm.base_hash_algo = BASE_HASH_ALGO_TPM_ALG_ECDSA_ECC_NIST_P384;
    spdm_context_ptr->connection_info.algorithm.base_asym_algo = BASE_ASYM_ALGO_TPM_ALG_ECDSA_ECC_NIST_P384;
    spdm_context_ptr->capability.flags |= SUPPORTS_MEAS_RESPONSE_WITH_SIG_GEN_MSG_MASK;

    // Set uuid 2
    spdm_context_ptr->uuid = 2;
    mctp_context_ptr->cached_addr = BMC_SLAVE_ADDRESS;
    mctp_context_ptr->msg_type = MCTP_MESSAGE_TYPE;

    alt_u8 get_measurement_pkt[43] = MCTP_SPDM_GET_MEASUREMENT_MSG;
    get_measurement_pkt[UT_GET_MEASUREMENT_REQUEST_ATTRIBUTE] = SPDM_GET_MEASUREMENT_REQUEST_GENERATE_SIG;
    get_measurement_pkt[UT_GET_MEASUREMENT_OPERATION_MEASUREMENT_NUMBER] = 0x01;
    get_measurement_pkt[UT_MCTP_SOM_EOM_BYTE_IDX] = FIRST_AND_LAST_PKT | TAG_OWNER;
    get_measurement_pkt[UT_MCTP_BYTE_COUNT_IDX] = 42;
    get_measurement_pkt[UT_MCTP_SOURCE_ADDR] = SRC_SLAVE_ADDR_WR;

    alt_u8 measurement_pkt[218] = MCTP_SPDM_MEASUREMENT_MSG;
    // Shall test for 1 block of measurement
    measurement_pkt[UT_MEASUREMENT_TOTAL_NUMBER_OF_BLOCKS] = 1;
    measurement_pkt[UT_MEASUREMENT_RECORD_LENGTH] = UT_RECORD_LENGTH;
    measurement_pkt[UT_MEASUREMENT_RECORD_LENGTH + 1] = 0x00;
    measurement_pkt[UT_MEASUREMENT_RECORD_LENGTH + 2] = 0x00;
    measurement_pkt[UT_MEASUREMENT_RECORD] = 0;

    // For simplicity, just randomly fill in the measurement hash
    // Do the same for AFM in the ufm after this
    for (alt_u32 i = 0; i < UT_RECORD_LENGTH; i++)
    {
        measurement_pkt[UT_MEASUREMENT_RECORD + i] = 0xbb;
    }

    alt_u16* dmtf_spec_meas_size = (alt_u16*) &measurement_pkt[UT_MEASUREMENT_RECORD + 5];
    *dmtf_spec_meas_size = 64;

    measurement_pkt[UT_MEASUREMENT_OPAQUE_LENGTH] = UT_OPAQUE_LENGTH;
    measurement_pkt[UT_MEASUREMENT_OPAQUE_LENGTH + 1] = 0x0;
    measurement_pkt[UT_MEASUREMENT_SIGNATURE] = 0;
    measurement_pkt[UT_MCTP_SOM_EOM_BYTE_IDX] = FIRST_AND_LAST_PKT;
    measurement_pkt[UT_MCTP_BYTE_COUNT_IDX] = 217;
    measurement_pkt[UT_MCTP_SOURCE_ADDR] = (BMC_SLAVE_ADDRESS << 1);

    alt_u32* pkt_ptr = (alt_u32*)&measurement_pkt[UT_MEASUREMENT_SIGNATURE];

    CRYPTO_MODE sha_mode = CRYPTO_384_MODE;
    alt_u32 sha_size = SHA384_LENGTH;

    alt_u8 priv_key[48] = {
      	  0xc9, 0x9a, 0x06, 0xda, 0x8b, 0xad, 0x33, 0xd6, 0xee, 0x2d, 0x62, 0x36,
      	  0x7b, 0x9c, 0x5a, 0x4e, 0xf7, 0x0d, 0xc8, 0x6f, 0x54, 0xd8, 0x82, 0x27,
      	  0xa5, 0x1d, 0x5a, 0x71, 0xe3, 0x95, 0x9a, 0xdf, 0xc3, 0xd1, 0x52, 0x71,
      	  0x43, 0x95, 0xe3, 0x63, 0xc7, 0xe0, 0xc0, 0xd5, 0x49, 0x14, 0xf7, 0xbc
    };

    LARGE_APPENDED_BUFFER big_buffer;
    LARGE_APPENDED_BUFFER* big_buffer_ptr = &big_buffer;
    reset_buffer((alt_u8*)big_buffer_ptr, sizeof(LARGE_APPENDED_BUFFER));

    append_large_buffer(big_buffer_ptr, (alt_u8*)&get_measurement_pkt[7], 36);
    append_large_buffer(big_buffer_ptr, (alt_u8*)&measurement_pkt[7], (211 - 96));

    alt_u8 test_pubkey_cx[SHA384_LENGTH] = {0};
    alt_u8 test_pubkey_cy[SHA384_LENGTH] = {0};
    alt_u8 sig_r[SHA384_LENGTH] = {0};
    alt_u8 sig_s[SHA384_LENGTH] = {0};

    alt_u8* msg_to_hash = (alt_u8*)big_buffer_ptr->buffer;

    // Manually generate the public key here
    generate_pubkey((alt_u32*)test_pubkey_cx, (alt_u32*)test_pubkey_cy, (alt_u32*)priv_key, sha_mode);

    // Manually generate signature
    generate_ecdsa_signature((alt_u32*)sig_r, (alt_u32*)sig_s, (alt_u32*)msg_to_hash, big_buffer_ptr->buffer_size,
    	               (alt_u32*)test_pubkey_cx, (alt_u32*)priv_key, sha_mode);

    alt_u32* ptr = (alt_u32*) sig_r;
    for (alt_u32 i = 0; i < (sha_size >> 2); i++)
    {
        pkt_ptr[i] = ptr[i];
    }

    pkt_ptr += (sha_size >> 2);
    ptr = (alt_u32*) sig_s;
    for (alt_u32 i = 0; i < (sha_size >> 2); i++)
    {
        pkt_ptr[i] = ptr[i];
    }

    // Begin the test by having an empty UFM on the AFM storage region
    alt_u32* ufm_afm_pointer = get_ufm_ptr_with_offset(UFM_AFM_1);
    EXPECT_EQ(*ufm_afm_pointer, (alt_u32)0xffffffff);

    // Now, we filled in the AFM with UUID. Assume UUID is 2
    // We filled in the first region in the UFM.
    *((alt_u16*)ufm_afm_pointer) = 2;

    alt_u8* leaf_cert_key = (alt_u8*)&spdm_context_ptr->local_context.stored_chal_meas_signing_key[0];
    for (alt_u32 i = 0; i < SHA384_LENGTH; i++)
    {
        leaf_cert_key[i] = test_pubkey_cx[i];
    }

    for (alt_u32 i = 0; i < SHA384_LENGTH; i++)
    {
        leaf_cert_key[i + SHA384_LENGTH] = test_pubkey_cy[i];
    }

    ufm_afm_pointer = get_ufm_ptr_with_offset(UFM_AFM_1);
    // For testing, store measurement hash into the ufm
    // Set total measurement to 1 for one index only
    ufm_afm_pointer = ufm_afm_pointer + 140;
    *ufm_afm_pointer = 1;
    // Make sure to set the possible measurement number correctly
    alt_u8* alt_u8_ufm_afm_pointer = (alt_u8*) ufm_afm_pointer;
    alt_u8_ufm_afm_pointer = alt_u8_ufm_afm_pointer + 1;
    // Set 1 since there are one measurement
    *alt_u8_ufm_afm_pointer = 1;

    // Get the offset to the measurement hash
    alt_u8_ufm_afm_pointer = alt_u8_ufm_afm_pointer + 3;
    for (alt_u32 i = 0; i < 64; i++)
    {
        // This is just a random chosen value. It can be any value as long as it is consistent for hash calculation
    	alt_u8_ufm_afm_pointer[i] = 0xbb;
    }

    // Send in the packet
    ut_send_in_bmc_mctp_packet(m_memory, measurement_pkt, 218, 0, 0);
    EXPECT_EQ(read_from_mailbox(MB_MCTP_BMC_PACKET_AVAIL_AND_RX_PACKET_ERROR), (alt_u32)0x01);

    EXPECT_EQ(requester_cpld_get_measurement(spdm_context_ptr, mctp_context_ptr, 1, BMC), (alt_u32)SPDM_RESPONSE_SUCCESS);

    EXPECT_EQ((alt_u32)(BMC_SLAVE_ADDRESS << 1), IORD(m_mctp_memory, 0));
    EXPECT_EQ((alt_u32)MCTP_COMMAND_CODE, IORD(m_mctp_memory, 0));
    for (alt_u32 i = 0; i < 43; i++)
    {
        EXPECT_EQ(get_measurement_pkt[i], IORD(m_mctp_memory, 0));
    }

}

TEST_F(PFRSpdmDebugTest, test_t0_flow_single_afm_openspdm)
{

    SYSTEM_MOCK* sys = SYSTEM_MOCK::get();
    // Reset system mocks and SPI flash
    sys->reset();
    sys->reset_spi_flash_mock();

    // Perform provisioning
    SYSTEM_MOCK::get()->provision_ufm_data(UFM_PFR_DATA_ATTEST_EXAMPLE_KEY_FILE);

    // Reset Nios firmware
    ut_reset_nios_fw();

    // Load the entire image to flash
    SYSTEM_MOCK::get()->load_to_flash(SPI_FLASH_BMC, FULL_PFR_IMAGE_BMC_V2P2_SVN2_ONE_AFM_UUID9_FILE, FULL_PFR_IMAGE_BMC_V2P2_SVN2_ONE_AFM_UUID9_FILE_SIZE);
    SYSTEM_MOCK::get()->load_to_flash(SPI_FLASH_PCH, FULL_PFR_IMAGE_PCH_FILE, FULL_PFR_IMAGE_PCH_FILE_SIZE);

    // Load some capsules to flash
    // If there're unexpected updates, unittest can catch them.
    SYSTEM_MOCK::get()->load_to_flash(SPI_FLASH_PCH, SIGNED_CAPSULE_PCH_V03P12_FILE,
            SIGNED_CAPSULE_PCH_V03P12_FILE_SIZE, get_ufm_pfr_data()->pch_staging_region);
    SYSTEM_MOCK::get()->load_to_flash(SPI_FLASH_BMC, SIGNED_CAPSULE_BMC_V14_FILE,
            SIGNED_CAPSULE_BMC_V14_FILE_SIZE, get_ufm_pfr_data()->bmc_staging_region);

    /*
     * Flow preparation
     */
    ut_prep_nios_gpi_signals();
    ut_send_block_complete_chkpt_msg();

    // Exit after 50 iterations in the T0 loop
    SYSTEM_MOCK::get()->insert_code_block(SYSTEM_MOCK::CODE_BLOCK_TYPES::T0_OPERATIONS_END_AFTER_50_ITERS);

    CRYPTO_MODE sha_mode = CRYPTO_384_MODE;
    alt_u32 sha_size = SHA384_LENGTH;

    LARGE_APPENDED_BUFFER big_buffer;
    LARGE_APPENDED_BUFFER* big_buffer_ptr = &big_buffer;
    reset_buffer((alt_u8*)big_buffer_ptr, sizeof(LARGE_APPENDED_BUFFER));

    alt_u8 priv_key_0[48] = {
        0x6d, 0x21, 0x7e, 0x55, 0x18, 0x5b, 0xfe, 0xa5, 0x54, 0xf7, 0x44, 0x62,
        0x89, 0xb2, 0x40, 0xb1, 0x1d, 0x56, 0xe6, 0x61, 0xf1, 0x16, 0x4f, 0x49,
        0x6f, 0xd9, 0x42, 0x8d, 0xbb, 0xeb, 0x4b, 0xf3, 0x9d, 0x0a, 0x7c, 0x40,
        0xd2, 0xdf, 0x39, 0x12, 0x89, 0xc9, 0x11, 0xb5, 0xa9, 0xaa, 0xef, 0x2b,
    };

    alt_u8 priv_key_1[48] = {
        0xb0, 0x21, 0x7e, 0x55, 0x18, 0x5b, 0x0e, 0xa5, 0x54, 0xf7, 0x44, 0x62,
        0x89, 0xb2, 0x40, 0xb1, 0x1d, 0x56, 0xe6, 0x61, 0xf1, 0x16, 0x4f, 0x49,
        0x6f, 0xd9, 0x42, 0x8d, 0x90, 0xeb, 0x4b, 0xf3, 0x9d, 0x0a, 0x7c, 0x40,
        0xd2, 0xdf, 0x39, 0x12, 0x89, 0xc9, 0x11, 0xb5, 0xa9, 0xd2, 0xef, 0x2b,
    };

    alt_u32 test_pubkey_cx_cy[SHA384_LENGTH/2] = {0};

    // Manually generate the public key here
    generate_pubkey(test_pubkey_cx_cy, test_pubkey_cx_cy + SHA384_LENGTH/4, (alt_u32*)priv_key_0, CRYPTO_384_MODE);

    alt_u32_memcpy(get_ufm_ptr_with_offset(UFM_CPLD_PUBLIC_KEY_0), test_pubkey_cx_cy, SHA384_LENGTH * 2);

    // Manually generate the public key here
    generate_pubkey(test_pubkey_cx_cy, test_pubkey_cx_cy + SHA384_LENGTH/4, (alt_u32*)priv_key_1, CRYPTO_384_MODE);

    alt_u32_memcpy(get_ufm_ptr_with_offset(UFM_CPLD_PUBLIC_KEY_1), test_pubkey_cx_cy, SHA384_LENGTH * 2);

    // Cert chain obtained from OPENSPDM responder
	alt_u32 cert_chain_buffer[MAX_CERT_CHAIN_SIZE/4] = {0};

    // Temporarily store the certificate
    alt_u32 cert_res[MAX_CERT_CHAIN_SIZE/4] = {0};
    alt_u32* stored_cert_ptr = (alt_u32*)cert_res;

    alt_u32 res_pub_key[100];

    alt_u32 cert_chain_size = pfr_generate_certificate_chain((alt_u8*) cert_chain_buffer, (alt_u32*) priv_key_0, get_ufm_ptr_with_offset(UFM_CPLD_PUBLIC_KEY_0), get_ufm_ptr_with_offset(UFM_CPLD_PUBLIC_KEY_1));
    alt_u32 cert_chain_root_cert_size = pfr_get_certificate_size((alt_u8*) cert_chain_buffer);

    EXPECT_EQ(1, pfr_verify_der_cert_chain((alt_u8*)cert_chain_buffer, cert_chain_size, res_pub_key, (alt_u32*)res_pub_key + 12));

    //build and store first 4 bytes of cert header into UFM
    alt_u32 cert_chain_header_first_word = 0x00000000;
    alt_u16* cert_chain_header_first_word_u16 = (alt_u16*) &cert_chain_header_first_word;
    cert_chain_header_first_word_u16[0] = (alt_u16) (4 + (12*4) + cert_chain_size);
    stored_cert_ptr[0] = cert_chain_header_first_word;
    //calculate and store root cert hash into UFM
    calculate_and_save_sha(&stored_cert_ptr[1], 0x00000000, cert_chain_buffer,
    		cert_chain_root_cert_size, CRYPTO_384_MODE, DISENGAGE_DMA);

    alt_u32 cert_chain_header_size = 52;

    alt_u32 cert_chain_size_padded = cert_chain_size;
    while (cert_chain_size_padded % 4 != 0) {
    	cert_chain_size_padded++;
    }
    alt_u32_memcpy(&stored_cert_ptr[1 + 12], cert_chain_buffer, cert_chain_size_padded);

    alt_u8 cert_chain_digest[SHA384_LENGTH] = {0};

    calculate_and_save_sha((alt_u32*)cert_chain_digest, 0, (alt_u32*)cert_res,
    		          cert_chain_size + cert_chain_header_size, CRYPTO_384_MODE, DISENGAGE_DMA);

    alt_u8 expected_get_ver_msg[11] = MCTP_SPDM_GET_VERSION_MSG;
    expected_get_ver_msg[UT_MCTP_BYTE_COUNT_IDX] = 0x0a;
    expected_get_ver_msg[UT_MCTP_SOURCE_ADDR] = SRC_SLAVE_ADDR_WR;
    expected_get_ver_msg[UT_MCTP_SOM_EOM_BYTE_IDX] = FIRST_AND_LAST_PKT | TAG_OWNER;

    // Obtain the example message for succesful_version
    alt_u8 succ_ver_mctp_pkt[19] = MCTP_SPDM_SUCCESSFUL_VERSION_MSG_THREE_VER_ENTRY;
    succ_ver_mctp_pkt[UT_MCTP_SOM_EOM_BYTE_IDX] = FIRST_AND_LAST_PKT;
    // Modify the source address
    succ_ver_mctp_pkt[UT_MCTP_SOURCE_ADDR] = 0x02 << 1;

    append_large_buffer(big_buffer_ptr, (alt_u8*)&expected_get_ver_msg[7], 4);
    append_large_buffer(big_buffer_ptr, (alt_u8*)&succ_ver_mctp_pkt[7], 12);

    ut_send_in_pcie_mctp_packet(m_memory, succ_ver_mctp_pkt, 19, 1, 0);
    EXPECT_EQ(read_from_mailbox(MB_MCTP_PCIE_PACKET_AVAIL_AND_RX_PACKET_ERROR), (alt_u32)0x01);

    alt_u8 get_cap_mctp_pkt[11] = MCTP_SPDM_GET_CAPABILITY_MSG;
    get_cap_mctp_pkt[UT_MCTP_SOM_EOM_BYTE_IDX] = FIRST_AND_LAST_PKT | TAG_OWNER;
    get_cap_mctp_pkt[UT_MCTP_SOURCE_ADDR] = SRC_SLAVE_ADDR_WR;

    alt_u8 succ_cap_mctp_pkt[19] = MCTP_SPDM_SUCCESSFUL_CAPABILITY_MSG;
    succ_cap_mctp_pkt[UT_MCTP_SOM_EOM_BYTE_IDX] = FIRST_AND_LAST_PKT;
    // Modify the source address
    succ_cap_mctp_pkt[UT_MCTP_SOURCE_ADDR] = 0x02 << 1;
    // Modify ct exponent
    succ_cap_mctp_pkt[UT_MCTP_SPDM_CAPABILITY_CT_EXPONENT_IDX] = 0x0c;
    // Modify flags
    alt_u32* cap_ptr = (alt_u32*)&succ_cap_mctp_pkt[UT_MCTP_SPDM_CAPABILITY_FLAGS_IDX];
    *cap_ptr = SUPPORTS_DIGEST_CERT_RESPONSE_MSG_MASK |
               SUPPORTS_CHALLENGE_AUTH_RESPONSE_MSG_MASK |
               SUPPORTS_MEAS_RESPONSE_WITH_SIG_GEN_MSG_MASK;

    append_large_buffer(big_buffer_ptr, (alt_u8*)&get_cap_mctp_pkt[7], 4);
    append_large_buffer(big_buffer_ptr, (alt_u8*)&succ_cap_mctp_pkt[7], 12);

    // If this is the last message, unset the final parameter of the functions
    ut_send_in_pcie_mctp_packet(m_memory, succ_cap_mctp_pkt, 19, 1, 0);
    EXPECT_EQ(read_from_mailbox(MB_MCTP_PCIE_PACKET_AVAIL_AND_RX_PACKET_ERROR), (alt_u32)0x01);

    alt_u8 expected_nego_algo[55] = MCTP_SPDM_NEGOTIATE_ALGORITHM;
    expected_nego_algo[UT_MCTP_SOM_EOM_BYTE_IDX] = FIRST_AND_LAST_PKT | TAG_OWNER;
    expected_nego_algo[UT_MCTP_BYTE_COUNT_IDX] = 38;
    expected_nego_algo[UT_MCTP_SOURCE_ADDR] = SRC_SLAVE_ADDR_WR;
    expected_nego_algo[UT_MCTP_SPDM_ALGORITHM_LENGTH] = 0;
    alt_u16* length_ptr = (alt_u16*)&expected_nego_algo[UT_MCTP_SPDM_ALGORITHM_LENGTH];
    *length_ptr = sizeof(NEGOTIATE_ALGORITHMS);
    expected_nego_algo[UT_MCTP_SPDM_ALGORITHM_MEASUREMENT_SPEC] = SPDM_ALGO_MEASUREMENT_SPECIFICATION_DMTF;
    alt_u32* nego_algo_ptr = (alt_u32*)&expected_nego_algo[UT_MCTP_SPDM_NEGOTIATE_ALGORITHM_BASE_ASYM_ALGO];
    *nego_algo_ptr = BASE_ASYM_ALGO_TPM_ALG_ECDSA_ECC_NIST_P384 |
                     BASE_ASYM_ALGO_TPM_ALG_ECDSA_ECC_NIST_P256;
    nego_algo_ptr = (alt_u32*)&expected_nego_algo[UT_MCTP_SPDM_NEGOTIATE_ALGORITHM_BASE_HASH_ALGO];
    *nego_algo_ptr = BASE_HASH_ALGO_TPM_ALG_ECDSA_ECC_NIST_P384 |
                     BASE_HASH_ALGO_TPM_ALG_ECDSA_ECC_NIST_P256;

    alt_u8 mctp_pkt[51] = MCTP_SPDM_SUCCESSFUL_ALGORITHM;
    mctp_pkt[UT_MCTP_SOM_EOM_BYTE_IDX] = FIRST_AND_LAST_PKT;
    mctp_pkt[UT_MCTP_BYTE_COUNT_IDX] = 50;
    mctp_pkt[UT_MCTP_SOURCE_ADDR] = 0x02 << 1;
    mctp_pkt[UT_MCTP_SPDM_ALGORITHM_LENGTH] = 0;
    length_ptr = (alt_u16*)&mctp_pkt[UT_MCTP_SPDM_ALGORITHM_LENGTH];
    *length_ptr = sizeof(SUCCESSFUL_ALGORITHMS);
    mctp_pkt[UT_MCTP_SPDM_ALGORITHM_MEASUREMENT_SPEC] = SPDM_ALGO_MEASUREMENT_SPECIFICATION_DMTF;
    alt_u32* succ_algo_ptr = (alt_u32*)&mctp_pkt[UT_MCTP_SPDM_ALGORITHM_MEASUREMENT_HASH_ALGO];
    *succ_algo_ptr = MEAS_HASH_ALGO_TPM_ALG_SHA_384;
    succ_algo_ptr = (alt_u32*)&mctp_pkt[UT_MCTP_SPDM_ALGORITHM_BASE_ASYM_SEL];
    *succ_algo_ptr = BASE_ASYM_ALGO_TPM_ALG_ECDSA_ECC_NIST_P384;
    succ_algo_ptr = (alt_u32*)&mctp_pkt[UT_MCTP_SPDM_ALGORITHM_BASE_HASH_SEL];
    *succ_algo_ptr = BASE_HASH_ALGO_TPM_ALG_ECDSA_ECC_NIST_P384;

    append_large_buffer(big_buffer_ptr, (alt_u8*)&expected_nego_algo[7], sizeof(NEGOTIATE_ALGORITHMS));
    append_large_buffer(big_buffer_ptr, (alt_u8*)&mctp_pkt[7], 44);

    // Send in the packet to RXFIFO
    // Need not stage the message as only one message exchange, hence set last param to 0
    ut_send_in_pcie_mctp_packet(m_memory, mctp_pkt, 51, 1, 0);
    EXPECT_EQ(read_from_mailbox(MB_MCTP_PCIE_PACKET_AVAIL_AND_RX_PACKET_ERROR), (alt_u32)0x01);

    alt_u8 mctp_get_digest_pkt[11] = MCTP_SPDM_GET_DIGEST_MSG;
    mctp_get_digest_pkt[UT_MCTP_SOM_EOM_BYTE_IDX] = FIRST_AND_LAST_PKT | TAG_OWNER;
    mctp_get_digest_pkt[UT_MCTP_BYTE_COUNT_IDX] = 0x0a;
    mctp_get_digest_pkt[UT_MCTP_SOURCE_ADDR] = SRC_SLAVE_ADDR_WR;

    alt_u8 mctp_digest_pkt[59] = MCTP_SPDM_DIGEST_MSG;
    mctp_digest_pkt[UT_MCTP_SOM_EOM_BYTE_IDX] = FIRST_AND_LAST_PKT;
    mctp_digest_pkt[UT_MCTP_BYTE_COUNT_IDX] = 58;
    mctp_digest_pkt[UT_MCTP_SOURCE_ADDR] = 0x02 << 1;
    mctp_digest_pkt[UT_MCTP_SPDM_DIGEST_SLOT_MASK] = 0x0;

    for (alt_u8 i = 11; i < 59; i++)
    {
    	mctp_digest_pkt[i] = cert_chain_digest[i - 11];
    }

    append_large_buffer(big_buffer_ptr, (alt_u8*)&mctp_get_digest_pkt[7], 4);
    append_large_buffer(big_buffer_ptr, (alt_u8*)&mctp_digest_pkt[7], 52);

    // Send in the packet to RXFIFO
    // Need not stage the message as only one message exchange, hence set last param to 0
    ut_send_in_pcie_mctp_packet(m_memory, mctp_digest_pkt, 59, 1, 0);
    EXPECT_EQ(read_from_mailbox(MB_MCTP_PCIE_PACKET_AVAIL_AND_RX_PACKET_ERROR), (alt_u32)0x01);

    cert_chain_size += cert_chain_header_size;
    alt_u8 mctp_cert_packet_with_cert_chain[500] = {0};

    alt_u8 mctp_get_cert_pkt[15] = MCTP_SPDM_GET_CERTIFICATE_MSG;
    mctp_get_cert_pkt[UT_MCTP_SOM_EOM_BYTE_IDX] = FIRST_AND_LAST_PKT | TAG_OWNER;
    mctp_get_cert_pkt[UT_MCTP_BYTE_COUNT_IDX] = 0x0e;
    mctp_get_cert_pkt[UT_MCTP_SOURCE_ADDR] = SRC_SLAVE_ADDR_WR;
    alt_u16* mctp_get_cert_pkt_ptr = (alt_u16*)&mctp_get_cert_pkt[UT_MCTP_SPDM_GET_CERTIFICATE_LENGTH_MASK - 1];
    *mctp_get_cert_pkt_ptr = CERT_REQUEST_SIZE;
    alt_u16* mctp_get_cert_pkt_ptr_2 = (alt_u16*)&mctp_get_cert_pkt[UT_MCTP_SPDM_GET_CERTIFICATE_OFFSET_MASK - 1];
    *mctp_get_cert_pkt_ptr_2 = 0;

    alt_u8 mctp_cert_pkt[15] = MCTP_SPDM_CERTIFICATE_MSG;
    mctp_cert_pkt[UT_MCTP_SOM_EOM_BYTE_IDX] = FIRST_AND_LAST_PKT;
    mctp_cert_pkt[UT_MCTP_BYTE_COUNT_IDX] = 206 + 8;
    mctp_cert_pkt[UT_MCTP_SOURCE_ADDR] = (0x02 << 1);
    alt_u16* mctp_get_pkt_ptr = (alt_u16*)&mctp_cert_pkt[UT_MCTP_SPDM_CERTIFICATE_PORTION_LENGTH_MASK - 1];
    *mctp_get_pkt_ptr = 0xc8;
    alt_u16* mctp_get_pkt_ptr_2 = (alt_u16*)&mctp_cert_pkt[UT_MCTP_SPDM_CERTIFICATE_REMAINDER_LENGTH_MASK - 1];
    *mctp_get_pkt_ptr_2 = cert_chain_size - 0xc8;

    alt_u8* cert_chain_buffer_ptr = (alt_u8*)cert_res;

    for (alt_u32 i = 0; i < 15; i++)
    {
        mctp_cert_packet_with_cert_chain[i] = mctp_cert_pkt[i];
    }

    for (alt_u32 i = 15; i < 15 + 200; i++)
    {
        mctp_cert_packet_with_cert_chain[i] = cert_chain_buffer_ptr[i - 15];
    }

    // Send in the packet to RXFIFO
    // Need not stage the message as only one message exchange, hence set last param to 0
    ut_send_in_pcie_mctp_packet(m_memory, mctp_cert_packet_with_cert_chain, 207 + 8, 1, 0);

    append_large_buffer(big_buffer_ptr, (alt_u8*)&mctp_get_cert_pkt[7], 8);
    append_large_buffer(big_buffer_ptr, (alt_u8*)&mctp_cert_packet_with_cert_chain[7], 208);

    // Set remainder
    *mctp_get_pkt_ptr_2 = cert_chain_size - 0xc8 - 0xc8;

    for (alt_u32 i = 0; i < 15; i++)
    {
        mctp_cert_packet_with_cert_chain[i] = mctp_cert_pkt[i];
    }

    for (alt_u32 i = 15; i < 15 + 200; i++)
    {
        mctp_cert_packet_with_cert_chain[i] = cert_chain_buffer_ptr[i - 15 + 200];
    }

    ut_send_in_pcie_mctp_packet(m_memory, mctp_cert_packet_with_cert_chain, 207 + 8, 1, 0);

    // Expect that CPLD request the remaining portion of the certificate
    *mctp_get_cert_pkt_ptr = 0xc8;
    // Expect CPLD to obtain the offset from previous cert chain
    *mctp_get_cert_pkt_ptr_2 = 0xc8;

    append_large_buffer(big_buffer_ptr, (alt_u8*)&mctp_get_cert_pkt[7], 8);
    append_large_buffer(big_buffer_ptr, (alt_u8*)&mctp_cert_packet_with_cert_chain[7], 208);

    // Set remainder
    *mctp_get_pkt_ptr_2 = cert_chain_size - 0xc8 - 0xc8 - 0xc8;

    for (alt_u32 i = 0; i < 15; i++)
    {
        mctp_cert_packet_with_cert_chain[i] = mctp_cert_pkt[i];
    }

    for (alt_u32 i = 15; i < 15 + 200; i++)
    {
        mctp_cert_packet_with_cert_chain[i] = cert_chain_buffer_ptr[i - 15 + 200 + 200];
    }

    ut_send_in_pcie_mctp_packet(m_memory, mctp_cert_packet_with_cert_chain, 207 + 8, 1, 0);

    // Expect that CPLD request the remaining portion of the certificate
    *mctp_get_cert_pkt_ptr = 0xc8;
    // Expect CPLD to obtain the offset from previous cert chain
    *mctp_get_cert_pkt_ptr_2 = 0xc8 + 0xc8;

    append_large_buffer(big_buffer_ptr, (alt_u8*)&mctp_get_cert_pkt[7], 8);
    append_large_buffer(big_buffer_ptr, (alt_u8*)&mctp_cert_packet_with_cert_chain[7], 208);

    // Set remainder
    *mctp_get_pkt_ptr_2 = cert_chain_size - 0xc8 - 0xc8 - 0xc8 - 0xc8;

    for (alt_u32 i = 0; i < 15; i++)
    {
        mctp_cert_packet_with_cert_chain[i] = mctp_cert_pkt[i];
    }

    for (alt_u32 i = 15; i < 15 + 200; i++)
    {
        mctp_cert_packet_with_cert_chain[i] = cert_chain_buffer_ptr[i - 15 + 200 + 200 + 200];
    }

    ut_send_in_pcie_mctp_packet(m_memory, mctp_cert_packet_with_cert_chain, 207 + 8, 1, 0);

    // Expect that CPLD request the remaining portion of the certificate
    *mctp_get_cert_pkt_ptr = 0xc8;
    // Expect CPLD to obtain the offset from previous cert chain
    *mctp_get_cert_pkt_ptr_2 = 0xc8 + 0xc8 + 0xc8;

    append_large_buffer(big_buffer_ptr, (alt_u8*)&mctp_get_cert_pkt[7], 8);
    append_large_buffer(big_buffer_ptr, (alt_u8*)&mctp_cert_packet_with_cert_chain[7], 208);

    // Set remainder
    *mctp_get_pkt_ptr_2 = cert_chain_size - 0xc8 - 0xc8 - 0xc8 - 0xc8 - 0xc8;

    for (alt_u32 i = 0; i < 15; i++)
    {
        mctp_cert_packet_with_cert_chain[i] = mctp_cert_pkt[i];
    }

    for (alt_u32 i = 15; i < 15 + 200; i++)
    {
        mctp_cert_packet_with_cert_chain[i] = cert_chain_buffer_ptr[i - 15 + 200 + 200 + 200 + 200];
    }

    ut_send_in_pcie_mctp_packet(m_memory, mctp_cert_packet_with_cert_chain, 207 + 8, 1, 0);

    // Expect that CPLD request the remaining portion of the certificate
    *mctp_get_cert_pkt_ptr = 0xc8;
    // Expect CPLD to obtain the offset from previous cert chain
    *mctp_get_cert_pkt_ptr_2 = 0xc8 + 0xc8 + 0xc8 + 0xc8;

    append_large_buffer(big_buffer_ptr, (alt_u8*)&mctp_get_cert_pkt[7], 8);
    append_large_buffer(big_buffer_ptr, (alt_u8*)&mctp_cert_packet_with_cert_chain[7], 208);

    // Set remainder
    *mctp_get_pkt_ptr_2 = cert_chain_size - 0xc8 - 0xc8 - 0xc8 - 0xc8 - 0xc8 - 0xc8;
    *mctp_get_pkt_ptr = 0xc8;

    for (alt_u32 i = 0; i < 15; i++)
    {
        mctp_cert_packet_with_cert_chain[i] = mctp_cert_pkt[i];
    }

    for (alt_u32 i = 15; i < 15 + 200; i++)
    {
        mctp_cert_packet_with_cert_chain[i] = cert_chain_buffer_ptr[i - 15 + 200 + 200 + 200 + 200 + 200];
    }

    ut_send_in_pcie_mctp_packet(m_memory, mctp_cert_packet_with_cert_chain, 207 + 8, 1, 0);

    // Expect that CPLD request the remaining portion of the certificate
    *mctp_get_cert_pkt_ptr = 0xc8;
    // Expect CPLD to obtain the offset from previous cert chain
    *mctp_get_cert_pkt_ptr_2 = 0xc8 + 0xc8 + 0xc8 + 0xc8 + 0xc8;

    append_large_buffer(big_buffer_ptr, (alt_u8*)&mctp_get_cert_pkt[7], 8);
    append_large_buffer(big_buffer_ptr, (alt_u8*)&mctp_cert_packet_with_cert_chain[7], 208);

    // Set remainder
    *mctp_get_pkt_ptr_2 = 0;
    *mctp_get_pkt_ptr = cert_chain_size - 0xc8 - 0xc8 - 0xc8 - 0xc8 - 0xc8 - 0xc8;
    mctp_cert_pkt[UT_MCTP_BYTE_COUNT_IDX] = 206 + 8 - 0xc8 + (*mctp_get_pkt_ptr);

    for (alt_u32 i = 0; i < 15; i++)
    {
        mctp_cert_packet_with_cert_chain[i] = mctp_cert_pkt[i];
    }

    for (alt_u32 i = 15; i < 15 + (alt_u32)(*mctp_get_pkt_ptr); i++)
    {
        mctp_cert_packet_with_cert_chain[i] = cert_chain_buffer_ptr[i - 15 + 200 + 200 + 200 + 200 + 200 + 200];
    }

    ut_send_in_pcie_mctp_packet(m_memory, mctp_cert_packet_with_cert_chain, 207 + 8 - 0xc8 + (*mctp_get_pkt_ptr), 1, 0);

    // Expect that CPLD request the remaining portion of the certificate
    *mctp_get_cert_pkt_ptr = cert_chain_size - 0xc8 - 0xc8 - 0xc8 - 0xc8 - 0xc8 - 0xc8;
    // Expect CPLD to obtain the offset from previous cert chain
    *mctp_get_cert_pkt_ptr_2 = 0xc8 + 0xc8 + 0xc8 + 0xc8 + 0xc8 + 0xc8;

    append_large_buffer(big_buffer_ptr, (alt_u8*)&mctp_get_cert_pkt[7], 8);
    append_large_buffer(big_buffer_ptr, (alt_u8*)&mctp_cert_packet_with_cert_chain[7], 208 - 0xc8 + (*mctp_get_pkt_ptr));

    alt_u8 mctp_pkt_challenge[43] = MCTP_SPDM_CHALLENGE_MSG;
    mctp_pkt_challenge[UT_CHALLENGE_SPDM_SLOT_NUMBER_IDX] = 0;
    mctp_pkt_challenge[UT_CHALLENGE_SPDM_MEAS_SUMMARY_HASH_TYPE_IDX] = 0xff;
    mctp_pkt_challenge[UT_MCTP_SOM_EOM_BYTE_IDX] = FIRST_AND_LAST_PKT | TAG_OWNER;
    mctp_pkt_challenge[UT_MCTP_BYTE_COUNT_IDX] = 42;
    mctp_pkt_challenge[UT_MCTP_SOURCE_ADDR] = SRC_SLAVE_ADDR_WR;

    alt_u8 mctp_pkt_challenge_auth[239] = MCTP_SPDM_CHALLENGE_AUTH_MSG;
    mctp_pkt_challenge_auth[UT_CHALLENGE_AUTH_SPDM_SLOT_NUMBER_IDX] = 0x0;
    mctp_pkt_challenge_auth[UT_CHALLENGE_AUTH_SPDM_SLOT_MASK_IDX] = 0x01;
    // TODO: Might need to fix endianness
    //mctp_pkt_challenge_auth[UT_CHALLENGE_AUTH_SPDM_OPAQUE_LENGTH_IDX] = 0;
    alt_u16* mctp_pkt_challenge_auth_ptr = (alt_u16*)&mctp_pkt_challenge_auth[UT_CHALLENGE_AUTH_SPDM_OPAQUE_LENGTH_IDX];
    *mctp_pkt_challenge_auth_ptr = 0x02;
    //mctp_pkt_challenge_auth[UT_CHALLENGE_AUTH_SPDM_OPAQUE_LENGTH_IDX + 1] = 0x02;
    mctp_pkt_challenge_auth[UT_MCTP_SOM_EOM_BYTE_IDX] = FIRST_AND_LAST_PKT;
    mctp_pkt_challenge_auth[UT_MCTP_BYTE_COUNT_IDX] = 238;
    mctp_pkt_challenge_auth[UT_MCTP_SOURCE_ADDR] = 0x02 << 1;

    alt_u8 meas_hash[SHA384_LENGTH] = {
    0x8d, 0x72, 0xb6, 0x20, 0xe1, 0x59, 0xb8, 0x52, 0xa3, 0x89, 0x31, 0x0e,
    0x03, 0x7c, 0x7e, 0x15, 0x93, 0x69, 0x87, 0xc4, 0xab, 0x82, 0x43, 0xfb,
    0xb3, 0xbb, 0x4b, 0xfd, 0x8b, 0x5a, 0xa7, 0x8d, 0x4b, 0xa2, 0xca, 0x36,
    0x3c, 0xe7, 0x29, 0x9c, 0x61, 0x0e, 0xb0, 0x2a, 0x75, 0x4e, 0x5a, 0x65
    };

    alt_u32* ptr = (alt_u32*) cert_chain_digest;
    alt_u32* pkt_ptr = (alt_u32*)&mctp_pkt_challenge_auth[UT_CHALLENGE_AUTH_SPDM_CERT_CHAIN_HASH_IDX];

    for (alt_u32 i = 0; i < SHA384_LENGTH/4; i++)
    {
        pkt_ptr[i] = ptr[i];
    }

    // Have to manually append CHALLENGE/CHALLENGE_AUTH message here to generate the right hash and signature
    // To clear this before calling challenge function
    append_large_buffer(big_buffer_ptr, (alt_u8*)&mctp_pkt_challenge[7], 36);
    append_large_buffer(big_buffer_ptr, (alt_u8*)&mctp_pkt_challenge_auth[7], (232 - 96));

    alt_u8 test_pubkey_cx[SHA384_LENGTH] = {0};
    alt_u8 test_pubkey_cy[SHA384_LENGTH] = {0};
    alt_u8 sig_r[SHA384_LENGTH] = {0};
    alt_u8 sig_s[SHA384_LENGTH] = {0};

    alt_u8* msg_to_hash = (alt_u8*)big_buffer_ptr->buffer;

    // Manually generate the public key here
    generate_pubkey((alt_u32*)test_pubkey_cx, (alt_u32*)test_pubkey_cy, (alt_u32*)priv_key_1, sha_mode);

    // Manually generate signature
    generate_ecdsa_signature((alt_u32*)sig_r, (alt_u32*)sig_s, (alt_u32*)msg_to_hash, big_buffer_ptr->buffer_size,
    	               (alt_u32*)test_pubkey_cx, (alt_u32*)priv_key_1, sha_mode);

    pkt_ptr = (alt_u32*)&mctp_pkt_challenge_auth[UT_CHALLENGE_AUTH_SPDM_SIGNATURE_IDX];

    ptr = (alt_u32*) sig_r;
    for (alt_u32 i = 0; i < (sha_size >> 2); i++)
    {
        pkt_ptr[i] = ptr[i];
    }

    pkt_ptr += (SHA384_LENGTH >> 2);
    ptr = (alt_u32*) sig_s;
    for (alt_u32 i = 0; i < (sha_size >> 2); i++)
    {
        pkt_ptr[i] = ptr[i];
    }

    // Send in the packet
    ut_send_in_pcie_mctp_packet(m_memory, mctp_pkt_challenge_auth, 239, 1, 0);
    EXPECT_EQ(read_from_mailbox(MB_MCTP_PCIE_PACKET_AVAIL_AND_RX_PACKET_ERROR), (alt_u32)0x01);

    alt_u8 get_measurement_pkt[43] = MCTP_SPDM_GET_MEASUREMENT_MSG;
    get_measurement_pkt[UT_GET_MEASUREMENT_REQUEST_ATTRIBUTE] = SPDM_GET_MEASUREMENT_REQUEST_GENERATE_SIG;
    get_measurement_pkt[UT_GET_MEASUREMENT_OPERATION_MEASUREMENT_NUMBER] = 0x01;
    get_measurement_pkt[UT_MCTP_SOM_EOM_BYTE_IDX] = FIRST_AND_LAST_PKT | TAG_OWNER;
    get_measurement_pkt[UT_MCTP_BYTE_COUNT_IDX] = 42;
    get_measurement_pkt[UT_MCTP_SOURCE_ADDR] = SRC_SLAVE_ADDR_WR;

    alt_u8 get_measurement_pkt_2[43] = MCTP_SPDM_GET_MEASUREMENT_MSG;
    get_measurement_pkt_2[UT_GET_MEASUREMENT_REQUEST_ATTRIBUTE] = SPDM_GET_MEASUREMENT_REQUEST_GENERATE_SIG;
    get_measurement_pkt_2[UT_GET_MEASUREMENT_OPERATION_MEASUREMENT_NUMBER] = 0x02;
    get_measurement_pkt_2[UT_MCTP_SOM_EOM_BYTE_IDX] = FIRST_AND_LAST_PKT | TAG_OWNER;
    get_measurement_pkt_2[UT_MCTP_BYTE_COUNT_IDX] = 42;
    get_measurement_pkt_2[UT_MCTP_SOURCE_ADDR] = SRC_SLAVE_ADDR_WR;

     alt_u8 measurement_pkt[218] = MCTP_SPDM_MEASUREMENT_MSG;
     // Shall test for 1 block of measurement
     measurement_pkt[UT_MEASUREMENT_TOTAL_NUMBER_OF_BLOCKS] = 1;
     measurement_pkt[UT_MEASUREMENT_RECORD_LENGTH] = UT_RECORD_LENGTH;
     measurement_pkt[UT_MEASUREMENT_RECORD_LENGTH + 1] = 0x00;
     measurement_pkt[UT_MEASUREMENT_RECORD_LENGTH + 2] = 0x00;
     measurement_pkt[UT_MEASUREMENT_RECORD] = 1;
     measurement_pkt[UT_MEASUREMENT_RECORD + 1] = 1;
     measurement_pkt[UT_MEASUREMENT_RECORD + 2] = UT_RECORD_LENGTH;
     measurement_pkt[UT_MEASUREMENT_RECORD + 3] = 0x00;
     measurement_pkt[UT_MEASUREMENT_RECORD + 4] = 0x01;
     measurement_pkt[UT_MEASUREMENT_RECORD + 5] = 64;
     measurement_pkt[UT_MEASUREMENT_RECORD + 6] = 0x00;

     // For simplicity, just randomly fill in the measurement hash
     // Do the same for AFM in the ufm after this
     for (alt_u32 i = 0; i < 64; i++)
     {
         if (i > 47)
         {
             measurement_pkt[UT_MEASUREMENT_RECORD + 7 + i] = 0;
         }
         else
         {
         	 measurement_pkt[UT_MEASUREMENT_RECORD + 7 + i] = meas_hash[48 - 1 - i];
         }
     }

     measurement_pkt[UT_MEASUREMENT_OPAQUE_LENGTH] = UT_OPAQUE_LENGTH;
     measurement_pkt[UT_MEASUREMENT_OPAQUE_LENGTH + 1] = 0x0;
     measurement_pkt[UT_MEASUREMENT_OPAQUE_LENGTH + 2] = 0xFE;
     measurement_pkt[UT_MEASUREMENT_OPAQUE_LENGTH + 3] = 0xCA;
     measurement_pkt[UT_MEASUREMENT_SIGNATURE] = 0;
     measurement_pkt[UT_MCTP_SOM_EOM_BYTE_IDX] = FIRST_AND_LAST_PKT;
     measurement_pkt[UT_MCTP_BYTE_COUNT_IDX] = 217;
     measurement_pkt[UT_MCTP_SOURCE_ADDR] = 0x02 << 1;

     alt_u32* meas_pkt_ptr = (alt_u32*)&measurement_pkt[UT_MEASUREMENT_SIGNATURE];

     LARGE_APPENDED_BUFFER big_buffer_2;
     LARGE_APPENDED_BUFFER* big_buffer_2_ptr = &big_buffer_2;
     reset_buffer((alt_u8*)big_buffer_2_ptr, sizeof(LARGE_APPENDED_BUFFER));

     append_large_buffer(big_buffer_2_ptr, (alt_u8*)&get_measurement_pkt[7], 36);
     append_large_buffer(big_buffer_2_ptr, (alt_u8*)&measurement_pkt[7], (211 - 96));

     msg_to_hash = (alt_u8*)big_buffer_2_ptr->buffer;

     // Manually generate signature
     generate_ecdsa_signature((alt_u32*)sig_r, (alt_u32*)sig_s, (alt_u32*)msg_to_hash, big_buffer_2_ptr->buffer_size,
     	               (alt_u32*)test_pubkey_cx, (alt_u32*)priv_key_1, sha_mode);

     ptr = (alt_u32*) sig_r;
     for (alt_u32 i = 0; i < (sha_size >> 2); i++)
     {
    	 meas_pkt_ptr[i] = ptr[i];
     }

     meas_pkt_ptr += (SHA384_LENGTH >> 2);
     ptr = (alt_u32*) sig_s;
     for (alt_u32 i = 0; i < (sha_size >> 2); i++)
     {
    	 meas_pkt_ptr[i] = ptr[i];
     }

     ut_send_in_pcie_mctp_packet(m_memory, measurement_pkt, 218, 1, 0);
     EXPECT_EQ(read_from_mailbox(MB_MCTP_PCIE_PACKET_AVAIL_AND_RX_PACKET_ERROR), (alt_u32)0x01);

    // Begin Test
    // To simulate the actual scenario in a T0 loop when CPLD (as a responder)
    // checks for any incoming requests from hosts, the unit test system will
    // inject beforehand all the necessary messages. In other words, the unit test
    // is required to staged all the necessary messages.

    // Once the firmware is done with one message, the firmware will indicate the
    // unit test system mock to prepare for the next staged message.
    // This is designed in this way so that the function can be called only once
    // for the entire message services.

    // A host starts a transaction by sending a Get_version message to CPLD
    // To stage the next message, set the last parameter of the function below
    // Staging the message in the unit test means the mock system will be waiting for
    // the completion of the processing of the first message before sending in the second message.

    reset_buffer((alt_u8*)big_buffer_2_ptr, sizeof(LARGE_APPENDED_BUFFER));

    measurement_pkt[UT_MEASUREMENT_RECORD] = 2;

    for (alt_u32 i = 0; i < 64; i++)
    {
        measurement_pkt[UT_MEASUREMENT_RECORD + 7 + i] = 0x3a;
    }

    append_large_buffer(big_buffer_2_ptr, (alt_u8*)&get_measurement_pkt_2[7], 36);
    append_large_buffer(big_buffer_2_ptr, (alt_u8*)&measurement_pkt[7], (211 - 96));

    msg_to_hash = (alt_u8*)big_buffer_2_ptr->buffer;

    // Manually generate signature
    generate_ecdsa_signature((alt_u32*)sig_r, (alt_u32*)sig_s, (alt_u32*)msg_to_hash, big_buffer_2_ptr->buffer_size,
    	               (alt_u32*)test_pubkey_cx, (alt_u32*)priv_key_1, sha_mode);

    meas_pkt_ptr = (alt_u32*)&measurement_pkt[UT_MEASUREMENT_SIGNATURE];

    ptr = (alt_u32*) sig_r;
    for (alt_u32 i = 0; i < (sha_size >> 2); i++)
    {
    	meas_pkt_ptr[i] = ptr[i];
    }

    meas_pkt_ptr += (SHA384_LENGTH >> 2);
    ptr = (alt_u32*) sig_s;
    for (alt_u32 i = 0; i < (sha_size >> 2); i++)
    {
    	meas_pkt_ptr[i] = ptr[i];
    }

    // Send in the packet
    ut_send_in_pcie_mctp_packet(m_memory, measurement_pkt, 218, 0, 0);
    EXPECT_EQ(read_from_mailbox(MB_MCTP_PCIE_PACKET_AVAIL_AND_RX_PACKET_ERROR), (alt_u32)0x01);
    /*
     * Execute the flow.
     */
    // Run PFR Main. Always run with the timeout
    ASSERT_DURATION_LE(250, pfr_main());

    EXPECT_EQ(read_from_mailbox(MB_PROVISION_STATUS_2), (alt_u32)MB_UFM_PROV_ATTESTATION_ENABLED);
    /*
     * Verify results
     */
    // Nios should transition from T0 to T-1 exactly once for authentication
    EXPECT_EQ(SYSTEM_MOCK::get()->get_t_minus_1_counter(), alt_u32(1));

    // Expect no spdm protocol error
    EXPECT_EQ(read_from_mailbox(MB_MAJOR_ERROR_CODE), (alt_u32) 0);
    EXPECT_EQ(read_from_mailbox(MB_MINOR_ERROR_CODE), (alt_u32) 0);

    // This is the expected message sent back to the hosts
    // The message is sent in byte by byte format
    EXPECT_EQ((alt_u32)(0x02 << 1), IORD(m_mctp_memory, 0));
    EXPECT_EQ((alt_u32)MCTP_COMMAND_CODE, IORD(m_mctp_memory, 0));
    for (alt_u32 i = 0; i < 11; i++)
    {
        EXPECT_EQ(expected_get_ver_msg[i], IORD(m_mctp_memory, 0));
    }

    // This is the expected message sent back to the hosts
    // The message is sent in byte by byte format
    EXPECT_EQ((alt_u32)(0x02 << 1), IORD(m_mctp_memory, 0));
    EXPECT_EQ((alt_u32)MCTP_COMMAND_CODE, IORD(m_mctp_memory, 0));
    for (alt_u32 i = 0; i < 11; i++)
    {
        EXPECT_EQ(get_cap_mctp_pkt[i], IORD(m_mctp_memory, 0));
    }

    EXPECT_EQ((alt_u32)(0x02 << 1), IORD(m_mctp_memory, 0));
    EXPECT_EQ((alt_u32)MCTP_COMMAND_CODE, IORD(m_mctp_memory, 0));
    for (alt_u32 i = 0; i < 39; i++)
    {
        EXPECT_EQ(expected_nego_algo[i], IORD(m_mctp_memory, 0));
    }

    EXPECT_EQ((alt_u32)(0x02 << 1), IORD(m_mctp_memory, 0));
    EXPECT_EQ((alt_u32)MCTP_COMMAND_CODE, IORD(m_mctp_memory, 0));
    for (alt_u32 i = 0; i < 11; i++)
    {
        EXPECT_EQ(mctp_get_digest_pkt[i], IORD(m_mctp_memory, 0));
    }

    // Expect CPLD to obtain the offset from previous cert chain
    *mctp_get_cert_pkt_ptr = 0xc8;
    // Expect CPLD to obtain the offset from previous cert chain
    *mctp_get_cert_pkt_ptr_2 = 0;

    EXPECT_EQ((alt_u32)(0x02 << 1), IORD(m_mctp_memory, 0));
    EXPECT_EQ((alt_u32)MCTP_COMMAND_CODE, IORD(m_mctp_memory, 0));
    for (alt_u32 i = 0; i < 15; i++)
    {
        EXPECT_EQ(mctp_get_cert_pkt[i], IORD(m_mctp_memory, 0));
    }

    // Expect CPLD to obtain the offset from previous cert chain
    *mctp_get_cert_pkt_ptr = 0xc8;
    // Expect CPLD to obtain the offset from previous cert chain
    *mctp_get_cert_pkt_ptr_2 = 0xc8;

    EXPECT_EQ((alt_u32)(0x02 << 1), IORD(m_mctp_memory, 0));
    EXPECT_EQ((alt_u32)MCTP_COMMAND_CODE, IORD(m_mctp_memory, 0));
    for (alt_u32 i = 0; i < 15; i++)
    {
        EXPECT_EQ(mctp_get_cert_pkt[i], IORD(m_mctp_memory, 0));
    }

    // Expect CPLD to obtain the offset from previous cert chain
    *mctp_get_cert_pkt_ptr = 0xc8;
    // Expect CPLD to obtain the offset from previous cert chain
    *mctp_get_cert_pkt_ptr_2 = 0xc8 + 0xc8;

    EXPECT_EQ((alt_u32)(0x02 << 1), IORD(m_mctp_memory, 0));
    EXPECT_EQ((alt_u32)MCTP_COMMAND_CODE, IORD(m_mctp_memory, 0));
    for (alt_u32 i = 0; i < 15; i++)
    {
        EXPECT_EQ(mctp_get_cert_pkt[i], IORD(m_mctp_memory, 0));
    }

    // Expect that CPLD request the remaining portion of the certificate
    *mctp_get_cert_pkt_ptr = 0xc8;
    // Expect CPLD to obtain the offset from previous cert chain
    *mctp_get_cert_pkt_ptr_2 = 0xc8 + 0xc8 + 0xc8;

    EXPECT_EQ((alt_u32)(0x02 << 1), IORD(m_mctp_memory, 0));
    EXPECT_EQ((alt_u32)MCTP_COMMAND_CODE, IORD(m_mctp_memory, 0));
    for (alt_u32 i = 0; i < 15; i++)
    {
        EXPECT_EQ(mctp_get_cert_pkt[i], IORD(m_mctp_memory, 0));
    }

    // Expect that CPLD request the remaining portion of the certificate
    *mctp_get_cert_pkt_ptr = 0xc8;
    // Expect CPLD to obtain the offset from previous cert chain
    *mctp_get_cert_pkt_ptr_2 = 0xc8 + 0xc8 + 0xc8 + 0xc8;

    EXPECT_EQ((alt_u32)(0x02 << 1), IORD(m_mctp_memory, 0));
    EXPECT_EQ((alt_u32)MCTP_COMMAND_CODE, IORD(m_mctp_memory, 0));
    for (alt_u32 i = 0; i < 15; i++)
    {
        EXPECT_EQ(mctp_get_cert_pkt[i], IORD(m_mctp_memory, 0));
    }

    // Expect that CPLD request the remaining portion of the certificate
    *mctp_get_cert_pkt_ptr = 0xc8;
    // Expect CPLD to obtain the offset from previous cert chain
    *mctp_get_cert_pkt_ptr_2 = 0xc8 + 0xc8 + 0xc8 + 0xc8 + 0xc8;

    EXPECT_EQ((alt_u32)(0x02 << 1), IORD(m_mctp_memory, 0));
    EXPECT_EQ((alt_u32)MCTP_COMMAND_CODE, IORD(m_mctp_memory, 0));
    for (alt_u32 i = 0; i < 15; i++)
    {
        EXPECT_EQ(mctp_get_cert_pkt[i], IORD(m_mctp_memory, 0));
    }

    // Expect that CPLD request the remaining portion of the certificate
    *mctp_get_cert_pkt_ptr = cert_chain_size - 0xc8 - 0xc8 - 0xc8 - 0xc8 - 0xc8 - 0xc8;
    // Expect CPLD to obtain the offset from previous cert chain
    *mctp_get_cert_pkt_ptr_2 = 0xc8 + 0xc8 + 0xc8 + 0xc8 + 0xc8 + 0xc8;

    EXPECT_EQ((alt_u32)(0x02 << 1), IORD(m_mctp_memory, 0));
    EXPECT_EQ((alt_u32)MCTP_COMMAND_CODE, IORD(m_mctp_memory, 0));
    for (alt_u32 i = 0; i < 15; i++)
    {
        EXPECT_EQ(mctp_get_cert_pkt[i], IORD(m_mctp_memory, 0));
    }

    EXPECT_EQ((alt_u32)(0x02 << 1), IORD(m_mctp_memory, 0));
    EXPECT_EQ((alt_u32)MCTP_COMMAND_CODE, IORD(m_mctp_memory, 0));
    for (alt_u32 i = 0; i < 43; i++)
    {
        EXPECT_EQ(mctp_pkt_challenge[i], IORD(m_mctp_memory, 0));
    }

    EXPECT_EQ((alt_u32)(0x02 << 1), IORD(m_mctp_memory, 0));
    EXPECT_EQ((alt_u32)MCTP_COMMAND_CODE, IORD(m_mctp_memory, 0));
    for (alt_u32 i = 0; i < 43; i++)
    {
        EXPECT_EQ(get_measurement_pkt[i], IORD(m_mctp_memory, 0));
    }

    EXPECT_EQ((alt_u32)(0x02 << 1), IORD(m_mctp_memory, 0));
    EXPECT_EQ((alt_u32)MCTP_COMMAND_CODE, IORD(m_mctp_memory, 0));
    for (alt_u32 i = 0; i < 43; i++)
    {
        EXPECT_EQ(get_measurement_pkt_2[i], IORD(m_mctp_memory, 0));
    }
}

TEST_F(PFRSpdmDebugTest, DISABLED_test_get_certificate_and_challenge_openspdm)
{
    // Initialise context
    // Move to separate function
    SPDM_CONTEXT spdm_context;
    SPDM_CONTEXT* spdm_context_ptr = &spdm_context;

    // Reset everything
    reset_buffer((alt_u8*)spdm_context_ptr, sizeof(SPDM_CONTEXT));

    MCTP_CONTEXT mctp_context;
    MCTP_CONTEXT* mctp_context_ptr = &mctp_context;

    reset_buffer((alt_u8*)mctp_context_ptr, sizeof(MCTP_CONTEXT));

    init_spdm_context(spdm_context_ptr);

    spdm_context_ptr->connection_info.algorithm.base_hash_algo = BASE_HASH_ALGO_TPM_ALG_ECDSA_ECC_NIST_P384;

    // Get Cert
    // Expect first message from host to request for version
    spdm_context_ptr->spdm_msg_state = SPDM_CERTIFICATE_FLAG;
    mctp_context_ptr->cached_addr = BMC_SLAVE_ADDRESS;
    mctp_context_ptr->msg_type = MCTP_MESSAGE_TYPE;

    // Cert chain obtained from OPENSPDM responder
	alt_u8 cert_chain_buffer[1544] = {
			//cert chain length and reserved bytes
			0x08, 0x06, 0x00, 0x00,
			//start of root hash
			0x5a, 0x64, 0xb3, 0x8b, 0x5d, 0x5f, 0x4d, 0xb3, 0x5f, 0xb2, 0xaa, 0x1d,
			0x46, 0x9f, 0x6a, 0xdc, 0xca, 0x7f, 0xac, 0x85, 0xbe, 0xf0, 0x84, 0x10,
			0x9c, 0xcd, 0x54, 0x09, 0xf0, 0xab, 0x38, 0x3a, 0xaa, 0xf7, 0xa6, 0x2e,
			0x3b, 0xd7, 0x81, 0x2c, 0xea, 0x24, 0x7e, 0x14, 0xa9, 0x56, 0x9d, 0x28,
			//start of cert chain
			0x30, 0x82, 0x01, 0xcf, 0x30, 0x82, 0x01, 0x56, 0xa0, 0x03, 0x02, 0x01,
			0x02, 0x02, 0x14, 0x20, 0x3a, 0xc2, 0x59, 0xcc, 0xda, 0xcb, 0xf6, 0x72,
			0xf1, 0xc0, 0x1a, 0x62, 0x1a, 0x45, 0x82, 0x90, 0x24, 0xb8, 0xaf, 0x30,
			0x0a, 0x06, 0x08, 0x2a, 0x86, 0x48, 0xce, 0x3d, 0x04, 0x03, 0x03, 0x30,
			0x1f, 0x31, 0x1d, 0x30, 0x1b, 0x06, 0x03, 0x55, 0x04, 0x03, 0x0c, 0x14,
			0x69, 0x6e, 0x74, 0x65, 0x6c, 0x20, 0x74, 0x65, 0x73, 0x74, 0x20, 0x45,
			0x43, 0x50, 0x32, 0x35, 0x36, 0x20, 0x43, 0x41, 0x30, 0x1e, 0x17, 0x0d,
			0x32, 0x31, 0x30, 0x32, 0x30, 0x39, 0x30, 0x30, 0x35, 0x30, 0x35, 0x38,
			0x5a, 0x17, 0x0d, 0x33, 0x31, 0x30, 0x32, 0x30, 0x37, 0x30, 0x30, 0x35,
			0x30, 0x35, 0x38, 0x5a, 0x30, 0x1f, 0x31, 0x1d, 0x30, 0x1b, 0x06, 0x03,
			0x55, 0x04, 0x03, 0x0c, 0x14, 0x69, 0x6e, 0x74, 0x65, 0x6c, 0x20, 0x74,
			0x65, 0x73, 0x74, 0x20, 0x45, 0x43, 0x50, 0x32, 0x35, 0x36, 0x20, 0x43,
			0x41, 0x30, 0x76, 0x30, 0x10, 0x06, 0x07, 0x2a, 0x86, 0x48, 0xce, 0x3d,
			0x02, 0x01, 0x06, 0x05, 0x2b, 0x81, 0x04, 0x00, 0x22, 0x03, 0x62, 0x00,
			0x04, 0x99, 0x8f, 0x81, 0x68, 0x9a, 0x83, 0x9b, 0x83, 0x39, 0xad, 0x0e,
			0x32, 0x8d, 0xb9, 0x42, 0x0d, 0xae, 0xcc, 0x91, 0xa9, 0xbc, 0x4a, 0xe1,
			0xbb, 0x79, 0x4c, 0x22, 0xfa, 0x3f, 0x0c, 0x9d, 0x93, 0x3c, 0x1a, 0x02,
			0x5c, 0xc2, 0x73, 0x05, 0xec, 0x43, 0x5d, 0x04, 0x02, 0xb1, 0x68, 0xb3,
			0xf4, 0xd8, 0xde, 0x0c, 0x8d, 0x53, 0xb7, 0x04, 0x8e, 0xa1, 0x43, 0x9a,
			0xeb, 0x31, 0x0d, 0xaa, 0xce, 0x89, 0x2d, 0xba, 0x73, 0xda, 0x4f, 0x1e,
			0x39, 0x5d, 0x92, 0x11, 0x21, 0x38, 0xb4, 0x00, 0xd4, 0xf5, 0x55, 0x8c,
			0xe8, 0x71, 0x30, 0x3d, 0x46, 0x83, 0xf4, 0xc4, 0x52, 0x50, 0xda, 0x12,
			0x5b, 0xa3, 0x53, 0x30, 0x51, 0x30, 0x1d, 0x06, 0x03, 0x55, 0x1d, 0x0e,
			0x04, 0x16, 0x04, 0x14, 0xcf, 0x09, 0xd4, 0x7a, 0xee, 0x08, 0x90, 0x62,
			0xbf, 0xe6, 0x9c, 0xb4, 0xb9, 0xdf, 0xe1, 0x41, 0x33, 0x1c, 0x03, 0xa5,
			0x30, 0x1f, 0x06, 0x03, 0x55, 0x1d, 0x23, 0x04, 0x18, 0x30, 0x16, 0x80,
			0x14, 0xcf, 0x09, 0xd4, 0x7a, 0xee, 0x08, 0x90, 0x62, 0xbf, 0xe6, 0x9c,
			0xb4, 0xb9, 0xdf, 0xe1, 0x41, 0x33, 0x1c, 0x03, 0xa5, 0x30, 0x0f, 0x06,
			0x03, 0x55, 0x1d, 0x13, 0x01, 0x01, 0xff, 0x04, 0x05, 0x30, 0x03, 0x01,
			0x01, 0xff, 0x30, 0x0a, 0x06, 0x08, 0x2a, 0x86, 0x48, 0xce, 0x3d, 0x04,
			0x03, 0x03, 0x03, 0x67, 0x00, 0x30, 0x64, 0x02, 0x30, 0x5a, 0xb4, 0xf5,
			0x95, 0x25, 0x82, 0xf6, 0x68, 0x3e, 0x49, 0xc7, 0xb4, 0xbb, 0x42, 0x81,
			0x91, 0x7e, 0x38, 0xd0, 0x2d, 0xac, 0x53, 0xae, 0x8e, 0xb0, 0x51, 0x50,
			0xaa, 0xf8, 0x7e, 0xff, 0xc0, 0x30, 0xab, 0xd5, 0x08, 0x5b, 0x06, 0xf7,
			0xe1, 0xbf, 0x39, 0xd2, 0x3e, 0xae, 0xbf, 0x8e, 0x48, 0x02, 0x30, 0x09,
			0x75, 0xa8, 0xc0, 0x6f, 0x4f, 0x3c, 0xad, 0x5d, 0x4e, 0x4f, 0xf8, 0x2c,
			0x3b, 0x39, 0x46, 0xa0, 0xdf, 0x83, 0x8e, 0xb5, 0xd3, 0x61, 0x61, 0x59,
			0xbc, 0x39, 0xd7, 0xad, 0x68, 0x5e, 0x0d, 0x4f, 0x3f, 0xe2, 0xca, 0xc1,
			0x74, 0x8f, 0x47, 0x37, 0x11, 0xc8, 0x22, 0x59, 0x6f, 0x64, 0x52, 0x30,
			0x82, 0x01, 0xd7, 0x30, 0x82, 0x01, 0x5d, 0xa0, 0x03, 0x02, 0x01, 0x02,
			0x02, 0x01, 0x01, 0x30, 0x0a, 0x06, 0x08, 0x2a, 0x86, 0x48, 0xce, 0x3d,
			0x04, 0x03, 0x03, 0x30, 0x1f, 0x31, 0x1d, 0x30, 0x1b, 0x06, 0x03, 0x55,
			0x04, 0x03, 0x0c, 0x14, 0x69, 0x6e, 0x74, 0x65, 0x6c, 0x20, 0x74, 0x65,
			0x73, 0x74, 0x20, 0x45, 0x43, 0x50, 0x32, 0x35, 0x36, 0x20, 0x43, 0x41,
			0x30, 0x1e, 0x17, 0x0d, 0x32, 0x31, 0x30, 0x32, 0x30, 0x39, 0x30, 0x30,
			0x35, 0x30, 0x35, 0x39, 0x5a, 0x17, 0x0d, 0x33,	0x31, 0x30, 0x32, 0x30,
			0x37, 0x30, 0x30, 0x35, 0x30, 0x35, 0x39, 0x5a, 0x30, 0x2e, 0x31, 0x2c,
			0x30, 0x2a, 0x06, 0x03, 0x55, 0x04, 0x03, 0x0c, 0x23, 0x69, 0x6e, 0x74,
			0x65, 0x6c, 0x20, 0x74, 0x65, 0x73, 0x74, 0x20, 0x45, 0x43, 0x50, 0x32,
			0x35, 0x36, 0x20, 0x69, 0x6e, 0x74, 0x65, 0x72, 0x6d, 0x65, 0x64, 0x69,
			0x61, 0x74, 0x65, 0x20, 0x63, 0x65, 0x72, 0x74, 0x30, 0x76, 0x30, 0x10,
			0x06, 0x07, 0x2a, 0x86, 0x48, 0xce, 0x3d, 0x02, 0x01, 0x06, 0x05, 0x2b,
			0x81, 0x04, 0x00, 0x22, 0x03, 0x62, 0x00, 0x04, 0x77, 0x1b, 0x24, 0xf6,
			0xc6, 0x76, 0x1f, 0xb8, 0x30, 0x07, 0x8b, 0xb8, 0xa3, 0x9e, 0xc0, 0x26,
			0xc1, 0xea, 0x7d, 0xfc, 0x29, 0x7d, 0xe0, 0x59, 0xb2, 0x64, 0x32, 0x75,
			0x4a, 0xe3, 0x02, 0x64, 0x3c, 0xbc, 0x85, 0x8e, 0xc6, 0xec, 0xef, 0xb0,
			0x79, 0xf4, 0xc1, 0xa4, 0xb9, 0xbb, 0x29, 0x6b, 0xae, 0xad, 0xf0, 0x7d,
			0x63, 0xc6, 0xaf, 0xb3, 0x73, 0x5e, 0x4f, 0x3f, 0xfe, 0x89, 0x8a, 0xbb,
			0x7d, 0x2b, 0x60, 0x3e, 0x16, 0xba, 0x82, 0xcf, 0xa4, 0x70, 0x04, 0x85,
			0xc3, 0xa3, 0x3c, 0x5e, 0x6a, 0xa0, 0xef, 0xda, 0xd5, 0x20, 0x30, 0x19,
			0xba, 0x79, 0x95, 0xb0, 0xc2, 0x7f, 0x4c, 0xdd, 0xa3, 0x5e, 0x30, 0x5c,
			0x30, 0x0c, 0x06, 0x03, 0x55, 0x1d, 0x13, 0x04, 0x05, 0x30, 0x03, 0x01,
			0x01, 0xff, 0x30, 0x0b,	0x06, 0x03, 0x55, 0x1d, 0x0f, 0x04, 0x04, 0x03,
			0x02, 0x01, 0xfe, 0x30, 0x1d, 0x06, 0x03, 0x55, 0x1d, 0x0e, 0x04, 0x16,
			0x04, 0x14, 0x12, 0xe0, 0x1a, 0x23, 0xc6, 0x23, 0xe4, 0x02, 0x58, 0x0b,
			0x06, 0xac, 0x90, 0xfa, 0x4b, 0x80, 0x3d, 0xc9, 0xf1, 0x1d, 0x30, 0x20,
			0x06, 0x03, 0x55, 0x1d, 0x25, 0x01, 0x01, 0xff, 0x04, 0x16, 0x30, 0x14,
			0x06, 0x08, 0x2b, 0x06, 0x01, 0x05, 0x05, 0x07, 0x03, 0x01, 0x06, 0x08,
			0x2b, 0x06, 0x01, 0x05, 0x05, 0x07, 0x03, 0x02, 0x30, 0x0a, 0x06, 0x08,
			0x2a, 0x86, 0x48, 0xce, 0x3d, 0x04, 0x03, 0x03, 0x03, 0x68, 0x00, 0x30,
			0x65, 0x02, 0x30, 0x03, 0x32, 0xb1, 0x8b, 0x20, 0xf4, 0x76, 0xda, 0x8c,
			0x83, 0x96, 0x87, 0x55, 0xd9, 0x12, 0x72, 0xbd, 0x58, 0x4d, 0x0a, 0x37,
			0xaf, 0x29, 0x95, 0x1d, 0x36, 0xc4, 0x9e, 0xa5, 0xcd, 0xe2, 0x3b, 0xf5,
			0xe0, 0x7a, 0x64, 0x36, 0x1e, 0xd4, 0xf1, 0xe1, 0xbb, 0x14, 0x57, 0x9e,
			0x86, 0x82, 0x72, 0x02, 0x31, 0x00, 0xc0, 0xd6, 0x02, 0x99, 0x50, 0x76,
			0x34, 0x16, 0xd6, 0x51, 0x9c, 0xc4, 0x86, 0x08, 0x68, 0x94, 0xbf, 0x3c,
			0x09, 0x7e, 0x10, 0xe5, 0x62, 0x8a, 0xba, 0x48, 0x0a, 0xa5, 0xed, 0x1a,
			0x6a, 0xf6, 0x3c, 0x2f, 0x4d, 0x38, 0x5d, 0x7d, 0x5c, 0x60, 0x63, 0x88,
			0x84, 0x5d, 0x49, 0x33, 0xe2, 0xa7, 0x30, 0x82, 0x02, 0x22, 0x30, 0x82,
			0x01, 0xa8, 0xa0, 0x03, 0x02, 0x01, 0x02, 0x02, 0x01, 0x03, 0x30, 0x0a,
			0x06, 0x08, 0x2a, 0x86, 0x48, 0xce, 0x3d, 0x04, 0x03, 0x03, 0x30, 0x2e,
			0x31, 0x2c, 0x30, 0x2a, 0x06, 0x03, 0x55, 0x04, 0x03, 0x0c, 0x23, 0x69,
			0x6e, 0x74, 0x65, 0x6c, 0x20, 0x74, 0x65, 0x73, 0x74, 0x20, 0x45, 0x43,
			0x50, 0x32, 0x35, 0x36, 0x20, 0x69, 0x6e, 0x74, 0x65, 0x72, 0x6d, 0x65,
			0x64, 0x69, 0x61, 0x74, 0x65, 0x20, 0x63, 0x65, 0x72, 0x74, 0x30, 0x1e,
			0x17, 0x0d, 0x32, 0x31, 0x30, 0x32, 0x30, 0x39, 0x30, 0x30, 0x35, 0x30,
			0x35, 0x39, 0x5a, 0x17, 0x0d, 0x32, 0x32, 0x30, 0x32, 0x30, 0x39, 0x30,
			0x30, 0x35, 0x30, 0x35, 0x39, 0x5a, 0x30, 0x2b, 0x31, 0x29, 0x30, 0x27,
			0x06, 0x03, 0x55, 0x04, 0x03, 0x0c, 0x20, 0x69, 0x6e, 0x74, 0x65, 0x6c,
			0x20, 0x74, 0x65, 0x73, 0x74, 0x20, 0x45, 0x43, 0x50, 0x32, 0x35, 0x36,
			0x20, 0x72, 0x65, 0x73, 0x70, 0x6f, 0x6e, 0x64, 0x65, 0x72, 0x20, 0x63,
			0x65, 0x72, 0x74, 0x30, 0x76, 0x30, 0x10, 0x06, 0x07, 0x2a, 0x86, 0x48,
			0xce, 0x3d, 0x02, 0x01, 0x06, 0x05, 0x2b, 0x81, 0x04, 0x00, 0x22, 0x03,
			0x62, 0x00, 0x04, 0x6c, 0x22, 0x41, 0xdf, 0xb7, 0xe4, 0xd6, 0x8d, 0x53,
			0x72, 0x4e, 0x4a, 0x1b, 0x99, 0x82, 0xe6, 0x56, 0xd2, 0x2d, 0x97, 0x4b,
			0x98, 0x40, 0xa9, 0x99, 0xd6, 0x0d, 0xd8, 0xe9,	0xa6, 0xfc, 0x74, 0xb9,
			0xce, 0x89, 0x48, 0xa7, 0xb5, 0x09, 0xb6, 0x24, 0x49, 0xd6, 0x23, 0xb3,
			0x5f, 0x3a, 0xf0, 0x99, 0xb0, 0xca, 0x63, 0x7d, 0x24, 0xfe, 0xe9, 0x12,
			0x19, 0x0f, 0xc2, 0x73, 0x1c, 0xe3, 0x76, 0x91, 0xec, 0x57, 0x6c, 0xcd,
			0x7b, 0xab, 0x32, 0xfd, 0x6d, 0x6e, 0x92, 0x7d, 0x37, 0x60, 0x01, 0xdb,
			0x13, 0x92, 0x3b, 0x77, 0xf7, 0x12, 0x97, 0x1d, 0x5e, 0xe3, 0xb9, 0x15,
			0x83, 0xaf, 0x89, 0xa3, 0x81, 0x9c, 0x30, 0x81, 0x99, 0x30, 0x0c, 0x06,
			0x03, 0x55, 0x1d, 0x13, 0x01, 0x01, 0xff, 0x04, 0x02, 0x30, 0x00, 0x30,
			0x0b, 0x06, 0x03, 0x55, 0x1d, 0x0f, 0x04, 0x04, 0x03, 0x02, 0x05, 0xe0,
			0x30, 0x1d, 0x06, 0x03, 0x55, 0x1d, 0x0e, 0x04, 0x16, 0x04, 0x14, 0x48,
			0x1f, 0x5d, 0x95, 0xce, 0x89, 0xd4, 0x7d, 0xa4, 0x4c, 0x21, 0x8f, 0x5b,
			0xd5, 0x50, 0x96, 0xff, 0xba, 0xe2, 0xee, 0x30, 0x31, 0x06, 0x03, 0x55,
			0x1d, 0x11, 0x04, 0x2a, 0x30, 0x28, 0xa0, 0x26, 0x06, 0x0a, 0x2b, 0x06,
			0x01, 0x04, 0x01, 0x83, 0x1c, 0x82, 0x12, 0x01, 0xa0, 0x18, 0x0c, 0x16,
			0x41, 0x43, 0x4d, 0x45, 0x3a, 0x57, 0x49, 0x44, 0x47, 0x45, 0x54, 0x3a,
			0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x30, 0x30, 0x2a,
			0x06, 0x03, 0x55, 0x1d, 0x25, 0x01, 0x01, 0xff, 0x04, 0x20, 0x30, 0x1e,
			0x06, 0x08, 0x2b, 0x06,	0x01, 0x05, 0x05, 0x07, 0x03, 0x01, 0x06, 0x08,
			0x2b, 0x06, 0x01, 0x05, 0x05, 0x07, 0x03, 0x02, 0x06, 0x08, 0x2b, 0x06,
			0x01, 0x05, 0x05, 0x07, 0x03, 0x09, 0x30, 0x0a, 0x06, 0x08, 0x2a, 0x86,
			0x48, 0xce, 0x3d, 0x04, 0x03, 0x03, 0x03, 0x68, 0x00, 0x30, 0x65, 0x02,
			0x30, 0x08, 0xe6, 0x1f, 0x0d, 0xdf, 0x18, 0xd3, 0x2f, 0x50, 0x49, 0x99,
			0xb0, 0xe2, 0x64, 0x95, 0x30, 0xa9, 0x5a, 0xbf, 0x83, 0x76, 0xae, 0x4a,
			0x39, 0xd8, 0xe2, 0x51, 0x12, 0x84, 0x9c, 0xbe, 0x11, 0x1d, 0x3b, 0x77,
			0x20, 0x6f, 0x05, 0x6c, 0xc7, 0x98, 0xb2, 0xba, 0xb8, 0x96, 0x75, 0x25,
			0xcf, 0x02, 0x31, 0x00, 0x93, 0x12, 0x5b, 0x66, 0x93, 0xc0, 0xe7, 0x56,
			0x1b, 0x68, 0x28, 0x27, 0xd8, 0x8e, 0x69, 0xaa, 0x30, 0x76, 0x05, 0x6f,
			0x4b, 0xd0, 0xce, 0x10, 0x0f, 0xf8, 0xdf, 0x4a, 0xab, 0x9b, 0x4d, 0xb1,
			0x47, 0xe4, 0xcd, 0xce, 0xce, 0x48, 0x0d, 0xf8, 0x35, 0x3d, 0xbc, 0x25,
			0xce, 0xec, 0xb9, 0xca
	};
    alt_u8 mctp_cert_packet_with_cert_chain[500] = {0};

    alt_u8 mctp_get_cert_pkt[15] = MCTP_SPDM_GET_CERTIFICATE_MSG;
    mctp_get_cert_pkt[UT_MCTP_SOM_EOM_BYTE_IDX] = FIRST_AND_LAST_PKT | TAG_OWNER;
    mctp_get_cert_pkt[UT_MCTP_BYTE_COUNT_IDX] = 0x0e;
    mctp_get_cert_pkt[UT_MCTP_SOURCE_ADDR] = SRC_SLAVE_ADDR_WR;
    //mctp_get_cert_pkt[UT_MCTP_SPDM_GET_CERTIFICATE_OFFSET_MASK] = 0;
    alt_u16* mctp_get_cert_pkt_ptr = (alt_u16*)&mctp_get_cert_pkt[UT_MCTP_SPDM_GET_CERTIFICATE_LENGTH_MASK - 1];
    *mctp_get_cert_pkt_ptr = CERT_REQUEST_SIZE;
    alt_u16* mctp_get_cert_pkt_ptr_2 = (alt_u16*)&mctp_get_cert_pkt[UT_MCTP_SPDM_GET_CERTIFICATE_OFFSET_MASK - 1];
    *mctp_get_cert_pkt_ptr_2 = 0;

    alt_u8 mctp_cert_pkt[15] = MCTP_SPDM_CERTIFICATE_MSG;
    mctp_cert_pkt[UT_MCTP_SOM_EOM_BYTE_IDX] = FIRST_AND_LAST_PKT;
    mctp_cert_pkt[UT_MCTP_BYTE_COUNT_IDX] = 206 + 8;
    mctp_cert_pkt[UT_MCTP_SOURCE_ADDR] = BMC_SLAVE_ADDRESS;
    //mctp_cert_pkt[UT_MCTP_SPDM_CERTIFICATE_PORTION_LENGTH_MASK] = 128;
    //mctp_cert_pkt[UT_MCTP_SPDM_CERTIFICATE_REMAINDER_LENGTH_MASK] = 0;
    alt_u16* mctp_get_pkt_ptr = (alt_u16*)&mctp_cert_pkt[UT_MCTP_SPDM_CERTIFICATE_PORTION_LENGTH_MASK - 1];
    *mctp_get_pkt_ptr = 0xc8;
    alt_u16* mctp_get_pkt_ptr_2 = (alt_u16*)&mctp_cert_pkt[UT_MCTP_SPDM_CERTIFICATE_REMAINDER_LENGTH_MASK - 1];
    *mctp_get_pkt_ptr_2 = 0x540;

    for (alt_u32 i = 0; i < 15; i++)
    {
        mctp_cert_packet_with_cert_chain[i] = mctp_cert_pkt[i];
    }

    for (alt_u32 i = 15; i < 15 + 200; i++)
    {
        mctp_cert_packet_with_cert_chain[i] = cert_chain_buffer[i - 15];
    }

    spdm_context_ptr->local_context.cert_flag = 1;
    alt_u8* stored_digest_buf_ptr = (alt_u8*)&spdm_context_ptr->local_context.stored_digest_buf[0];

    const alt_u8 expected_hash[SHA384_LENGTH] = {
        0x28, 0xaf, 0x70, 0x27, 0xbc, 0x2d, 0x95, 0xb5, 0xa0, 0xe4, 0x26, 0x04,
		0xc5, 0x8c, 0x5c, 0x3c, 0xbf, 0xa2, 0xc8, 0x24, 0xa6, 0x30, 0xca, 0x2f,
		0x0f, 0x4a, 0x79, 0x35, 0x57, 0xfb, 0x39, 0x3b, 0xdd, 0x8a, 0xc8, 0x8a,
		0x92, 0xd8, 0xa3, 0x70, 0x17, 0x12, 0x83, 0x9b, 0x66, 0xe1, 0x3a, 0x3a
    };

    for (alt_u32 i = 0; i < SHA384_LENGTH; i++)
    {
        stored_digest_buf_ptr[i] = expected_hash[i];
    }

    // Send in the packet to RXFIFO
    // Need not stage the message as only one message exchange, hence set last param to 0
    ut_send_in_bmc_mctp_packet(m_memory, mctp_cert_packet_with_cert_chain, 207 + 8, 1, 0);

    // Set remainder
    *mctp_get_pkt_ptr_2 = 0x478;

    for (alt_u32 i = 0; i < 15; i++)
    {
        mctp_cert_packet_with_cert_chain[i] = mctp_cert_pkt[i];
    }

    for (alt_u32 i = 15; i < 15 + 200; i++)
    {
        mctp_cert_packet_with_cert_chain[i] = cert_chain_buffer[i - 15 + 200];
    }

    ut_send_in_bmc_mctp_packet(m_memory, mctp_cert_packet_with_cert_chain, 207 + 8, 1, 0);

    // Set remainder
    *mctp_get_pkt_ptr_2 = 0x3b0;

    for (alt_u32 i = 0; i < 15; i++)
    {
        mctp_cert_packet_with_cert_chain[i] = mctp_cert_pkt[i];
    }

    for (alt_u32 i = 15; i < 15 + 200; i++)
    {
        mctp_cert_packet_with_cert_chain[i] = cert_chain_buffer[i - 15 + 200 + 200];
    }

    ut_send_in_bmc_mctp_packet(m_memory, mctp_cert_packet_with_cert_chain, 207 + 8, 1, 0);

    // Set remainder
    *mctp_get_pkt_ptr_2 = 0x2e8;

    for (alt_u32 i = 0; i < 15; i++)
    {
        mctp_cert_packet_with_cert_chain[i] = mctp_cert_pkt[i];
    }

    for (alt_u32 i = 15; i < 15 + 200; i++)
    {
        mctp_cert_packet_with_cert_chain[i] = cert_chain_buffer[i - 15 + 200 + 200 + 200];
    }

    ut_send_in_bmc_mctp_packet(m_memory, mctp_cert_packet_with_cert_chain, 207 + 8, 1, 0);

    // Set remainder
    *mctp_get_pkt_ptr_2 = 0x220;

    for (alt_u32 i = 0; i < 15; i++)
    {
        mctp_cert_packet_with_cert_chain[i] = mctp_cert_pkt[i];
    }

    for (alt_u32 i = 15; i < 15 + 200; i++)
    {
        mctp_cert_packet_with_cert_chain[i] = cert_chain_buffer[i - 15 + 200 + 200 + 200 + 200];
    }

    ut_send_in_bmc_mctp_packet(m_memory, mctp_cert_packet_with_cert_chain, 207 + 8, 1, 0);

    // Set remainder
    *mctp_get_pkt_ptr_2 = 0x158;

    for (alt_u32 i = 0; i < 15; i++)
    {
        mctp_cert_packet_with_cert_chain[i] = mctp_cert_pkt[i];
    }

    for (alt_u32 i = 15; i < 15 + 200; i++)
    {
        mctp_cert_packet_with_cert_chain[i] = cert_chain_buffer[i - 15 + 200 + 200 + 200 + 200 + 200];
    }

    ut_send_in_bmc_mctp_packet(m_memory, mctp_cert_packet_with_cert_chain, 207 + 8, 1, 0);

    // Set remainder
    *mctp_get_pkt_ptr_2 = 0x90;

    for (alt_u32 i = 0; i < 15; i++)
    {
        mctp_cert_packet_with_cert_chain[i] = mctp_cert_pkt[i];
    }

    for (alt_u32 i = 15; i < 15 + 200; i++)
    {
        mctp_cert_packet_with_cert_chain[i] = cert_chain_buffer[i - 15 + 200 + 200 + 200 + 200 + 200 + 200];
    }

    ut_send_in_bmc_mctp_packet(m_memory, mctp_cert_packet_with_cert_chain, 207 + 8, 1, 0);

    // Set remainder
    *mctp_get_pkt_ptr_2 = 0x00;
    *mctp_get_pkt_ptr = 0x90;
    mctp_cert_pkt[UT_MCTP_BYTE_COUNT_IDX] = 206 + 8 - 56;

    for (alt_u32 i = 0; i < 15; i++)
    {
        mctp_cert_packet_with_cert_chain[i] = mctp_cert_pkt[i];
    }

    for (alt_u32 i = 15; i < 15 + 200 - 56; i++)
    {
        mctp_cert_packet_with_cert_chain[i] = cert_chain_buffer[i - 15 + 200 + 200 + 200 + 200 + 200 + 200 + 200];
    }

    ut_send_in_bmc_mctp_packet(m_memory, mctp_cert_packet_with_cert_chain, 207 + 8 - 56, 0, 0);

    EXPECT_EQ(requester_cpld_get_certificate(spdm_context_ptr, mctp_context_ptr, BMC), alt_u32(0));

    EXPECT_EQ((alt_u32)(BMC_SLAVE_ADDRESS << 1), IORD(m_mctp_memory, 0));
    EXPECT_EQ((alt_u32)MCTP_COMMAND_CODE, IORD(m_mctp_memory, 0));
    for (alt_u32 i = 0; i < 15; i++)
    {
        EXPECT_EQ(mctp_get_cert_pkt[i], IORD(m_mctp_memory, 0));
    }

    // Expect CPLD to obtain the offset from previous cert chain
    *mctp_get_cert_pkt_ptr_2 = 0xc8;

    EXPECT_EQ((alt_u32)(BMC_SLAVE_ADDRESS << 1), IORD(m_mctp_memory, 0));
    EXPECT_EQ((alt_u32)MCTP_COMMAND_CODE, IORD(m_mctp_memory, 0));
    for (alt_u32 i = 0; i < 15; i++)
    {
        EXPECT_EQ(mctp_get_cert_pkt[i], IORD(m_mctp_memory, 0));
    }

    // Expect that CPLD request the remaining portion of the certificate
    *mctp_get_cert_pkt_ptr = 0xc8;
    // Expect CPLD to obtain the offset from previous cert chain
    *mctp_get_cert_pkt_ptr_2 = 0x190;

    EXPECT_EQ((alt_u32)(BMC_SLAVE_ADDRESS << 1), IORD(m_mctp_memory, 0));
    EXPECT_EQ((alt_u32)MCTP_COMMAND_CODE, IORD(m_mctp_memory, 0));
    for (alt_u32 i = 0; i < 15; i++)
    {
        EXPECT_EQ(mctp_get_cert_pkt[i], IORD(m_mctp_memory, 0));
    }

    // Expect that CPLD request the remaining portion of the certificate
    *mctp_get_cert_pkt_ptr = 0xc8;
    // Expect CPLD to obtain the offset from previous cert chain
    *mctp_get_cert_pkt_ptr_2 = 0x258;

    EXPECT_EQ((alt_u32)(BMC_SLAVE_ADDRESS << 1), IORD(m_mctp_memory, 0));
    EXPECT_EQ((alt_u32)MCTP_COMMAND_CODE, IORD(m_mctp_memory, 0));
    for (alt_u32 i = 0; i < 15; i++)
    {
        EXPECT_EQ(mctp_get_cert_pkt[i], IORD(m_mctp_memory, 0));
    }

    // Expect that CPLD request the remaining portion of the certificate
    *mctp_get_cert_pkt_ptr = 0xc8;
    // Expect CPLD to obtain the offset from previous cert chain
    *mctp_get_cert_pkt_ptr_2 = 0x320;

    EXPECT_EQ((alt_u32)(BMC_SLAVE_ADDRESS << 1), IORD(m_mctp_memory, 0));
    EXPECT_EQ((alt_u32)MCTP_COMMAND_CODE, IORD(m_mctp_memory, 0));
    for (alt_u32 i = 0; i < 15; i++)
    {
        EXPECT_EQ(mctp_get_cert_pkt[i], IORD(m_mctp_memory, 0));
    }

    // Expect that CPLD request the remaining portion of the certificate
    *mctp_get_cert_pkt_ptr = 0xc8;
    // Expect CPLD to obtain the offset from previous cert chain
    *mctp_get_cert_pkt_ptr_2 = 0x3e8;

    EXPECT_EQ((alt_u32)(BMC_SLAVE_ADDRESS << 1), IORD(m_mctp_memory, 0));
    EXPECT_EQ((alt_u32)MCTP_COMMAND_CODE, IORD(m_mctp_memory, 0));
    for (alt_u32 i = 0; i < 15; i++)
    {
        EXPECT_EQ(mctp_get_cert_pkt[i], IORD(m_mctp_memory, 0));
    }

    // Expect that CPLD request the remaining portion of the certificate
    *mctp_get_cert_pkt_ptr = 0xc8;
    // Expect CPLD to obtain the offset from previous cert chain
    *mctp_get_cert_pkt_ptr_2 = 0x4b0;

    EXPECT_EQ((alt_u32)(BMC_SLAVE_ADDRESS << 1), IORD(m_mctp_memory, 0));
    EXPECT_EQ((alt_u32)MCTP_COMMAND_CODE, IORD(m_mctp_memory, 0));
    for (alt_u32 i = 0; i < 15; i++)
    {
        EXPECT_EQ(mctp_get_cert_pkt[i], IORD(m_mctp_memory, 0));
    }

    // Expect that CPLD request the remaining portion of the certificate
    *mctp_get_cert_pkt_ptr = 0x90;
    // Expect CPLD to obtain the offset from previous cert chain
    *mctp_get_cert_pkt_ptr_2 = 0x578;

    EXPECT_EQ((alt_u32)(BMC_SLAVE_ADDRESS << 1), IORD(m_mctp_memory, 0));
    EXPECT_EQ((alt_u32)MCTP_COMMAND_CODE, IORD(m_mctp_memory, 0));
    for (alt_u32 i = 0; i < 15; i++)
    {
        EXPECT_EQ(mctp_get_cert_pkt[i], IORD(m_mctp_memory, 0));
    }

    // Challenge

}
