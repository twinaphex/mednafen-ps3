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
 * PSX assembly interpreter.
 */
#include <stdint.h>
#include "psxcommon.h"
#include "r3000a.h"
#include "gte.h"
#include "psxhle.h"

static int branch = 0;
static int branch2 = 0;
static u32 branchPC;

// Subsets
void (*psxBSC[64])(uint32_t aCode);
void (*psxSPC[64])(uint32_t aCode);
void (*psxREG[32])(uint32_t aCode);
void (*psxCP0[32])();
void (*psxCP2[64])();
void (*psxCP2BSC[32])();

// Execute one instruction
void TOTAL_INLINE execI()
{ 
	//ROBO: This will go crashy crashy if you try to execute garbage addresses !
	psxRegs.code = GETLE32(Read_ICache(psxRegs.pc, FALSE));

	psxRegs.pc += 4;
	psxRegs.cycle += BIAS;

	psxBSC[psxRegs.code >> 26](psxRegs.code);
}


static void delayRead(int reg, u32 bpc) {
	u32 rold, rnew;

//	SysPrintf("delayRead at %x!\n", psxRegs.pc);

	rold = psxRegs.GPR.r[reg];
	psxBSC[psxRegs.code >> 26](psxRegs.code); // branch delay load
	rnew = psxRegs.GPR.r[reg];

	psxRegs.pc = bpc;

	psxBranchTest();

	psxRegs.GPR.r[reg] = rold;
	execI(); // first branch opcode
	psxRegs.GPR.r[reg] = rnew;

	branch = 0;
}

static void delayWrite(int reg, u32 bpc) {

/*	SysPrintf("delayWrite at %x!\n", psxRegs.pc);

	SysPrintf("%s\n", disR3000AF(psxRegs.code, psxRegs.pc-4));
	SysPrintf("%s\n", disR3000AF(PSXMu32(bpc), bpc));*/

	// no changes from normal behavior

	psxBSC[psxRegs.code >> 26](psxRegs.code);

	branch = 0;
	psxRegs.pc = bpc;

	psxBranchTest();
}

static void delayReadWrite(int reg, u32 bpc) {

//	SysPrintf("delayReadWrite at %x!\n", psxRegs.pc);

	// the branch delay load is skipped

	branch = 0;
	psxRegs.pc = bpc;

	psxBranchTest();
}

// this defines shall be used with the tmp 
// of the next func (instead of _Funct_...)
#define _tFunct_  ((tmp      ) & 0x3F)  // The funct part of the instruction register 
#define _tRd_     ((tmp >> 11) & 0x1F)  // The rd part of the instruction register 
#define _tRt_     ((tmp >> 16) & 0x1F)  // The rt part of the instruction register 
#define _tRs_     ((tmp >> 21) & 0x1F)  // The rs part of the instruction register 
#define _tSa_     ((tmp >>  6) & 0x1F)  // The sa part of the instruction register

