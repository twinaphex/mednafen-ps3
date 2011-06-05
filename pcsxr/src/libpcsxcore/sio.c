/***************************************************************************
 *   Copyright (C) 2007 Ryan Schultz, PCSX-df Team, PCSX team              *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   51 Franklin Street, Fifth Floor, Boston, MA 02111-1307 USA.           *
 ***************************************************************************/

/*
* SIO functions.
*/

#include "sio.h"
#include <sys/stat.h>

// Status Flags
#define TX_RDY		0x0001
#define RX_RDY		0x0002
#define TX_EMPTY	0x0004
#define PARITY_ERR	0x0008
#define RX_OVERRUN	0x0010
#define FRAMING_ERR	0x0020
#define SYNC_DETECT	0x0040
#define DSR			0x0080
#define CTS			0x0100
#define IRQ			0x0200

// Control Flags
#define TX_PERM		0x0001
#define DTR			0x0002
#define RX_PERM		0x0004
#define BREAK		0x0008
#define RESET_ERR	0x0010
#define RTS			0x0020
#define SIO_RESET	0x0040

PSXSIO_DataDef		PSXSIO_Data;

// *** FOR WORKS ON PADS AND MEMORY CARDS *****
unsigned char cardh[4] = { 0x00, 0x00, 0x5a, 0x5d };

#define buf PSXSIO_Data.Buffer
#define StatReg PSXSIO_Data.Status
#define ModeReg PSXSIO_Data.Mode
#define CtrlReg PSXSIO_Data.Control
#define BaudReg PSXSIO_Data.Baud
#define bufcount PSXSIO_Data.bufcount
#define parp PSXSIO_Data.parp
#define mcdst PSXSIO_Data.mcdst
#define rdwr PSXSIO_Data.rdwr
#define adrH PSXSIO_Data.adrH
#define adrL PSXSIO_Data.adrL
#define padst PSXSIO_Data.padst
#define gsdonglest PSXSIO_Data.gsdonglest
#define Mcd1Data PSXSIO_Data.Mcd1Data
#define Mcd2Data PSXSIO_Data.Mcd2Data

#if 0
// Breaks Twisted Metal 2 intro
#define SIO_INT(eCycle) { \
	if (!Config.Sio) { \
		PSXCPU_SetEvent(PSXINT_SIO, psxRegs.cycle, eCycle); \
	} \
	\
	StatReg &= ~RX_RDY; \
	StatReg &= ~TX_RDY; \
}
#endif

#define SIO_INT(eCycle) { \
	if (!Config.Sio) { \
		PSXCPU_SetEvent(PSXINT_SIO, psxRegs.cycle, eCycle); \
	} \
}


// clk cycle byte
// 4us * 8bits = (PSXCLK / 1000000) * 32; (linuzappz)
// TODO: add SioModePrescaler
#define SIO_CYCLES (BaudReg * 8)

// rely on this for now - someone's actual testing
//#define SIO_CYCLES (PSXCLK / 57600)
//PCSX 1.9.91
//#define SIO_CYCLES 200
//PCSX 1.9.91
//#define SIO_CYCLES 270
// ePSXe 1.6.0
//#define SIO_CYCLES		535
// ePSXe 1.7.0
//#define SIO_CYCLES 635

void					PSXSIO_WriteControl				(uint16_t aValue)
{
	PSXSIO_Data.Control = aValue & ~RESET_ERR;
	if(aValue & RESET_ERR)
	{
		PSXSIO_Data.Status &= ~IRQ;
	}

	if((PSXSIO_Data.Control & SIO_RESET) || (!PSXSIO_Data.Control))
	{
		padst = 0; mcdst = 0; parp = 0;
		StatReg = TX_RDY | TX_EMPTY;
		psxRegs.interrupt &= ~(1 << PSXINT_SIO);
	}
}


