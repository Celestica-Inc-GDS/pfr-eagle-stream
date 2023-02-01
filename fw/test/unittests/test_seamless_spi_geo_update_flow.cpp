#include <iostream>

// Include the GTest headers
#include "gtest_headers.h"

// Include the SYSTEM MOCK and PFR headers
#include "ut_nios_wrapper.h"
#include "testdata_info.h"


class SpiGeoSeamlessUpdateFlowTest : public testing::Test
{
public:
    const alt_u32 m_bios_fv_regions_start_addr[PCH_NUM_BIOS_FV_REGIONS] = PCH_SPI_BIOS_FV_REGIONS_START_ADDR;
    const alt_u32 m_bios_fv_regions_end_addr[PCH_NUM_BIOS_FV_REGIONS] = PCH_SPI_BIOS_FV_REGIONS_END_ADDR;

    virtual void SetUp()
    {
        SYSTEM_MOCK* sys = SYSTEM_MOCK::get();
        // Reset system mocks and SPI flash
        sys->reset();
        sys->reset_spi_flash_mock();

        // Perform provisioning
        SYSTEM_MOCK::get()->provision_ufm_data(UFM_PFR_DATA_SPI_GEO_EXAMPLE_KEY_FILE);

        // Reset Nios firmware
        ut_reset_nios_fw();

        // Load the entire image to flash
        SYSTEM_MOCK::get()->load_to_flash(SPI_FLASH_BMC, FULL_PFR_IMAGE_BMC_FILE, FULL_PFR_IMAGE_BMC_FILE_SIZE);
        SYSTEM_MOCK::get()->load_to_flash(SPI_FLASH_PCH, FULL_PFR_IMAGE_PCH_FILE, FULL_PFR_IMAGE_PCH_FILE_SIZE);
    }

    virtual void TearDown() {}
};

/**
 * @brief This test exercises the in-band happy path of spi-geo Seamless update flow, with BIOS FV capsule.
 */
TEST_F(SpiGeoSeamlessUpdateFlowTest, test_seamless_update_ib_happy_path)
{
    /*
     * Test Preparation
     */
    // Load the Seamless FV capsule
    SYSTEM_MOCK::get()->load_to_flash(
            SPI_FLASH_PCH,
            PCH_FW_UPDATE_SIGNED_CAPSULE_PCH_SEAMLESS_BIOS_FILE,
            PCH_FW_UPDATE_SIGNED_CAPSULE_PCH_SEAMLESS_BIOS_FILE_SIZE,
            get_ufm_pfr_data()->pch_staging_region
    );

    // Load the entire images locally for comparison purpose
    alt_u32 *full_orig_image = new alt_u32[FULL_PFR_IMAGE_PCH_FILE_SIZE/4];
    SYSTEM_MOCK::get()->init_x86_mem_from_file(FULL_PFR_IMAGE_PCH_FILE, full_orig_image);

    alt_u32 *full_new_image = new alt_u32[FULL_PFR_IMAGE_PCH_V03P12_FILE_SIZE/4];
    SYSTEM_MOCK::get()->init_x86_mem_from_file(FULL_PFR_IMAGE_PCH_V03P12_FILE, full_new_image);

    alt_u32 *new_signed_fvm = new alt_u32[PCH_FW_UPDATE_SIGNED_FVM_PCH_BIOS_FILE_SIZE/4];
    SYSTEM_MOCK::get()->init_x86_mem_from_file(PCH_FW_UPDATE_SIGNED_FVM_PCH_BIOS_FILE, new_signed_fvm);

    // UFM should not be provisioned
    EXPECT_FALSE(is_ufm_provisioned());
    // SPI GEO should be provisioned
    EXPECT_TRUE(is_ufm_spi_geo_provisioned());

    /*
     * Flow preparation
     */
    ut_prep_nios_gpi_signals();

    // Exit after 50 iterations in the T0 loop
    SYSTEM_MOCK::get()->insert_code_block(SYSTEM_MOCK::CODE_BLOCK_TYPES::T0_OPERATIONS_END_AFTER_50_ITERS);

    // Send the Seamless update intent
    ut_send_in_update_intent_once_upon_entry_to_t0(MB_PCH_UPDATE_INTENT_PART2, MB_UPDATE_INTENT_PCH_FV_SEAMLESS_MASK);
    /*
     * Execute the flow. Active update flow will be triggered.
     */
    // Run PFR Main. Always run with the timeout
    ASSERT_DURATION_LE(GTEST_TIMEOUT_SHORT, pfr_main());

    /*
     * Verify results
     */
    switch_spi_flash(SPI_FLASH_PCH);
    alt_u32* flash_x86_ptr = SYSTEM_MOCK::get()->get_x86_ptr_to_spi_flash(SPI_FLASH_PCH);

    // Seamless update should not require T-1 entry
    EXPECT_EQ(SYSTEM_MOCK::get()->get_t_minus_1_bmc_only_counter(), alt_u32(0));
    EXPECT_EQ(SYSTEM_MOCK::get()->get_t_minus_1_pch_only_counter(), alt_u32(0));
    EXPECT_EQ(SYSTEM_MOCK::get()->get_t_minus_1_counter(), alt_u32(1));

    EXPECT_EQ(read_from_mailbox(MB_PANIC_EVENT_COUNT), alt_u32(0));
    EXPECT_EQ(read_from_mailbox(MB_LAST_PANIC_REASON), alt_u32(0));
    EXPECT_EQ(read_from_mailbox(MB_RECOVERY_COUNT), alt_u32(0));
    EXPECT_EQ(read_from_mailbox(MB_LAST_RECOVERY_REASON), alt_u32(0));
    EXPECT_EQ(read_from_mailbox(MB_MAJOR_ERROR_CODE), alt_u32(0));
    EXPECT_EQ(read_from_mailbox(MB_MINOR_ERROR_CODE), alt_u32(0));

    // Check PCH FW version (should not change in seamless update flow)
    EXPECT_EQ(read_from_mailbox(MB_PCH_PFM_ACTIVE_MAJOR_VER), alt_u32(PCH_ACTIVE_PFM_MAJOR_VER));
    EXPECT_EQ(read_from_mailbox(MB_PCH_PFM_ACTIVE_MINOR_VER), alt_u32(PCH_ACTIVE_PFM_MINOR_VER));
    EXPECT_EQ(read_from_mailbox(MB_PCH_PFM_RECOVERY_MAJOR_VER), alt_u32(PCH_RECOVERY_PFM_MAJOR_VER));
    EXPECT_EQ(read_from_mailbox(MB_PCH_PFM_RECOVERY_MINOR_VER), alt_u32(PCH_RECOVERY_PFM_MINOR_VER));

    // Check BMC FW version
    EXPECT_EQ(read_from_mailbox(MB_BMC_PFM_ACTIVE_MAJOR_VER), alt_u32(BMC_ACTIVE_PFM_MAJOR_VER));
    EXPECT_EQ(read_from_mailbox(MB_BMC_PFM_ACTIVE_MINOR_VER), alt_u32(BMC_ACTIVE_PFM_MINOR_VER));
    EXPECT_EQ(read_from_mailbox(MB_BMC_PFM_RECOVERY_MAJOR_VER), alt_u32(BMC_RECOVERY_PFM_MAJOR_VER));
    EXPECT_EQ(read_from_mailbox(MB_BMC_PFM_RECOVERY_MINOR_VER), alt_u32(BMC_RECOVERY_PFM_MINOR_VER));

    // Make sure only data in BIOS FV regions are changed
    for (alt_u32 region_i = 0; region_i < PCH_NUM_STATIC_SPI_REGIONS; region_i++)
    {
        // If this region is a BIOS FV region, compare against full_new_image
        alt_u32* gold_image = full_orig_image;
        for (alt_u32 addr_i = 0; addr_i < PCH_NUM_BIOS_FV_REGIONS; addr_i++)
        {
            if (testdata_pch_static_regions_start_addr[region_i] == m_bios_fv_regions_start_addr[addr_i])
            {
                gold_image = full_new_image;
            }
        }

        // Iterate through all words in this SPI region and make sure they match the expected data
        for (alt_u32 word_i = testdata_pch_static_regions_start_addr[region_i] >> 2;
                word_i < testdata_pch_static_regions_end_addr[region_i] >> 2; word_i++)
        {
            ASSERT_EQ(gold_image[word_i], flash_x86_ptr[word_i]);
        }
    }
    for (alt_u32 region_i = 0; region_i < PCH_NUM_DYNAMIC_SPI_REGIONS; region_i++)
    {
        // By default, the dynamic SPI regions are not updated in Seamless update.
        alt_u32* gold_image = full_orig_image;

        // Iterate through all words in this SPI region and make sure they match the expected data
        for (alt_u32 word_i = testdata_pch_dynamic_regions_start_addr[region_i] >> 2;
                word_i < testdata_pch_dynamic_regions_end_addr[region_i] >> 2; word_i++)
        {
            ASSERT_EQ(gold_image[word_i], flash_x86_ptr[word_i]);
        }
    }

    // Verify the active PFR region
    // PFM should not change. Only one FVM will be updated.
    alt_u32 pch_active_pfm_start = get_ufm_pfr_data()->pch_active_pfm;
    alt_u32 pch_active_pfm_end = pch_active_pfm_start + SIGNED_PFM_MAX_SIZE;

    // Info on updated FVM
    alt_u16 target_fv_type = ((FVM*) incr_alt_u32_ptr(new_signed_fvm, SIGNATURE_SIZE))->fv_type;
    alt_u32 target_fvm_start_addr = get_active_fvm_addr(target_fv_type);
    alt_u32 target_fvm_end_addr = target_fvm_start_addr + (4096 - PCH_FW_UPDATE_SIGNED_FVM_PCH_BIOS_FILE_SIZE % 4096);

    for (alt_u32 word_i = pch_active_pfm_start >> 2; word_i < pch_active_pfm_end >> 2; word_i++)
    {
        if (((target_fvm_start_addr >> 2) <= word_i) && (word_i < (target_fvm_end_addr >> 2)))
        {
            // Check against the expected new FVM, if the address falls within this FVM in active PFR region
            alt_u32 new_signed_fvm_word_i = word_i - (target_fvm_start_addr >> 2);
            if (new_signed_fvm_word_i < (PCH_FW_UPDATE_SIGNED_FVM_PCH_BIOS_FILE_SIZE >> 2))
            {
                ASSERT_EQ(new_signed_fvm[new_signed_fvm_word_i], flash_x86_ptr[word_i]);
            }
            else
            {
                // Data would be blank (0xFFFFFFFF) for the remaining of the page that this FVM occupies
                ASSERT_EQ(alt_u32(0xFFFFFFFF), flash_x86_ptr[word_i]);
            }
        }
        else
        {
            // The rest of Active region should be unchanged
            ASSERT_EQ(full_orig_image[word_i], flash_x86_ptr[word_i]);
        }
    }

    /*
     * Clean ups
     */
    delete[] full_orig_image;
    delete[] full_new_image;
    delete[] new_signed_fvm;
}

/**
 * @brief This test exercises the out-of-band happy path of spi-geo Seamless update flow, with BIOS FV capsule.
 */
TEST_F(SpiGeoSeamlessUpdateFlowTest, test_seamless_update_oob_happy_path)
{
    /*
     * Test Preparation
     */
    // Load the Seamless FV capsule
    SYSTEM_MOCK::get()->load_to_flash(
            SPI_FLASH_BMC,
            PCH_FW_UPDATE_SIGNED_CAPSULE_PCH_SEAMLESS_BIOS_FILE,
            PCH_FW_UPDATE_SIGNED_CAPSULE_PCH_SEAMLESS_BIOS_FILE_SIZE,
            get_ufm_pfr_data()->bmc_staging_region + BMC_STAGING_REGION_PCH_UPDATE_CAPSULE_OFFSET
    );

    // Load the entire images locally for comparison purpose
    alt_u32 *full_orig_image = new alt_u32[FULL_PFR_IMAGE_PCH_FILE_SIZE/4];
    SYSTEM_MOCK::get()->init_x86_mem_from_file(FULL_PFR_IMAGE_PCH_FILE, full_orig_image);

    alt_u32 *full_new_image = new alt_u32[FULL_PFR_IMAGE_PCH_V03P12_FILE_SIZE/4];
    SYSTEM_MOCK::get()->init_x86_mem_from_file(FULL_PFR_IMAGE_PCH_V03P12_FILE, full_new_image);

    alt_u32 *new_signed_fvm = new alt_u32[PCH_FW_UPDATE_SIGNED_FVM_PCH_BIOS_FILE_SIZE/4];
    SYSTEM_MOCK::get()->init_x86_mem_from_file(PCH_FW_UPDATE_SIGNED_FVM_PCH_BIOS_FILE, new_signed_fvm);

    // UFM should not be provisioned
    EXPECT_FALSE(is_ufm_provisioned());
    // SPI GEO should be provisioned
    EXPECT_TRUE(is_ufm_spi_geo_provisioned());
    /*
     * Flow preparation
     */
    ut_prep_nios_gpi_signals();

    // Exit after 50 iterations in the T0 loop
    SYSTEM_MOCK::get()->insert_code_block(SYSTEM_MOCK::CODE_BLOCK_TYPES::T0_OPERATIONS_END_AFTER_50_ITERS);

    // Send the Seamless update intent
    ut_send_in_update_intent_once_upon_entry_to_t0(MB_BMC_UPDATE_INTENT_PART2, MB_UPDATE_INTENT_PCH_FV_SEAMLESS_MASK);

    /*
     * Execute the flow. Active update flow will be triggered.
     */
    // Run PFR Main. Always run with the timeout
    ASSERT_DURATION_LE(GTEST_TIMEOUT_SHORT, pfr_main());

    /*
     * Verify results
     */
    switch_spi_flash(SPI_FLASH_PCH);
    alt_u32* flash_x86_ptr = SYSTEM_MOCK::get()->get_x86_ptr_to_spi_flash(SPI_FLASH_PCH);

    // Seamless update should not require T-1 entry
    EXPECT_EQ(SYSTEM_MOCK::get()->get_t_minus_1_bmc_only_counter(), alt_u32(0));
    EXPECT_EQ(SYSTEM_MOCK::get()->get_t_minus_1_pch_only_counter(), alt_u32(0));
    EXPECT_EQ(SYSTEM_MOCK::get()->get_t_minus_1_counter(), alt_u32(1));

    EXPECT_EQ(read_from_mailbox(MB_PANIC_EVENT_COUNT), alt_u32(0));
    EXPECT_EQ(read_from_mailbox(MB_LAST_PANIC_REASON), alt_u32(0));
    EXPECT_EQ(read_from_mailbox(MB_RECOVERY_COUNT), alt_u32(0));
    EXPECT_EQ(read_from_mailbox(MB_LAST_RECOVERY_REASON), alt_u32(0));
    EXPECT_EQ(read_from_mailbox(MB_MAJOR_ERROR_CODE), alt_u32(0));
    EXPECT_EQ(read_from_mailbox(MB_MINOR_ERROR_CODE), alt_u32(0));

    // Check PCH FW version (should not change in seamless update flow)
    EXPECT_EQ(read_from_mailbox(MB_PCH_PFM_ACTIVE_MAJOR_VER), alt_u32(PCH_ACTIVE_PFM_MAJOR_VER));
    EXPECT_EQ(read_from_mailbox(MB_PCH_PFM_ACTIVE_MINOR_VER), alt_u32(PCH_ACTIVE_PFM_MINOR_VER));
    EXPECT_EQ(read_from_mailbox(MB_PCH_PFM_RECOVERY_MAJOR_VER), alt_u32(PCH_RECOVERY_PFM_MAJOR_VER));
    EXPECT_EQ(read_from_mailbox(MB_PCH_PFM_RECOVERY_MINOR_VER), alt_u32(PCH_RECOVERY_PFM_MINOR_VER));

    // Check BMC FW version
    EXPECT_EQ(read_from_mailbox(MB_BMC_PFM_ACTIVE_MAJOR_VER), alt_u32(BMC_ACTIVE_PFM_MAJOR_VER));
    EXPECT_EQ(read_from_mailbox(MB_BMC_PFM_ACTIVE_MINOR_VER), alt_u32(BMC_ACTIVE_PFM_MINOR_VER));
    EXPECT_EQ(read_from_mailbox(MB_BMC_PFM_RECOVERY_MAJOR_VER), alt_u32(BMC_RECOVERY_PFM_MAJOR_VER));
    EXPECT_EQ(read_from_mailbox(MB_BMC_PFM_RECOVERY_MINOR_VER), alt_u32(BMC_RECOVERY_PFM_MINOR_VER));

    // Make sure only data in BIOS FV regions are changed
    for (alt_u32 region_i = 0; region_i < PCH_NUM_STATIC_SPI_REGIONS; region_i++)
    {
        // If this region is a BIOS FV region, compare against full_new_image
        alt_u32* gold_image = full_orig_image;
        for (alt_u32 addr_i = 0; addr_i < PCH_NUM_BIOS_FV_REGIONS; addr_i++)
        {
            if (testdata_pch_static_regions_start_addr[region_i] == m_bios_fv_regions_start_addr[addr_i])
            {
                gold_image = full_new_image;
            }
        }

        // Iterate through all words in this SPI region and make sure they match the expected data
        for (alt_u32 word_i = testdata_pch_static_regions_start_addr[region_i] >> 2;
                word_i < testdata_pch_static_regions_end_addr[region_i] >> 2; word_i++)
        {
            ASSERT_EQ(gold_image[word_i], flash_x86_ptr[word_i]);
        }
    }
    for (alt_u32 region_i = 0; region_i < PCH_NUM_DYNAMIC_SPI_REGIONS; region_i++)
    {
        // By default, the dynamic SPI regions are not updated in Seamless update.
        alt_u32* gold_image = full_orig_image;

        // Iterate through all words in this SPI region and make sure they match the expected data
        for (alt_u32 word_i = testdata_pch_dynamic_regions_start_addr[region_i] >> 2;
                word_i < testdata_pch_dynamic_regions_end_addr[region_i] >> 2; word_i++)
        {
            ASSERT_EQ(gold_image[word_i], flash_x86_ptr[word_i]);
        }
    }

    // Verify the active PFR region
    // PFM should not change. Only one FVM will be updated.
    alt_u32 pch_active_pfm_start = get_ufm_pfr_data()->pch_active_pfm;
    alt_u32 pch_active_pfm_end = pch_active_pfm_start + SIGNED_PFM_MAX_SIZE;

    // Info on updated FVM
    alt_u16 target_fv_type = ((FVM*) incr_alt_u32_ptr(new_signed_fvm, SIGNATURE_SIZE))->fv_type;
    alt_u32 target_fvm_start_addr = get_active_fvm_addr(target_fv_type);
    alt_u32 target_fvm_end_addr = target_fvm_start_addr + (4096 - PCH_FW_UPDATE_SIGNED_FVM_PCH_BIOS_FILE_SIZE % 4096);

    for (alt_u32 word_i = pch_active_pfm_start >> 2; word_i < pch_active_pfm_end >> 2; word_i++)
    {
        if (((target_fvm_start_addr >> 2) <= word_i) && (word_i < (target_fvm_end_addr >> 2)))
        {
            // Check against the expected new FVM, if the address falls within this FVM in active PFR region
            alt_u32 new_signed_fvm_word_i = word_i - (target_fvm_start_addr >> 2);
            if (new_signed_fvm_word_i < (PCH_FW_UPDATE_SIGNED_FVM_PCH_BIOS_FILE_SIZE >> 2))
            {
                ASSERT_EQ(new_signed_fvm[new_signed_fvm_word_i], flash_x86_ptr[word_i]);
            }
            else
            {
                // Data would be blank (0xFFFFFFFF) for the remaining of the page that this FVM occupies
                ASSERT_EQ(alt_u32(0xFFFFFFFF), flash_x86_ptr[word_i]);
            }
        }
        else
        {
            // The rest of Active region should be unchanged
            ASSERT_EQ(full_orig_image[word_i], flash_x86_ptr[word_i]);
        }
    }

    /*
     * Clean ups
     */
    delete[] full_orig_image;
    delete[] full_new_image;
    delete[] new_signed_fvm;
}

