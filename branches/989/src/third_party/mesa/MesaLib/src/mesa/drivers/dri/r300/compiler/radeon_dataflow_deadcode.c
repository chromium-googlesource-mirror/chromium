/*
 * Copyright (C) 2009 Nicolai Haehnle.
 *
 * All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 *
 * The above copyright notice and this permission notice (including the
 * next paragraph) shall be included in all copies or substantial
 * portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE COPYRIGHT OWNER(S) AND/OR ITS SUPPLIERS BE
 * LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
 * OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
 * WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 */

#include "radeon_dataflow.h"

#include "radeon_compiler.h"


struct updatemask_state {
	unsigned char Output[RC_REGISTER_MAX_INDEX];
	unsigned char Temporary[RC_REGISTER_MAX_INDEX];
	unsigned char Address;
	unsigned char Special[RC_NUM_SPECIAL_REGISTERS];
};

struct instruction_state {
	unsigned char WriteMask:4;
	unsigned char WriteALUResult:1;
	unsigned char SrcReg[3];
};

struct loopinfo {
	struct updatemask_state * Breaks;
	unsigned int BreakCount;
	unsigned int BreaksReserved;
};

struct branchinfo {
	unsigned int HaveElse:1;

	struct updatemask_state StoreEndif;
	struct updatemask_state StoreElse;
};

struct deadcode_state {
	struct radeon_compiler * C;
	struct instruction_state * Instructions;

	struct updatemask_state R;

	struct branchinfo * BranchStack;
	unsigned int BranchStackSize;
	unsigned int BranchStackReserved;

	struct loopinfo * LoopStack;
	unsigned int LoopStackSize;
	unsigned int LoopStackReserved;
};


static void or_updatemasks(
	struct updatemask_state * dst,
	struct updatemask_state * a,
	struct updatemask_state * b)
{
	for(unsigned int i = 0; i < RC_REGISTER_MAX_INDEX; ++i) {
		dst->Output[i] = a->Output[i] | b->Output[i];
		dst->Temporary[i] = a->Temporary[i] | b->Temporary[i];
	}

	for(unsigned int i = 0; i < RC_NUM_SPECIAL_REGISTERS; ++i)
		dst->Special[i] = a->Special[i] | b->Special[i];

	dst->Address = a->Address | b->Address;
}

static void push_break(struct deadcode_state *s)
{
	struct loopinfo * loop = &s->LoopStack[s->LoopStackSize - 1];
	memory_pool_array_reserve(&s->C->Pool, struct updatemask_state,
		loop->Breaks, loop->BreakCount, loop->BreaksReserved, 1);

	memcpy(&loop->Breaks[loop->BreakCount++], &s->R, sizeof(s->R));
}

static void push_loop(struct deadcode_state * s)
{
	memory_pool_array_reserve(&s->C->Pool, struct loopinfo, s->LoopStack,
			s->LoopStackSize, s->LoopStackReserved, 1);
	memset(&s->LoopStack[s->LoopStackSize++], 0, sizeof(struct loopinfo));
}

static void push_branch(struct deadcode_state * s)
{
	memory_pool_array_reserve(&s->C->Pool, struct branchinfo, s->BranchStack,
			s->BranchStackSize, s->BranchStackReserved, 1);

	struct branchinfo * branch = &s->BranchStack[s->BranchStackSize++];
	branch->HaveElse = 0;
	memcpy(&branch->StoreEndif, &s->R, sizeof(s->R));
}

static unsigned char * get_used_ptr(struct deadcode_state *s, rc_register_file file, unsigned int index)
{
	if (file == RC_FILE_OUTPUT || file == RC_FILE_TEMPORARY) {
		if (index >= RC_REGISTER_MAX_INDEX) {
			rc_error(s->C, "%s: index %i is out of bounds for file %i\n", __FUNCTION__, index, file);
			return 0;
		}

		if (file == RC_FILE_OUTPUT)
			return &s->R.Output[index];
		else
			return &s->R.Temporary[index];
	} else if (file == RC_FILE_ADDRESS) {
		return &s->R.Address;
	} else if (file == RC_FILE_SPECIAL) {
		if (index >= RC_NUM_SPECIAL_REGISTERS) {
			rc_error(s->C, "%s: special file index %i out of bounds\n", __FUNCTION__, index);
			return 0;
		}

		return &s->R.Special[index];
	}

	return 0;
}

static void mark_used(struct deadcode_state * s, rc_register_file file, unsigned int index, unsigned int mask)
{
	unsigned char * pused = get_used_ptr(s, file, index);
	if (pused)
		*pused |= mask;
}

