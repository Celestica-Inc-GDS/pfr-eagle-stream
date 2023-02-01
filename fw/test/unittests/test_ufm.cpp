#include <iostream>

// Include the GTest headers
#include "gtest_headers.h"

// Include the SYSTEM MOCK and PFR headers
#include "ut_nios_wrapper.h"

class UFMTest : public testing::Test
{
public:
    // Imagining the content structure of a provisioned UFM
    /*alt_u8 m_ufm_example_data[UFM_PFR_DATA_SIZE + 4 + 4] = {
            // UFM Status bits
            0xf1, 0xfd, 0xff, 0xff,
            // Root Key hash
            // For testing purpose, this hash can be obtained by:
            //   1. Extract the root entry public key X (in little endian byte order) and Y (in little endian byte order).
            //   2. Write it to a file (with system_mock write_x86_mem_to_file function).
            //   3. Get the 32-byte SHA 256 hash by running sha256sum linux command on that file.
            //   4. Reformat that hash by: echo "<hash>" | xxd -r -p | xxd -i
            0x6b, 0xa4, 0xa6, 0x98, 0x53, 0x63, 0xf0, 0xe7,
            0xe9, 0x98, 0x76, 0x62, 0x7d, 0xe7, 0x12, 0x41, 0xda, 0xab, 0x4b, 0x96,
            0xbd, 0x67, 0x99, 0x82, 0x81, 0x40, 0x27, 0x87, 0xa5, 0x10, 0x6e, 0x73,
            // The root key hash for sha-384.
            0x4d, 0x5b, 0xa2, 0xda, 0x8d, 0xc7, 0x50, 0xf2, 0x45, 0x45, 0xda, 0x3d,
            0x09, 0x13, 0x4a, 0xec, 0x97, 0x95, 0x5b, 0x6e, 0xf8, 0x32, 0xd1, 0xa2,
            0x75, 0x03, 0x60, 0x5f, 0xb0, 0xf6, 0x77, 0xb6, 0xd4, 0x44, 0x64, 0x75,
            0x05, 0xe9, 0x4e, 0x86, 0xcf, 0x3c, 0xe8, 0x86, 0x21, 0xe2, 0x82, 0x9d,
            // start address of PCH SPI Active Region PFM
            0x00, 0x00, 0xff, 0x02,
            // start address of PCH SPI Recovery Region
            0x00, 0x00, 0xbf, 0x01,
            // start address of PCH SPI Staging Region
            0x00, 0x00, 0x7f, 0x00,
            // start address of BMC SPI Active Region PFM
            0x00, 0x00, 0x08, 0x00,
            // start address of BMC SPI Recovery Region
            0x00, 0x00, 0xa0, 0x02,
            // start address of BMC SPI Staging Region
            0x00, 0x00, 0xa0, 0x04,
            // PIT password
            0xff, 0xff, 0xff, 0xff,
            0xff, 0xff, 0xff, 0xff,
            // PIT PCH FW HASH
            0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
            0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
            0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
            0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
            // PIT BMC FW HASH
            0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
            0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
            0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
            0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
            // PFR SVN enforcement policy
            //   Policy for CPLD binaries
            0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
            //   Policy for PCH binaries
            0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
            //   Policy for BMC binaries
            0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
            // CSK Cancellation policy (e.g. cancel key ID 2, assuming one-hot encoding)
            //   Policy for signing PCH PFM
            0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
            0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
            //   Policy for signing PCH Update Capsule
            0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
            0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
            //   Policy for signing BMC PFM
            0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
            0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
            //   Policy for signing BMC Update Capsule
            0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
            0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
            //   Policy for signing CPLD Update capsule
            0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
            0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
            //   SVN policy for AFM
            0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    };*/

    /*alt_u8 m_ufm_example_data[128] = {
    		  0x0a, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    		  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    		  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    		  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    		  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    		  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    		  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    		  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    		  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    		  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    		  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
    };*/

    UFM_PFR_DATA* m_ufm_data;

    virtual void SetUp()
    {
        SYSTEM_MOCK::get()->reset();
        m_ufm_data = (UFM_PFR_DATA*) get_ufm_pfr_data();

        // If we need to re-generate the hex file, un-comment this line.
        //SYSTEM_MOCK::get()->write_x86_mem_to_file("testdata/ufm_rk_hash_256_with_afm_svn.hex", m_ufm_example_data, 316 + 4 + 4);
    }
    virtual void TearDown() {}
};

TEST_F(UFMTest, test_sanity)
{
    EXPECT_EQ(sizeof(UFM_PFR_DATA), (alt_u32) UFM_PFR_DATA_SIZE);

    // There's assumption in the Nios FW that all UFM data fit in the first page.
    EXPECT_TRUE(UFM_PFR_DATA_SIZE < UFM_FLASH_PAGE_SIZE);
}

TEST_F(UFMTest, test_sanity_system_ufm)
{
    EXPECT_EQ(((alt_u32*) m_ufm_data)[0], (alt_u32) 0xFFFFFFFF);
    EXPECT_EQ(((alt_u32*) m_ufm_data)[8], (alt_u32) 0xFFFFFFFF);
    EXPECT_EQ(((alt_u32*) m_ufm_data)[12], (alt_u32) 0xFFFFFFFF);
    EXPECT_EQ(((alt_u32*) m_ufm_data)[20], (alt_u32) 0xFFFFFFFF);
}

TEST_F(UFMTest, test_unprovisioned)
{
    EXPECT_FALSE(is_ufm_locked());
    EXPECT_FALSE(is_ufm_provisioned());
}

TEST_F(UFMTest, test_provisioned)
{
    SYSTEM_MOCK::get()->provision_ufm_data(UFM_PFR_DATA_EXAMPLE_KEY_FILE);
    EXPECT_FALSE(is_ufm_locked());
    EXPECT_TRUE(is_ufm_provisioned());
#ifdef GTEST_USE_CRYPTO_384
    EXPECT_EQ(m_ufm_data->root_key_hash_384[0], (alt_u32) 0xdaa25b4d);
    EXPECT_EQ(m_ufm_data->root_key_hash_256[0], (alt_u32) 0xffffffff);
#else
    EXPECT_EQ(m_ufm_data->root_key_hash_256[0], (alt_u32) 0x98a6a46b);
    EXPECT_EQ(m_ufm_data->root_key_hash_384[0], (alt_u32) 0xffffffff);
#endif
}