/**
 * @brief This test exercises the in-band happy path of spi-geo Seamless update flow, with BIOS FV capsule
 * and UPDATE_DYNAMIC bit set in update intent.
 */
TEST_F(SpiGeoSeamlessUpdateFlowTest, test_ib_seamless_update_static_plus_dynamic)
{
    /*
     * Test Preparation
     */
    // Load the Seamless FV capsule
    SYSTEM_MOCK::get()->load_to_flash(SPI_FLASH_PCH, PCH_FW_UPDATE_SIGNED_CAPSULE_PCH_SEAMLESS_BIOS_FILE,
            PCH_FW_UPDATE_SIGNED_CAPSULE_PCH_SEAMLESS_BIOS_FILE_SIZE, get_ufm_pfr_data()->pch_staging_region);

    // Load the entire images locally for comparison purpose
    alt_u32 *full_orig_image = new alt_u32[FULL_PFR_IMAGE_PCH_FILE_SIZE/4];
    SYSTEM_MOCK::get()->init_x86_mem_from_file(FULL_PFR_IMAGE_PCH_FILE, full_orig_image);

    alt_u32 *full_new_image = new alt_u32[FULL_PFR_IMAGE_PCH_V03P12_FILE_SIZE/4];
    SYSTEM_MOCK::get()->init_x86_mem_from_file(FULL_PFR_IMAGE_PCH_V03P12_FILE, full_new_image);

    alt_u32 *new_signed_fvm = new alt_u32[PCH_FW_UPDATE_SIGNED_FVM_PCH_BIOS_FILE_SIZE/4];
    SYSTEM_MOCK::get()->init_x86_mem_from_file(PCH_FW_UPDATE_SIGNED_FVM_PCH_BIOS_FILE, new_signed_fvm);

    // UFM should not be provisioned
    EXPECT_FALSE(is_ufm_provisioned());
    // SPI GEO should be provisioned
    EXPECT_TRUE(is_ufm_spi_geo_provisioned());
    /*
     * Flow preparation
     */
    ut_prep_nios_gpi_signals();

    // Exit after 50 iterations in the T0 loop
    SYSTEM_MOCK::get()->insert_code_block(SYSTEM_MOCK::CODE_BLOCK_TYPES::T0_OPERATIONS_END_AFTER_50_ITERS);

    // Send the Seamless update intent
    write_to_mailbox(MB_PCH_UPDATE_INTENT_PART2, MB_UPDATE_INTENT_PCH_FV_SEAMLESS_MASK);
    write_to_mailbox(MB_PCH_UPDATE_INTENT_PART1, MB_UPDATE_INTENT_UPDATE_DYNAMIC_MASK);

    /*
     * Execute the flow. Active update flow will be triggered.
     */
    // Run PFR Main. Always run with the timeout
    ASSERT_DURATION_LE(GTEST_TIMEOUT_SHORT, pfr_main());

    /*
     * Verify results
     */
    switch_spi_flash(SPI_FLASH_PCH);
    alt_u32* flash_x86_ptr = SYSTEM_MOCK::get()->get_x86_ptr_to_spi_flash(SPI_FLASH_PCH);

    // Seamless update should not require T-1 entry
    EXPECT_EQ(SYSTEM_MOCK::get()->get_t_minus_1_bmc_only_counter(), alt_u32(0));
    EXPECT_EQ(SYSTEM_MOCK::get()->get_t_minus_1_pch_only_counter(), alt_u32(0));
    EXPECT_EQ(SYSTEM_MOCK::get()->get_t_minus_1_counter(), alt_u32(1));

    EXPECT_EQ(read_from_mailbox(MB_PANIC_EVENT_COUNT), alt_u32(0));
    EXPECT_EQ(read_from_mailbox(MB_LAST_PANIC_REASON), alt_u32(0));
    EXPECT_EQ(read_from_mailbox(MB_RECOVERY_COUNT), alt_u32(0));
    EXPECT_EQ(read_from_mailbox(MB_LAST_RECOVERY_REASON), alt_u32(0));
    EXPECT_EQ(read_from_mailbox(MB_MAJOR_ERROR_CODE), alt_u32(0));
    EXPECT_EQ(read_from_mailbox(MB_MINOR_ERROR_CODE), alt_u32(0));

    // Check PCH FW version (should not change in seamless update flow)
    EXPECT_EQ(read_from_mailbox(MB_PCH_PFM_ACTIVE_MAJOR_VER), alt_u32(PCH_ACTIVE_PFM_MAJOR_VER));
    EXPECT_EQ(read_from_mailbox(MB_PCH_PFM_ACTIVE_MINOR_VER), alt_u32(PCH_ACTIVE_PFM_MINOR_VER));
    EXPECT_EQ(read_from_mailbox(MB_PCH_PFM_RECOVERY_MAJOR_VER), alt_u32(PCH_RECOVERY_PFM_MAJOR_VER));
    EXPECT_EQ(read_from_mailbox(MB_PCH_PFM_RECOVERY_MINOR_VER), alt_u32(PCH_RECOVERY_PFM_MINOR_VER));

    // Check BMC FW version
    EXPECT_EQ(read_from_mailbox(MB_BMC_PFM_ACTIVE_MAJOR_VER), alt_u32(BMC_ACTIVE_PFM_MAJOR_VER));
    EXPECT_EQ(read_from_mailbox(MB_BMC_PFM_ACTIVE_MINOR_VER), alt_u32(BMC_ACTIVE_PFM_MINOR_VER));
    EXPECT_EQ(read_from_mailbox(MB_BMC_PFM_RECOVERY_MAJOR_VER), alt_u32(BMC_RECOVERY_PFM_MAJOR_VER));
    EXPECT_EQ(read_from_mailbox(MB_BMC_PFM_RECOVERY_MINOR_VER), alt_u32(BMC_RECOVERY_PFM_MINOR_VER));

    // Make sure only data in BIOS FV regions are changed
    for (alt_u32 region_i = 0; region_i < PCH_NUM_STATIC_SPI_REGIONS; region_i++)
    {
        // If this region is a BIOS FV region, compare against full_new_image
        alt_u32* gold_image = full_orig_image;
        for (alt_u32 addr_i = 0; addr_i < PCH_NUM_BIOS_FV_REGIONS; addr_i++)
        {
            if (testdata_pch_static_regions_start_addr[region_i] == m_bios_fv_regions_start_addr[addr_i])
            {
                gold_image = full_new_image;
            }
        }

        // Iterate through all words in this SPI region and make sure they match the expected data
        for (alt_u32 word_i = testdata_pch_static_regions_start_addr[region_i] >> 2;
                word_i < testdata_pch_static_regions_end_addr[region_i] >> 2; word_i++)
        {
            ASSERT_EQ(gold_image[word_i], flash_x86_ptr[word_i]);
        }
    }
    for (alt_u32 region_i = 0; region_i < PCH_NUM_DYNAMIC_SPI_REGIONS; region_i++)
    {
        // If this region is a BIOS FV region, compare against full_new_image
        alt_u32* gold_image = full_orig_image;
        for (alt_u32 addr_i = 0; addr_i < PCH_NUM_BIOS_FV_REGIONS; addr_i++)
        {
            if (testdata_pch_dynamic_regions_start_addr[region_i] == m_bios_fv_regions_start_addr[addr_i])
            {
                gold_image = full_new_image;
            }
        }

        // Iterate through all words in this SPI region and make sure they match the expected data
        for (alt_u32 word_i = testdata_pch_dynamic_regions_start_addr[region_i] >> 2;
                word_i < testdata_pch_dynamic_regions_end_addr[region_i] >> 2; word_i++)
        {
            ASSERT_EQ(gold_image[word_i], flash_x86_ptr[word_i]);
        }
    }

    // Verify the active PFR region
    // PFM should not change. Only one FVM will be updated.
    alt_u32 pch_active_pfm_start = get_ufm_pfr_data()->pch_active_pfm;
    alt_u32 pch_active_pfm_end = pch_active_pfm_start + SIGNED_PFM_MAX_SIZE;

    // Info on updated FVM
    alt_u16 target_fv_type = ((FVM*) incr_alt_u32_ptr(new_signed_fvm, SIGNATURE_SIZE))->fv_type;
    alt_u32 target_fvm_start_addr = get_active_fvm_addr(target_fv_type);
    alt_u32 target_fvm_end_addr = target_fvm_start_addr + (4096 - PCH_FW_UPDATE_SIGNED_FVM_PCH_BIOS_FILE_SIZE % 4096);

    for (alt_u32 word_i = pch_active_pfm_start >> 2; word_i < pch_active_pfm_end >> 2; word_i++)
    {
        if (((target_fvm_start_addr >> 2) <= word_i) && (word_i < (target_fvm_end_addr >> 2)))
        {
            // Check against the expected new FVM, if the address falls within this FVM in active PFR region
            alt_u32 new_signed_fvm_word_i = word_i - (target_fvm_start_addr >> 2);
            if (new_signed_fvm_word_i < (PCH_FW_UPDATE_SIGNED_FVM_PCH_BIOS_FILE_SIZE >> 2))
            {
                ASSERT_EQ(new_signed_fvm[new_signed_fvm_word_i], flash_x86_ptr[word_i]);
            }
            else
            {
                // Data would be blank (0xFFFFFFFF) for the remaining of the page that this FVM occupies
                ASSERT_EQ(alt_u32(0xFFFFFFFF), flash_x86_ptr[word_i]);
            }
        }
        else
        {
            // The rest of Active region should be unchanged
            ASSERT_EQ(full_orig_image[word_i], flash_x86_ptr[word_i]);
        }
    }

    /*
     * Clean ups
     */
    delete[] full_orig_image;
    delete[] full_new_image;
    delete[] new_signed_fvm;
}

/**
 * @brief This test exercises the out-of-band happy path of spi-geo Seamless update flow, with BIOS FV capsule
 * and UPDATE_DYNAMIC bit set in update intent.
 */