//TODO: ? Change this to a table of function pointers?
//      Turning the main instruction handler into a switch caused slowdown
//		so maybe going the other way here will be speed up?
int psxTestLoadDelay(int reg, u32 tmp) {
	if (tmp == 0) return 0; // NOP
	switch (tmp >> 26) {
		case 0x00: // SPECIAL
			switch (_tFunct_) {
				case 0x00: // SLL
				case 0x02: case 0x03: // SRL/SRA
					if (_tRd_ == reg && _tRt_ == reg) return 1; else
					if (_tRt_ == reg) return 2; else
					if (_tRd_ == reg) return 3;
					break;

				case 0x08: // JR
					if (_tRs_ == reg) return 2;
					break;
				case 0x09: // JALR
					if (_tRd_ == reg && _tRs_ == reg) return 1; else
					if (_tRs_ == reg) return 2; else
					if (_tRd_ == reg) return 3;
					break;

				// SYSCALL/BREAK just a break;

				case 0x20: case 0x21: case 0x22: case 0x23:
				case 0x24: case 0x25: case 0x26: case 0x27: 
				case 0x2a: case 0x2b: // ADD/ADDU...
				case 0x04: case 0x06: case 0x07: // SLLV...
					if (_tRd_ == reg && (_tRt_ == reg || _tRs_ == reg)) return 1; else
					if (_tRt_ == reg || _tRs_ == reg) return 2; else
					if (_tRd_ == reg) return 3;
					break;

				case 0x10: case 0x12: // MFHI/MFLO
					if (_tRd_ == reg) return 3;
					break;
				case 0x11: case 0x13: // MTHI/MTLO
					if (_tRs_ == reg) return 2;
					break;

				case 0x18: case 0x19:
				case 0x1a: case 0x1b: // MULT/DIV...
					if (_tRt_ == reg || _tRs_ == reg) return 2;
					break;
			}
			break;

		case 0x01: // REGIMM
			switch (_tRt_) {
				case 0x00: case 0x01:
				case 0x10: case 0x11: // BLTZ/BGEZ...
					// Xenogears - lbu v0 / beq v0
					// - no load delay (fixes battle loading)
					break;

					if (_tRs_ == reg) return 2;
					break;
			}
			break;

		// J would be just a break;
		case 0x03: // JAL
			if (31 == reg) return 3;
			break;

		case 0x04: case 0x05: // BEQ/BNE
			// Xenogears - lbu v0 / beq v0
			// - no load delay (fixes battle loading)
			break;

			if (_tRs_ == reg || _tRt_ == reg) return 2;
			break;

		case 0x06: case 0x07: // BLEZ/BGTZ
			// Xenogears - lbu v0 / beq v0
			// - no load delay (fixes battle loading)
			break;

			if (_tRs_ == reg) return 2;
			break;

		case 0x08: case 0x09: case 0x0a: case 0x0b:
		case 0x0c: case 0x0d: case 0x0e: // ADDI/ADDIU...
			if (_tRt_ == reg && _tRs_ == reg) return 1; else
			if (_tRs_ == reg) return 2; else
			if (_tRt_ == reg) return 3;
			break;

		case 0x0f: // LUI
			if (_tRt_ == reg) return 3;
			break;

		case 0x10: // COP0
			switch (_tFunct_) {
				case 0x00: // MFC0
					if (_tRt_ == reg) return 3;
					break;
				case 0x02: // CFC0
					if (_tRt_ == reg) return 3;
					break;
				case 0x04: // MTC0
					if (_tRt_ == reg) return 2;
					break;
				case 0x06: // CTC0
					if (_tRt_ == reg) return 2;
					break;
				// RFE just a break;
			}
			break;

		case 0x12: // COP2
			switch (_tFunct_) {
				case 0x00: 
					switch (_tRs_) {
						case 0x00: // MFC2
							if (_tRt_ == reg) return 3;
							break;
						case 0x02: // CFC2
							if (_tRt_ == reg) return 3;
							break;
						case 0x04: // MTC2
							if (_tRt_ == reg) return 2;
							break;
						case 0x06: // CTC2
							if (_tRt_ == reg) return 2;
							break;
					}
					break;
				// RTPS... break;
			}
			break;

		case 0x22: case 0x26: // LWL/LWR
			if (_tRt_ == reg) return 3; else
			if (_tRs_ == reg) return 2;
			break;

		case 0x20: case 0x21: case 0x23:
		case 0x24: case 0x25: // LB/LH/LW/LBU/LHU
			if (_tRt_ == reg && _tRs_ == reg) return 1; else
			if (_tRs_ == reg) return 2; else
			if (_tRt_ == reg) return 3;
			break;

		case 0x28: case 0x29: case 0x2a:
		case 0x2b: case 0x2e: // SB/SH/SWL/SW/SWR
			if (_tRt_ == reg || _tRs_ == reg) return 2;
			break;

		case 0x32: case 0x3a: // LWC2/SWC2
			if (_tRs_ == reg) return 2;
			break;
	}

	return 0;
}

void psxDelayTest(int reg, u32 bpc) {
	u32 *code;
	u32 tmp;

	// Don't execute yet - just peek
	code = Read_ICache(bpc, TRUE);

//ROBO	tmp = ((code == NULL) ? 0 : SWAP32(*code));
	tmp = ((code == NULL) ? 0 : GETLE32(code));
	branch = 1;

	switch (psxTestLoadDelay(reg, tmp)) {
		case 1:
			delayReadWrite(reg, bpc); return;
		case 2:
			delayRead(reg, bpc); return;
		case 3:
			delayWrite(reg, bpc); return;
	}
	psxBSC[psxRegs.code >> 26](psxRegs.code);

	branch = 0;
	psxRegs.pc = bpc;

	psxBranchTest();
}

