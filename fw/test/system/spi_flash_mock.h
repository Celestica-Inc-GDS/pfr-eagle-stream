#ifndef SYSTEM_SPI_FLASH_MOCK_H
#define SYSTEM_SPI_FLASH_MOCK_H

// Standard headers
#include <memory>
#include <string>
#include <unordered_map>
#include <fstream>

// Mock headers
#include "alt_types_mock.h"
#include "memory_mock.h"
#include "unordered_map_memory_mock.h"
#include "nios_gpio_mock.h"

// PFR system
#include "pfr_sys.h"
#include "spi_common.h"

class SPI_FLASH_MOCK
{
public:
    static SPI_FLASH_MOCK* get();

    void reset();
    void reset(SPI_FLASH_TYPE_ENUM spi_flash_type);

    void load(SPI_FLASH_TYPE_ENUM spi_flash_type, const std::string& file_path,
            int file_size, int load_offset);

    // Expose the x86 addresses of these flash memories
    //   Return the flash pointer based on the BMC/PCH mux
    alt_u32* get_spi_flash_ptr()
    {
        if (m_nios_gpio_mock_inst->check_bit(U_GPO_1_ADDR, GPO_1_SPI_MASTER_BMC_PCHN))
        {
            return m_bmc_flash_mem;
        }
        return m_pch_flash_mem;
    }

    // Expose the x86 addresses of these flash memories
    //   Return the pointer to the selected flash
    alt_u32* get_spi_flash_ptr(SPI_FLASH_TYPE_ENUM spi_flash_type)
    {
        if (spi_flash_type == SPI_FLASH_BMC)
        {
            return m_bmc_flash_mem;
        }
        return m_pch_flash_mem;
    }

private:
    // Singleton inst
    static SPI_FLASH_MOCK* s_inst;

    SPI_FLASH_MOCK();
    ~SPI_FLASH_MOCK();

    // Memory area for the flash memory
    alt_u32 *m_pch_flash_mem = nullptr;
    alt_u32 *m_bmc_flash_mem = nullptr;

    // Instance of NIOS GPIO mock
    NIOS_GPIO_MOCK* m_nios_gpio_mock_inst = NIOS_GPIO_MOCK::get();
};

#endif /* SYSTEM_SPI_FLASH_MOCK_H_ */