TEST_F(SpiGeoSeamlessUpdateFlowTest, test_oob_seamless_update_static_plus_dynamic)
{
    /*
     * Test Preparation
     */
    // Load the Seamless FV capsule
    SYSTEM_MOCK::get()->load_to_flash(
            SPI_FLASH_BMC,
            PCH_FW_UPDATE_SIGNED_CAPSULE_PCH_SEAMLESS_BIOS_FILE,
            PCH_FW_UPDATE_SIGNED_CAPSULE_PCH_SEAMLESS_BIOS_FILE_SIZE,
            get_ufm_pfr_data()->bmc_staging_region + BMC_STAGING_REGION_PCH_UPDATE_CAPSULE_OFFSET
    );

    // Load the entire images locally for comparison purpose
    alt_u32 *full_orig_image = new alt_u32[FULL_PFR_IMAGE_PCH_FILE_SIZE/4];
    SYSTEM_MOCK::get()->init_x86_mem_from_file(FULL_PFR_IMAGE_PCH_FILE, full_orig_image);

    alt_u32 *full_new_image = new alt_u32[FULL_PFR_IMAGE_PCH_V03P12_FILE_SIZE/4];
    SYSTEM_MOCK::get()->init_x86_mem_from_file(FULL_PFR_IMAGE_PCH_V03P12_FILE, full_new_image);

    alt_u32 *new_signed_fvm = new alt_u32[PCH_FW_UPDATE_SIGNED_FVM_PCH_BIOS_FILE_SIZE/4];
    SYSTEM_MOCK::get()->init_x86_mem_from_file(PCH_FW_UPDATE_SIGNED_FVM_PCH_BIOS_FILE, new_signed_fvm);

    // UFM should not be provisioned
    EXPECT_FALSE(is_ufm_provisioned());
    // SPI GEO should be provisioned
    EXPECT_TRUE(is_ufm_spi_geo_provisioned());
    /*
     * Flow preparation
     */
    ut_prep_nios_gpi_signals();


    // Exit after 50 iterations in the T0 loop
    SYSTEM_MOCK::get()->insert_code_block(SYSTEM_MOCK::CODE_BLOCK_TYPES::T0_OPERATIONS_END_AFTER_50_ITERS);

    // Send the Seamless update intent
    write_to_mailbox(MB_BMC_UPDATE_INTENT_PART2, MB_UPDATE_INTENT_PCH_FV_SEAMLESS_MASK);
    write_to_mailbox(MB_BMC_UPDATE_INTENT_PART1, MB_UPDATE_INTENT_UPDATE_DYNAMIC_MASK);

    /*
     * Execute the flow. Active update flow will be triggered.
     */
    // Run PFR Main. Always run with the timeout
    ASSERT_DURATION_LE(GTEST_TIMEOUT_SHORT, pfr_main());

    /*
     * Verify results
     */
    switch_spi_flash(SPI_FLASH_PCH);
    alt_u32* flash_x86_ptr = SYSTEM_MOCK::get()->get_x86_ptr_to_spi_flash(SPI_FLASH_PCH);

    // Seamless update should not require T-1 entry
    EXPECT_EQ(SYSTEM_MOCK::get()->get_t_minus_1_bmc_only_counter(), alt_u32(0));
    EXPECT_EQ(SYSTEM_MOCK::get()->get_t_minus_1_pch_only_counter(), alt_u32(0));
    EXPECT_EQ(SYSTEM_MOCK::get()->get_t_minus_1_counter(), alt_u32(1));

    EXPECT_EQ(read_from_mailbox(MB_PANIC_EVENT_COUNT), alt_u32(0));
    EXPECT_EQ(read_from_mailbox(MB_LAST_PANIC_REASON), alt_u32(0));
    EXPECT_EQ(read_from_mailbox(MB_RECOVERY_COUNT), alt_u32(0));
    EXPECT_EQ(read_from_mailbox(MB_LAST_RECOVERY_REASON), alt_u32(0));
    EXPECT_EQ(read_from_mailbox(MB_MAJOR_ERROR_CODE), alt_u32(0));
    EXPECT_EQ(read_from_mailbox(MB_MINOR_ERROR_CODE), alt_u32(0));

    // Check PCH FW version (should not change in seamless update flow)
    EXPECT_EQ(read_from_mailbox(MB_PCH_PFM_ACTIVE_MAJOR_VER), alt_u32(PCH_ACTIVE_PFM_MAJOR_VER));
    EXPECT_EQ(read_from_mailbox(MB_PCH_PFM_ACTIVE_MINOR_VER), alt_u32(PCH_ACTIVE_PFM_MINOR_VER));
    EXPECT_EQ(read_from_mailbox(MB_PCH_PFM_RECOVERY_MAJOR_VER), alt_u32(PCH_RECOVERY_PFM_MAJOR_VER));
    EXPECT_EQ(read_from_mailbox(MB_PCH_PFM_RECOVERY_MINOR_VER), alt_u32(PCH_RECOVERY_PFM_MINOR_VER));

    // Check BMC FW version
    EXPECT_EQ(read_from_mailbox(MB_BMC_PFM_ACTIVE_MAJOR_VER), alt_u32(BMC_ACTIVE_PFM_MAJOR_VER));
    EXPECT_EQ(read_from_mailbox(MB_BMC_PFM_ACTIVE_MINOR_VER), alt_u32(BMC_ACTIVE_PFM_MINOR_VER));
    EXPECT_EQ(read_from_mailbox(MB_BMC_PFM_RECOVERY_MAJOR_VER), alt_u32(BMC_RECOVERY_PFM_MAJOR_VER));
    EXPECT_EQ(read_from_mailbox(MB_BMC_PFM_RECOVERY_MINOR_VER), alt_u32(BMC_RECOVERY_PFM_MINOR_VER));

    // Make sure only data in BIOS FV regions are changed
    for (alt_u32 region_i = 0; region_i < PCH_NUM_STATIC_SPI_REGIONS; region_i++)
    {
        // If this region is a BIOS FV region, compare against full_new_image
        alt_u32* gold_image = full_orig_image;
        for (alt_u32 addr_i = 0; addr_i < PCH_NUM_BIOS_FV_REGIONS; addr_i++)
        {
            if (testdata_pch_static_regions_start_addr[region_i] == m_bios_fv_regions_start_addr[addr_i])
            {
                gold_image = full_new_image;
            }
        }

        // Iterate through all words in this SPI region and make sure they match the expected data
        for (alt_u32 word_i = testdata_pch_static_regions_start_addr[region_i] >> 2;
                word_i < testdata_pch_static_regions_end_addr[region_i] >> 2; word_i++)
        {
            ASSERT_EQ(gold_image[word_i], flash_x86_ptr[word_i]);
        }
    }
    for (alt_u32 region_i = 0; region_i < PCH_NUM_DYNAMIC_SPI_REGIONS; region_i++)
    {
        // If this region is a BIOS FV region, compare against full_new_image
        alt_u32* gold_image = full_orig_image;
        for (alt_u32 addr_i = 0; addr_i < PCH_NUM_BIOS_FV_REGIONS; addr_i++)
        {
            if (testdata_pch_dynamic_regions_start_addr[region_i] == m_bios_fv_regions_start_addr[addr_i])
            {
                gold_image = full_new_image;
            }
        }

        // Iterate through all words in this SPI region and make sure they match the expected data
        for (alt_u32 word_i = testdata_pch_dynamic_regions_start_addr[region_i] >> 2;
                word_i < testdata_pch_dynamic_regions_end_addr[region_i] >> 2; word_i++)
        {
            ASSERT_EQ(gold_image[word_i], flash_x86_ptr[word_i]);
        }
    }

    // Verify the active PFR region
    // PFM should not change. Only one FVM will be updated.
    alt_u32 pch_active_pfm_start = get_ufm_pfr_data()->pch_active_pfm;
    alt_u32 pch_active_pfm_end = pch_active_pfm_start + SIGNED_PFM_MAX_SIZE;

    // Info on updated FVM
    alt_u16 target_fv_type = ((FVM*) incr_alt_u32_ptr(new_signed_fvm, SIGNATURE_SIZE))->fv_type;
    alt_u32 target_fvm_start_addr = get_active_fvm_addr(target_fv_type);
    alt_u32 target_fvm_end_addr = target_fvm_start_addr + (4096 - PCH_FW_UPDATE_SIGNED_FVM_PCH_BIOS_FILE_SIZE % 4096);

    for (alt_u32 word_i = pch_active_pfm_start >> 2; word_i < pch_active_pfm_end >> 2; word_i++)
    {
        if (((target_fvm_start_addr >> 2) <= word_i) && (word_i < (target_fvm_end_addr >> 2)))
        {
            // Check against the expected new FVM, if the address falls within this FVM in active PFR region
            alt_u32 new_signed_fvm_word_i = word_i - (target_fvm_start_addr >> 2);
            if (new_signed_fvm_word_i < (PCH_FW_UPDATE_SIGNED_FVM_PCH_BIOS_FILE_SIZE >> 2))
            {
                ASSERT_EQ(new_signed_fvm[new_signed_fvm_word_i], flash_x86_ptr[word_i]);
            }
            else
            {
                // Data would be blank (0xFFFFFFFF) for the remaining of the page that this FVM occupies
                ASSERT_EQ(alt_u32(0xFFFFFFFF), flash_x86_ptr[word_i]);
            }
        }
        else
        {
            // The rest of Active region should be unchanged
            ASSERT_EQ(full_orig_image[word_i], flash_x86_ptr[word_i]);
        }
    }

    /*
     * Clean ups
     */
    delete[] full_orig_image;
    delete[] full_new_image;
    delete[] new_signed_fvm;
}

/**
 * @brief This test exercises the in-band updateof spi-geo Seamless update flow, with BIOS FV capsule but
 * send to the wrong update register (non-seamless)
 */
TEST_F(SpiGeoSeamlessUpdateFlowTest, test_seamless_update_ib_incorrect_update_register)
{
    /*
     * Test Preparation
     */
    // Load the Seamless FV capsule
    SYSTEM_MOCK::get()->load_to_flash(
            SPI_FLASH_PCH,
            PCH_FW_UPDATE_SIGNED_CAPSULE_PCH_SEAMLESS_BIOS_FILE,
            PCH_FW_UPDATE_SIGNED_CAPSULE_PCH_SEAMLESS_BIOS_FILE_SIZE,
            get_ufm_pfr_data()->pch_staging_region
    );

    // Load the entire images locally for comparison purpose
    alt_u32 *full_orig_image = new alt_u32[FULL_PFR_IMAGE_PCH_FILE_SIZE/4];
    SYSTEM_MOCK::get()->init_x86_mem_from_file(FULL_PFR_IMAGE_PCH_FILE, full_orig_image);

    alt_u32 *full_new_image = new alt_u32[FULL_PFR_IMAGE_PCH_V03P12_FILE_SIZE/4];
    SYSTEM_MOCK::get()->init_x86_mem_from_file(FULL_PFR_IMAGE_PCH_V03P12_FILE, full_new_image);

    alt_u32 *new_signed_fvm = new alt_u32[PCH_FW_UPDATE_SIGNED_FVM_PCH_BIOS_FILE_SIZE/4];
    SYSTEM_MOCK::get()->init_x86_mem_from_file(PCH_FW_UPDATE_SIGNED_FVM_PCH_BIOS_FILE, new_signed_fvm);

    // UFM should not be provisioned
    EXPECT_FALSE(is_ufm_provisioned());
    // SPI GEO should be provisioned
    EXPECT_TRUE(is_ufm_spi_geo_provisioned());
    /*
     * Flow preparation
     */
    ut_prep_nios_gpi_signals();

    // Exit after 50 iterations in the T0 loop
    SYSTEM_MOCK::get()->insert_code_block(SYSTEM_MOCK::CODE_BLOCK_TYPES::T0_OPERATIONS_END_AFTER_50_ITERS);

    // Send the Seamless update intent
    ut_send_in_update_intent_once_upon_entry_to_t0(MB_PCH_UPDATE_INTENT_PART1, MB_UPDATE_INTENT_PCH_FV_SEAMLESS_MASK);
    /*
     * Execute the flow. Active update flow will be triggered.
     */
    // Run PFR Main. Always run with the timeout
    ASSERT_DURATION_LE(GTEST_TIMEOUT_SHORT, pfr_main());

    /*
     * Verify results
     */
    switch_spi_flash(SPI_FLASH_PCH);
    alt_u32* flash_x86_ptr = SYSTEM_MOCK::get()->get_x86_ptr_to_spi_flash(SPI_FLASH_PCH);

    // Seamless update should not require T-1 entry
    EXPECT_EQ(SYSTEM_MOCK::get()->get_t_minus_1_bmc_only_counter(), alt_u32(0));
    EXPECT_EQ(SYSTEM_MOCK::get()->get_t_minus_1_pch_only_counter(), alt_u32(0));
    EXPECT_EQ(SYSTEM_MOCK::get()->get_t_minus_1_counter(), alt_u32(1));

    EXPECT_EQ(read_from_mailbox(MB_PANIC_EVENT_COUNT), alt_u32(0));
    EXPECT_EQ(read_from_mailbox(MB_LAST_PANIC_REASON), alt_u32(0));
    EXPECT_EQ(read_from_mailbox(MB_RECOVERY_COUNT), alt_u32(0));
    EXPECT_EQ(read_from_mailbox(MB_LAST_RECOVERY_REASON), alt_u32(0));
    EXPECT_EQ(read_from_mailbox(MB_MAJOR_ERROR_CODE), alt_u32(0));
    EXPECT_EQ(read_from_mailbox(MB_MINOR_ERROR_CODE), alt_u32(0));

    // Check PCH FW version (should not change in seamless update flow)
    EXPECT_EQ(read_from_mailbox(MB_PCH_PFM_ACTIVE_MAJOR_VER), alt_u32(PCH_ACTIVE_PFM_MAJOR_VER));
    EXPECT_EQ(read_from_mailbox(MB_PCH_PFM_ACTIVE_MINOR_VER), alt_u32(PCH_ACTIVE_PFM_MINOR_VER));
    EXPECT_EQ(read_from_mailbox(MB_PCH_PFM_RECOVERY_MAJOR_VER), alt_u32(PCH_RECOVERY_PFM_MAJOR_VER));
    EXPECT_EQ(read_from_mailbox(MB_PCH_PFM_RECOVERY_MINOR_VER), alt_u32(PCH_RECOVERY_PFM_MINOR_VER));

    // Check BMC FW version
    EXPECT_EQ(read_from_mailbox(MB_BMC_PFM_ACTIVE_MAJOR_VER), alt_u32(BMC_ACTIVE_PFM_MAJOR_VER));
    EXPECT_EQ(read_from_mailbox(MB_BMC_PFM_ACTIVE_MINOR_VER), alt_u32(BMC_ACTIVE_PFM_MINOR_VER));
    EXPECT_EQ(read_from_mailbox(MB_BMC_PFM_RECOVERY_MAJOR_VER), alt_u32(BMC_RECOVERY_PFM_MAJOR_VER));
    EXPECT_EQ(read_from_mailbox(MB_BMC_PFM_RECOVERY_MINOR_VER), alt_u32(BMC_RECOVERY_PFM_MINOR_VER));

    // Make sure the data in BIOS FV regions are not changed
    for (alt_u32 region_i = 0; region_i < PCH_NUM_STATIC_SPI_REGIONS; region_i++)
    {
        // If this region is a BIOS FV region, compare against full_new_image
        alt_u32* gold_image = full_orig_image;

        // Iterate through all words in this SPI region and make sure they match the expected data
        for (alt_u32 word_i = testdata_pch_static_regions_start_addr[region_i] >> 2;
                word_i < testdata_pch_static_regions_end_addr[region_i] >> 2; word_i++)
        {
            ASSERT_EQ(gold_image[word_i], flash_x86_ptr[word_i]);
        }
    }
    for (alt_u32 region_i = 0; region_i < PCH_NUM_DYNAMIC_SPI_REGIONS; region_i++)
    {
        // By default, the dynamic SPI regions are not updated in Seamless update.
        alt_u32* gold_image = full_orig_image;

        // Iterate through all words in this SPI region and make sure they match the expected data
        for (alt_u32 word_i = testdata_pch_dynamic_regions_start_addr[region_i] >> 2;
                word_i < testdata_pch_dynamic_regions_end_addr[region_i] >> 2; word_i++)
        {
            ASSERT_EQ(gold_image[word_i], flash_x86_ptr[word_i]);
        }
    }

    // Verify the active PFR region
    // PFM should not change. Only one FVM will be updated.
    alt_u32 pch_active_pfm_start = get_ufm_pfr_data()->pch_active_pfm;
    alt_u32 pch_active_pfm_end = pch_active_pfm_start + SIGNED_PFM_MAX_SIZE;

    // Info on updated FVM
    alt_u16 target_fv_type = ((FVM*) incr_alt_u32_ptr(new_signed_fvm, SIGNATURE_SIZE))->fv_type;
    alt_u32 target_fvm_start_addr = get_active_fvm_addr(target_fv_type);
    alt_u32 target_fvm_end_addr = target_fvm_start_addr + (4096 - PCH_FW_UPDATE_SIGNED_FVM_PCH_BIOS_FILE_SIZE % 4096);

    for (alt_u32 word_i = pch_active_pfm_start >> 2; word_i < pch_active_pfm_end >> 2; word_i++)
    {
        if (((target_fvm_start_addr >> 2) <= word_i) && (word_i < (target_fvm_end_addr >> 2)))
        {
            // Check against the expected new FVM, if the address falls within this FVM in active PFR region
            alt_u32 new_signed_fvm_word_i = word_i - (target_fvm_start_addr >> 2);
            if (!(new_signed_fvm_word_i < (PCH_FW_UPDATE_SIGNED_FVM_PCH_BIOS_FILE_SIZE >> 2)))
            {
                // Data would be blank (0xFFFFFFFF) for the remaining of the page that this FVM occupies
                ASSERT_EQ(alt_u32(0xFFFFFFFF), flash_x86_ptr[word_i]);
            }
        }
        else
        {
            // The rest of Active region should be unchanged
            ASSERT_EQ(full_orig_image[word_i], flash_x86_ptr[word_i]);
        }
    }

    /*
     * Clean ups
     */
    delete[] full_orig_image;
    delete[] full_new_image;
    delete[] new_signed_fvm;
}

/**
 * @brief This test exercises the out-of-band update of spi-geo Seamless update flow, with BIOS FV capsule but
 * send to the wrong update register (non-seamless)
 */
