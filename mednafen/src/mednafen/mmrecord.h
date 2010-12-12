#ifndef __MDFN_MMRECORD_H
#define __MDFN_MMRECORD_H

bool MMRecord_Active(void);
bool MMRecord_Start(const char *filename);
void MMRecord_WriteFrame(const EmulateSpecStruct *espec);
bool MMRecord_End(void);

#endif