void sioWrite8(unsigned char value) {
#ifdef PAD_LOG
	PAD_LOG("sio write8 %x\n", value);
#endif
	switch (padst) {
		case 1: SIO_INT(SIO_CYCLES);
			/*
			$41-4F
			$41 = Find bits in poll respones
			$42 = Polling command
			$43 = Config mode (Dual shock?)
			$44 = Digital / Analog (after $F3)
			$45 = Get status info (Dual shock?)

			ID:
			$41 = Digital
			$73 = Analogue Red LED
			$53 = Analogue Green LED

			$23 = NegCon
			$12 = Mouse
			*/

			if ((value & 0x40) == 0x40) {
				padst = 2; parp = 1;
				switch (CtrlReg & 0x2002) {
					case 0x0002:
						buf[parp] = pkPADpoll(value);
						break;
					case 0x2002:
						buf[parp] = pkPADpoll(value);
						break;
					}

				if (!(buf[parp] & 0x0f)) {
					bufcount = 2 + 32;
				} else {
					bufcount = 2 + (buf[parp] & 0x0f) * 2;
				}


				// Digital / Dual Shock Controller
				if (buf[parp] == 0x41) {
					switch (value) {
						// enter config mode
						case 0x43:
							buf[1] = 0x43;
							break;

						// get status
						case 0x45:
							buf[1] = 0xf3;
							break;
					}
				}


				// NegCon - Wipeout 3
				if( buf[parp] == 0x23 ) {
					switch (value) {
						// enter config mode
						case 0x43:
							buf[1] = 0x79;
							break;

						// get status
						case 0x45:
							buf[1] = 0xf3;
							break;
					}
				}
			}
			else padst = 0;
			return;
		case 2:
			parp++;
/*			if (buf[1] == 0x45) {
				buf[parp] = 0;
				SIO_INT(SIO_CYCLES);
				return;
			}*/
			switch (CtrlReg & 0x2002) {
				case 0x0002: buf[parp] = pkPADpoll(value); break;
				case 0x2002: buf[parp] = pkPADpoll(value); break;
			}

			if (parp == bufcount) { padst = 0; return; }
			SIO_INT(SIO_CYCLES);
			return;
	}

	switch (mcdst) {
		case 1:
			SIO_INT(SIO_CYCLES);
			if (rdwr) { parp++; return; }
			parp = 1;
			switch (value) {
				case 0x52: rdwr = 1; break;
				case 0x57: rdwr = 2; break;
				default: mcdst = 0;
			}
			return;
		case 2: // address H
			SIO_INT(SIO_CYCLES);
			adrH = value;
			*buf = 0;
			parp = 0;
			bufcount = 1;
			mcdst = 3;
			return;
		case 3: // address L
			SIO_INT(SIO_CYCLES);
			adrL = value;
			*buf = adrH;
			parp = 0;
			bufcount = 1;
			mcdst = 4;
			return;
		case 4:
			SIO_INT(SIO_CYCLES);
			parp = 0;
			switch (rdwr) {
				case 1: // read
					buf[0] = 0x5c;
					buf[1] = 0x5d;
					buf[2] = adrH;
					buf[3] = adrL;
					switch (CtrlReg & 0x2002) {
						case 0x0002:
							memcpy(&buf[4], Mcd1Data + (adrL | (adrH << 8)) * 128, 128);
							break;
						case 0x2002:
							memcpy(&buf[4], Mcd2Data + (adrL | (adrH << 8)) * 128, 128);
							break;
					}
					{
					char xor = 0;
					int i;
					for (i = 2; i < 128 + 4; i++)
						xor ^= buf[i];
					buf[132] = xor;
					}
					buf[133] = 0x47;
					bufcount = 133;
					break;
				case 2: // write
					buf[0] = adrL;
					buf[1] = value;
					buf[129] = 0x5c;
					buf[130] = 0x5d;
					buf[131] = 0x47;
					bufcount = 131;
					break;
			}
			mcdst = 5;
			return;
		case 5:
			parp++;
			if (rdwr == 2) {
				if (parp < 128) buf[parp + 1] = value;
			}
			SIO_INT(SIO_CYCLES);
			return;
	}

	switch (value) {
		case 0x01: // start pad
			StatReg |= RX_RDY;		// Transfer is Ready

			switch (CtrlReg & 0x2002) {
				case 0x0002: buf[0] = pkPADstartPoll(1); break;
				case 0x2002: buf[0] = pkPADstartPoll(2); break;
			}

			bufcount = 2;
			parp = 0;
			padst = 1;
			SIO_INT(SIO_CYCLES);
			return;
		case 0x81: // start memcard
			StatReg |= RX_RDY;
			memcpy(buf, cardh, 4);
			parp = 0;
			bufcount = 3;
			mcdst = 1;
			rdwr = 0;
			SIO_INT(SIO_CYCLES);
			return;

		default: // no hardware found
			StatReg |= RX_RDY;
			return;
	}
}