TEST_F(SpiGeoSeamlessUpdateFlowTest, test_seamless_update_oob_incorrect_update_register)
{
    /*
     * Test Preparation
     */
    // Load the Seamless FV capsule
    SYSTEM_MOCK::get()->load_to_flash(
            SPI_FLASH_BMC,
            PCH_FW_UPDATE_SIGNED_CAPSULE_PCH_SEAMLESS_BIOS_FILE,
            PCH_FW_UPDATE_SIGNED_CAPSULE_PCH_SEAMLESS_BIOS_FILE_SIZE,
            get_ufm_pfr_data()->bmc_staging_region + BMC_STAGING_REGION_PCH_UPDATE_CAPSULE_OFFSET
    );

    // Load the entire images locally for comparison purpose
    alt_u32 *full_orig_image = new alt_u32[FULL_PFR_IMAGE_PCH_FILE_SIZE/4];
    SYSTEM_MOCK::get()->init_x86_mem_from_file(FULL_PFR_IMAGE_PCH_FILE, full_orig_image);

    alt_u32 *full_new_image = new alt_u32[FULL_PFR_IMAGE_PCH_V03P12_FILE_SIZE/4];
    SYSTEM_MOCK::get()->init_x86_mem_from_file(FULL_PFR_IMAGE_PCH_V03P12_FILE, full_new_image);

    alt_u32 *new_signed_fvm = new alt_u32[PCH_FW_UPDATE_SIGNED_FVM_PCH_BIOS_FILE_SIZE/4];
    SYSTEM_MOCK::get()->init_x86_mem_from_file(PCH_FW_UPDATE_SIGNED_FVM_PCH_BIOS_FILE, new_signed_fvm);

    // UFM should not be provisioned
    EXPECT_FALSE(is_ufm_provisioned());
    // SPI GEO should be provisioned
    EXPECT_TRUE(is_ufm_spi_geo_provisioned());
    /*
     * Flow preparation
     */
    ut_prep_nios_gpi_signals();

    // Exit after 50 iterations in the T0 loop
    SYSTEM_MOCK::get()->insert_code_block(SYSTEM_MOCK::CODE_BLOCK_TYPES::T0_OPERATIONS_END_AFTER_50_ITERS);

    // Send the Seamless update intent
    ut_send_in_update_intent_once_upon_entry_to_t0(MB_BMC_UPDATE_INTENT_PART1, MB_UPDATE_INTENT_PCH_FV_SEAMLESS_MASK);
    /*
     * Execute the flow. Active update flow will be triggered.
     */
    // Run PFR Main. Always run with the timeout
    ASSERT_DURATION_LE(GTEST_TIMEOUT_SHORT, pfr_main());

    /*
     * Verify results
     */
    switch_spi_flash(SPI_FLASH_PCH);
    alt_u32* flash_x86_ptr = SYSTEM_MOCK::get()->get_x86_ptr_to_spi_flash(SPI_FLASH_PCH);

    // Seamless update should not require T-1 entry
    EXPECT_EQ(SYSTEM_MOCK::get()->get_t_minus_1_bmc_only_counter(), alt_u32(0));
    EXPECT_EQ(SYSTEM_MOCK::get()->get_t_minus_1_pch_only_counter(), alt_u32(0));
    EXPECT_EQ(SYSTEM_MOCK::get()->get_t_minus_1_counter(), alt_u32(1));

    EXPECT_EQ(read_from_mailbox(MB_PANIC_EVENT_COUNT), alt_u32(0));
    EXPECT_EQ(read_from_mailbox(MB_LAST_PANIC_REASON), alt_u32(0));
    EXPECT_EQ(read_from_mailbox(MB_RECOVERY_COUNT), alt_u32(0));
    EXPECT_EQ(read_from_mailbox(MB_LAST_RECOVERY_REASON), alt_u32(0));
    EXPECT_EQ(read_from_mailbox(MB_MAJOR_ERROR_CODE), alt_u32(0));
    EXPECT_EQ(read_from_mailbox(MB_MINOR_ERROR_CODE), alt_u32(0));

    // Check PCH FW version (should not change in seamless update flow)
    EXPECT_EQ(read_from_mailbox(MB_PCH_PFM_ACTIVE_MAJOR_VER), alt_u32(PCH_ACTIVE_PFM_MAJOR_VER));
    EXPECT_EQ(read_from_mailbox(MB_PCH_PFM_ACTIVE_MINOR_VER), alt_u32(PCH_ACTIVE_PFM_MINOR_VER));
    EXPECT_EQ(read_from_mailbox(MB_PCH_PFM_RECOVERY_MAJOR_VER), alt_u32(PCH_RECOVERY_PFM_MAJOR_VER));
    EXPECT_EQ(read_from_mailbox(MB_PCH_PFM_RECOVERY_MINOR_VER), alt_u32(PCH_RECOVERY_PFM_MINOR_VER));

    // Check BMC FW version
    EXPECT_EQ(read_from_mailbox(MB_BMC_PFM_ACTIVE_MAJOR_VER), alt_u32(BMC_ACTIVE_PFM_MAJOR_VER));
    EXPECT_EQ(read_from_mailbox(MB_BMC_PFM_ACTIVE_MINOR_VER), alt_u32(BMC_ACTIVE_PFM_MINOR_VER));
    EXPECT_EQ(read_from_mailbox(MB_BMC_PFM_RECOVERY_MAJOR_VER), alt_u32(BMC_RECOVERY_PFM_MAJOR_VER));
    EXPECT_EQ(read_from_mailbox(MB_BMC_PFM_RECOVERY_MINOR_VER), alt_u32(BMC_RECOVERY_PFM_MINOR_VER));

    // Make sure the data in BIOS FV regions are not changed
    for (alt_u32 region_i = 0; region_i < PCH_NUM_STATIC_SPI_REGIONS; region_i++)
    {
        // If this region is a BIOS FV region, compare against full_new_image
        alt_u32* gold_image = full_orig_image;

        // Iterate through all words in this SPI region and make sure they match the expected data
        for (alt_u32 word_i = testdata_pch_static_regions_start_addr[region_i] >> 2;
                word_i < testdata_pch_static_regions_end_addr[region_i] >> 2; word_i++)
        {
            ASSERT_EQ(gold_image[word_i], flash_x86_ptr[word_i]);
        }
    }
    for (alt_u32 region_i = 0; region_i < PCH_NUM_DYNAMIC_SPI_REGIONS; region_i++)
    {
        // By default, the dynamic SPI regions are not updated in Seamless update.
        alt_u32* gold_image = full_orig_image;

        // Iterate through all words in this SPI region and make sure they match the expected data
        for (alt_u32 word_i = testdata_pch_dynamic_regions_start_addr[region_i] >> 2;
                word_i < testdata_pch_dynamic_regions_end_addr[region_i] >> 2; word_i++)
        {
            ASSERT_EQ(gold_image[word_i], flash_x86_ptr[word_i]);
        }
    }

    // Verify the active PFR region
    // PFM should not change. Only one FVM will be updated.
    alt_u32 pch_active_pfm_start = get_ufm_pfr_data()->pch_active_pfm;
    alt_u32 pch_active_pfm_end = pch_active_pfm_start + SIGNED_PFM_MAX_SIZE;

    // Info on updated FVM
    alt_u16 target_fv_type = ((FVM*) incr_alt_u32_ptr(new_signed_fvm, SIGNATURE_SIZE))->fv_type;
    alt_u32 target_fvm_start_addr = get_active_fvm_addr(target_fv_type);
    alt_u32 target_fvm_end_addr = target_fvm_start_addr + (4096 - PCH_FW_UPDATE_SIGNED_FVM_PCH_BIOS_FILE_SIZE % 4096);

    for (alt_u32 word_i = pch_active_pfm_start >> 2; word_i < pch_active_pfm_end >> 2; word_i++)
    {
        if (((target_fvm_start_addr >> 2) <= word_i) && (word_i < (target_fvm_end_addr >> 2)))
        {
            // Check against the expected new FVM, if the address falls within this FVM in active PFR region
            alt_u32 new_signed_fvm_word_i = word_i - (target_fvm_start_addr >> 2);
            if (!(new_signed_fvm_word_i < (PCH_FW_UPDATE_SIGNED_FVM_PCH_BIOS_FILE_SIZE >> 2)))
            {
                // Data would be blank (0xFFFFFFFF) for the remaining of the page that this FVM occupies
                ASSERT_EQ(alt_u32(0xFFFFFFFF), flash_x86_ptr[word_i]);
            }
        }
        else
        {
            // The rest of Active region should be unchanged
            ASSERT_EQ(full_orig_image[word_i], flash_x86_ptr[word_i]);
        }
    }

    /*
     * Clean ups
     */
    delete[] full_orig_image;
    delete[] full_new_image;
    delete[] new_signed_fvm;
}

/**
 * @brief This test exercises the out-of-band update of spi-geo Seamless update flow, with BIOS FV capsule but
 * send the unsupported update intent (non-seamless)
 */
TEST_F(SpiGeoSeamlessUpdateFlowTest, test_seamless_update_oob_unsupported_update_intent)
{
    /*
     * Test Preparation
     */
    // Load the Seamless FV capsule
    SYSTEM_MOCK::get()->load_to_flash(
            SPI_FLASH_BMC,
            PCH_FW_UPDATE_SIGNED_CAPSULE_PCH_SEAMLESS_BIOS_FILE,
            PCH_FW_UPDATE_SIGNED_CAPSULE_PCH_SEAMLESS_BIOS_FILE_SIZE,
            get_ufm_pfr_data()->bmc_staging_region + BMC_STAGING_REGION_PCH_UPDATE_CAPSULE_OFFSET
    );

    // Load the entire images locally for comparison purpose
    alt_u32 *full_orig_image = new alt_u32[FULL_PFR_IMAGE_PCH_FILE_SIZE/4];
    SYSTEM_MOCK::get()->init_x86_mem_from_file(FULL_PFR_IMAGE_PCH_FILE, full_orig_image);

    alt_u32 *full_new_image = new alt_u32[FULL_PFR_IMAGE_PCH_V03P12_FILE_SIZE/4];
    SYSTEM_MOCK::get()->init_x86_mem_from_file(FULL_PFR_IMAGE_PCH_V03P12_FILE, full_new_image);

    alt_u32 *new_signed_fvm = new alt_u32[PCH_FW_UPDATE_SIGNED_FVM_PCH_BIOS_FILE_SIZE/4];
    SYSTEM_MOCK::get()->init_x86_mem_from_file(PCH_FW_UPDATE_SIGNED_FVM_PCH_BIOS_FILE, new_signed_fvm);

    // UFM should not be provisioned
    EXPECT_FALSE(is_ufm_provisioned());
    // SPI GEO should be provisioned
    EXPECT_TRUE(is_ufm_spi_geo_provisioned());
    /*
     * Flow preparation
     */
    ut_prep_nios_gpi_signals();

    // Exit after 50 iterations in the T0 loop
    SYSTEM_MOCK::get()->insert_code_block(SYSTEM_MOCK::CODE_BLOCK_TYPES::T0_OPERATIONS_END_AFTER_50_ITERS);

    // Send the Seamless update intent
    ut_send_in_update_intent_once_upon_entry_to_t0(MB_BMC_UPDATE_INTENT_PART2, MB_UPDATE_INTENT_PCH_RECOVERY_MASK);
    /*
     * Execute the flow. Active update flow will be triggered.
     */
    // Run PFR Main. Always run with the timeout
    ASSERT_DURATION_LE(GTEST_TIMEOUT_SHORT, pfr_main());

    /*
     * Verify results
     */
    switch_spi_flash(SPI_FLASH_PCH);
    alt_u32* flash_x86_ptr = SYSTEM_MOCK::get()->get_x86_ptr_to_spi_flash(SPI_FLASH_PCH);

    // Seamless update should not require T-1 entry
    EXPECT_EQ(SYSTEM_MOCK::get()->get_t_minus_1_bmc_only_counter(), alt_u32(0));
    EXPECT_EQ(SYSTEM_MOCK::get()->get_t_minus_1_pch_only_counter(), alt_u32(0));
    EXPECT_EQ(SYSTEM_MOCK::get()->get_t_minus_1_counter(), alt_u32(1));

    EXPECT_EQ(read_from_mailbox(MB_PANIC_EVENT_COUNT), alt_u32(0));
    EXPECT_EQ(read_from_mailbox(MB_LAST_PANIC_REASON), alt_u32(0));
    EXPECT_EQ(read_from_mailbox(MB_RECOVERY_COUNT), alt_u32(0));
    EXPECT_EQ(read_from_mailbox(MB_LAST_RECOVERY_REASON), alt_u32(0));
    EXPECT_EQ(read_from_mailbox(MB_MAJOR_ERROR_CODE), alt_u32(0));
    EXPECT_EQ(read_from_mailbox(MB_MINOR_ERROR_CODE), alt_u32(0));

    // Check PCH FW version (should not change in seamless update flow)
    EXPECT_EQ(read_from_mailbox(MB_PCH_PFM_ACTIVE_MAJOR_VER), alt_u32(PCH_ACTIVE_PFM_MAJOR_VER));
    EXPECT_EQ(read_from_mailbox(MB_PCH_PFM_ACTIVE_MINOR_VER), alt_u32(PCH_ACTIVE_PFM_MINOR_VER));
    EXPECT_EQ(read_from_mailbox(MB_PCH_PFM_RECOVERY_MAJOR_VER), alt_u32(PCH_RECOVERY_PFM_MAJOR_VER));
    EXPECT_EQ(read_from_mailbox(MB_PCH_PFM_RECOVERY_MINOR_VER), alt_u32(PCH_RECOVERY_PFM_MINOR_VER));

    // Check BMC FW version
    EXPECT_EQ(read_from_mailbox(MB_BMC_PFM_ACTIVE_MAJOR_VER), alt_u32(BMC_ACTIVE_PFM_MAJOR_VER));
    EXPECT_EQ(read_from_mailbox(MB_BMC_PFM_ACTIVE_MINOR_VER), alt_u32(BMC_ACTIVE_PFM_MINOR_VER));
    EXPECT_EQ(read_from_mailbox(MB_BMC_PFM_RECOVERY_MAJOR_VER), alt_u32(BMC_RECOVERY_PFM_MAJOR_VER));
    EXPECT_EQ(read_from_mailbox(MB_BMC_PFM_RECOVERY_MINOR_VER), alt_u32(BMC_RECOVERY_PFM_MINOR_VER));

    // Make sure data in BIOS FV regions are not changed
    for (alt_u32 region_i = 0; region_i < PCH_NUM_STATIC_SPI_REGIONS; region_i++)
    {
        // If this region is a BIOS FV region, compare against full_new_image
        alt_u32* gold_image = full_orig_image;

        // Iterate through all words in this SPI region and make sure they match the expected data
        for (alt_u32 word_i = testdata_pch_static_regions_start_addr[region_i] >> 2;
                word_i < testdata_pch_static_regions_end_addr[region_i] >> 2; word_i++)
        {
            ASSERT_EQ(gold_image[word_i], flash_x86_ptr[word_i]);
        }
    }
    for (alt_u32 region_i = 0; region_i < PCH_NUM_DYNAMIC_SPI_REGIONS; region_i++)
    {
        // By default, the dynamic SPI regions are not updated in Seamless update.
        alt_u32* gold_image = full_orig_image;

        // Iterate through all words in this SPI region and make sure they match the expected data
        for (alt_u32 word_i = testdata_pch_dynamic_regions_start_addr[region_i] >> 2;
                word_i < testdata_pch_dynamic_regions_end_addr[region_i] >> 2; word_i++)
        {
            ASSERT_EQ(gold_image[word_i], flash_x86_ptr[word_i]);
        }
    }

    // Verify the active PFR region
    // PFM should not change. Only one FVM will be updated.
    alt_u32 pch_active_pfm_start = get_ufm_pfr_data()->pch_active_pfm;
    alt_u32 pch_active_pfm_end = pch_active_pfm_start + SIGNED_PFM_MAX_SIZE;

    // Info on updated FVM
    alt_u16 target_fv_type = ((FVM*) incr_alt_u32_ptr(new_signed_fvm, SIGNATURE_SIZE))->fv_type;
    alt_u32 target_fvm_start_addr = get_active_fvm_addr(target_fv_type);
    alt_u32 target_fvm_end_addr = target_fvm_start_addr + (4096 - PCH_FW_UPDATE_SIGNED_FVM_PCH_BIOS_FILE_SIZE % 4096);

    for (alt_u32 word_i = pch_active_pfm_start >> 2; word_i < pch_active_pfm_end >> 2; word_i++)
    {
        if (((target_fvm_start_addr >> 2) <= word_i) && (word_i < (target_fvm_end_addr >> 2)))
        {
            // Check against the expected new FVM, if the address falls within this FVM in active PFR region
            alt_u32 new_signed_fvm_word_i = word_i - (target_fvm_start_addr >> 2);
            if (!(new_signed_fvm_word_i < (PCH_FW_UPDATE_SIGNED_FVM_PCH_BIOS_FILE_SIZE >> 2)))
            {
                // Data would be blank (0xFFFFFFFF) for the remaining of the page that this FVM occupies
                ASSERT_EQ(alt_u32(0xFFFFFFFF), flash_x86_ptr[word_i]);
            }
        }
        else
        {
            // The rest of Active region should be unchanged
            ASSERT_EQ(full_orig_image[word_i], flash_x86_ptr[word_i]);
        }
    }

    /*
     * Clean ups
     */
    delete[] full_orig_image;
    delete[] full_new_image;
    delete[] new_signed_fvm;
}

/**
 * @brief This test exercises the in-band update of spi-geo Seamless update flow, with BIOS FV capsule but
 * send the unsupported update intent (non-seamless)
 */