__inline void doBranch(uint32_t aCode, u32 tar) {
	u32 *code;
	u32 tmp;

	branch2 = branch = 1;
	branchPC = tar;

	// branch delay slot
	code = Read_ICache(psxRegs.pc, TRUE);

//	psxRegs.code = ((code == NULL) ? 0 : SWAP32(*code));
	psxRegs.code = ((code == NULL) ? 0 : GETLE32(code));

	psxRegs.pc += 4;
	psxRegs.cycle += BIAS;

	// check for load delay
	tmp = psxRegs.code >> 26;
	switch (tmp) {
		case 0x10: // COP0
			switch (_Rs_) {
				case 0x00: // MFC0
				case 0x02: // CFC0
					psxDelayTest(_Rt_, branchPC);
					return;
			}
			break;
		case 0x12: // COP2
			switch (_Funct_) {
				case 0x00:
					switch (_Rs_) {
						case 0x00: // MFC2
						case 0x02: // CFC2
							psxDelayTest(_Rt_, branchPC);
							return;
					}
					break;
			}
			break;
		case 0x32: // LWC2
			psxDelayTest(_Rt_, branchPC);
			return;
		default:
			if (tmp >= 0x20 && tmp <= 0x26) { // LB/LH/LWL/LW/LBU/LHU/LWR
				psxDelayTest(_Rt_, branchPC);
				return;
			}
			break;
	}

	psxBSC[psxRegs.code >> 26](psxRegs.code);

	branch = 0;
	psxRegs.pc = branchPC;

	psxBranchTest();
}

/*********************************************************
* Instruction Logic                                      *
*********************************************************/
/*********************************************************
* Branch and jump                                        *
*********************************************************/
#define _SetLink(x)				psxRegs.GPR.r[x] = _PC_ + 4;								// Sets the return address in the link register
#define _JumpTarget_			((_Target_ * 4) + (_PC_ & 0xf0000000))						// Calculates the target during a jump instruction
#define _BranchTarget_			((s16)_Im_ * 4 + _PC_)										// Calculates the target during a branch instruction
#define RepBranchi32(op)      	if(_rRsS_ op _rRtS_) doBranch(aCode, _BranchTarget_);
#define RepZBranchi32(op)		if(_rRsS_ op 0) doBranch(aCode, _BranchTarget_);
#define RepZBranchLinki32(op)	if(_rRsS_ op 0) { _SetLink(31); doBranch(aCode, _BranchTarget_); }

/*********************************************************
* Register branch logic                                  *
* Format:  OP rs, offset                                 *
*********************************************************/
void psxBGEZ(uint32_t aCode)		{ RepZBranchi32(>=)											}				// Branch if Rs >= 0
void psxBGEZAL(uint32_t aCode)		{ RepZBranchLinki32(>=)										}				// Branch if Rs >= 0 and link
void psxBGTZ(uint32_t aCode)		{ RepZBranchi32(>)											}				// Branch if Rs >  0
void psxBLEZ(uint32_t aCode)		{ RepZBranchi32(<=)											}				// Branch if Rs <= 0
void psxBLTZ(uint32_t aCode)		{ RepZBranchi32(<)											}				// Branch if Rs <  0
void psxBLTZAL(uint32_t aCode)		{ RepZBranchLinki32(<)										}				// Branch if Rs <  0 and link

/*********************************************************
* Register branch logic                                  *
* Format:  OP rs, rt, offset                             *
*********************************************************/
void psxBEQ(uint32_t aCode)			{ RepBranchi32(==)											}				// Branch if Rs == Rt
void psxBNE(uint32_t aCode)			{ RepBranchi32(!=)											}				// Branch if Rs != Rt

/*********************************************************
* Jump to target                                         *
* Format:  OP target                                     *
*********************************************************/
void psxJ(uint32_t aCode)			{               doBranch(aCode, _JumpTarget_);				}
void psxJAL(uint32_t aCode)			{ _SetLink(31); doBranch(aCode, _JumpTarget_);				}

/*********************************************************
* Register jump                                          *
* Format:  OP rs, rd                                     *
*********************************************************/
void psxJR(uint32_t aCode)			{doBranch(aCode, _rRs_); psxJumpTest();						}
void psxJALR(uint32_t aCode)		{u32 temp = _rRs_; if (LIKELY(_Rd_ != 0)) { _SetLink(_Rd_);} doBranch(aCode, temp);}

/*********************************************************
* Shift                                                  *
*********************************************************/
/*********************************************************
* Shift arithmetic with constant shift                   *
* Format:  OP rd, rt, sa                                 *
*********************************************************/
void psxSLL(uint32_t aCode) 		{ if (UNLIKELY(!_Rd_)) return; _rRd_ = _rRt_ << _Sa_;		}				// Rd = Rt << sa
void psxSRA(uint32_t aCode) 		{ if (UNLIKELY(!_Rd_)) return; _rRdS_ = _rRtS_ >> _Sa_;		}				// Rd = Rt >> sa (arithmetic)
void psxSRL(uint32_t aCode) 		{ if (UNLIKELY(!_Rd_)) return; _rRd_ = _rRt_ >> _Sa_;		}				// Rd = Rt >> sa (logical)

