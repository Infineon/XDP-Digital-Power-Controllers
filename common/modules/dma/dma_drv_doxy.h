/* ============================================================================
** Copyright (C) 2014 Infineon Technologies AG
** All rights reserved.
** ============================================================================
**
** ============================================================================
** This document contains proprietary information of Infineon Technologies AG.
** Passing on and copying of this document, and communication of its contents
** is not permitted without Infineon's prior written authorisation.
** ============================================================================
*/
/**
 * @file    dma_drv_doxy.h
 * @brief   Doxygen overview of DMA FW drivers.
 */

#ifndef DMA_DRV_DOXY_H
#define DMA_DRV_DOXY_H

/** @page pgShasta_dma_drv   DMA Driver module

@section sctShasta_DMA_drv_Intro   DMA Driver
The DMA driver firmware module provides direct memory access.
See
- dma_drv.h for details on the API
- hal_dma.h API for the channel descriptor
- CPUS ITS for details on the HW Interface e.g the DMA Shell
    <br/>https://sec-ishare.infineon.com/sites/shasta/Shared%20Documents/03_System/CPUS.pdf
    Chapter 7 and 8
- ARM's original DMA HW manual
    <br/><a href="../../../../../doc/arm/DDI0417A_udmac_pl230_r0p0_trm.pdf">PrimeCell uDMA Controller (PL230)</a>
<br/>
@section sctShasta_dma_drv_example   Example application
Below is a code example how to use the driver.
<br/>
-# Channel 0 for an automatic mem2mem copy.
-# Channel 6 to make a dtimer controlled mem2mem copy.
-# Channel 10 to perform a I2C slave to master transaction (TX).
-# Channel 11 to perform a I2C master to slave transaction (RX).

\code
//dtimer configuration variables
STATIC DTIMER_CONFIG_t dtimer_cfg[4];
//Interrupt counter for the dtimer
STATIC uint32_t x[4];
//dtimer driver handlers
STATIC DRIVER_DTIMER *drv_dtimer;

STATIC void DTIMER_ISR1(void)
{
    drv_dtimer->ClearIRQ(DTIMER1);
    x[1]++;
    if (x[1] > 20)
    {
        drv_dtimer->Uninitialize(DTIMER1);
    }
}

STATIC DRIVER_DMA *dma_cpy;
STATIC DMA_CONFIGURATION dma_cfg[16];
#define DMA_SIZE (10)
#define N_ROWS (2)
STATIC uint32_t src_buf[N_ROWS][DMA_SIZE];
STATIC uint32_t dest_buf[N_ROWS][DMA_SIZE];

//Parameter for PMBUS Interface Transaction tests
#define RX_SIZE (32)
#define TX_SIZE (4)
STATIC uint32_t rx_buf[32];
STATIC const uint32_t rx_result[] =
               { 0x2c, 0x00, 0x09, 0x70, 0x00, 0x00, 0x00, 0x00,
                 0x30, 0x00, 0x09, 0x70, 0xFF, 0xFF, 0xFF, 0xFF,
                 0x2c, 0x00, 0x09, 0x70, 0x2c, 0x00, 0x09, 0x70,
                 0x30, 0x00, 0x09, 0x70, 0x30, 0x00, 0x09, 0x70
               };

STATIC void DTIMER_ISR0(void)
{
    drv_dtimer->ClearIRQ(DTIMER0);
    x[0]++;
    if (x[0] > 20)
    {
        drv_dtimer->Uninitialize(DTIMER0);
    }
}

//PMBUS RX Transactions
STATIC void DMA_ISR11(void)
{
    DMA_teardown(11);
    DMA_disable_hw();

    uint32_t dma_err = 0;
    // Check if the data were transfered correctly
    for (uint32_t i = 0; i < RX_SIZE; i++) {
        dma_err += (rx_buf[i] - rx_result[i]);
        RGU_SPARE_FF__SET((uint32_t)i) ;
        SCU_SPARE_FF__SET((uint32_t)rx_buf[i]) ;
        CGU_SPARE_FF__SET((uint32_t)dma_err) ;
    }

    RGU_SPARE_FF__SET((uint32_t)(RX_SIZE+1)) ;
    SCU_SPARE_FF__SET((uint32_t)dma_err) ;

}
//PMBUS TX Transactions
STATIC void DMA_ISR10(void)
{
    DMA_teardown(10);
    DMA_disable_hw();
    for (uint32_t i = 0; i < TX_SIZE; i++) {
        RGU_SPARE_FF__SET((uint32_t)i) ;
        SCU_SPARE_FF__SET((uint32_t)src_buf[0][i]) ;
    }
}
STATIC void DMA_ISR7(void)
{
    DTIMER_Uninitialize(DTIMER1);
    DMA_teardown(7);
    DMA_disable_hw();
}

STATIC void DMA_ISR6(void)
{
    DTIMER_Uninitialize(DTIMER0);
    DMA_teardown(6);
    DMA_disable_hw();
}

STATIC void DMA_ISR0(void)
{
    DMA_teardown(0);
    DMA_disable_hw();
}

STATIC void DMA_ISR1(void)
{
    DMA_teardown(1);
    DMA_disable_hw();
}

void test_execute(void)
{
    // Initialize pmbus to perform DMA tests with PMBUS module

    // Since the operation and maintenance is the underlying "rescue module" this one comes first
    // in a logical order.

    // get a handler for dtimer driver
    drv_dtimer = &Driver_DTIMER;

    // get a handler for DMA driver
    dma_cpy = &Driver_DMA;

    // power up the DMA.
    //This must be done before configuring the DMA
    dma_cpy->enable_hw();

    // Fill the source buffer with test data
    for (uint32_t i = 0; i < (DMA_SIZE); i++) {
        src_buf[0][i] = 1 * (i + 1);
        src_buf[1][i] = 16 * (i + 1);
    }

    memset(dest_buf, 0, sizeof(dest_buf));

    uint32_t ch = 0;
    DMA_CONFIGURATION * dma_ch_config = &dma_cfg[ch];

    // Setup DMA channel 0 for a simple mem2mem transfer
    dma_ch_config->cycle_control = AUTO_REQUEST;
    dma_ch_config->data_width = DATA_WIDTH_WORD;
    dma_ch_config->dest = &dest_buf[0][0];
    dma_ch_config->src = &src_buf[0][0];
    dma_ch_config->dma_ChIRQ = DMA_ISR0;
    dma_ch_config->is_dest_port_inc = 1;
    dma_ch_config->is_src_port_inc = 1;
    dma_ch_config->r_pow = 0;
    dma_ch_config->size_m1 = (sizeof(dest_buf)/N_ROWS) - 1;

    dma_cpy->setup(ch, dma_cfg[ch]);
    dma_cpy->start(ch);  //starts the DMA cycle for channel 0

    ch = 1;
    dma_ch_config = &dma_cfg[ch];

    // Setup DMA channel 1 for a simple mem2mem transfer
    dma_ch_config->cycle_control = AUTO_REQUEST;
    dma_ch_config->data_width = DATA_WIDTH_WORD;
    dma_ch_config->dest = &dest_buf[1][0];
    dma_ch_config->src = &src_buf[1][0];
    dma_ch_config->dma_ChIRQ = DMA_ISR1;
    dma_ch_config->is_dest_port_inc = 1;
    dma_ch_config->is_src_port_inc = 1;
    dma_ch_config->r_pow = 0;
    dma_ch_config->size_m1 = (sizeof(dest_buf)/N_ROWS) - 1;

    dma_cpy->setup(ch, dma_cfg[ch]);
    dma_cpy->start(ch);  //starts the DMA cycle for channel 0

    // Requests must come after start
    // After starting DMA configuration cannot be changed until the DMA is done
    dma_cpy->req_sw(0); // creates a sw request to initiate the transfer
    dma_cpy->req_sw(1); // creates a sw request to initiate the transfer

    // Wait until dma transfer is finished
    while (DMA_is_active()){}

    // Check if transfer was correct
    uint32_t dma_err = 0;

    // Check if the data were transfered correctly
    for (uint32_t i = 0; i < (DMA_SIZE); i++) {
        if ((src_buf[0][i] != dest_buf[0][i]) ||
                (src_buf[1][i] != dest_buf[1][i])   )
        {
            dma_err = 1;
            break;
        }
    }

    // Test 1 with error
    if (dma_err == 1)
    {
        SCU_SPARE_FF__SET((uint32_t)0x11UL) ;
    }
    else
    {
        SCU_SPARE_FF__SET((uint32_t)0x01UL) ;
    }

    //*************************** Checking DMA channel 6 and 7 DTIMER0 and DTIMER1 **************
    // power up
    dma_cpy->enable_hw();

    for (uint32_t i = 0; i < (DMA_SIZE); i++) {
        src_buf[0][i] = 4 * (i + 1);
        src_buf[1][i] = 64 * (i + 1);
    }

    memset(dest_buf, 0, sizeof(dest_buf));

    ch = 6;
    dma_ch_config = &dma_cfg[ch];

    // Setup DMA channel 6 which is connected to DTIMER0
    // use simple mem2mem transfer
    dma_ch_config->cycle_control = BASIC;
    dma_ch_config->data_width = DATA_WIDTH_WORD;
    dma_ch_config->dest = &dest_buf[0][0];
    dma_ch_config->src = &src_buf[0][0];
    dma_ch_config->dma_ChIRQ = DMA_ISR6;
    dma_ch_config->is_dest_port_inc = 1;
    dma_ch_config->is_src_port_inc = 1;
    dma_ch_config->r_pow = 0;
    dma_ch_config->size_m1 = (sizeof(dest_buf)/N_ROWS) - 1;

    dma_cpy->setup(ch, dma_cfg[ch]);
    dma_cpy->start(ch);


    ch = 7;
    dma_ch_config = &dma_cfg[ch];

    // Setup DMA channel 7 which is connected to DTIMER1
    // use simple mem2mem transfer
    dma_ch_config->cycle_control = BASIC;
    dma_ch_config->data_width = DATA_WIDTH_WORD;
    dma_ch_config->dest = &dest_buf[1][0];
    dma_ch_config->src = &src_buf[1][0];
    dma_ch_config->dma_ChIRQ = DMA_ISR7;
    dma_ch_config->is_dest_port_inc = 1;
    dma_ch_config->is_src_port_inc = 1;
    dma_ch_config->r_pow = 0;
    dma_ch_config->size_m1 = (sizeof(dest_buf)/N_ROWS) - 1;


    dma_cpy->setup(ch, dma_cfg[ch]);
    dma_cpy->start(ch);

    // Configure DTIMER 0 to create a request every 200 clock cycles
    memset(&dtimer_cfg[0], 0, sizeof(dtimer_cfg[0]));
    dtimer_cfg[0].mode   = PERIODIC_INT;
    dtimer_cfg[0].load   = 200;
    dtimer_cfg[0].bgload = 200;
    dtimer_cfg[0].dtimer_ISR = DTIMER_ISR0;
    x[0] = 0;
    drv_dtimer->Initialize(DTIMER0, dtimer_cfg[0]);

    // Configure DTIMER 1 to create a request every 400 clock cycles
    memset(&dtimer_cfg[1], 0, sizeof(dtimer_cfg[1]));
    dtimer_cfg[1].mode   = PERIODIC_INT;
    dtimer_cfg[1].load   = 400;
    dtimer_cfg[1].bgload = 400;
    dtimer_cfg[1].dtimer_ISR = DTIMER_ISR1;
    x[1] = 0;
    drv_dtimer->Initialize(DTIMER1, dtimer_cfg[1]);

    // Wait until dma transfer is finished
    while (DMA_is_active()){}

    // Check if transfer was correct
    dma_err = 0;

    // Check if the data were transfered correctly
    for (uint32_t i = 0; i < (DMA_SIZE); i++) {
        if ((src_buf[0][i] != dest_buf[0][i]) ||
                (src_buf[1][i] != dest_buf[1][i])   )
        {
            dma_err = 1;
            break;
        }
    }
    // Test 3 with error
    if (dma_err == 1)
    {
        SCU_SPARE_FF__SET((uint32_t)0x12UL) ;
    }
    else
    {
        SCU_SPARE_FF__SET((uint32_t)0x02UL) ;
    }

    //*************************** Checking DMA channel 10 and 11 PMBUS TX and RX *************
    // power up
    dma_cpy->enable_hw();

    for (uint32_t i = 0; i < (DMA_SIZE); i++) {
        src_buf[0][i] = 4 * (i + 1);
        src_buf[1][i] = 64 * (i + 1);
    }

    memset(dest_buf, 0, sizeof(dest_buf));

    ch = 10;
    dma_ch_config = &dma_cfg[ch];

    // PMBUS TX: sending 4 byte data to the master
    dma_ch_config->cycle_control = BASIC;
    dma_ch_config->data_width = DATA_WIDTH_WORD;
    dma_ch_config->dest = __PMBUS_DATA_LUT__ADDRESS(pmbus_data_lut_0);
    dma_ch_config->src = &src_buf[0][0];
    dma_ch_config->dma_ChIRQ = DMA_ISR10;
    dma_ch_config->is_dest_port_inc = 0;
    dma_ch_config->is_src_port_inc = 1;
    dma_ch_config->r_pow = 0;
    dma_ch_config->size_m1 = (TX_SIZE*4) - 1; // only 4 transactions are performed

    dma_cpy->setup(ch, dma_cfg[ch]);
    dma_cpy->start(ch);


    ch = 11;
    dma_ch_config = &dma_cfg[ch];

    //  PMBUS RX: receiving 32 byte data from the master
    dma_ch_config->cycle_control = BASIC;
    dma_ch_config->data_width = DATA_WIDTH_WORD;
    dma_ch_config->dest = &rx_buf[0];
    dma_ch_config->src = __PMBUS_RXDATA__ADDRESS();
    dma_ch_config->dma_ChIRQ = DMA_ISR11;
    dma_ch_config->is_dest_port_inc = 1;
    dma_ch_config->is_src_port_inc = 0;
    dma_ch_config->r_pow = 0;
    dma_ch_config->size_m1 =(RX_SIZE*4) - 1; // only 32 transactions are performed for 4 frames


    dma_cpy->setup(ch, dma_cfg[ch]);
    dma_cpy->start(ch);

    // for PMBUS the data must be checked in the DMA Interrupt which is issued
    // when the transfer is complete

}

\endcode
*/

#endif /* DMA_DRV_DOXY_H */