TEST_F(SpiGeoSeamlessUpdateFlowTest, test_seamless_update_ib_unsupported_update_intent)
{
    /*
     * Test Preparation
     */
    // Load the Seamless FV capsule
    SYSTEM_MOCK::get()->load_to_flash(
            SPI_FLASH_PCH,
            PCH_FW_UPDATE_SIGNED_CAPSULE_PCH_SEAMLESS_BIOS_FILE,
            PCH_FW_UPDATE_SIGNED_CAPSULE_PCH_SEAMLESS_BIOS_FILE_SIZE,
            get_ufm_pfr_data()->pch_staging_region
    );

    // Load the entire images locally for comparison purpose
    alt_u32 *full_orig_image = new alt_u32[FULL_PFR_IMAGE_PCH_FILE_SIZE/4];
    SYSTEM_MOCK::get()->init_x86_mem_from_file(FULL_PFR_IMAGE_PCH_FILE, full_orig_image);

    alt_u32 *full_new_image = new alt_u32[FULL_PFR_IMAGE_PCH_V03P12_FILE_SIZE/4];
    SYSTEM_MOCK::get()->init_x86_mem_from_file(FULL_PFR_IMAGE_PCH_V03P12_FILE, full_new_image);

    alt_u32 *new_signed_fvm = new alt_u32[PCH_FW_UPDATE_SIGNED_FVM_PCH_BIOS_FILE_SIZE/4];
    SYSTEM_MOCK::get()->init_x86_mem_from_file(PCH_FW_UPDATE_SIGNED_FVM_PCH_BIOS_FILE, new_signed_fvm);

    // UFM should not be provisioned
    EXPECT_FALSE(is_ufm_provisioned());
    // SPI GEO should be provisioned
    EXPECT_TRUE(is_ufm_spi_geo_provisioned());
    /*
     * Flow preparation
     */
    ut_prep_nios_gpi_signals();

    // Exit after 50 iterations in the T0 loop
    SYSTEM_MOCK::get()->insert_code_block(SYSTEM_MOCK::CODE_BLOCK_TYPES::T0_OPERATIONS_END_AFTER_50_ITERS);

    // Send the Seamless update intent
    ut_send_in_update_intent_once_upon_entry_to_t0(MB_BMC_UPDATE_INTENT_PART2, MB_UPDATE_INTENT_BMC_ACTIVE_MASK);
    /*
     * Execute the flow. Active update flow will be triggered.
     */
    // Run PFR Main. Always run with the timeout
    ASSERT_DURATION_LE(GTEST_TIMEOUT_SHORT, pfr_main());

    /*
     * Verify results
     */
    switch_spi_flash(SPI_FLASH_PCH);
    alt_u32* flash_x86_ptr = SYSTEM_MOCK::get()->get_x86_ptr_to_spi_flash(SPI_FLASH_PCH);

    // Seamless update should not require T-1 entry
    EXPECT_EQ(SYSTEM_MOCK::get()->get_t_minus_1_bmc_only_counter(), alt_u32(0));
    EXPECT_EQ(SYSTEM_MOCK::get()->get_t_minus_1_pch_only_counter(), alt_u32(0));
    EXPECT_EQ(SYSTEM_MOCK::get()->get_t_minus_1_counter(), alt_u32(1));

    EXPECT_EQ(read_from_mailbox(MB_PANIC_EVENT_COUNT), alt_u32(0));
    EXPECT_EQ(read_from_mailbox(MB_LAST_PANIC_REASON), alt_u32(0));
    EXPECT_EQ(read_from_mailbox(MB_RECOVERY_COUNT), alt_u32(0));
    EXPECT_EQ(read_from_mailbox(MB_LAST_RECOVERY_REASON), alt_u32(0));
    EXPECT_EQ(read_from_mailbox(MB_MAJOR_ERROR_CODE), alt_u32(0));
    EXPECT_EQ(read_from_mailbox(MB_MINOR_ERROR_CODE), alt_u32(0));

    // Check PCH FW version (should not change in seamless update flow)
    EXPECT_EQ(read_from_mailbox(MB_PCH_PFM_ACTIVE_MAJOR_VER), alt_u32(PCH_ACTIVE_PFM_MAJOR_VER));
    EXPECT_EQ(read_from_mailbox(MB_PCH_PFM_ACTIVE_MINOR_VER), alt_u32(PCH_ACTIVE_PFM_MINOR_VER));
    EXPECT_EQ(read_from_mailbox(MB_PCH_PFM_RECOVERY_MAJOR_VER), alt_u32(PCH_RECOVERY_PFM_MAJOR_VER));
    EXPECT_EQ(read_from_mailbox(MB_PCH_PFM_RECOVERY_MINOR_VER), alt_u32(PCH_RECOVERY_PFM_MINOR_VER));

    // Check BMC FW version
    EXPECT_EQ(read_from_mailbox(MB_BMC_PFM_ACTIVE_MAJOR_VER), alt_u32(BMC_ACTIVE_PFM_MAJOR_VER));
    EXPECT_EQ(read_from_mailbox(MB_BMC_PFM_ACTIVE_MINOR_VER), alt_u32(BMC_ACTIVE_PFM_MINOR_VER));
    EXPECT_EQ(read_from_mailbox(MB_BMC_PFM_RECOVERY_MAJOR_VER), alt_u32(BMC_RECOVERY_PFM_MAJOR_VER));
    EXPECT_EQ(read_from_mailbox(MB_BMC_PFM_RECOVERY_MINOR_VER), alt_u32(BMC_RECOVERY_PFM_MINOR_VER));

    // Make sure the data in BIOS FV regions are not changed
    for (alt_u32 region_i = 0; region_i < PCH_NUM_STATIC_SPI_REGIONS; region_i++)
    {
        // If this region is a BIOS FV region, compare against full_new_image
        alt_u32* gold_image = full_orig_image;

        // Iterate through all words in this SPI region and make sure they match the expected data
        for (alt_u32 word_i = testdata_pch_static_regions_start_addr[region_i] >> 2;
                word_i < testdata_pch_static_regions_end_addr[region_i] >> 2; word_i++)
        {
            ASSERT_EQ(gold_image[word_i], flash_x86_ptr[word_i]);
        }
    }
    for (alt_u32 region_i = 0; region_i < PCH_NUM_DYNAMIC_SPI_REGIONS; region_i++)
    {
        // By default, the dynamic SPI regions are not updated in Seamless update.
        alt_u32* gold_image = full_orig_image;

        // Iterate through all words in this SPI region and make sure they match the expected data
        for (alt_u32 word_i = testdata_pch_dynamic_regions_start_addr[region_i] >> 2;
                word_i < testdata_pch_dynamic_regions_end_addr[region_i] >> 2; word_i++)
        {
            ASSERT_EQ(gold_image[word_i], flash_x86_ptr[word_i]);
        }
    }

    // Verify the active PFR region
    // PFM should not change. Only one FVM will be updated.
    alt_u32 pch_active_pfm_start = get_ufm_pfr_data()->pch_active_pfm;
    alt_u32 pch_active_pfm_end = pch_active_pfm_start + SIGNED_PFM_MAX_SIZE;

    // Info on updated FVM
    alt_u16 target_fv_type = ((FVM*) incr_alt_u32_ptr(new_signed_fvm, SIGNATURE_SIZE))->fv_type;
    alt_u32 target_fvm_start_addr = get_active_fvm_addr(target_fv_type);
    alt_u32 target_fvm_end_addr = target_fvm_start_addr + (4096 - PCH_FW_UPDATE_SIGNED_FVM_PCH_BIOS_FILE_SIZE % 4096);

    for (alt_u32 word_i = pch_active_pfm_start >> 2; word_i < pch_active_pfm_end >> 2; word_i++)
    {
        if (((target_fvm_start_addr >> 2) <= word_i) && (word_i < (target_fvm_end_addr >> 2)))
        {
            // Check against the expected new FVM, if the address falls within this FVM in active PFR region
            alt_u32 new_signed_fvm_word_i = word_i - (target_fvm_start_addr >> 2);
            if (!(new_signed_fvm_word_i < (PCH_FW_UPDATE_SIGNED_FVM_PCH_BIOS_FILE_SIZE >> 2)))
            {
                // Data would be blank (0xFFFFFFFF) for the remaining of the page that this FVM occupies
                ASSERT_EQ(alt_u32(0xFFFFFFFF), flash_x86_ptr[word_i]);
            }
        }
        else
        {
            // The rest of Active region should be unchanged
            ASSERT_EQ(full_orig_image[word_i], flash_x86_ptr[word_i]);
        }
    }

    /*
     * Clean ups
     */
    delete[] full_orig_image;
    delete[] full_new_image;
    delete[] new_signed_fvm;
}

/**
 * @brief This test exercises the in-band happy path of spi-geo Seamless update flow, with BIOS FV capsule.
 * After that, erase the spi geo provisioning data and and provision cpld and re-authenticate flash
 */
TEST_F(SpiGeoSeamlessUpdateFlowTest, test_unprovision_seamless_update_ib_happy_path_and_then_provision)
{
    /*
     * Test Preparation
     */
    // Load the Seamless FV capsule
    SYSTEM_MOCK::get()->load_to_flash(
            SPI_FLASH_PCH,
            PCH_FW_UPDATE_SIGNED_CAPSULE_PCH_SEAMLESS_BIOS_FILE,
            PCH_FW_UPDATE_SIGNED_CAPSULE_PCH_SEAMLESS_BIOS_FILE_SIZE,
            get_ufm_pfr_data()->pch_staging_region
    );

    // Load the entire images locally for comparison purpose
    alt_u32 *full_orig_image = new alt_u32[FULL_PFR_IMAGE_PCH_FILE_SIZE/4];
    SYSTEM_MOCK::get()->init_x86_mem_from_file(FULL_PFR_IMAGE_PCH_FILE, full_orig_image);

    alt_u32 *full_new_image = new alt_u32[FULL_PFR_IMAGE_PCH_V03P12_FILE_SIZE/4];
    SYSTEM_MOCK::get()->init_x86_mem_from_file(FULL_PFR_IMAGE_PCH_V03P12_FILE, full_new_image);

    alt_u32 *new_signed_fvm = new alt_u32[PCH_FW_UPDATE_SIGNED_FVM_PCH_BIOS_FILE_SIZE/4];
    SYSTEM_MOCK::get()->init_x86_mem_from_file(PCH_FW_UPDATE_SIGNED_FVM_PCH_BIOS_FILE, new_signed_fvm);

    // UFM should not be provisioned
    EXPECT_FALSE(is_ufm_provisioned());
    // SPI GEO should be provisioned
    EXPECT_TRUE(is_ufm_spi_geo_provisioned());

    /*
     * Flow preparation
     */
    ut_prep_nios_gpi_signals();

    // Exit after 50 iterations in the T0 loop
    SYSTEM_MOCK::get()->insert_code_block(SYSTEM_MOCK::CODE_BLOCK_TYPES::T0_OPERATIONS_END_AFTER_50_ITERS);

    // Send the Seamless update intent
    ut_send_in_update_intent_once_upon_entry_to_t0(MB_PCH_UPDATE_INTENT_PART2, MB_UPDATE_INTENT_PCH_FV_SEAMLESS_MASK);
    /*
     * Execute the flow. Active update flow will be triggered.
     */
    // Run PFR Main. Always run with the timeout
    ASSERT_DURATION_LE(GTEST_TIMEOUT_SHORT, pfr_main());

    /*
     * Verify results
     */
    switch_spi_flash(SPI_FLASH_PCH);
    alt_u32* flash_x86_ptr = SYSTEM_MOCK::get()->get_x86_ptr_to_spi_flash(SPI_FLASH_PCH);

    // Seamless update should not require T-1 entry
    EXPECT_EQ(SYSTEM_MOCK::get()->get_t_minus_1_bmc_only_counter(), alt_u32(0));
    EXPECT_EQ(SYSTEM_MOCK::get()->get_t_minus_1_pch_only_counter(), alt_u32(0));
    EXPECT_EQ(SYSTEM_MOCK::get()->get_t_minus_1_counter(), alt_u32(1));

    EXPECT_EQ(read_from_mailbox(MB_PANIC_EVENT_COUNT), alt_u32(0));
    EXPECT_EQ(read_from_mailbox(MB_LAST_PANIC_REASON), alt_u32(0));
    EXPECT_EQ(read_from_mailbox(MB_RECOVERY_COUNT), alt_u32(0));
    EXPECT_EQ(read_from_mailbox(MB_LAST_RECOVERY_REASON), alt_u32(0));
    EXPECT_EQ(read_from_mailbox(MB_MAJOR_ERROR_CODE), alt_u32(0));
    EXPECT_EQ(read_from_mailbox(MB_MINOR_ERROR_CODE), alt_u32(0));

    // Check PCH FW version (should not change in seamless update flow)
    EXPECT_EQ(read_from_mailbox(MB_PCH_PFM_ACTIVE_MAJOR_VER), alt_u32(PCH_ACTIVE_PFM_MAJOR_VER));
    EXPECT_EQ(read_from_mailbox(MB_PCH_PFM_ACTIVE_MINOR_VER), alt_u32(PCH_ACTIVE_PFM_MINOR_VER));
    EXPECT_EQ(read_from_mailbox(MB_PCH_PFM_RECOVERY_MAJOR_VER), alt_u32(PCH_RECOVERY_PFM_MAJOR_VER));
    EXPECT_EQ(read_from_mailbox(MB_PCH_PFM_RECOVERY_MINOR_VER), alt_u32(PCH_RECOVERY_PFM_MINOR_VER));

    // Check BMC FW version
    EXPECT_EQ(read_from_mailbox(MB_BMC_PFM_ACTIVE_MAJOR_VER), alt_u32(BMC_ACTIVE_PFM_MAJOR_VER));
    EXPECT_EQ(read_from_mailbox(MB_BMC_PFM_ACTIVE_MINOR_VER), alt_u32(BMC_ACTIVE_PFM_MINOR_VER));
    EXPECT_EQ(read_from_mailbox(MB_BMC_PFM_RECOVERY_MAJOR_VER), alt_u32(BMC_RECOVERY_PFM_MAJOR_VER));
    EXPECT_EQ(read_from_mailbox(MB_BMC_PFM_RECOVERY_MINOR_VER), alt_u32(BMC_RECOVERY_PFM_MINOR_VER));

    // Make sure only data in BIOS FV regions are changed
    for (alt_u32 region_i = 0; region_i < PCH_NUM_STATIC_SPI_REGIONS; region_i++)
    {
        // If this region is a BIOS FV region, compare against full_new_image
        alt_u32* gold_image = full_orig_image;
        for (alt_u32 addr_i = 0; addr_i < PCH_NUM_BIOS_FV_REGIONS; addr_i++)
        {
            if (testdata_pch_static_regions_start_addr[region_i] == m_bios_fv_regions_start_addr[addr_i])
            {
                gold_image = full_new_image;
            }
        }

        // Iterate through all words in this SPI region and make sure they match the expected data
        for (alt_u32 word_i = testdata_pch_static_regions_start_addr[region_i] >> 2;
                word_i < testdata_pch_static_regions_end_addr[region_i] >> 2; word_i++)
        {
            ASSERT_EQ(gold_image[word_i], flash_x86_ptr[word_i]);
        }
    }
    for (alt_u32 region_i = 0; region_i < PCH_NUM_DYNAMIC_SPI_REGIONS; region_i++)
    {
        // By default, the dynamic SPI regions are not updated in Seamless update.
        alt_u32* gold_image = full_orig_image;

        // Iterate through all words in this SPI region and make sure they match the expected data
        for (alt_u32 word_i = testdata_pch_dynamic_regions_start_addr[region_i] >> 2;
                word_i < testdata_pch_dynamic_regions_end_addr[region_i] >> 2; word_i++)
        {
            ASSERT_EQ(gold_image[word_i], flash_x86_ptr[word_i]);
        }
    }

    // Verify the active PFR region
    // PFM should not change. Only one FVM will be updated.
    alt_u32 pch_active_pfm_start = get_ufm_pfr_data()->pch_active_pfm;
    alt_u32 pch_active_pfm_end = pch_active_pfm_start + SIGNED_PFM_MAX_SIZE;

    // Info on updated FVM
    alt_u16 target_fv_type = ((FVM*) incr_alt_u32_ptr(new_signed_fvm, SIGNATURE_SIZE))->fv_type;
    alt_u32 target_fvm_start_addr = get_active_fvm_addr(target_fv_type);
    alt_u32 target_fvm_end_addr = target_fvm_start_addr + (4096 - PCH_FW_UPDATE_SIGNED_FVM_PCH_BIOS_FILE_SIZE % 4096);

    for (alt_u32 word_i = pch_active_pfm_start >> 2; word_i < pch_active_pfm_end >> 2; word_i++)
    {
        if (((target_fvm_start_addr >> 2) <= word_i) && (word_i < (target_fvm_end_addr >> 2)))
        {
            // Check against the expected new FVM, if the address falls within this FVM in active PFR region
            alt_u32 new_signed_fvm_word_i = word_i - (target_fvm_start_addr >> 2);
            if (new_signed_fvm_word_i < (PCH_FW_UPDATE_SIGNED_FVM_PCH_BIOS_FILE_SIZE >> 2))
            {
                ASSERT_EQ(new_signed_fvm[new_signed_fvm_word_i], flash_x86_ptr[word_i]);
            }
            else
            {
                // Data would be blank (0xFFFFFFFF) for the remaining of the page that this FVM occupies
                ASSERT_EQ(alt_u32(0xFFFFFFFF), flash_x86_ptr[word_i]);
            }
        }
        else
        {
            // The rest of Active region should be unchanged
            ASSERT_EQ(full_orig_image[word_i], flash_x86_ptr[word_i]);
        }
    }

    // Reprovision ufm with rk hash, pch and bmc offset
    SYSTEM_MOCK::get()->provision_ufm_data(UFM_PFR_DATA_EXAMPLE_KEY_FILE);

    // UFM should be provisioned
    EXPECT_TRUE(is_ufm_provisioned());
    // SPI GEO should not be provisioned
    EXPECT_FALSE(is_ufm_spi_geo_provisioned());

    // Authenticate the updated image including validating the signature
    // Run PFR Main. Always run with the timeout
    ASSERT_DURATION_LE(GTEST_TIMEOUT_SHORT, pfr_main());

    EXPECT_EQ(SYSTEM_MOCK::get()->get_t_minus_1_bmc_only_counter(), alt_u32(0));
    EXPECT_EQ(SYSTEM_MOCK::get()->get_t_minus_1_pch_only_counter(), alt_u32(0));
    // Comes from calling pfr_main twice
    EXPECT_EQ(SYSTEM_MOCK::get()->get_t_minus_1_counter(), alt_u32(2));

    // Expect no corruption in update
    EXPECT_EQ(read_from_mailbox(MB_PANIC_EVENT_COUNT), alt_u32(0));
    EXPECT_EQ(read_from_mailbox(MB_LAST_PANIC_REASON), alt_u32(0));
    EXPECT_EQ(read_from_mailbox(MB_RECOVERY_COUNT), alt_u32(0));
    EXPECT_EQ(read_from_mailbox(MB_LAST_RECOVERY_REASON), alt_u32(0));
    EXPECT_EQ(read_from_mailbox(MB_MAJOR_ERROR_CODE), alt_u32(0));
    EXPECT_EQ(read_from_mailbox(MB_MINOR_ERROR_CODE), alt_u32(0));

    // Check PCH FW version (should not change in seamless update flow)
    EXPECT_EQ(read_from_mailbox(MB_PCH_PFM_ACTIVE_MAJOR_VER), alt_u32(PCH_ACTIVE_PFM_MAJOR_VER));
    EXPECT_EQ(read_from_mailbox(MB_PCH_PFM_ACTIVE_MINOR_VER), alt_u32(PCH_ACTIVE_PFM_MINOR_VER));
    EXPECT_EQ(read_from_mailbox(MB_PCH_PFM_RECOVERY_MAJOR_VER), alt_u32(PCH_RECOVERY_PFM_MAJOR_VER));
    EXPECT_EQ(read_from_mailbox(MB_PCH_PFM_RECOVERY_MINOR_VER), alt_u32(PCH_RECOVERY_PFM_MINOR_VER));

    // Check BMC FW version
    EXPECT_EQ(read_from_mailbox(MB_BMC_PFM_ACTIVE_MAJOR_VER), alt_u32(BMC_ACTIVE_PFM_MAJOR_VER));
    EXPECT_EQ(read_from_mailbox(MB_BMC_PFM_ACTIVE_MINOR_VER), alt_u32(BMC_ACTIVE_PFM_MINOR_VER));
    EXPECT_EQ(read_from_mailbox(MB_BMC_PFM_RECOVERY_MAJOR_VER), alt_u32(BMC_RECOVERY_PFM_MAJOR_VER));
    EXPECT_EQ(read_from_mailbox(MB_BMC_PFM_RECOVERY_MINOR_VER), alt_u32(BMC_RECOVERY_PFM_MINOR_VER));

    /*
     * Clean ups
     */
    delete[] full_orig_image;
    delete[] full_new_image;
    delete[] new_signed_fvm;
}