unsigned char sioRead8() {
	unsigned char ret = 0;

	if ((StatReg & RX_RDY)/* && (CtrlReg & RX_PERM)*/) {
//		StatReg &= ~RX_OVERRUN;
		ret = buf[parp];
		if (parp == bufcount) {
			StatReg &= ~RX_RDY;		// Receive is not Ready now
			if (mcdst == 5) {
				mcdst = 0;
				if (rdwr == 2) {
					switch (CtrlReg & 0x2002) {
						case 0x0002:
							memcpy(Mcd1Data + (adrL | (adrH << 8)) * 128, &buf[1], 128);
							SaveMcd(Config.Mcd1, Mcd1Data, (adrL | (adrH << 8)) * 128, 128);
							break;
						case 0x2002:
							memcpy(Mcd2Data + (adrL | (adrH << 8)) * 128, &buf[1], 128);
							SaveMcd(Config.Mcd2, Mcd2Data, (adrL | (adrH << 8)) * 128, 128);
							break;
					}
				}
			}
			if (padst == 2) padst = 0;
			if (mcdst == 1) {
				mcdst = 2;
				StatReg|= RX_RDY;
			}
		}
	}

#ifdef PAD_LOG
	PAD_LOG("sio read8 ;ret = %x\n", ret);
#endif
	return ret;
}

void sioInterrupt() {
#ifdef PAD_LOG
	PAD_LOG("Sio Interrupt (CP0.Status = %x)\n", psxRegs.CP0.n.Status);
#endif
//	SysPrintf("Sio Interrupt\n");
	StatReg |= IRQ;
	psxHu32ref(0x1070) |= SWAPu32(0x80);

#if 0
	// Rhapsody: fixes input problems
	// Twisted Metal 2: breaks intro
	StatReg |= TX_RDY;
	StatReg |= RX_RDY;
#endif
}

void LoadMcd(int mcd, char *str) {
	FILE *f;
	char *data = NULL;

	if (mcd == 1) data = Mcd1Data;
	if (mcd == 2) data = Mcd2Data;

	if (*str == 0) {
		sprintf(str, "memcards/card%d.mcd", mcd);
		SysPrintf(_("No memory card value was specified - creating a default card %s\n"), str);
	}
	f = fopen(str, "rb");
	if (f == NULL) {
		SysPrintf(_("The memory card %s doesn't exist - creating it\n"), str);
		CreateMcd(str);
		f = fopen(str, "rb");
		if (f != NULL) {
			struct stat buff;

			if (stat(str, &buff) != -1) {
				if (buff.st_size == MCD_SIZE + 64)
					fseek(f, 64, SEEK_SET);
				else if(buff.st_size == MCD_SIZE + 3904)
					fseek(f, 3904, SEEK_SET);
			}
			fread(data, 1, MCD_SIZE, f);
			fclose(f);
		}
		else
			SysMessage(_("Memory card %s failed to load!\n"), str);
	}
	else {
		struct stat buff;
		SysPrintf(_("Loading memory card %s\n"), str);
		if (stat(str, &buff) != -1) {
			if (buff.st_size == MCD_SIZE + 64)
				fseek(f, 64, SEEK_SET);
			else if(buff.st_size == MCD_SIZE + 3904)
				fseek(f, 3904, SEEK_SET);
		}
		fread(data, 1, MCD_SIZE, f);
		fclose(f);
	}
}

void LoadMcds(char *mcd1, char *mcd2) {
	LoadMcd(1, mcd1);
	LoadMcd(2, mcd2);
}

void SaveMcd(char *mcd, char *data, uint32_t adr, int size) {
	FILE *f;

	f = fopen(mcd, "r+b");
	if (f != NULL) {
		struct stat buff;

		if (stat(mcd, &buff) != -1) {
			if (buff.st_size == MCD_SIZE + 64)
				fseek(f, adr + 64, SEEK_SET);
			else if (buff.st_size == MCD_SIZE + 3904)
				fseek(f, adr + 3904, SEEK_SET);
			else
				fseek(f, adr, SEEK_SET);
		} else
			fseek(f, adr, SEEK_SET);

		fwrite(data + adr, 1, size, f);
		fclose(f);
		return;
	}

#if 0
	// try to create it again if we can't open it
	f = fopen(mcd, "wb");
	if (f != NULL) {
		fwrite(data, 1, MCD_SIZE, f);
		fclose(f);
	}
#endif

	ConvertMcd(mcd, data);
}