/*********************************************************
* Shift arithmetic with variant register shift           *
* Format:  OP rd, rt, rs                                 *
*********************************************************/
void psxSLLV(uint32_t aCode) 	{ if (UNLIKELY(!_Rd_)) return; _rRd_ = _rRt_ << _rRs_;		}				// Rd = Rt << rs
void psxSRAV(uint32_t aCode) 	{ if (UNLIKELY(!_Rd_)) return; _rRdS_ = _rRtS_ >> _rRs_;	}				// Rd = Rt >> rs (arithmetic)
void psxSRLV(uint32_t aCode) 	{ if (UNLIKELY(!_Rd_)) return; _rRd_ = _rRt_ >> _rRs_;		}				// Rd = Rt >> rs (logical)

/*********************************************************
* Load higher 16 bits of the first word in GPR with imm  *
* Format:  OP rt, immediate                              *
*********************************************************/
void psxLUI(uint32_t aCode)		{ if (UNLIKELY(!_Rt_)) return; _rRt_ = psxRegs.code << 16;	}				// Upper halfword of Rt = Im

/*********************************************************
* ALU                                                    *
*********************************************************/
/*********************************************************
* Arithmetic with immediate operand                      *
* Format:  OP rt, rs, immediate                          *
*********************************************************/
//Basic evedence suggests that this is faster than an isel type hack
//(time) 183563219 / (count) 97448790	(1.883689054) Using snes9x-ps3 isel.h
//(time) 192753295 / (count) 106571978	(1.808667706) Using following code
//These times were obtained via SYS_TIMEBASE_GET and spread across all of basic ALU ops
//With less than 2 ticks on average the timinig may very well be off
void psxADDI(uint32_t aCode) 	{ if (UNLIKELY(!_Rt_)) return; _rRt_ = _rRs_ + _Imm_ ;		}				// Rt = Rs + Im 	(Exception on Integer Overflow)
void psxADDIU(uint32_t aCode) 	{ if (UNLIKELY(!_Rt_)) return; _rRt_ = _rRs_ + _Imm_ ;		}				// Rt = Rs + Im
void psxANDI(uint32_t aCode) 	{ if (UNLIKELY(!_Rt_)) return; _rRt_ = _rRs_ & _ImmU_;		}				// Rt = Rs And Im
void psxORI(uint32_t aCode) 		{ if (UNLIKELY(!_Rt_)) return; _rRt_ = _rRs_ | _ImmU_;		}				// Rt = Rs Or  Im
void psxXORI(uint32_t aCode) 	{ if (UNLIKELY(!_Rt_)) return; _rRt_ = _rRs_ ^ _ImmU_;		}				// Rt = Rs Xor Im
void psxSLTI(uint32_t aCode) 	{ if (UNLIKELY(!_Rt_)) return; _rRt_ = _rRsS_ < _Imm_ ;		}				// Rt = Rs < Im		(Signed)
void psxSLTIU(uint32_t aCode) 	{ if (UNLIKELY(!_Rt_)) return; _rRt_ = _rRs_ < ((u32)_Imm_);}				// Rt = Rs < Im		(Unsigned)

/*********************************************************
* Register arithmetic                                    *
* Format:  OP rd, rs, rt                                 *
*********************************************************/
void psxADD(uint32_t aCode)		{ if (UNLIKELY(!_Rd_)) return; _rRd_ =  _rRs_ + _rRt_;		}				// Rd = Rs + Rt		(Exception on Integer Overflow)
void psxADDU(uint32_t aCode) 	{ if (UNLIKELY(!_Rd_)) return; _rRd_ =  _rRs_ + _rRt_;		}				// Rd = Rs + Rt
void psxSUB(uint32_t aCode)	 	{ if (UNLIKELY(!_Rd_)) return; _rRd_ =  _rRs_ - _rRt_;		}				// Rd = Rs - Rt		(Exception on Integer Overflow)
void psxSUBU(uint32_t aCode) 	{ if (UNLIKELY(!_Rd_)) return; _rRd_ =  _rRs_ - _rRt_;		}				// Rd = Rs - Rt
void psxAND(uint32_t aCode)	 	{ if (UNLIKELY(!_Rd_)) return; _rRd_ =  _rRs_ & _rRt_;		}				// Rd = Rs And Rt
void psxOR(uint32_t aCode) 		{ if (UNLIKELY(!_Rd_)) return; _rRd_ =  _rRs_ | _rRt_;		}				// Rd = Rs Or  Rt
void psxXOR(uint32_t aCode) 		{ if (UNLIKELY(!_Rd_)) return; _rRd_ =  _rRs_ ^ _rRt_;		}				// Rd = Rs Xor Rt
void psxNOR(uint32_t aCode) 		{ if (UNLIKELY(!_Rd_)) return; _rRd_ =~(_rRs_ | _rRt_); 	}				// Rd = Rs Nor Rt
void psxSLT(uint32_t aCode) 		{ if (UNLIKELY(!_Rd_)) return; _rRd_ =  _rRsS_ < _rRtS_;	}				// Rd = Rs < Rt		(Signed)
void psxSLTU(uint32_t aCode)		{ if (UNLIKELY(!_Rd_)) return; _rRd_ =  _rRs_ < _rRt_;		}				// Rd = Rs < Rt		(Unsigned)