/**
 * @brief This test exercises the out-of-band happy path of spi-geo Seamless update flow, with BIOS FV capsule.
 * After that, erase the spi geo provisioning data and and provision cpld and re-authenticate flash
 */
TEST_F(SpiGeoSeamlessUpdateFlowTest, test_unprovision_seamless_update_oob_happy_path_and_then_provision)
{
    /*
     * Test Preparation
     */
    // Load the Seamless FV capsule
    SYSTEM_MOCK::get()->load_to_flash(
            SPI_FLASH_BMC,
            PCH_FW_UPDATE_SIGNED_CAPSULE_PCH_SEAMLESS_BIOS_FILE,
            PCH_FW_UPDATE_SIGNED_CAPSULE_PCH_SEAMLESS_BIOS_FILE_SIZE,
            get_ufm_pfr_data()->bmc_staging_region + BMC_STAGING_REGION_PCH_UPDATE_CAPSULE_OFFSET
    );

    // Load the entire images locally for comparison purpose
    alt_u32 *full_orig_image = new alt_u32[FULL_PFR_IMAGE_PCH_FILE_SIZE/4];
    SYSTEM_MOCK::get()->init_x86_mem_from_file(FULL_PFR_IMAGE_PCH_FILE, full_orig_image);

    alt_u32 *full_new_image = new alt_u32[FULL_PFR_IMAGE_PCH_V03P12_FILE_SIZE/4];
    SYSTEM_MOCK::get()->init_x86_mem_from_file(FULL_PFR_IMAGE_PCH_V03P12_FILE, full_new_image);

    alt_u32 *new_signed_fvm = new alt_u32[PCH_FW_UPDATE_SIGNED_FVM_PCH_BIOS_FILE_SIZE/4];
    SYSTEM_MOCK::get()->init_x86_mem_from_file(PCH_FW_UPDATE_SIGNED_FVM_PCH_BIOS_FILE, new_signed_fvm);

    // UFM should not be provisioned
    EXPECT_FALSE(is_ufm_provisioned());
    // SPI GEO should be provisioned
    EXPECT_TRUE(is_ufm_spi_geo_provisioned());

    /*
     * Flow preparation
     */
    ut_prep_nios_gpi_signals();

    // Exit after 50 iterations in the T0 loop
    SYSTEM_MOCK::get()->insert_code_block(SYSTEM_MOCK::CODE_BLOCK_TYPES::T0_OPERATIONS_END_AFTER_50_ITERS);

    // Send the Seamless update intent
    ut_send_in_update_intent_once_upon_entry_to_t0(MB_BMC_UPDATE_INTENT_PART2, MB_UPDATE_INTENT_PCH_FV_SEAMLESS_MASK);
    /*
     * Execute the flow. Active update flow will be triggered.
     */
    // Run PFR Main. Always run with the timeout
    ASSERT_DURATION_LE(GTEST_TIMEOUT_SHORT, pfr_main());

    /*
     * Verify results
     */
    switch_spi_flash(SPI_FLASH_PCH);
    alt_u32* flash_x86_ptr = SYSTEM_MOCK::get()->get_x86_ptr_to_spi_flash(SPI_FLASH_PCH);

    // Seamless update should not require T-1 entry
    EXPECT_EQ(SYSTEM_MOCK::get()->get_t_minus_1_bmc_only_counter(), alt_u32(0));
    EXPECT_EQ(SYSTEM_MOCK::get()->get_t_minus_1_pch_only_counter(), alt_u32(0));
    EXPECT_EQ(SYSTEM_MOCK::get()->get_t_minus_1_counter(), alt_u32(1));

    EXPECT_EQ(read_from_mailbox(MB_PANIC_EVENT_COUNT), alt_u32(0));
    EXPECT_EQ(read_from_mailbox(MB_LAST_PANIC_REASON), alt_u32(0));
    EXPECT_EQ(read_from_mailbox(MB_RECOVERY_COUNT), alt_u32(0));
    EXPECT_EQ(read_from_mailbox(MB_LAST_RECOVERY_REASON), alt_u32(0));
    EXPECT_EQ(read_from_mailbox(MB_MAJOR_ERROR_CODE), alt_u32(0));
    EXPECT_EQ(read_from_mailbox(MB_MINOR_ERROR_CODE), alt_u32(0));

    // Check PCH FW version (should not change in seamless update flow)
    EXPECT_EQ(read_from_mailbox(MB_PCH_PFM_ACTIVE_MAJOR_VER), alt_u32(PCH_ACTIVE_PFM_MAJOR_VER));
    EXPECT_EQ(read_from_mailbox(MB_PCH_PFM_ACTIVE_MINOR_VER), alt_u32(PCH_ACTIVE_PFM_MINOR_VER));
    EXPECT_EQ(read_from_mailbox(MB_PCH_PFM_RECOVERY_MAJOR_VER), alt_u32(PCH_RECOVERY_PFM_MAJOR_VER));
    EXPECT_EQ(read_from_mailbox(MB_PCH_PFM_RECOVERY_MINOR_VER), alt_u32(PCH_RECOVERY_PFM_MINOR_VER));

    // Check BMC FW version
    EXPECT_EQ(read_from_mailbox(MB_BMC_PFM_ACTIVE_MAJOR_VER), alt_u32(BMC_ACTIVE_PFM_MAJOR_VER));
    EXPECT_EQ(read_from_mailbox(MB_BMC_PFM_ACTIVE_MINOR_VER), alt_u32(BMC_ACTIVE_PFM_MINOR_VER));
    EXPECT_EQ(read_from_mailbox(MB_BMC_PFM_RECOVERY_MAJOR_VER), alt_u32(BMC_RECOVERY_PFM_MAJOR_VER));
    EXPECT_EQ(read_from_mailbox(MB_BMC_PFM_RECOVERY_MINOR_VER), alt_u32(BMC_RECOVERY_PFM_MINOR_VER));

    // Make sure only data in BIOS FV regions are changed
    for (alt_u32 region_i = 0; region_i < PCH_NUM_STATIC_SPI_REGIONS; region_i++)
    {
        // If this region is a BIOS FV region, compare against full_new_image
        alt_u32* gold_image = full_orig_image;
        for (alt_u32 addr_i = 0; addr_i < PCH_NUM_BIOS_FV_REGIONS; addr_i++)
        {
            if (testdata_pch_static_regions_start_addr[region_i] == m_bios_fv_regions_start_addr[addr_i])
            {
                gold_image = full_new_image;
            }
        }

        // Iterate through all words in this SPI region and make sure they match the expected data
        for (alt_u32 word_i = testdata_pch_static_regions_start_addr[region_i] >> 2;
                word_i < testdata_pch_static_regions_end_addr[region_i] >> 2; word_i++)
        {
            ASSERT_EQ(gold_image[word_i], flash_x86_ptr[word_i]);
        }
    }
    for (alt_u32 region_i = 0; region_i < PCH_NUM_DYNAMIC_SPI_REGIONS; region_i++)
    {
        // By default, the dynamic SPI regions are not updated in Seamless update.
        alt_u32* gold_image = full_orig_image;

        // Iterate through all words in this SPI region and make sure they match the expected data
        for (alt_u32 word_i = testdata_pch_dynamic_regions_start_addr[region_i] >> 2;
                word_i < testdata_pch_dynamic_regions_end_addr[region_i] >> 2; word_i++)
        {
            ASSERT_EQ(gold_image[word_i], flash_x86_ptr[word_i]);
        }
    }

    // Verify the active PFR region
    // PFM should not change. Only one FVM will be updated.
    alt_u32 pch_active_pfm_start = get_ufm_pfr_data()->pch_active_pfm;
    alt_u32 pch_active_pfm_end = pch_active_pfm_start + SIGNED_PFM_MAX_SIZE;

    // Info on updated FVM
    alt_u16 target_fv_type = ((FVM*) incr_alt_u32_ptr(new_signed_fvm, SIGNATURE_SIZE))->fv_type;
    alt_u32 target_fvm_start_addr = get_active_fvm_addr(target_fv_type);
    alt_u32 target_fvm_end_addr = target_fvm_start_addr + (4096 - PCH_FW_UPDATE_SIGNED_FVM_PCH_BIOS_FILE_SIZE % 4096);

    for (alt_u32 word_i = pch_active_pfm_start >> 2; word_i < pch_active_pfm_end >> 2; word_i++)
    {
        if (((target_fvm_start_addr >> 2) <= word_i) && (word_i < (target_fvm_end_addr >> 2)))
        {
            // Check against the expected new FVM, if the address falls within this FVM in active PFR region
            alt_u32 new_signed_fvm_word_i = word_i - (target_fvm_start_addr >> 2);
            if (new_signed_fvm_word_i < (PCH_FW_UPDATE_SIGNED_FVM_PCH_BIOS_FILE_SIZE >> 2))
            {
                ASSERT_EQ(new_signed_fvm[new_signed_fvm_word_i], flash_x86_ptr[word_i]);
            }
            else
            {
                // Data would be blank (0xFFFFFFFF) for the remaining of the page that this FVM occupies
                ASSERT_EQ(alt_u32(0xFFFFFFFF), flash_x86_ptr[word_i]);
            }
        }
        else
        {
            // The rest of Active region should be unchanged
            ASSERT_EQ(full_orig_image[word_i], flash_x86_ptr[word_i]);
        }
    }

    // Reprovision ufm with rk hash, pch and bmc offset
    SYSTEM_MOCK::get()->provision_ufm_data(UFM_PFR_DATA_EXAMPLE_KEY_FILE);

    // UFM should be provisioned
    EXPECT_TRUE(is_ufm_provisioned());
    // SPI GEO should not be provisioned
    EXPECT_FALSE(is_ufm_spi_geo_provisioned());

    // Authenticate the updated image including validating the signature
    // Run PFR Main. Always run with the timeout
    ASSERT_DURATION_LE(GTEST_TIMEOUT_SHORT, pfr_main());

    EXPECT_EQ(SYSTEM_MOCK::get()->get_t_minus_1_bmc_only_counter(), alt_u32(0));
    EXPECT_EQ(SYSTEM_MOCK::get()->get_t_minus_1_pch_only_counter(), alt_u32(0));
    // Comes from calling pfr_main twice
    EXPECT_EQ(SYSTEM_MOCK::get()->get_t_minus_1_counter(), alt_u32(2));

    // Expect no corruption in update
    EXPECT_EQ(read_from_mailbox(MB_PANIC_EVENT_COUNT), alt_u32(0));
    EXPECT_EQ(read_from_mailbox(MB_LAST_PANIC_REASON), alt_u32(0));
    EXPECT_EQ(read_from_mailbox(MB_RECOVERY_COUNT), alt_u32(0));
    EXPECT_EQ(read_from_mailbox(MB_LAST_RECOVERY_REASON), alt_u32(0));
    EXPECT_EQ(read_from_mailbox(MB_MAJOR_ERROR_CODE), alt_u32(0));
    EXPECT_EQ(read_from_mailbox(MB_MINOR_ERROR_CODE), alt_u32(0));

    // Check PCH FW version (should not change in seamless update flow)
    EXPECT_EQ(read_from_mailbox(MB_PCH_PFM_ACTIVE_MAJOR_VER), alt_u32(PCH_ACTIVE_PFM_MAJOR_VER));
    EXPECT_EQ(read_from_mailbox(MB_PCH_PFM_ACTIVE_MINOR_VER), alt_u32(PCH_ACTIVE_PFM_MINOR_VER));
    EXPECT_EQ(read_from_mailbox(MB_PCH_PFM_RECOVERY_MAJOR_VER), alt_u32(PCH_RECOVERY_PFM_MAJOR_VER));
    EXPECT_EQ(read_from_mailbox(MB_PCH_PFM_RECOVERY_MINOR_VER), alt_u32(PCH_RECOVERY_PFM_MINOR_VER));

    // Check BMC FW version
    EXPECT_EQ(read_from_mailbox(MB_BMC_PFM_ACTIVE_MAJOR_VER), alt_u32(BMC_ACTIVE_PFM_MAJOR_VER));
    EXPECT_EQ(read_from_mailbox(MB_BMC_PFM_ACTIVE_MINOR_VER), alt_u32(BMC_ACTIVE_PFM_MINOR_VER));
    EXPECT_EQ(read_from_mailbox(MB_BMC_PFM_RECOVERY_MAJOR_VER), alt_u32(BMC_RECOVERY_PFM_MAJOR_VER));
    EXPECT_EQ(read_from_mailbox(MB_BMC_PFM_RECOVERY_MINOR_VER), alt_u32(BMC_RECOVERY_PFM_MINOR_VER));

    /*
     * Clean ups
     */
    delete[] full_orig_image;
    delete[] full_new_image;
    delete[] new_signed_fvm;
}

/**
 * @brief This test exercises the in-band happy path of spi-geo Seamless update flow, with BIOS FV capsule
 * and UPDATE_DYNAMIC bit set in update intent.
 */
