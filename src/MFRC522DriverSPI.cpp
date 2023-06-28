/* SPDX-License-Identifier: LGPL-2.1 */
#include "MFRC522DriverSPI.h"

/////////////////////////////////////////////////////////////////////////////////////
// Basic interface functions for communicating with the MFRC522DriverSPI
/////////////////////////////////////////////////////////////////////////////////////

bool MFRC522DriverSPI::init() {
  return true;
}

/**
 * Writes a byte to the specified register in the MFRC522DriverSPI chip.
 * The interface is described in the datasheet section 8.1.2.
 */
void MFRC522DriverSPI::PCD_WriteRegister(const PCD_Register reg,    ///< The register to write to. One of the PCD_Register enums.
                                         const byte value            ///< The value to write.
                                        ) {
  // When using SPI all addresses are shifted one bit left in the "SPI address byte" (section 8.1.2.3)
  std::vector<uint8_t> tx;
  tx.push_back(reg << 1); // MSB == 0 is for writing. LSB is not used in address. Datasheet section 8.1.2.3.
  tx.push_back(value);
  Transaction(tx);
} // End PCD_WriteRegister()

/**
 * Writes a number of bytes to the specified register in the MFRC522DriverSPI chip.
 * The interface is described in the datasheet section 8.1.2.
 */
void MFRC522DriverSPI::PCD_WriteRegister(const PCD_Register reg,    ///< The register to write to. One of the PCD_Register enums.
                                         const byte count,            ///< The number of bytes to write to the register.
                                         byte *const values        ///< The values to write. Byte array.
                                        ) {
  std::vector<uint8_t> tx;
  tx.push_back(reg << 1); // MSB == 0 is for writing. LSB is not used in address. Datasheet section 8.1.2.3.
  for (byte i = 0; i < count; ++i)
    tx.push_back(values[i]);
  Transaction(tx);
} // End PCD_WriteRegister()

/**
 * Reads a byte from the specified register in the MFRC522DriverSPI chip.
 * The interface is described in the datasheet section 8.1.2.
 */
byte MFRC522DriverSPI::PCD_ReadRegister(const PCD_Register reg    ///< The register to read from. One of the PCD_Register enums.
                                       ) {
  std::vector<uint8_t> tx;
  tx.push_back((byte)0x80 | (reg << 1)); // MSB == 1 is for reading. LSB is not used in address. Datasheet section 8.1.2.3.
  tx.push_back(0);          // Read the value back. Send 0 to stop reading.
  std::vector<uint8_t> rx = Transaction(tx);
  if (rx.size() == 2)
    return rx[1];
  return 0;
} // End PCD_ReadRegister()

/**
 * Reads a number of bytes from the specified register in the MFRC522DriverSPI chip.
 * The interface is described in the datasheet section 8.1.2.
 */
void MFRC522DriverSPI::PCD_ReadRegister(const PCD_Register reg,    ///< The register to read from. One of the PCD_Register enums.
                                        const byte count,            ///< The number of bytes to read.
                                        byte *const values,        ///< Byte array to store the values in.
                                        const byte rxAlign        ///< Only bit positions rxAlign..7 in values[0] are updated.
                                       ) {
  if(count == 0) {
    return;
  }
  //Serial.print(F("Reading ")); 	Serial.print(count); Serial.println(F(" bytes from register."));
  const byte address  = (byte)0x80 | (reg << 1);        // MSB == 1 is for reading. LSB is not used in address. Datasheet section 8.1.2.3.
  //count--;								// One read is performed outside of the loop // TODO is this correct?

  int index = 0;              // Index in values array.
  std::vector<uint8_t> tx;
  tx.push_back(address);
  if (rxAlign) // Only update bit positions rxAlign..7 in values[0]
  {
    tx.push_back(address);
    ++index;
  }
  while (index < count - 1) // One read is performed outside of the loop
  {
    tx.push_back(address);
    ++index;
  }
  tx.push_back(0); // Read the final byte. Send 0 to stop reading.

  index = 0;
  std::vector<uint8_t> rx = Transaction(tx);
  if (rxAlign) // Only update bit positions rxAlign..7 in values[0]
  {
    // Create bit mask for bit positions rxAlign..7
    byte mask = (byte)(0xFF << rxAlign) & 0xFF;
    // Apply mask to both current value of values[0] and the new data in value.
    values[0] = (values[0] & ~mask) | (rx[1] & mask);
    ++index;
  }
  while (index <= count)
  {
    values[index] = rx[index + 1];	// Read value and tell that we want to read the same address again.
    ++index;
  }
} // End PCD_ReadRegister()

