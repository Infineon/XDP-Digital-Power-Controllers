var i2c__drv_8h =
[
    [ "I2C_RX_BUF_SIZE", "i2c__drv_8h.html#aca4305445cbc530f64138e913f2c5894", null ],
    [ "I2C_STATUS_MASK", "i2c__drv_8h.html#ada26c194db4a2e5b05470ecd565e2180", null ],
    [ "I2C_TX_BUF_SIZE", "i2c__drv_8h.html#a3fbc082262663b04c692d6d5f07ba1f2", null ],
    [ "I2C_STATUS_TYPE", "i2c__drv_8h.html#a4e84981f199e1c8735f64d20994aa620", null ],
    [ "I2C_STATUS_TYPE_e", "i2c__drv_8h.html#a62180b2af27996e33c7b94c80d897dc0", [
      [ "I2C_BUS_ERROR", "i2c__drv_8h.html#a62180b2af27996e33c7b94c80d897dc0a2e0f28fb9272ce7c8200b7d3a4831414", null ],
      [ "I2C_START", "i2c__drv_8h.html#a62180b2af27996e33c7b94c80d897dc0a3a5b6dff25cc014b099563334e716da5", null ],
      [ "I2C_REP_START", "i2c__drv_8h.html#a62180b2af27996e33c7b94c80d897dc0a86aa617923c740220be977cad909c992", null ],
      [ "I2C_MT_SLA_ACK", "i2c__drv_8h.html#a62180b2af27996e33c7b94c80d897dc0aa49bb9214d438b7e87eecb938f921935", null ],
      [ "I2C_MT_SLA_NACK", "i2c__drv_8h.html#a62180b2af27996e33c7b94c80d897dc0a42ec91f8ff613ec74457a0899d03360f", null ],
      [ "I2C_MT_DATA_ACK", "i2c__drv_8h.html#a62180b2af27996e33c7b94c80d897dc0a0598a9ef9ce1d863dbe99fa3bcbe42ce", null ],
      [ "I2C_MT_DATA_NACK", "i2c__drv_8h.html#a62180b2af27996e33c7b94c80d897dc0a6364f6e96f244d07a2e62b6e39028f57", null ],
      [ "I2C_MT_ARB_LOST", "i2c__drv_8h.html#a62180b2af27996e33c7b94c80d897dc0a62e3a107dd2bbfe9b8162d4c65a26b95", null ],
      [ "I2C_MR_SLA_ACK", "i2c__drv_8h.html#a62180b2af27996e33c7b94c80d897dc0acf7de0fdde8e39958d87b327f5f79098", null ],
      [ "I2C_MR_SLA_NACK", "i2c__drv_8h.html#a62180b2af27996e33c7b94c80d897dc0a387002e455ae82459a6a6e6e17ea67e0", null ],
      [ "I2C_MR_DATA_ACK", "i2c__drv_8h.html#a62180b2af27996e33c7b94c80d897dc0a9a44509a130957e5df4e2934bb080733", null ],
      [ "I2C_MR_DATA_NACK", "i2c__drv_8h.html#a62180b2af27996e33c7b94c80d897dc0a3fa877aef63126b82083d897b44db21d", null ],
      [ "I2C_SR_SLA_ACK", "i2c__drv_8h.html#a62180b2af27996e33c7b94c80d897dc0ace99b90cba7fb6a089e770846fb87926", null ],
      [ "I2C_SR_ARB_LOST_SLA_ACK", "i2c__drv_8h.html#a62180b2af27996e33c7b94c80d897dc0a7b0cb693d5f0e79ed0c8a0023a890df8", null ],
      [ "I2C_SR_GCALL_ACK", "i2c__drv_8h.html#a62180b2af27996e33c7b94c80d897dc0a3997e34325fc649e1275cb8ff6bc8de1", null ],
      [ "I2C_SR_ARB_LOST_GCALL_ACK", "i2c__drv_8h.html#a62180b2af27996e33c7b94c80d897dc0a8f4f13f3c1ed2eaf33d49e7467a8febe", null ],
      [ "I2C_SR_DATA_ACK", "i2c__drv_8h.html#a62180b2af27996e33c7b94c80d897dc0abf782f9d1188c23a856745f288cf124c", null ],
      [ "I2C_SR_DATA_NACK", "i2c__drv_8h.html#a62180b2af27996e33c7b94c80d897dc0a281c56f4173a15beff66339d9db8046a", null ],
      [ "I2C_SR_GCALL_DATA_ACK", "i2c__drv_8h.html#a62180b2af27996e33c7b94c80d897dc0af6b8b9c6f8649d9fa1f625177e7c9911", null ],
      [ "I2C_SR_GCALL_DATA_NACK", "i2c__drv_8h.html#a62180b2af27996e33c7b94c80d897dc0aa3db40c621912f6fa9851958e7933ce3", null ],
      [ "I2C_SR_STOP", "i2c__drv_8h.html#a62180b2af27996e33c7b94c80d897dc0ac622e8b695c5a4b0aa7ea864eded7cdb", null ],
      [ "I2C_ST_SLA_ACK", "i2c__drv_8h.html#a62180b2af27996e33c7b94c80d897dc0aa9738610f90b3e695ef344122952b1cc", null ],
      [ "I2C_ST_ARB_LOST_SLA_ACK", "i2c__drv_8h.html#a62180b2af27996e33c7b94c80d897dc0a10e8659cdfe7e4ebd9468ef70dabe207", null ],
      [ "I2C_ST_DATA_ACK", "i2c__drv_8h.html#a62180b2af27996e33c7b94c80d897dc0a012c5d272c989286f5a01d21c5b205a0", null ],
      [ "I2C_ST_DATA_NACK", "i2c__drv_8h.html#a62180b2af27996e33c7b94c80d897dc0a9ee922f311e6c5dcaada7f1224e4daa4", null ],
      [ "I2C_ST_LAST_DATA", "i2c__drv_8h.html#a62180b2af27996e33c7b94c80d897dc0a603a39874fce44f05f2c1cd31feeb8ae", null ],
      [ "I2C_ST_LAST_NACK", "i2c__drv_8h.html#a62180b2af27996e33c7b94c80d897dc0aedf0cb91dfe2c72f27694d0901750bb1", null ],
      [ "I2C_ST_2ND_ACK", "i2c__drv_8h.html#a62180b2af27996e33c7b94c80d897dc0a3bb35bab0cdb158d236a8b4082176573", null ],
      [ "I2C_ST_2ND_NACK", "i2c__drv_8h.html#a62180b2af27996e33c7b94c80d897dc0ab4956ce3a94f906c43338ae4e5c3c80e", null ],
      [ "I2C_NO_INFO", "i2c__drv_8h.html#a62180b2af27996e33c7b94c80d897dc0ad75f1b73dd1d8946abd1eb2c4b1d149e", null ]
    ] ],
    [ "i2c_AckByteI", "i2c__drv_8h.html#a868bead5c130102dc078667c886b3a75", null ],
    [ "i2c_init", "i2c__drv_8h.html#a3a72107911930a88d780373c68937fd8", null ],
    [ "i2c_irq_disable", "i2c__drv_8h.html#acc8b7552e59ba3bacb60bfa2b954af7b", null ],
    [ "i2c_irq_enable", "i2c__drv_8h.html#a9e9d074ad708559111cdebbea240ce45", null ],
    [ "I2C_IRQHandler", "i2c__drv_8h.html#a420d56f594345b56efdac63fdabf71c2", null ],
    [ "i2c_MasterRx", "i2c__drv_8h.html#a68dcdf9e8ec4f8a489d85caa9dd6aa58", null ],
    [ "i2c_MasterRxI", "i2c__drv_8h.html#a6994a6cd461f3ea166c04055a5f0fad1", null ],
    [ "i2c_MasterTx", "i2c__drv_8h.html#a5b8c0825cf26c7ed499b4c28682cadf1", null ],
    [ "i2c_MasterTxI", "i2c__drv_8h.html#a899d057aecebcf9b65cb03130031e6aa", null ],
    [ "i2c_RxByte", "i2c__drv_8h.html#aa4edc8d3730b418cd4937c0b68ba0977", null ],
    [ "i2c_RxLByte", "i2c__drv_8h.html#a528f66c09e1f0f6098068ad5fb212be1", null ],
    [ "i2c_SetDevAddr", "i2c__drv_8h.html#a61c112df9d7542a40d27b613af3b941b", null ],
    [ "i2c_SetGCE", "i2c__drv_8h.html#a4a48a372b7b8b4f2c279491bb8a63d8e", null ],
    [ "i2c_SetSLA", "i2c__drv_8h.html#a00f5cf18f70b338cb9195b80c8755760", null ],
    [ "i2c_SetSLAX", "i2c__drv_8h.html#a9ae1115cba46022b9a90d23f5c046f08", null ],
    [ "i2c_TxByte", "i2c__drv_8h.html#aacac6070a1bdd6ab90b090512a8e15b3", null ],
    [ "i2c_TxByteI", "i2c__drv_8h.html#a3eec9ec83825d630d7bfa52f260a756b", null ],
    [ "i2c_TxLByte", "i2c__drv_8h.html#a32bf25d490cfddd5c7b8e88650e9299d", null ],
    [ "i2c_TxLByteI", "i2c__drv_8h.html#a68df3361f6faa95bf7b371f16868f4a9", null ],
    [ "i2c_TxStart", "i2c__drv_8h.html#a46d1b6380096f55f8dc806232498d28a", null ],
    [ "i2c_TxStartI", "i2c__drv_8h.html#ac5923483591e4a276166807c4bac6187", null ],
    [ "i2c_TxStop", "i2c__drv_8h.html#a763c2dc9f789ef38a61949cfd31b8309", null ],
    [ "i2c_TxStopI", "i2c__drv_8h.html#a887edcf6b5d81c0199b33383853af069", null ],
    [ "i2c_TxStopStartI", "i2c__drv_8h.html#a24c481c54a6807bd005f0c25efb40bfc", null ],
    [ "i2c_uninit", "i2c__drv_8h.html#ac15efb7df754e311fb8084edb7eb187a", null ],
    [ "i2c_Wait4Idle", "i2c__drv_8h.html#a377d39dd52a815bf1a009f98f3fe6403", null ],
    [ "i2c_Wait4IdleI", "i2c__drv_8h.html#afaf4c0ecb2cd6e04155ac7451babb4e3", null ],
    [ "i2c_Wait4Sent", "i2c__drv_8h.html#aca70d094cea399a453c01ae319dcdc79", null ]
];