TEST_F(SpiGeoSeamlessUpdateFlowTest, test_spigeo_ib_seamless_update_static_plus_dynamic_then_provision)
{
    /*
     * Test Preparation
     */
    // Load the Seamless FV capsule
    SYSTEM_MOCK::get()->load_to_flash(SPI_FLASH_PCH, PCH_FW_UPDATE_SIGNED_CAPSULE_PCH_SEAMLESS_BIOS_FILE,
            PCH_FW_UPDATE_SIGNED_CAPSULE_PCH_SEAMLESS_BIOS_FILE_SIZE, get_ufm_pfr_data()->pch_staging_region);

    // Load the entire images locally for comparison purpose
    alt_u32 *full_orig_image = new alt_u32[FULL_PFR_IMAGE_PCH_FILE_SIZE/4];
    SYSTEM_MOCK::get()->init_x86_mem_from_file(FULL_PFR_IMAGE_PCH_FILE, full_orig_image);

    alt_u32 *full_new_image = new alt_u32[FULL_PFR_IMAGE_PCH_V03P12_FILE_SIZE/4];
    SYSTEM_MOCK::get()->init_x86_mem_from_file(FULL_PFR_IMAGE_PCH_V03P12_FILE, full_new_image);

    alt_u32 *new_signed_fvm = new alt_u32[PCH_FW_UPDATE_SIGNED_FVM_PCH_BIOS_FILE_SIZE/4];
    SYSTEM_MOCK::get()->init_x86_mem_from_file(PCH_FW_UPDATE_SIGNED_FVM_PCH_BIOS_FILE, new_signed_fvm);

    // UFM should not be provisioned
    EXPECT_FALSE(is_ufm_provisioned());
    // SPI GEO should be provisioned
    EXPECT_TRUE(is_ufm_spi_geo_provisioned());
    /*
     * Flow preparation
     */
    ut_prep_nios_gpi_signals();

    // Exit after 50 iterations in the T0 loop
    SYSTEM_MOCK::get()->insert_code_block(SYSTEM_MOCK::CODE_BLOCK_TYPES::T0_OPERATIONS_END_AFTER_50_ITERS);

    // Send the Seamless update intent
    write_to_mailbox(MB_PCH_UPDATE_INTENT_PART2, MB_UPDATE_INTENT_PCH_FV_SEAMLESS_MASK);
    write_to_mailbox(MB_PCH_UPDATE_INTENT_PART1, MB_UPDATE_INTENT_UPDATE_DYNAMIC_MASK);

    /*
     * Execute the flow. Active update flow will be triggered.
     */
    // Run PFR Main. Always run with the timeout
    ASSERT_DURATION_LE(GTEST_TIMEOUT_SHORT, pfr_main());

    /*
     * Verify results
     */
    switch_spi_flash(SPI_FLASH_PCH);
    alt_u32* flash_x86_ptr = SYSTEM_MOCK::get()->get_x86_ptr_to_spi_flash(SPI_FLASH_PCH);

    // Seamless update should not require T-1 entry
    EXPECT_EQ(SYSTEM_MOCK::get()->get_t_minus_1_bmc_only_counter(), alt_u32(0));
    EXPECT_EQ(SYSTEM_MOCK::get()->get_t_minus_1_pch_only_counter(), alt_u32(0));
    EXPECT_EQ(SYSTEM_MOCK::get()->get_t_minus_1_counter(), alt_u32(1));

    EXPECT_EQ(read_from_mailbox(MB_PANIC_EVENT_COUNT), alt_u32(0));
    EXPECT_EQ(read_from_mailbox(MB_LAST_PANIC_REASON), alt_u32(0));
    EXPECT_EQ(read_from_mailbox(MB_RECOVERY_COUNT), alt_u32(0));
    EXPECT_EQ(read_from_mailbox(MB_LAST_RECOVERY_REASON), alt_u32(0));
    EXPECT_EQ(read_from_mailbox(MB_MAJOR_ERROR_CODE), alt_u32(0));
    EXPECT_EQ(read_from_mailbox(MB_MINOR_ERROR_CODE), alt_u32(0));

    // Check PCH FW version (should not change in seamless update flow)
    EXPECT_EQ(read_from_mailbox(MB_PCH_PFM_ACTIVE_MAJOR_VER), alt_u32(PCH_ACTIVE_PFM_MAJOR_VER));
    EXPECT_EQ(read_from_mailbox(MB_PCH_PFM_ACTIVE_MINOR_VER), alt_u32(PCH_ACTIVE_PFM_MINOR_VER));
    EXPECT_EQ(read_from_mailbox(MB_PCH_PFM_RECOVERY_MAJOR_VER), alt_u32(PCH_RECOVERY_PFM_MAJOR_VER));
    EXPECT_EQ(read_from_mailbox(MB_PCH_PFM_RECOVERY_MINOR_VER), alt_u32(PCH_RECOVERY_PFM_MINOR_VER));

    // Check BMC FW version
    EXPECT_EQ(read_from_mailbox(MB_BMC_PFM_ACTIVE_MAJOR_VER), alt_u32(BMC_ACTIVE_PFM_MAJOR_VER));
    EXPECT_EQ(read_from_mailbox(MB_BMC_PFM_ACTIVE_MINOR_VER), alt_u32(BMC_ACTIVE_PFM_MINOR_VER));
    EXPECT_EQ(read_from_mailbox(MB_BMC_PFM_RECOVERY_MAJOR_VER), alt_u32(BMC_RECOVERY_PFM_MAJOR_VER));
    EXPECT_EQ(read_from_mailbox(MB_BMC_PFM_RECOVERY_MINOR_VER), alt_u32(BMC_RECOVERY_PFM_MINOR_VER));

    // Make sure only data in BIOS FV regions are changed
    for (alt_u32 region_i = 0; region_i < PCH_NUM_STATIC_SPI_REGIONS; region_i++)
    {
        // If this region is a BIOS FV region, compare against full_new_image
        alt_u32* gold_image = full_orig_image;
        for (alt_u32 addr_i = 0; addr_i < PCH_NUM_BIOS_FV_REGIONS; addr_i++)
        {
            if (testdata_pch_static_regions_start_addr[region_i] == m_bios_fv_regions_start_addr[addr_i])
            {
                gold_image = full_new_image;
            }
        }

        // Iterate through all words in this SPI region and make sure they match the expected data
        for (alt_u32 word_i = testdata_pch_static_regions_start_addr[region_i] >> 2;
                word_i < testdata_pch_static_regions_end_addr[region_i] >> 2; word_i++)
        {
            ASSERT_EQ(gold_image[word_i], flash_x86_ptr[word_i]);
        }
    }
    for (alt_u32 region_i = 0; region_i < PCH_NUM_DYNAMIC_SPI_REGIONS; region_i++)
    {
        // If this region is a BIOS FV region, compare against full_new_image
        alt_u32* gold_image = full_orig_image;
        for (alt_u32 addr_i = 0; addr_i < PCH_NUM_BIOS_FV_REGIONS; addr_i++)
        {
            if (testdata_pch_dynamic_regions_start_addr[region_i] == m_bios_fv_regions_start_addr[addr_i])
            {
                gold_image = full_new_image;
            }
        }

        // Iterate through all words in this SPI region and make sure they match the expected data
        for (alt_u32 word_i = testdata_pch_dynamic_regions_start_addr[region_i] >> 2;
                word_i < testdata_pch_dynamic_regions_end_addr[region_i] >> 2; word_i++)
        {
            ASSERT_EQ(gold_image[word_i], flash_x86_ptr[word_i]);
        }
    }

    // Verify the active PFR region
    // PFM should not change. Only one FVM will be updated.
    alt_u32 pch_active_pfm_start = get_ufm_pfr_data()->pch_active_pfm;
    alt_u32 pch_active_pfm_end = pch_active_pfm_start + SIGNED_PFM_MAX_SIZE;

    // Info on updated FVM
    alt_u16 target_fv_type = ((FVM*) incr_alt_u32_ptr(new_signed_fvm, SIGNATURE_SIZE))->fv_type;
    alt_u32 target_fvm_start_addr = get_active_fvm_addr(target_fv_type);
    alt_u32 target_fvm_end_addr = target_fvm_start_addr + (4096 - PCH_FW_UPDATE_SIGNED_FVM_PCH_BIOS_FILE_SIZE % 4096);

    for (alt_u32 word_i = pch_active_pfm_start >> 2; word_i < pch_active_pfm_end >> 2; word_i++)
    {
        if (((target_fvm_start_addr >> 2) <= word_i) && (word_i < (target_fvm_end_addr >> 2)))
        {
            // Check against the expected new FVM, if the address falls within this FVM in active PFR region
            alt_u32 new_signed_fvm_word_i = word_i - (target_fvm_start_addr >> 2);
            if (new_signed_fvm_word_i < (PCH_FW_UPDATE_SIGNED_FVM_PCH_BIOS_FILE_SIZE >> 2))
            {
                ASSERT_EQ(new_signed_fvm[new_signed_fvm_word_i], flash_x86_ptr[word_i]);
            }
            else
            {
                // Data would be blank (0xFFFFFFFF) for the remaining of the page that this FVM occupies
                ASSERT_EQ(alt_u32(0xFFFFFFFF), flash_x86_ptr[word_i]);
            }
        }
        else
        {
            // The rest of Active region should be unchanged
            ASSERT_EQ(full_orig_image[word_i], flash_x86_ptr[word_i]);
        }
    }

    // Reprovision ufm with rk hash, pch and bmc offset
    SYSTEM_MOCK::get()->provision_ufm_data(UFM_PFR_DATA_EXAMPLE_KEY_FILE);

    // UFM should be provisioned
    EXPECT_TRUE(is_ufm_provisioned());
    // SPI GEO should not be provisioned
    EXPECT_FALSE(is_ufm_spi_geo_provisioned());

    // Authenticate the updated image including validating the signature
    // Run PFR Main. Always run with the timeout
    ASSERT_DURATION_LE(GTEST_TIMEOUT_SHORT, pfr_main());

    EXPECT_EQ(SYSTEM_MOCK::get()->get_t_minus_1_bmc_only_counter(), alt_u32(0));
    EXPECT_EQ(SYSTEM_MOCK::get()->get_t_minus_1_pch_only_counter(), alt_u32(0));
    // Comes from calling pfr_main twice
    EXPECT_EQ(SYSTEM_MOCK::get()->get_t_minus_1_counter(), alt_u32(2));

    // Expect no corruption in update
    EXPECT_EQ(read_from_mailbox(MB_PANIC_EVENT_COUNT), alt_u32(0));
    EXPECT_EQ(read_from_mailbox(MB_LAST_PANIC_REASON), alt_u32(0));
    EXPECT_EQ(read_from_mailbox(MB_RECOVERY_COUNT), alt_u32(0));
    EXPECT_EQ(read_from_mailbox(MB_LAST_RECOVERY_REASON), alt_u32(0));
    EXPECT_EQ(read_from_mailbox(MB_MAJOR_ERROR_CODE), alt_u32(0));
    EXPECT_EQ(read_from_mailbox(MB_MINOR_ERROR_CODE), alt_u32(0));

    // Check PCH FW version (should not change in seamless update flow)
    EXPECT_EQ(read_from_mailbox(MB_PCH_PFM_ACTIVE_MAJOR_VER), alt_u32(PCH_ACTIVE_PFM_MAJOR_VER));
    EXPECT_EQ(read_from_mailbox(MB_PCH_PFM_ACTIVE_MINOR_VER), alt_u32(PCH_ACTIVE_PFM_MINOR_VER));
    EXPECT_EQ(read_from_mailbox(MB_PCH_PFM_RECOVERY_MAJOR_VER), alt_u32(PCH_RECOVERY_PFM_MAJOR_VER));
    EXPECT_EQ(read_from_mailbox(MB_PCH_PFM_RECOVERY_MINOR_VER), alt_u32(PCH_RECOVERY_PFM_MINOR_VER));

    // Check BMC FW version
    EXPECT_EQ(read_from_mailbox(MB_BMC_PFM_ACTIVE_MAJOR_VER), alt_u32(BMC_ACTIVE_PFM_MAJOR_VER));
    EXPECT_EQ(read_from_mailbox(MB_BMC_PFM_ACTIVE_MINOR_VER), alt_u32(BMC_ACTIVE_PFM_MINOR_VER));
    EXPECT_EQ(read_from_mailbox(MB_BMC_PFM_RECOVERY_MAJOR_VER), alt_u32(BMC_RECOVERY_PFM_MAJOR_VER));
    EXPECT_EQ(read_from_mailbox(MB_BMC_PFM_RECOVERY_MINOR_VER), alt_u32(BMC_RECOVERY_PFM_MINOR_VER));

    /*
     * Clean ups
     */
    delete[] full_orig_image;
    delete[] full_new_image;
    delete[] new_signed_fvm;
}

/**
 * @brief This test exercises the out-of-band happy path of spi-geo Seamless update flow, with BIOS FV capsule
 * and UPDATE_DYNAMIC bit set in update intent.
 */