/*********************************************************
* Register mult/div & Register trap logic                *
* Format:  OP rs, rt                                     *
*********************************************************/
void psxDIV(uint32_t aCode)
{
	if (LIKELY(_rRtS_ != 0))
	{
		_rLoS_ = _rRsS_ / _rRtS_;
		_rHiS_ = _rRsS_ % _rRtS_;
	}
	else
	{
		_rLoS_ = 0xffffffff;
		_rHiS_ = _rRsS_;
	}
}

void psxDIVU(uint32_t aCode)
{
	if (LIKELY(_rRt_ != 0))
	{
		_rLo_ = _rRs_ / _rRt_;
		_rHi_ = _rRs_ % _rRt_;
	}
	else
	{
		_rLo_ = 0xffffffff;
		_rHi_ = _rRs_;
	}
}

//TODO: ? Better way to handle sign
//TODO: Use a union for lo and hi
void psxMULT(uint32_t aCode)
{
	u64 res = (s64)((s64)_rRsS_ * (s64)_rRtS_);

	psxRegs.GPR.n.lo = (u32)(res & 0xffffffff);
	psxRegs.GPR.n.hi = (u32)((res >> 32) & 0xffffffff);
}

void psxMULTU(uint32_t aCode)
{
	u64 res = (u64)((u64)_rRs_ * (u64)_rRt_);

	psxRegs.GPR.n.lo = (u32)(res & 0xffffffff);
	psxRegs.GPR.n.hi = (u32)((res >> 32) & 0xffffffff);
}


/*********************************************************
* Special purpose instructions                           *
* Format:  OP                                            *
*********************************************************/
void psxBREAK(uint32_t aCode) {
	// Break exception - psx rom doens't handles this
}

void psxSYSCALL(uint32_t aCode) {
	psxRegs.pc -= 4;
	psxException(0x20, branch);
}

void psxRFE(uint32_t aCode) {
//	SysPrintf("psxRFE\n");
	psxRegs.CP0.n.Status = (psxRegs.CP0.n.Status & 0xfffffff0) |
						  ((psxRegs.CP0.n.Status & 0x3c) >> 2);
}

/*********************************************************
* Move from HI/LO to GPR                                 *
* Format:  OP rd                                         *
*********************************************************/
void psxMFHI(uint32_t aCode) { if (UNLIKELY(!_Rd_)) return; _rRd_ = _rHi_; } // Rd = Hi
void psxMFLO(uint32_t aCode) { if (UNLIKELY(!_Rd_)) return; _rRd_ = _rLo_; } // Rd = Lo

/*********************************************************
* Move to GPR to HI/LO & Register jump                   *
* Format:  OP rs                                         *
*********************************************************/
void psxMTHI(uint32_t aCode) { _rHi_ = _rRs_; } // Hi = Rs
void psxMTLO(uint32_t aCode) { _rLo_ = _rRs_; } // Lo = Rs

/*********************************************************
* Load and store for GPR                                 *
* Format:  OP rt, offset(base)                           *
*********************************************************/
#define _oB_ (_rRs_ + _Imm_)
u32 TOTAL_INLINE testBranchDelay(uint32_t aCode)
{
	if(UNLIKELY(branch == 0))
	{
		psxRegs.pc -= 4;
		doBranch(aCode, psxRegs.pc + 4);
		return 0;
	}

	return 1;
}


void psxLB(uint32_t aCode)
{
	if(LIKELY(testBranchDelay(aCode)))
	{
		u32 value = psxMemRead8(_oB_);
		value |= (value & 0x80) ? 0xFFFFFF00 : 0;
		_rRt_ = _Rt_ ? value : 0;
	}
}

