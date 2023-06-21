#include "sha256.hpp"

void SHA256(uint8_t *Output, uint8_t *InputData, uint32_t InputByteLength) {
	uint32_t Hash[8] = { 0x6A09E667, 0xBB67AE85, 0x3C6EF372, 0xA54FF53A,
			0x510E527F, 0x9B05688C, 0x1F83D9AB, 0x5BE0CD19 };

	uint32_t RoundConstants[64] = { 0x428A2F98, 0x71374491, 0xB5C0FBCF,
			0xE9B5DBA5, 0x3956C25B, 0x59F111F1, 0x923F82A4, 0xAB1C5ED5,
			0xD807AA98, 0x12835B01, 0x243185BE, 0x550C7DC3, 0x72BE5D74,
			0x80DEB1FE, 0x9BDC06A7, 0xC19BF174, 0xE49B69C1, 0xEFBE4786,
			0x0FC19DC6, 0x240CA1CC, 0x2DE92C6F, 0x4A7484AA, 0x5CB0A9DC,
			0x76F988DA, 0x983E5152, 0xA831C66D, 0xB00327C8, 0xBF597FC7,
			0xC6E00BF3, 0xD5A79147, 0x06CA6351, 0x14292967, 0x27B70A85,
			0x2E1B2138, 0x4D2C6DFC, 0x53380D13, 0x650A7354, 0x766A0ABB,
			0x81C2C92E, 0x92722C85, 0xA2BFE8A1, 0xA81A664B, 0xC24B8B70,
			0xC76C51A3, 0xD192E819, 0xD6990624, 0xF40E3585, 0x106AA070,
			0x19A4C116, 0x1E376C08, 0x2748774C, 0x34B0BCB5, 0x391C0CB3,
			0x4ED8AA4A, 0x5B9CCA4F, 0x682E6FF3, 0x748F82EE, 0x78A5636F,
			0x84C87814, 0x8CC70208, 0x90BEFFFA, 0xA4506CEB, 0xBEF9A3F7,
			0xC67178F2 };

	/* ========== Preprocessing and padding ========== */
	uint32_t PaddingByteLength = 64 - InputByteLength % 64;
	if (PaddingByteLength < 9) {
		/* if appended 1 and bitlength don't fit into current chunk, extend padding to another chunk */
		PaddingByteLength += 64;

	}

	uint8_t Padding[PaddingByteLength] = { 0 };
	Padding[0] = 0x80; //Append "1"

	/* Writing size in big endian format into the last two 32-bit words of padding */
	uint32_t InputBitLength = (InputByteLength) * 8;
	uint8_t *InputBitLengthBytes = reinterpret_cast<uint8_t*>(&InputBitLength);
	/* Reverse bytes */
	InputBitLengthBytes[0] ^= InputBitLengthBytes[3];
	InputBitLengthBytes[3] ^= InputBitLengthBytes[0];
	InputBitLengthBytes[0] ^= InputBitLengthBytes[3];
	InputBitLengthBytes[1] ^= InputBitLengthBytes[2];
	InputBitLengthBytes[2] ^= InputBitLengthBytes[1];
	InputBitLengthBytes[1] ^= InputBitLengthBytes[2];
	for (uint8_t i = 0; i < 4; i++) {
		Padding[PaddingByteLength - 4 + i] = InputBitLengthBytes[i]; //Accessing only by byte
	}

	/* ========== Main loop ========== */
	uint32_t DataIndex = 0;
	for (uint32_t chunk = 0; chunk < (PaddingByteLength + InputByteLength) / 64;
			chunk++) {

		uint8_t MessageSchedule[64 * 4] alignas(4);
				uint32_t *MessageScheduleWord = reinterpret_cast<uint32_t*> (MessageSchedule);

				for (uint8_t i = 0; i < 64; i++) {
					if (DataIndex < InputByteLength) {
						MessageSchedule[i] = InputData[DataIndex];
						DataIndex++;
					} else {
						MessageSchedule[i] = Padding[DataIndex - InputByteLength];
						DataIndex++;
					}
				}

				/* Reverse data bytes upon fetching */
				for(uint8_t i = 0; i < 16; i++) {
					MessageSchedule[4*i+0] ^= MessageSchedule[4*i+3];
					MessageSchedule[4*i+3] ^= MessageSchedule[4*i+0];
					MessageSchedule[4*i+0] ^= MessageSchedule[4*i+3];
					MessageSchedule[4*i+1] ^= MessageSchedule[4*i+2];
					MessageSchedule[4*i+2] ^= MessageSchedule[4*i+1];
					MessageSchedule[4*i+1] ^= MessageSchedule[4*i+2];
				}

				for (uint8_t i = 16; i < 64; i++) {
					uint32_t S0 = ((MessageScheduleWord[i - 15] >> 7) | (MessageScheduleWord[i - 15] << (32 - 7)))
					^ ((MessageScheduleWord[i - 15] >> 18) | (MessageScheduleWord[i - 15] << (32 - 18)))
					^ (MessageScheduleWord[i - 15] >> 3);
					uint32_t S1 = ((MessageScheduleWord[i - 2] >> 17) | (MessageScheduleWord[i - 2] << (32 - 17)))
					^ ((MessageScheduleWord[i - 2] >> 19) | (MessageScheduleWord[i - 2] << (32 - 19)))
					^ (MessageScheduleWord[i - 2] >> 10);
					MessageScheduleWord[i] = MessageScheduleWord[i - 16]
					+ MessageScheduleWord[i - 7] + S0 + S1;
				}

				uint32_t WorkingVars[8];
				for (uint8_t i = 0; i < 8; i++) {
					WorkingVars[i] = Hash[i];
				}

				for (uint8_t i = 0; i < 64; i++) {
					uint32_t S1 = ((WorkingVars[4] >> 6) | (WorkingVars[4] << (32 - 6)))
					^ ((WorkingVars[4] >> 11) | (WorkingVars[4] << (32 - 11)))
					^ ((WorkingVars[4] >> 25) | (WorkingVars[4] << (32 - 25)));
					uint32_t ch = (WorkingVars[4] & WorkingVars[5])
					^ ((~WorkingVars[4]) & WorkingVars[6]);
					uint32_t temp1 = WorkingVars[7] + S1 + ch + RoundConstants[i]
					+ MessageScheduleWord[i];
					uint32_t S0 = ((WorkingVars[0] >> 2) | (WorkingVars[0] << (32 - 2)))
					^ ((WorkingVars[0] >> 13) | (WorkingVars[0] << (32 - 13)))
					^ ((WorkingVars[0] >> 22) | (WorkingVars[0] << (32 - 22)));
					uint32_t maj = (WorkingVars[0] & WorkingVars[1])
					^ (WorkingVars[0] & WorkingVars[2])
					^ (WorkingVars[1] & WorkingVars[2]);
					uint32_t temp2 = S0 + maj;

					WorkingVars[7] = WorkingVars[6];
					WorkingVars[6] = WorkingVars[5];
					WorkingVars[5] = WorkingVars[4];
					WorkingVars[4] = WorkingVars[3] + temp1;
					WorkingVars[3] = WorkingVars[2];
					WorkingVars[2] = WorkingVars[1];
					WorkingVars[1] = WorkingVars[0];
					WorkingVars[0] = temp1 + temp2;

				}

				for (uint8_t i = 0; i < 8; i++) {
					Hash[i] += WorkingVars[i];
				}

			}
	for (uint8_t i = 0; i < 8; i++) {
		Output[4 * i + 0] = static_cast<uint8_t>(Hash[4 * i]);
		Output[4 * i + 1] = static_cast<uint8_t>(Hash[4 * i] >> 8);
		Output[4 * i + 2] = static_cast<uint8_t>(Hash[4 * i] >> 16);
		Output[4 * i + 3] = static_cast<uint8_t>(Hash[4 * i] >> 24);
	}

}