void CreateMcd(char *mcd) {
	FILE *f;
	struct stat buff;
	int s = MCD_SIZE;
	int i = 0, j;

	f = fopen(mcd, "wb");
	if (f == NULL)
		return;

	if (stat(mcd, &buff) != -1) {
		if ((buff.st_size == MCD_SIZE + 3904) || strstr(mcd, ".gme")) {
			s = s + 3904;
			fputc('1', f);
			s--;
			fputc('2', f);
			s--;
			fputc('3', f);
			s--;
			fputc('-', f);
			s--;
			fputc('4', f);
			s--;
			fputc('5', f);
			s--;
			fputc('6', f);
			s--;
			fputc('-', f);
			s--;
			fputc('S', f);
			s--;
			fputc('T', f);
			s--;
			fputc('D', f);
			s--;
			for (i = 0; i < 7; i++) {
				fputc(0, f);
				s--;
			}
			fputc(1, f);
			s--;
			fputc(0, f);
			s--;
			fputc(1, f);
			s--;
			fputc('M', f);
			s--;
			fputc('Q', f);
			s--;
			for (i = 0; i < 14; i++) {
				fputc(0xa0, f);
				s--;
			}
			fputc(0, f);
			s--;
			fputc(0xff, f);
			while (s-- > (MCD_SIZE + 1))
				fputc(0, f);
		} else if ((buff.st_size == MCD_SIZE + 64) || strstr(mcd, ".mem") || strstr(mcd, ".vgs")) {
			s = s + 64;
			fputc('V', f);
			s--;
			fputc('g', f);
			s--;
			fputc('s', f);
			s--;
			fputc('M', f);
			s--;
			for (i = 0; i < 3; i++) {
				fputc(1, f);
				s--;
				fputc(0, f);
				s--;
				fputc(0, f);
				s--;
				fputc(0, f);
				s--;
			}
			fputc(0, f);
			s--;
			fputc(2, f);
			while (s-- > (MCD_SIZE + 1))
				fputc(0, f);
		}
	}
	fputc('M', f);
	s--;
	fputc('C', f);
	s--;
	while (s-- > (MCD_SIZE - 127))
		fputc(0, f);
	fputc(0xe, f);
	s--;

	for (i = 0; i < 15; i++) { // 15 blocks
		fputc(0xa0, f);
		s--;
		fputc(0x00, f);
		s--;
		fputc(0x00, f);
		s--;
		fputc(0x00, f);
		s--;
		fputc(0x00, f);
		s--;
		fputc(0x00, f);
		s--;
		fputc(0x00, f);
		s--;
		fputc(0x00, f);
		s--;
		fputc(0xff, f);
		s--;
		fputc(0xff, f);
		s--;
		for (j = 0; j < 117; j++) {
			fputc(0x00, f);
			s--;
		}
		fputc(0xa0, f);
		s--;
	}

	for (i = 0; i < 20; i++) {
		fputc(0xff, f);
		s--;
		fputc(0xff, f);
		s--;
		fputc(0xff, f);
		s--;
		fputc(0xff, f);
		s--;
		fputc(0x00, f);
		s--;
		fputc(0x00, f);
		s--;
		fputc(0x00, f);
		s--;
		fputc(0x00, f);
		s--;
		fputc(0xff, f);
		s--;
		fputc(0xff, f);
		s--;
		for (j = 0; j < 118; j++) {
			fputc(0x00, f);
			s--;
		}
	}

	while ((s--) >= 0)
		fputc(0, f);

	fclose(f);
}

void ConvertMcd(char *mcd, char *data) {
	FILE *f;
	int i = 0;
	int s = MCD_SIZE;

	if (strstr(mcd, ".gme")) {
		f = fopen(mcd, "wb");
		if (f != NULL) {
			fwrite(data - 3904, 1, MCD_SIZE + 3904, f);
			fclose(f);
		}
		f = fopen(mcd, "r+");
		s = s + 3904;
		fputc('1', f); s--;
		fputc('2', f); s--;
		fputc('3', f); s--;
		fputc('-', f); s--;
		fputc('4', f); s--;
		fputc('5', f); s--;
		fputc('6', f); s--;
		fputc('-', f); s--;
		fputc('S', f); s--;
		fputc('T', f); s--;
		fputc('D', f); s--;
		for (i = 0; i < 7; i++) {
			fputc(0, f); s--;
		}
		fputc(1, f); s--;
		fputc(0, f); s--;
		fputc(1, f); s--;
		fputc('M', f); s--;
		fputc('Q', f); s--;
		for(i=0;i<14;i++) {
			fputc(0xa0, f); s--;
		}
		fputc(0, f); s--;
		fputc(0xff, f);
		while (s-- > (MCD_SIZE+1)) fputc(0, f);
		fclose(f);
	} else if(strstr(mcd, ".mem") || strstr(mcd,".vgs")) {
		f = fopen(mcd, "wb");
		if (f != NULL) {
			fwrite(data-64, 1, MCD_SIZE+64, f);
			fclose(f);
		}
		f = fopen(mcd, "r+");
		s = s + 64;
		fputc('V', f); s--;
		fputc('g', f); s--;
		fputc('s', f); s--;
		fputc('M', f); s--;
		for(i=0;i<3;i++) {
			fputc(1, f); s--;
			fputc(0, f); s--;
			fputc(0, f); s--;
			fputc(0, f); s--;
		}
		fputc(0, f); s--;
		fputc(2, f);
		while (s-- > (MCD_SIZE+1)) fputc(0, f);
		fclose(f);
	} else {
		f = fopen(mcd, "wb");
		if (f != NULL) {
			fwrite(data, 1, MCD_SIZE, f);
			fclose(f);
		}
	}
}