void psxLBU(uint32_t aCode)
{
	if(LIKELY(testBranchDelay(aCode)))
	{
		u32 value = psxMemRead8(_oB_);
		_rRt_ = _Rt_ ? value : 0;
	}
}

void psxLH(uint32_t aCode)
{
	if(LIKELY(testBranchDelay(aCode)))
	{
		u32 value = psxMemRead16(_oB_);
		value |= (value & 0x8000) ? 0xFFFF0000 : 0;
		_rRt_ = _Rt_ ? value : 0;
	}
}

void psxLHU(uint32_t aCode)
{
	if(LIKELY(testBranchDelay(aCode)))
	{
		u32 value = psxMemRead16(_oB_);
		_rRt_ = _Rt_ ? value : 0;
	}
}

void psxLW(uint32_t aCode)
{
	if(LIKELY(testBranchDelay(aCode)))
	{
		u32 value = psxMemRead32(_oB_);
		_rRt_ = _Rt_ ? value : 0;
	}
}

u32 LWL_MASK[4] = { 0xffffff, 0xffff, 0xff, 0 };
u32 LWL_SHIFT[4] = { 24, 16, 8, 0 };

void psxLWL(uint32_t aCode) {
	u32 addr = _oB_;
	u32 shift = addr & 3;
	u32 mem = psxMemRead32(addr & ~3);


	// load delay = 1 latency
	if(LIKELY(testBranchDelay(aCode)))
	{
		if (UNLIKELY(!_Rt_)) return;
		_rRt_ =	( _rRt_ & LWL_MASK[shift]) | 
						( mem << LWL_SHIFT[shift]);
	}

	/*
	Mem = 1234.  Reg = abcd

	0   4bcd   (mem << 24) | (reg & 0x00ffffff)
	1   34cd   (mem << 16) | (reg & 0x0000ffff)
	2   234d   (mem <<  8) | (reg & 0x000000ff)
	3   1234   (mem      ) | (reg & 0x00000000)
	*/
}

u32 LWR_MASK[4] = { 0, 0xff000000, 0xffff0000, 0xffffff00 };
u32 LWR_SHIFT[4] = { 0, 8, 16, 24 };

void psxLWR(uint32_t aCode) {
	u32 addr = _oB_;
	u32 shift = addr & 3;
	u32 mem = psxMemRead32(addr & ~3);
	
	if(LIKELY(testBranchDelay(aCode)))
	{
		if (UNLIKELY(!_Rt_)) return;
		_rRt_ =	( _rRt_ & LWR_MASK[shift]) | 
						( mem >> LWR_SHIFT[shift]);
	}

	/*
	Mem = 1234.  Reg = abcd

	0   1234   (mem      ) | (reg & 0x00000000)
	1   a123   (mem >>  8) | (reg & 0xff000000)
	2   ab12   (mem >> 16) | (reg & 0xffff0000)
	3   abc1   (mem >> 24) | (reg & 0xffffff00)
	*/
}

void psxSB(uint32_t aCode) { psxMemWrite8 (_oB_, _rRt_); }
void psxSH(uint32_t aCode) { psxMemWrite16(_oB_, _rRt_); }
void psxSW(uint32_t aCode) { psxMemWrite32(_oB_, _rRt_); }

u32 SWL_MASK[4] = { 0xffffff00, 0xffff0000, 0xff000000, 0 };
u32 SWL_SHIFT[4] = { 24, 16, 8, 0 };

void psxSWL(uint32_t aCode) {
	u32 addr = _oB_;
	u32 shift = addr & 3;
	u32 mem = psxMemRead32(addr & ~3);

	psxMemWrite32(addr & ~3,  (_rRt_ >> SWL_SHIFT[shift]) |
			     (  mem & SWL_MASK[shift]) );
	/*
	Mem = 1234.  Reg = abcd

	0   123a   (reg >> 24) | (mem & 0xffffff00)
	1   12ab   (reg >> 16) | (mem & 0xffff0000)
	2   1abc   (reg >>  8) | (mem & 0xff000000)
	3   abcd   (reg      ) | (mem & 0x00000000)
	*/
}

u32 SWR_MASK[4] = { 0, 0xff, 0xffff, 0xffffff };
u32 SWR_SHIFT[4] = { 0, 8, 16, 24 };