static void update_instruction(struct deadcode_state * s, struct rc_instruction * inst)
{
	const struct rc_opcode_info * opcode = rc_get_opcode_info(inst->U.I.Opcode);
	struct instruction_state * insts = &s->Instructions[inst->IP];
	unsigned int usedmask = 0;

	if (opcode->HasDstReg) {
		unsigned char * pused = get_used_ptr(s, inst->U.I.DstReg.File, inst->U.I.DstReg.Index);
		if (pused) {
			usedmask = *pused & inst->U.I.DstReg.WriteMask;
			if (!inst->U.I.DstReg.RelAddr)
				*pused &= ~usedmask;
		}

		if (inst->U.I.DstReg.RelAddr)
			mark_used(s, RC_FILE_ADDRESS, 0, RC_MASK_X);
	}

	insts->WriteMask |= usedmask;

	if (inst->U.I.WriteALUResult) {
		unsigned char * pused = get_used_ptr(s, RC_FILE_SPECIAL, RC_SPECIAL_ALU_RESULT);
		if (pused && *pused) {
			if (inst->U.I.WriteALUResult == RC_ALURESULT_X)
				usedmask |= RC_MASK_X;
			else if (inst->U.I.WriteALUResult == RC_ALURESULT_W)
				usedmask |= RC_MASK_W;

			*pused = 0;
			insts->WriteALUResult = 1;
		}
	}

	unsigned int srcmasks[3];
	rc_compute_sources_for_writemask(inst, usedmask, srcmasks);

	for(unsigned int src = 0; src < opcode->NumSrcRegs; ++src) {
		unsigned int refmask = 0;
		unsigned int newsrcmask = srcmasks[src] & ~insts->SrcReg[src];
		insts->SrcReg[src] |= newsrcmask;

		for(unsigned int chan = 0; chan < 4; ++chan) {
			if (GET_BIT(newsrcmask, chan))
				refmask |= 1 << GET_SWZ(inst->U.I.SrcReg[src].Swizzle, chan);
		}

		/* get rid of spurious bits from ZERO, ONE, etc. swizzles */
		refmask &= RC_MASK_XYZW;

		if (!refmask)
			continue;

		mark_used(s, inst->U.I.SrcReg[src].File, inst->U.I.SrcReg[src].Index, refmask);

		if (inst->U.I.SrcReg[src].RelAddr)
			mark_used(s, RC_FILE_ADDRESS, 0, RC_MASK_X);
	}
}

static void mark_output_use(void * data, unsigned int index, unsigned int mask)
{
	struct deadcode_state * s = data;

	mark_used(s, RC_FILE_OUTPUT, index, mask);
}

