#include <iostream>

// Include the GTest headers
#include "gtest_headers.h"

// Include the SYSTEM MOCK and PFR headers
#include "ut_nios_wrapper.h"

class PFMTest : public testing::Test
{
public:
    alt_u8 m_raw_pfm_x86[256] = {
            0x1d, 0xce, 0xb3, 0x02, 0x03, 0x01, 0x01, 0x01, 0xff, 0xff, 0xff, 0xff,
            0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
            0xff, 0xff, 0xff, 0xff, 0x00, 0x01, 0x00, 0x00, 0x01, 0x01, 0x01, 0x00,
            0xff, 0xff, 0xff, 0xff, 0x00, 0x40, 0x00, 0x00, 0x00, 0xc0, 0x05, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x03, 0x00, 0x00,
            0xff, 0xff, 0xff, 0xff, 0x00, 0xc0, 0x05, 0x00, 0x00, 0x80, 0x11, 0x00,
            0x02, 0xff, 0xff, 0xff, 0xff, 0x03, 0xab, 0xab, 0x2b, 0xe6, 0x0a, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x01, 0x01, 0x01, 0x00, 0xff, 0xff, 0xff, 0xff,
            0x00, 0xc0, 0x31, 0x00, 0x00, 0x00, 0x43, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x02, 0xff, 0xff, 0xff, 0xff, 0x03, 0xab, 0xab,
            0x2b, 0xe6, 0x0a, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0xff,
            0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
            0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
            0xff, 0xff, 0xff, 0xff
    };

    virtual void SetUp()
    {
        SYSTEM_MOCK* sys = SYSTEM_MOCK::get();
        sys->reset();
    }

    virtual void TearDown() {}
};

TEST_F(PFMTest, test_basic)
{
    alt_u32* m_raw_pfm_x86_u32_ptr = (alt_u32*) m_raw_pfm_x86;
    EXPECT_EQ(*m_raw_pfm_x86_u32_ptr, alt_u32(PFM_MAGIC));
}

TEST_F(PFMTest, test_pfm_struct)
{
    PFM* pfm = (PFM*) m_raw_pfm_x86;

    EXPECT_EQ(pfm->tag, alt_u32(PFM_MAGIC));
    EXPECT_EQ(pfm->svn, (alt_u8) 0x03);
    EXPECT_EQ(pfm->bkc_version, (alt_u8) 0x01);
    EXPECT_EQ(pfm->major_rev, (alt_u8) 0x01);
    EXPECT_EQ(pfm->minor_rev, (alt_u8) 0x01);
    EXPECT_EQ(pfm->length, (alt_u32) 256);
}

TEST_F(PFMTest, test_pfm_spi_region_definition)
{
    PFM* pfm = (PFM*) m_raw_pfm_x86;
    PFM_SPI_REGION_DEF* rule_def = (PFM_SPI_REGION_DEF*) (pfm->pfm_body);

    EXPECT_EQ(rule_def->def_type, alt_u32(SPI_REGION_DEF_TYPE));
    EXPECT_EQ(rule_def->hash_algorithm, alt_u32(PFM_HASH_ALGO_SHA256_MASK));
    EXPECT_EQ(rule_def->start_addr, alt_u32(0x4000));
    EXPECT_EQ(rule_def->end_addr, alt_u32(0x5C000));
}

#ifdef GTEST_ENABLE_SEAMLESS_FEATURES

TEST_F(PFMTest, test_fvm_struct)
{
    alt_u8 data[256] = {
        0xd4, 0xc2, 0xe7, 0xa8, 0x07, 0xff, 0x00, 0x00,
        0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00,
        0x01, 0x1d, 0x02, 0x00, 0xff, 0xff, 0xff, 0xff, 0x00, 0x30, 0x00, 0x00,
        0x00, 0x80, 0x02, 0x00, 0x3c, 0x7a, 0x49, 0xaa, 0x7b, 0x08, 0xfd, 0xb9,
        0x1c, 0x3a, 0xff, 0x6f, 0xd8, 0x83, 0xae, 0xf8, 0x25, 0x1b, 0xc4, 0x98,
        0x1d, 0x57, 0x2a, 0xda, 0x3f, 0x4e, 0xda, 0x17, 0x88, 0x2f, 0x47, 0x0a,
        0xae, 0x3c, 0x51, 0xf1, 0x20, 0x4a, 0xc0, 0x1d, 0xf4, 0x1b, 0x12, 0xb8,
        0x2f, 0xc2, 0x57, 0xa9, 0x01, 0x1f, 0x00, 0x00, 0xff, 0xff, 0xff, 0xff,
        0x00, 0x80, 0x02, 0x00, 0x00, 0x40, 0x09, 0x00, 0x01, 0x1d, 0x02, 0x00,
        0xff, 0xff, 0xff, 0xff, 0x00, 0x40, 0x09, 0x00, 0x00, 0x70, 0x7c, 0x00,
        0xec, 0xbe, 0xe5, 0xa5, 0x6e, 0x18, 0xd6, 0xe6, 0x62, 0x03, 0x93, 0xa5,
        0x15, 0x06, 0x33, 0x05, 0xb2, 0x92, 0x1b, 0xa8, 0x95, 0x6a, 0x65, 0x16,
        0x99, 0x90, 0xac, 0x10, 0x1c, 0xb3, 0xa5, 0xf6, 0x7b, 0x48, 0x83, 0xae,
        0x4c, 0x51, 0x58, 0x63, 0xcd, 0x4a, 0x53, 0x7a, 0x6f, 0x8e, 0x08, 0xfa,
        0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
        0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
        0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
        0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
        0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
        0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
        0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff
    };
    FVM* fvm = (FVM*) data;

    EXPECT_EQ(fvm->tag, alt_u32(FVM_MAGIC));
    EXPECT_EQ(fvm->svn, (alt_u8) 7);
    EXPECT_EQ(fvm->major_rev, (alt_u8) 0);
    EXPECT_EQ(fvm->minor_rev, (alt_u8) 0);
    EXPECT_EQ(fvm->length, (alt_u32) 256);
}

/**
 * @brief Nios expects the PFM and FVM structure to be matching in the most part.
 *
 * Nios sometimes map PFM to a FVM data for simplicity. If this test fails, the
 * assumption no longer holds and there could be functional issue with the way
 * Nios is reading the FVM.
 */
TEST_F(PFMTest, test_consistency_between_pfm_and_fvm)
{
    EXPECT_EQ(sizeof(PFM), sizeof(FVM));

    alt_u8 data[128] = {
            1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20,
            21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40,
            41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60,
            61, 62, 63, 64, 65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79, 80,
            81, 82, 83, 84, 85, 86, 87, 88, 89, 90, 91, 92, 93, 94, 95, 96, 97, 98, 99, 100,
            101, 102, 103, 104, 105, 106, 107, 108, 109, 110, 111, 112, 113, 114, 115, 116, 117, 118, 119,
            120, 121, 122, 123, 124, 125, 126, 127, 128
    };

    PFM* pfm = (PFM*) data;
    FVM* fvm = (FVM*) data;

    EXPECT_EQ(pfm->tag, fvm->tag);
    EXPECT_EQ(pfm->svn, fvm->svn);
    EXPECT_EQ(pfm->pfm_body[0], fvm->fvm_body[0]);
}

#endif