TEST_F(SpiGeoSeamlessUpdateFlowTest, test_spi_geo_oob_seamless_update_static_plus_dynamic_then_provision)
{
    /*
     * Test Preparation
     */
    // Load the Seamless FV capsule
    SYSTEM_MOCK::get()->load_to_flash(
            SPI_FLASH_BMC,
            PCH_FW_UPDATE_SIGNED_CAPSULE_PCH_SEAMLESS_BIOS_FILE,
            PCH_FW_UPDATE_SIGNED_CAPSULE_PCH_SEAMLESS_BIOS_FILE_SIZE,
            get_ufm_pfr_data()->bmc_staging_region + BMC_STAGING_REGION_PCH_UPDATE_CAPSULE_OFFSET
    );

    // Load the entire images locally for comparison purpose
    alt_u32 *full_orig_image = new alt_u32[FULL_PFR_IMAGE_PCH_FILE_SIZE/4];
    SYSTEM_MOCK::get()->init_x86_mem_from_file(FULL_PFR_IMAGE_PCH_FILE, full_orig_image);

    alt_u32 *full_new_image = new alt_u32[FULL_PFR_IMAGE_PCH_V03P12_FILE_SIZE/4];
    SYSTEM_MOCK::get()->init_x86_mem_from_file(FULL_PFR_IMAGE_PCH_V03P12_FILE, full_new_image);

    alt_u32 *new_signed_fvm = new alt_u32[PCH_FW_UPDATE_SIGNED_FVM_PCH_BIOS_FILE_SIZE/4];
    SYSTEM_MOCK::get()->init_x86_mem_from_file(PCH_FW_UPDATE_SIGNED_FVM_PCH_BIOS_FILE, new_signed_fvm);

    // UFM should not be provisioned
    EXPECT_FALSE(is_ufm_provisioned());
    // SPI GEO should be provisioned
    EXPECT_TRUE(is_ufm_spi_geo_provisioned());
    /*
     * Flow preparation
     */
    ut_prep_nios_gpi_signals();


    // Exit after 50 iterations in the T0 loop
    SYSTEM_MOCK::get()->insert_code_block(SYSTEM_MOCK::CODE_BLOCK_TYPES::T0_OPERATIONS_END_AFTER_50_ITERS);

    // Send the Seamless update intent
    write_to_mailbox(MB_BMC_UPDATE_INTENT_PART2, MB_UPDATE_INTENT_PCH_FV_SEAMLESS_MASK);
    write_to_mailbox(MB_BMC_UPDATE_INTENT_PART1, MB_UPDATE_INTENT_UPDATE_DYNAMIC_MASK);

    /*
     * Execute the flow. Active update flow will be triggered.
     */
    // Run PFR Main. Always run with the timeout
    ASSERT_DURATION_LE(GTEST_TIMEOUT_SHORT, pfr_main());

    /*
     * Verify results
     */
    switch_spi_flash(SPI_FLASH_PCH);
    alt_u32* flash_x86_ptr = SYSTEM_MOCK::get()->get_x86_ptr_to_spi_flash(SPI_FLASH_PCH);

    // Seamless update should not require T-1 entry
    EXPECT_EQ(SYSTEM_MOCK::get()->get_t_minus_1_bmc_only_counter(), alt_u32(0));
    EXPECT_EQ(SYSTEM_MOCK::get()->get_t_minus_1_pch_only_counter(), alt_u32(0));
    EXPECT_EQ(SYSTEM_MOCK::get()->get_t_minus_1_counter(), alt_u32(1));

    EXPECT_EQ(read_from_mailbox(MB_PANIC_EVENT_COUNT), alt_u32(0));
    EXPECT_EQ(read_from_mailbox(MB_LAST_PANIC_REASON), alt_u32(0));
    EXPECT_EQ(read_from_mailbox(MB_RECOVERY_COUNT), alt_u32(0));
    EXPECT_EQ(read_from_mailbox(MB_LAST_RECOVERY_REASON), alt_u32(0));
    EXPECT_EQ(read_from_mailbox(MB_MAJOR_ERROR_CODE), alt_u32(0));
    EXPECT_EQ(read_from_mailbox(MB_MINOR_ERROR_CODE), alt_u32(0));

    // Check PCH FW version (should not change in seamless update flow)
    EXPECT_EQ(read_from_mailbox(MB_PCH_PFM_ACTIVE_MAJOR_VER), alt_u32(PCH_ACTIVE_PFM_MAJOR_VER));
    EXPECT_EQ(read_from_mailbox(MB_PCH_PFM_ACTIVE_MINOR_VER), alt_u32(PCH_ACTIVE_PFM_MINOR_VER));
    EXPECT_EQ(read_from_mailbox(MB_PCH_PFM_RECOVERY_MAJOR_VER), alt_u32(PCH_RECOVERY_PFM_MAJOR_VER));
    EXPECT_EQ(read_from_mailbox(MB_PCH_PFM_RECOVERY_MINOR_VER), alt_u32(PCH_RECOVERY_PFM_MINOR_VER));

    // Check BMC FW version
    EXPECT_EQ(read_from_mailbox(MB_BMC_PFM_ACTIVE_MAJOR_VER), alt_u32(BMC_ACTIVE_PFM_MAJOR_VER));
    EXPECT_EQ(read_from_mailbox(MB_BMC_PFM_ACTIVE_MINOR_VER), alt_u32(BMC_ACTIVE_PFM_MINOR_VER));
    EXPECT_EQ(read_from_mailbox(MB_BMC_PFM_RECOVERY_MAJOR_VER), alt_u32(BMC_RECOVERY_PFM_MAJOR_VER));
    EXPECT_EQ(read_from_mailbox(MB_BMC_PFM_RECOVERY_MINOR_VER), alt_u32(BMC_RECOVERY_PFM_MINOR_VER));

    // Make sure only data in BIOS FV regions are changed
    for (alt_u32 region_i = 0; region_i < PCH_NUM_STATIC_SPI_REGIONS; region_i++)
    {
        // If this region is a BIOS FV region, compare against full_new_image
        alt_u32* gold_image = full_orig_image;
        for (alt_u32 addr_i = 0; addr_i < PCH_NUM_BIOS_FV_REGIONS; addr_i++)
        {
            if (testdata_pch_static_regions_start_addr[region_i] == m_bios_fv_regions_start_addr[addr_i])
            {
                gold_image = full_new_image;
            }
        }

        // Iterate through all words in this SPI region and make sure they match the expected data
        for (alt_u32 word_i = testdata_pch_static_regions_start_addr[region_i] >> 2;
                word_i < testdata_pch_static_regions_end_addr[region_i] >> 2; word_i++)
        {
            ASSERT_EQ(gold_image[word_i], flash_x86_ptr[word_i]);
        }
    }
    for (alt_u32 region_i = 0; region_i < PCH_NUM_DYNAMIC_SPI_REGIONS; region_i++)
    {
        // If this region is a BIOS FV region, compare against full_new_image
        alt_u32* gold_image = full_orig_image;
        for (alt_u32 addr_i = 0; addr_i < PCH_NUM_BIOS_FV_REGIONS; addr_i++)
        {
            if (testdata_pch_dynamic_regions_start_addr[region_i] == m_bios_fv_regions_start_addr[addr_i])
            {
                gold_image = full_new_image;
            }
        }

        // Iterate through all words in this SPI region and make sure they match the expected data
        for (alt_u32 word_i = testdata_pch_dynamic_regions_start_addr[region_i] >> 2;
                word_i < testdata_pch_dynamic_regions_end_addr[region_i] >> 2; word_i++)
        {
            ASSERT_EQ(gold_image[word_i], flash_x86_ptr[word_i]);
        }
    }

    // Verify the active PFR region
    // PFM should not change. Only one FVM will be updated.
    alt_u32 pch_active_pfm_start = get_ufm_pfr_data()->pch_active_pfm;
    alt_u32 pch_active_pfm_end = pch_active_pfm_start + SIGNED_PFM_MAX_SIZE;

    // Info on updated FVM
    alt_u16 target_fv_type = ((FVM*) incr_alt_u32_ptr(new_signed_fvm, SIGNATURE_SIZE))->fv_type;
    alt_u32 target_fvm_start_addr = get_active_fvm_addr(target_fv_type);
    alt_u32 target_fvm_end_addr = target_fvm_start_addr + (4096 - PCH_FW_UPDATE_SIGNED_FVM_PCH_BIOS_FILE_SIZE % 4096);

    for (alt_u32 word_i = pch_active_pfm_start >> 2; word_i < pch_active_pfm_end >> 2; word_i++)
    {
        if (((target_fvm_start_addr >> 2) <= word_i) && (word_i < (target_fvm_end_addr >> 2)))
        {
            // Check against the expected new FVM, if the address falls within this FVM in active PFR region
            alt_u32 new_signed_fvm_word_i = word_i - (target_fvm_start_addr >> 2);
            if (new_signed_fvm_word_i < (PCH_FW_UPDATE_SIGNED_FVM_PCH_BIOS_FILE_SIZE >> 2))
            {
                ASSERT_EQ(new_signed_fvm[new_signed_fvm_word_i], flash_x86_ptr[word_i]);
            }
            else
            {
                // Data would be blank (0xFFFFFFFF) for the remaining of the page that this FVM occupies
                ASSERT_EQ(alt_u32(0xFFFFFFFF), flash_x86_ptr[word_i]);
            }
        }
        else
        {
            // The rest of Active region should be unchanged
            ASSERT_EQ(full_orig_image[word_i], flash_x86_ptr[word_i]);
        }
    }

    // Reprovision ufm with rk hash, pch and bmc offset
    SYSTEM_MOCK::get()->provision_ufm_data(UFM_PFR_DATA_EXAMPLE_KEY_FILE);

    // UFM should be provisioned
    EXPECT_TRUE(is_ufm_provisioned());
    // SPI GEO should not be provisioned
    EXPECT_FALSE(is_ufm_spi_geo_provisioned());

    // Authenticate the updated image including validating the signature
    // Run PFR Main. Always run with the timeout
    ASSERT_DURATION_LE(GTEST_TIMEOUT_SHORT, pfr_main());

    EXPECT_EQ(SYSTEM_MOCK::get()->get_t_minus_1_bmc_only_counter(), alt_u32(0));
    EXPECT_EQ(SYSTEM_MOCK::get()->get_t_minus_1_pch_only_counter(), alt_u32(0));
    // Comes from calling pfr_main twice
    EXPECT_EQ(SYSTEM_MOCK::get()->get_t_minus_1_counter(), alt_u32(2));

    // Expect no corruption in update
    EXPECT_EQ(read_from_mailbox(MB_PANIC_EVENT_COUNT), alt_u32(0));
    EXPECT_EQ(read_from_mailbox(MB_LAST_PANIC_REASON), alt_u32(0));
    EXPECT_EQ(read_from_mailbox(MB_RECOVERY_COUNT), alt_u32(0));
    EXPECT_EQ(read_from_mailbox(MB_LAST_RECOVERY_REASON), alt_u32(0));
    EXPECT_EQ(read_from_mailbox(MB_MAJOR_ERROR_CODE), alt_u32(0));
    EXPECT_EQ(read_from_mailbox(MB_MINOR_ERROR_CODE), alt_u32(0));

    // Check PCH FW version (should not change in seamless update flow)
    EXPECT_EQ(read_from_mailbox(MB_PCH_PFM_ACTIVE_MAJOR_VER), alt_u32(PCH_ACTIVE_PFM_MAJOR_VER));
    EXPECT_EQ(read_from_mailbox(MB_PCH_PFM_ACTIVE_MINOR_VER), alt_u32(PCH_ACTIVE_PFM_MINOR_VER));
    EXPECT_EQ(read_from_mailbox(MB_PCH_PFM_RECOVERY_MAJOR_VER), alt_u32(PCH_RECOVERY_PFM_MAJOR_VER));
    EXPECT_EQ(read_from_mailbox(MB_PCH_PFM_RECOVERY_MINOR_VER), alt_u32(PCH_RECOVERY_PFM_MINOR_VER));

    // Check BMC FW version
    EXPECT_EQ(read_from_mailbox(MB_BMC_PFM_ACTIVE_MAJOR_VER), alt_u32(BMC_ACTIVE_PFM_MAJOR_VER));
    EXPECT_EQ(read_from_mailbox(MB_BMC_PFM_ACTIVE_MINOR_VER), alt_u32(BMC_ACTIVE_PFM_MINOR_VER));
    EXPECT_EQ(read_from_mailbox(MB_BMC_PFM_RECOVERY_MAJOR_VER), alt_u32(BMC_RECOVERY_PFM_MAJOR_VER));
    EXPECT_EQ(read_from_mailbox(MB_BMC_PFM_RECOVERY_MINOR_VER), alt_u32(BMC_RECOVERY_PFM_MINOR_VER));
    /*
     * Clean ups
     */
    delete[] full_orig_image;
    delete[] full_new_image;
    delete[] new_signed_fvm;
}

/**
 * @brief This test exercises the out-of-band path of spi-geo Seamless update flow, with BIOS FV capsule.
 * However, BMC did not clear the reset bit
 */
TEST_F(SpiGeoSeamlessUpdateFlowTest, test_seamless_update_oob_bmc_reset)
{
    /*
     * Test Preparation
     */
    // Load the Seamless FV capsule
    SYSTEM_MOCK::get()->load_to_flash(
            SPI_FLASH_BMC,
            PCH_FW_UPDATE_SIGNED_CAPSULE_PCH_SEAMLESS_BIOS_FILE,
            PCH_FW_UPDATE_SIGNED_CAPSULE_PCH_SEAMLESS_BIOS_FILE_SIZE,
            get_ufm_pfr_data()->bmc_staging_region + BMC_STAGING_REGION_PCH_UPDATE_CAPSULE_OFFSET
    );

    // Load the entire images locally for comparison purpose
    alt_u32 *full_orig_image = new alt_u32[FULL_PFR_IMAGE_PCH_FILE_SIZE/4];
    SYSTEM_MOCK::get()->init_x86_mem_from_file(FULL_PFR_IMAGE_PCH_FILE, full_orig_image);

    alt_u32 *full_new_image = new alt_u32[FULL_PFR_IMAGE_PCH_V03P12_FILE_SIZE/4];
    SYSTEM_MOCK::get()->init_x86_mem_from_file(FULL_PFR_IMAGE_PCH_V03P12_FILE, full_new_image);

    alt_u32 *new_signed_fvm = new alt_u32[PCH_FW_UPDATE_SIGNED_FVM_PCH_BIOS_FILE_SIZE/4];
    SYSTEM_MOCK::get()->init_x86_mem_from_file(PCH_FW_UPDATE_SIGNED_FVM_PCH_BIOS_FILE, new_signed_fvm);

    // UFM should not be provisioned
    EXPECT_FALSE(is_ufm_provisioned());
    // SPI GEO should be provisioned
    EXPECT_TRUE(is_ufm_spi_geo_provisioned());
    /*
     * Flow preparation
     */
    ut_prep_nios_gpi_signals();

    // Exit after 50 iterations in the T0 loop
    SYSTEM_MOCK::get()->insert_code_block(SYSTEM_MOCK::CODE_BLOCK_TYPES::T0_OPERATIONS_END_AFTER_50_ITERS);

    // Send the Seamless update intent
    ut_send_in_update_intent_once_upon_entry_to_t0(MB_BMC_UPDATE_INTENT_PART2, MB_UPDATE_INTENT_BMC_SEAMLESS_RESET_MASK | MB_UPDATE_INTENT_PCH_FV_SEAMLESS_MASK);

    /*
     * Execute the flow. Active update flow will be triggered.
     */
    // Run PFR Main. Always run with the timeout
    ASSERT_DURATION_LE(GTEST_TIMEOUT_SHORT, pfr_main());

    /*
     * Verify results
     */
    switch_spi_flash(SPI_FLASH_PCH);
    alt_u32* flash_x86_ptr = SYSTEM_MOCK::get()->get_x86_ptr_to_spi_flash(SPI_FLASH_PCH);

    // Seamless update should not require T-1 entry
    // BMC should be reset
    EXPECT_EQ(SYSTEM_MOCK::get()->get_t_minus_1_bmc_only_counter(), alt_u32(2));
    EXPECT_EQ(SYSTEM_MOCK::get()->get_t_minus_1_pch_only_counter(), alt_u32(0));
    EXPECT_EQ(SYSTEM_MOCK::get()->get_t_minus_1_counter(), alt_u32(1));

    EXPECT_EQ(read_from_mailbox(MB_PANIC_EVENT_COUNT), alt_u32(1));
    EXPECT_EQ(read_from_mailbox(MB_LAST_PANIC_REASON), alt_u32(LAST_PANIC_BMC_RESET_DETECTED));
    EXPECT_EQ(read_from_mailbox(MB_RECOVERY_COUNT), alt_u32(0));
    EXPECT_EQ(read_from_mailbox(MB_LAST_RECOVERY_REASON), alt_u32(0));
    EXPECT_EQ(read_from_mailbox(MB_MAJOR_ERROR_CODE), alt_u32(0));
    EXPECT_EQ(read_from_mailbox(MB_MINOR_ERROR_CODE), alt_u32(0));

    // Check PCH FW version (should not change in seamless update flow)
    EXPECT_EQ(read_from_mailbox(MB_PCH_PFM_ACTIVE_MAJOR_VER), alt_u32(PCH_ACTIVE_PFM_MAJOR_VER));
    EXPECT_EQ(read_from_mailbox(MB_PCH_PFM_ACTIVE_MINOR_VER), alt_u32(PCH_ACTIVE_PFM_MINOR_VER));
    EXPECT_EQ(read_from_mailbox(MB_PCH_PFM_RECOVERY_MAJOR_VER), alt_u32(PCH_RECOVERY_PFM_MAJOR_VER));
    EXPECT_EQ(read_from_mailbox(MB_PCH_PFM_RECOVERY_MINOR_VER), alt_u32(PCH_RECOVERY_PFM_MINOR_VER));

    // Check BMC FW version
    EXPECT_EQ(read_from_mailbox(MB_BMC_PFM_ACTIVE_MAJOR_VER), alt_u32(BMC_ACTIVE_PFM_MAJOR_VER));
    EXPECT_EQ(read_from_mailbox(MB_BMC_PFM_ACTIVE_MINOR_VER), alt_u32(BMC_ACTIVE_PFM_MINOR_VER));
    EXPECT_EQ(read_from_mailbox(MB_BMC_PFM_RECOVERY_MAJOR_VER), alt_u32(BMC_RECOVERY_PFM_MAJOR_VER));
    EXPECT_EQ(read_from_mailbox(MB_BMC_PFM_RECOVERY_MINOR_VER), alt_u32(BMC_RECOVERY_PFM_MINOR_VER));

    // Make sure only data in BIOS FV regions are changed
    for (alt_u32 region_i = 0; region_i < PCH_NUM_STATIC_SPI_REGIONS; region_i++)
    {
        // If this region is a BIOS FV region, compare against full_new_image
        alt_u32* gold_image = full_orig_image;
        for (alt_u32 addr_i = 0; addr_i < PCH_NUM_BIOS_FV_REGIONS; addr_i++)
        {
            if (testdata_pch_static_regions_start_addr[region_i] == m_bios_fv_regions_start_addr[addr_i])
            {
                gold_image = full_new_image;
            }
        }

        // Iterate through all words in this SPI region and make sure they match the expected data
        for (alt_u32 word_i = testdata_pch_static_regions_start_addr[region_i] >> 2;
                word_i < testdata_pch_static_regions_end_addr[region_i] >> 2; word_i++)
        {
            ASSERT_EQ(gold_image[word_i], flash_x86_ptr[word_i]);
        }
    }
    for (alt_u32 region_i = 0; region_i < PCH_NUM_DYNAMIC_SPI_REGIONS; region_i++)
    {
        // By default, the dynamic SPI regions are not updated in Seamless update.
        alt_u32* gold_image = full_orig_image;

        // Iterate through all words in this SPI region and make sure they match the expected data
        for (alt_u32 word_i = testdata_pch_dynamic_regions_start_addr[region_i] >> 2;
                word_i < testdata_pch_dynamic_regions_end_addr[region_i] >> 2; word_i++)
        {
            ASSERT_EQ(gold_image[word_i], flash_x86_ptr[word_i]);
        }
    }

    // Verify the active PFR region
    // PFM should not change. Only one FVM will be updated.
    alt_u32 pch_active_pfm_start = get_ufm_pfr_data()->pch_active_pfm;
    alt_u32 pch_active_pfm_end = pch_active_pfm_start + SIGNED_PFM_MAX_SIZE;

    // Info on updated FVM
    alt_u16 target_fv_type = ((FVM*) incr_alt_u32_ptr(new_signed_fvm, SIGNATURE_SIZE))->fv_type;
    alt_u32 target_fvm_start_addr = get_active_fvm_addr(target_fv_type);
    alt_u32 target_fvm_end_addr = target_fvm_start_addr + (4096 - PCH_FW_UPDATE_SIGNED_FVM_PCH_BIOS_FILE_SIZE % 4096);

    for (alt_u32 word_i = pch_active_pfm_start >> 2; word_i < pch_active_pfm_end >> 2; word_i++)
    {
        if (((target_fvm_start_addr >> 2) <= word_i) && (word_i < (target_fvm_end_addr >> 2)))
        {
            // Check against the expected new FVM, if the address falls within this FVM in active PFR region
            alt_u32 new_signed_fvm_word_i = word_i - (target_fvm_start_addr >> 2);
            if (new_signed_fvm_word_i < (PCH_FW_UPDATE_SIGNED_FVM_PCH_BIOS_FILE_SIZE >> 2))
            {
                ASSERT_EQ(new_signed_fvm[new_signed_fvm_word_i], flash_x86_ptr[word_i]);
            }
            else
            {
                // Data would be blank (0xFFFFFFFF) for the remaining of the page that this FVM occupies
                ASSERT_EQ(alt_u32(0xFFFFFFFF), flash_x86_ptr[word_i]);
            }
        }
        else
        {
            // The rest of Active region should be unchanged
            ASSERT_EQ(full_orig_image[word_i], flash_x86_ptr[word_i]);
        }
    }

    /*
     * Clean ups
     */
    delete[] full_orig_image;
    delete[] full_new_image;
    delete[] new_signed_fvm;
}