void rc_dataflow_deadcode(struct radeon_compiler * c, void *user)
{
	struct deadcode_state s;
	unsigned int nr_instructions;
	unsigned has_temp_reladdr_src = 0;
	rc_dataflow_mark_outputs_fn dce = (rc_dataflow_mark_outputs_fn)user;

	/* Give up if there is relative addressing of destination operands. */
	for(struct rc_instruction * inst = c->Program.Instructions.Next;
	    inst != &c->Program.Instructions;
	    inst = inst->Next) {
		const struct rc_opcode_info *opcode = rc_get_opcode_info(inst->U.I.Opcode);
		if (opcode->HasDstReg &&
		    inst->U.I.DstReg.WriteMask &&
		    inst->U.I.DstReg.RelAddr) {
			return;
		}
	}

	memset(&s, 0, sizeof(s));
	s.C = c;

	nr_instructions = rc_recompute_ips(c);
	s.Instructions = memory_pool_malloc(&c->Pool, sizeof(struct instruction_state)*nr_instructions);
	memset(s.Instructions, 0, sizeof(struct instruction_state)*nr_instructions);

	dce(c, &s, &mark_output_use);

	for(struct rc_instruction * inst = c->Program.Instructions.Prev;
	    inst != &c->Program.Instructions;
	    inst = inst->Prev) {
		const struct rc_opcode_info * opcode = rc_get_opcode_info(inst->U.I.Opcode);

		switch(opcode->Opcode){
		/* Mark all sources in the loop body as used before doing
		 * normal deadcode analysis.  This is probably not optimal.
		 */
		case RC_OPCODE_ENDLOOP:
		{
			int endloops = 1;
			struct rc_instruction *ptr;
			for(ptr = inst->Prev; endloops > 0; ptr = ptr->Prev){
				opcode = rc_get_opcode_info(ptr->U.I.Opcode);
				if(ptr->U.I.Opcode == RC_OPCODE_BGNLOOP){
					endloops--;
					continue;
				}
				if(ptr->U.I.Opcode == RC_OPCODE_ENDLOOP){
					endloops++;
					continue;
				}
				if(opcode->HasDstReg){
					int src = 0;
					unsigned int srcmasks[3];
					rc_compute_sources_for_writemask(ptr,
						ptr->U.I.DstReg.WriteMask, srcmasks);
					for(src=0; src < opcode->NumSrcRegs; src++){
						mark_used(&s,
							ptr->U.I.SrcReg[src].File,
							ptr->U.I.SrcReg[src].Index,
							srcmasks[src]);
					}
				}
			}
			push_loop(&s);
			break;
		}
		case RC_OPCODE_BRK:
			push_break(&s);
			break;
		case RC_OPCODE_BGNLOOP:
		{
			unsigned int i;
			struct loopinfo * loop = &s.LoopStack[s.LoopStackSize-1];
			for(i = 0; i < loop->BreakCount; i++) {
				or_updatemasks(&s.R, &s.R, &loop->Breaks[i]);
			}
			break;
		}
		case RC_OPCODE_CONT:
			break;
		case RC_OPCODE_ENDIF:
			push_branch(&s);
			break;
		default:
			if (opcode->IsFlowControl && s.BranchStackSize) {
				struct branchinfo * branch = &s.BranchStack[s.BranchStackSize-1];
				if (opcode->Opcode == RC_OPCODE_IF) {
					or_updatemasks(&s.R,
							&s.R,
							branch->HaveElse ? &branch->StoreElse : &branch->StoreEndif);

					s.BranchStackSize--;
				} else if (opcode->Opcode == RC_OPCODE_ELSE) {
					if (branch->HaveElse) {
						rc_error(c, "%s: Multiple ELSE for one IF/ENDIF\n", __FUNCTION__);
					} else {
						memcpy(&branch->StoreElse, &s.R, sizeof(s.R));
						memcpy(&s.R, &branch->StoreEndif, sizeof(s.R));
						branch->HaveElse = 1;
					}
				} else {
					rc_error(c, "%s: Unhandled control flow instruction %s\n", __FUNCTION__, opcode->Name);
				}
			}

			if (!has_temp_reladdr_src) {
				for (unsigned i = 0; i < opcode->NumSrcRegs; i++) {
					if (inst->U.I.SrcReg[i].File == RC_FILE_TEMPORARY &&
					    inst->U.I.SrcReg[i].RelAddr) {
						/* If there is a register read from a temporary file with relative addressing,
						 * mark all preceding written registers as used. */
						for (struct rc_instruction *ptr = inst->Prev;
						     ptr != &c->Program.Instructions;
						     ptr = ptr->Prev) {
							opcode = rc_get_opcode_info(ptr->U.I.Opcode);
							if (opcode->HasDstReg &&
							    ptr->U.I.DstReg.File == RC_FILE_TEMPORARY &&
							    ptr->U.I.DstReg.WriteMask) {
								mark_used(&s,
									  ptr->U.I.DstReg.File,
									  ptr->U.I.DstReg.Index,
									  ptr->U.I.DstReg.WriteMask);
							}
						}

						has_temp_reladdr_src = 1;
						break;
					}
				}
			}
		}

		update_instruction(&s, inst);
	}

	unsigned int ip = 0;
	for(struct rc_instruction * inst = c->Program.Instructions.Next;
	    inst != &c->Program.Instructions;
	    inst = inst->Next, ++ip) {
		const struct rc_opcode_info * opcode = rc_get_opcode_info(inst->U.I.Opcode);
		int dead = 1;

		if (!opcode->HasDstReg) {
			dead = 0;
		} else {
			inst->U.I.DstReg.WriteMask = s.Instructions[ip].WriteMask;
			if (s.Instructions[ip].WriteMask)
				dead = 0;

			if (s.Instructions[ip].WriteALUResult)
				dead = 0;
			else
				inst->U.I.WriteALUResult = RC_ALURESULT_NONE;
		}

		if (dead) {
			struct rc_instruction * todelete = inst;
			inst = inst->Prev;
			rc_remove_instruction(todelete);
			continue;
		}

		unsigned int srcmasks[3];
		unsigned int usemask = s.Instructions[ip].WriteMask;

		if (inst->U.I.WriteALUResult == RC_ALURESULT_X)
			usemask |= RC_MASK_X;
		else if (inst->U.I.WriteALUResult == RC_ALURESULT_W)
			usemask |= RC_MASK_W;

		rc_compute_sources_for_writemask(inst, usemask, srcmasks);

		for(unsigned int src = 0; src < 3; ++src) {
			for(unsigned int chan = 0; chan < 4; ++chan) {
				if (!GET_BIT(srcmasks[src], chan))
					SET_SWZ(inst->U.I.SrcReg[src].Swizzle, chan, RC_SWIZZLE_UNUSED);
			}
		}
	}

	rc_calculate_inputs_outputs(c);
}
