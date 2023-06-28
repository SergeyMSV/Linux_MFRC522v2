/* SPDX-License-Identifier: LGPL-2.1 */
#pragma once

#include <SPI.h>
#include <MFRC522Driver.h>
#include <MFRC522DriverPin.h>

class MFRC522DriverSPI : public MFRC522Driver {
public:
  //using PCD_Register = MFRC522Constants::PCD_Register;
  /////////////////////////////////////////////////////////////////////////////////////
  // Functions for setting up the Arduino.
  /////////////////////////////////////////////////////////////////////////////////////
  
  bool init() override;
  
  /////////////////////////////////////////////////////////////////////////////////////
  // Basic interface functions for communicating with the MFRC522.
  /////////////////////////////////////////////////////////////////////////////////////
  void PCD_WriteRegister(const PCD_Register reg, const byte value) override;
  void PCD_WriteRegister(const PCD_Register reg, const byte count, byte *const values) override;
  byte PCD_ReadRegister(const PCD_Register reg) override;
  void PCD_ReadRegister(const PCD_Register reg, const byte count, byte *const values, const byte rxAlign = 0) override;

  MFRC522DriverSPI () = default;

protected:
  virtual std::vector<uint8_t> Transaction(const std::vector<uint8_t>& tx) = 0;
};