void psxSWR(uint32_t aCode) {
	u32 addr = _oB_;
	u32 shift = addr & 3;
	u32 mem = psxMemRead32(addr & ~3);

	psxMemWrite32(addr & ~3,  (_rRt_ << SWR_SHIFT[shift]) |
			     (  mem & SWR_MASK[shift]) );

	/*
	Mem = 1234.  Reg = abcd

	0   abcd   (reg      ) | (mem & 0x00000000)
	1   bcd4   (reg <<  8) | (mem & 0x000000ff)
	2   cd34   (reg << 16) | (mem & 0x0000ffff)
	3   d234   (reg << 24) | (mem & 0x00ffffff)
	*/
}

/*********************************************************
* Moves between GPR and COPx                             *
* Format:  OP rt, fs                                     *
*********************************************************/
void psxMFC0(uint32_t aCode)
{
	if(LIKELY(testBranchDelay(aCode)))
	{
		if (UNLIKELY(!_Rt_)) return;
	
		_rRtS_ = (int)_rFs_;
	}
}

void psxCFC0(uint32_t aCode)
{
	if(LIKELY(testBranchDelay(aCode)))
	{
		if (UNLIKELY(!_Rt_)) return;
	
		_rRtS_ = (int)_rFs_;
	}
}

void psxTestSWInts() {
	// the next code is untested, if u know please
	// tell me if it works ok or not (linuzappz)
	if (psxRegs.CP0.n.Cause & psxRegs.CP0.n.Status & 0x0300 &&
		psxRegs.CP0.n.Status & 0x1) {
		psxException(psxRegs.CP0.n.Cause, branch);
	}
}

__inline void MTC0(int reg, u32 val) {
//	SysPrintf("MTC0 %d: %x\n", reg, val);
	switch (reg) {
		case 12: // Status
			psxRegs.CP0.r[12] = val;
			psxTestSWInts();
			break;

		case 13: // Cause
			psxRegs.CP0.n.Cause = val & ~(0xfc00);
			psxTestSWInts();
			break;

		default:
			psxRegs.CP0.r[reg] = val;
			break;
	}
}

void psxMTC0(uint32_t aCode) { MTC0(_Rd_, _rRt_); }
void psxCTC0(uint32_t aCode) { MTC0(_Rd_, _rRt_); }



void psxMFC2(uint32_t aCode)
{
	if(LIKELY(testBranchDelay(aCode)))
	{
		gteMFC2(aCode);
	}
}


void psxCFC2(uint32_t aCode)
{
	if(LIKELY(testBranchDelay(aCode)))
	{
		gteCFC2(aCode);
	}
}


/*********************************************************
* Unknow instruction (would generate an exception)       *
* Format:  ?                                             *
*********************************************************/
void psxNULL(uint32_t aCode)		{}
void psxSPECIAL(uint32_t aCode)	{psxSPC[_Funct_](aCode);}
void psxREGIMM(uint32_t aCode)	{psxREG[_Rt_](aCode);}
void psxCOP0(uint32_t aCode)	{psxCP0[_Rs_](aCode);}
void psxCOP2(uint32_t aCode)	{if ((psxRegs.CP0.n.Status & 0x40000000) != 0 ) psxCP2[_Funct_](aCode);}
void psxBASIC(uint32_t aCode)	{psxCP2BSC[_Rs_](aCode);}
void psxHLE(uint32_t aCode)		{psxHLEt[psxRegs.code & 0x07]();}

void (*psxBSC[64])(uint32_t aCode) = {
	psxSPECIAL, psxREGIMM, psxJ   , psxJAL  , psxBEQ , psxBNE , psxBLEZ, psxBGTZ,
	psxADDI   , psxADDIU , psxSLTI, psxSLTIU, psxANDI, psxORI , psxXORI, psxLUI ,
	psxCOP0   , psxNULL  , psxCOP2, psxNULL , psxNULL, psxNULL, psxNULL, psxNULL,
	psxNULL   , psxNULL  , psxNULL, psxNULL , psxNULL, psxNULL, psxNULL, psxNULL,
	psxLB     , psxLH    , psxLWL , psxLW   , psxLBU , psxLHU , psxLWR , psxNULL,
	psxSB     , psxSH    , psxSWL , psxSW   , psxNULL, psxNULL, psxSWR , psxNULL, 
	psxNULL   , psxNULL  , gteLWC2, psxNULL , psxNULL, psxNULL, psxNULL, psxNULL,
	psxNULL   , psxNULL  , gteSWC2, psxHLE  , psxNULL, psxNULL, psxNULL, psxNULL 
};