void GetMcdBlockInfo(int mcd, int block, McdBlock *Info) {
	unsigned char *data = NULL, *ptr, *str, *sstr;
	unsigned short clut[16];
	unsigned short c;
	int i, x;

	memset(Info, 0, sizeof(McdBlock));

	if (mcd == 1) data = Mcd1Data;
	if (mcd == 2) data = Mcd2Data;

	ptr = data + block * 8192 + 2;

	Info->IconCount = *ptr & 0x3;

	ptr += 2;

	x = 0;

	str = Info->Title;
	sstr = Info->sTitle;

	for (i = 0; i < 48; i++) {
		c = *(ptr) << 8;
		c |= *(ptr + 1);
		if (!c) break;

		// Convert ASCII characters to half-width
		if (c >= 0x8281 && c <= 0x829A)
			c = (c - 0x8281) + 'a';
		else if (c >= 0x824F && c <= 0x827A)
			c = (c - 0x824F) + '0';
		else if (c == 0x8140) c = ' ';
		else if (c == 0x8143) c = ',';
		else if (c == 0x8144) c = '.';
		else if (c == 0x8146) c = ':';
		else if (c == 0x8147) c = ';';
		else if (c == 0x8148) c = '?';
		else if (c == 0x8149) c = '!';
		else if (c == 0x815E) c = '/';
		else if (c == 0x8168) c = '"';
		else if (c == 0x8169) c = '(';
		else if (c == 0x816A) c = ')';
		else if (c == 0x816D) c = '[';
		else if (c == 0x816E) c = ']';
		else if (c == 0x817C) c = '-';
		else {
			str[i] = ' ';
			sstr[x++] = *ptr++; sstr[x++] = *ptr++;
			continue;
		}

		str[i] = sstr[x++] = c;
		ptr += 2;
	}

	trim(str);
	trim(sstr);

	ptr = data + block * 8192 + 0x60; // icon palette data

	for (i = 0; i < 16; i++) {
		clut[i] = *((unsigned short *)ptr);
		ptr += 2;
	}

	for (i = 0; i < Info->IconCount; i++) {
		short *icon = &Info->Icon[i * 16 * 16];

		ptr = data + block * 8192 + 128 + 128 * i; // icon data

		for (x = 0; x < 16 * 16; x++) {
			icon[x++] = clut[*ptr & 0xf];
			icon[x] = clut[*ptr >> 4];
			ptr++;
		}
	}

	ptr = data + block * 128;

	Info->Flags = *ptr;

	ptr += 0xa;
	strncpy(Info->ID, ptr, 12);
	ptr += 12;
	strncpy(Info->Name, ptr, 16);
}

int sioFreeze(gzFile f, int Mode) {
	gzfreeze(buf, sizeof(buf));
	gzfreeze(&StatReg, sizeof(StatReg));
	gzfreeze(&ModeReg, sizeof(ModeReg));
	gzfreeze(&CtrlReg, sizeof(CtrlReg));
	gzfreeze(&BaudReg, sizeof(BaudReg));
	gzfreeze(&bufcount, sizeof(bufcount));
	gzfreeze(&parp, sizeof(parp));
	gzfreeze(&mcdst, sizeof(mcdst));
	gzfreeze(&rdwr, sizeof(rdwr));
	gzfreeze(&adrH, sizeof(adrH));
	gzfreeze(&adrL, sizeof(adrL));
	gzfreeze(&padst, sizeof(padst));

	return 0;
}



