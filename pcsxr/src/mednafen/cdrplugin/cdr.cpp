#include <stdint.h>
#include "src/mednafen.h"
#include "src/cdrom/cdromif.h"

struct CdrStat {
    uint32_t Type;
    uint32_t Status;
    unsigned char Time[3];
};

struct SubQ {
    char res0[12];
    unsigned char ControlAndADR;
    unsigned char TrackNumber;
    unsigned char IndexNumber;
    unsigned char TrackRelativeAddress[3];
    unsigned char Filler;
    unsigned char AbsoluteAddress[3];
    unsigned char CRC[2];
    char res1[72];
};

static uint8_t	Buffer[8192];

//BCD to int
#define btoi(b)     					((b) / 16 * 10 + (b) % 16)
//MSF to sector
#define MSF2SECT(m, s, f)               (((m) * 60 + (s) - 2) * 75 + (f))

//Prototypes
extern "C"
{
	long			CDR__getStatus			(struct CdrStat *stat);
	long			pkCDRinit				(void);
	long			pkCDRshutdown			(void);
	long			pkCDRopen				(void);
	long			pkCDRclose				(void);
	long			pkCDRgetTN				(unsigned char * aBuffer);
	long			pkCDRgetTD				(unsigned char aTrack, unsigned char* aBuffer);
	long			pkCDRreadTrack			(unsigned char* aPosition);

	unsigned char*	pkCDRgetBuffer			(void);
	unsigned char*	pkCDRgetBufferSub		(void);
	long			pkCDRconfigure			(void);
	long			pkCDRtest				(void);
	void			pkCDRabout				(void);
	long			pkCDRplay				(unsigned char* aTime);
	long			pkCDRstop				(void);
	long			pkCDRsetfilename		(char* aFileName);
	long			pkCDRgetStatus			(struct CdrStat* aStatus);
	char*			pkCDRgetDriveLetter		(void);
	long			pkCDRreadCDDA			(unsigned char aMinutes, unsigned char aSeconds, unsigned char aFrames, unsigned char* aBuffer);
	long			pkCDRgetTE				(unsigned char, unsigned char *, unsigned char *, unsigned char *);

}


long			pkCDRinit				(void)
{
	return 0;
}

long			pkCDRshutdown			(void)
{
	return 0;
}

long			pkCDRopen				(void)
{
	return 0;
}

long			pkCDRclose				(void)
{
	return 0;
}

long			pkCDRgetTN				(unsigned char * aBuffer)
{
	CD_TOC toc;
	CDIF_ReadTOC(&toc);

	aBuffer[0] = toc.first_track;
	aBuffer[1] = toc.last_track;

	return 0;
}

long			pkCDRgetTD				(unsigned char aTrack, unsigned char* aBuffer)
{
	uint32_t sectors = CDIF_GetTrackSectorCount(aTrack);

	aBuffer[2] = sectors / 75 / 60;
	sectors -= aBuffer[2] * 75 * 60;
	aBuffer[1] = sectors / 75;
	sectors -= aBuffer[1] * 75;
	aBuffer[0] = sectors;

	return 0;
}

long			pkCDRreadTrack			(unsigned char* aPosition)
{
	int sector = MSF2SECT(btoi(aPosition[0]), btoi(aPosition[1]), btoi(aPosition[2]));
	sector += CDIF_GetTrackStartPositionLBA(1);
	CDIF_ReadRawSector(Buffer, sector);

#if 0
    if (subChanMixed) {
        fseek(cdHandle, MSF2SECT(btoi(time[0]), btoi(time[1]), btoi(time[2])) * (CD_FRAMESIZE_RAW + SUB_FRAMESIZE), SEEK_SET);
        fread(cdbuffer, 1, CD_FRAMESIZE_RAW, cdHandle);
        fread(subbuffer, 1, SUB_FRAMESIZE, cdHandle);

        if (subChanRaw) DecodeRawSubData();
    }
    else {
        if(isMode1ISO) {
            fseek(cdHandle, MSF2SECT(btoi(time[0]), btoi(time[1]), btoi(time[2])) * MODE1_DATA_SIZE, SEEK_SET);
            fread(cdbuffer + 12, 1, MODE1_DATA_SIZE, cdHandle);
            memset(cdbuffer, 0, 12); //not really necessary, fake mode 2 header
            cdbuffer[0] = (time[0]);
            cdbuffer[1] = (time[1]);
            cdbuffer[2] = (time[2]);
            cdbuffer[3] = 1; //mode 1
        } else {
            fseek(cdHandle, MSF2SECT(btoi(time[0]), btoi(time[1]), btoi(time[2])) * CD_FRAMESIZE_RAW, SEEK_SET);
            fread(cdbuffer, 1, CD_FRAMESIZE_RAW, cdHandle);
        }

        if (subHandle != NULL) {
            fseek(subHandle, MSF2SECT(btoi(time[0]), btoi(time[1]), btoi(time[2])) * SUB_FRAMESIZE, SEEK_SET);
            fread(subbuffer, 1, SUB_FRAMESIZE, subHandle);

            if (subChanRaw) DecodeRawSubData();
        }
    }

    return 0;
#endif

	return 0;
}

unsigned char*	pkCDRgetBuffer			(void)
{
	return Buffer + 12;
}

unsigned char*	pkCDRgetBufferSub		(void)
{
	return 0;
}

long			pkCDRconfigure			(void)
{
	return 0;
}

long			pkCDRtest				(void)
{
	return 0;
}

void			pkCDRabout				(void)
{
	return;
}

long			pkCDRplay				(unsigned char* aTime)
{
	//TODO
	return 0;
}

long			pkCDRstop				(void)
{
	//TODO
	return 0;
}

long			pkCDRsetfilename		(char* aFileName)
{
	//NOTHING
	return 0;
}

long			pkCDRgetStatus			(struct CdrStat* aStatus)
{
    CDR__getStatus(aStatus);

//TODO:
//    if (playing) {
//        stat->Status |= 0x80;
//    }

    // relative -> absolute time
//    sect = cddaCurOffset / CD_FRAMESIZE_RAW + 150;
//    sec2msf(sect, (u8 *)stat->Time);

    // BIOS - boot ID (CD type)
//    stat->Type = ti[1].type;

	return 0;
}

char*			pkCDRgetDriveLetter		(void)
{
	//NOTHING
	return 0;
}

long			pkCDRreadCDDA			(unsigned char aMinutes, unsigned char aSeconds, unsigned char aFrames, unsigned char* aBuffer)
{
	//TODO:
	return 0;
}

long			pkCDRgetTE				(unsigned char, unsigned char *, unsigned char *, unsigned char *)
{
	return 0;
}