void (*psxSPC[64])(uint32_t aCode) = {
	psxSLL , psxNULL , psxSRL , psxSRA , psxSLLV   , psxNULL , psxSRLV, psxSRAV,
	psxJR  , psxJALR , psxNULL, psxNULL, psxSYSCALL, psxBREAK, psxNULL, psxNULL,
	psxMFHI, psxMTHI , psxMFLO, psxMTLO, psxNULL   , psxNULL , psxNULL, psxNULL,
	psxMULT, psxMULTU, psxDIV , psxDIVU, psxNULL   , psxNULL , psxNULL, psxNULL,
	psxADD , psxADDU , psxSUB , psxSUBU, psxAND    , psxOR   , psxXOR , psxNOR ,
	psxNULL, psxNULL , psxSLT , psxSLTU, psxNULL   , psxNULL , psxNULL, psxNULL,
	psxNULL, psxNULL , psxNULL, psxNULL, psxNULL   , psxNULL , psxNULL, psxNULL,
	psxNULL, psxNULL , psxNULL, psxNULL, psxNULL   , psxNULL , psxNULL, psxNULL
};

void (*psxREG[32])(uint32_t aCode) = {
	psxBLTZ  , psxBGEZ  , psxNULL, psxNULL, psxNULL, psxNULL, psxNULL, psxNULL,
	psxNULL  , psxNULL  , psxNULL, psxNULL, psxNULL, psxNULL, psxNULL, psxNULL,
	psxBLTZAL, psxBGEZAL, psxNULL, psxNULL, psxNULL, psxNULL, psxNULL, psxNULL,
	psxNULL  , psxNULL  , psxNULL, psxNULL, psxNULL, psxNULL, psxNULL, psxNULL
};

void (*psxCP0[32])(uint32_t aCode) = {
	psxMFC0, psxNULL, psxCFC0, psxNULL, psxMTC0, psxNULL, psxCTC0, psxNULL,
	psxNULL, psxNULL, psxNULL, psxNULL, psxNULL, psxNULL, psxNULL, psxNULL,
	psxRFE , psxNULL, psxNULL, psxNULL, psxNULL, psxNULL, psxNULL, psxNULL,
	psxNULL, psxNULL, psxNULL, psxNULL, psxNULL, psxNULL, psxNULL, psxNULL
};

void (*psxCP2[64])(uint32_t aCode) = {
	psxBASIC, gteRTPS , psxNULL , psxNULL, psxNULL, psxNULL , gteNCLIP, psxNULL, // 00
	psxNULL , psxNULL , psxNULL , psxNULL, gteOP  , psxNULL , psxNULL , psxNULL, // 08
	gteDPCS , gteINTPL, gteMVMVA, gteNCDS, gteCDP , psxNULL , gteNCDT , psxNULL, // 10
	psxNULL , psxNULL , psxNULL , gteNCCS, gteCC  , psxNULL , gteNCS  , psxNULL, // 18
	gteNCT  , psxNULL , psxNULL , psxNULL, psxNULL, psxNULL , psxNULL , psxNULL, // 20
	gteSQR  , gteDCPL , gteDPCT , psxNULL, psxNULL, gteAVSZ3, gteAVSZ4, psxNULL, // 28 
	gteRTPT , psxNULL , psxNULL , psxNULL, psxNULL, psxNULL , psxNULL , psxNULL, // 30
	psxNULL , psxNULL , psxNULL , psxNULL, psxNULL, gteGPF  , gteGPL  , gteNCCT  // 38
};

void (*psxCP2BSC[32])(uint32_t aCode) = {
	psxMFC2, psxNULL, psxCFC2, psxNULL, gteMTC2, psxNULL, gteCTC2, psxNULL,
	psxNULL, psxNULL, psxNULL, psxNULL, psxNULL, psxNULL, psxNULL, psxNULL,
	psxNULL, psxNULL, psxNULL, psxNULL, psxNULL, psxNULL, psxNULL, psxNULL,
	psxNULL, psxNULL, psxNULL, psxNULL, psxNULL, psxNULL, psxNULL, psxNULL
};


///////////////////////////////////////////

static int intInit()
{
	return 0;
}

static void intReset()
{
	psxRegs.ICache_valid = FALSE;
}

int wanna_leave = 0;
void intExecute()
{
	wanna_leave = 0;
	while(LIKELY(wanna_leave == 0))
	{
		execI();
	}
}

static void intExecuteBlock()
{
	branch2 = 0;
	while(!branch2)
	{
		execI();
	}
}

static void intClear(u32 Addr, u32 Size)
{
}

static void intShutdown()
{
}

R3000Acpu psxInt = {
	intInit,
	intReset,
	intExecute,
	intExecuteBlock,
	intClear,
	intShutdown
};

