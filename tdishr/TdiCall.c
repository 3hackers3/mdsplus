/*	TdiCall.C
	Call a routine in an image.
	A CALL has pointers to
	0	descriptor of image logical name (default SYS$SHARE:.EXE)
	1	descriptor of routine name.
	2...	descriptors of (ndesc-2) arguments.
	You must check number of arguments.
	Limit is 255 arguments.
		result = image->entry(in1, ...)

	Or the result type may be given.
		result = image->entry:type(in1, ...)
	where type is F for floating, B for byte, etc.,
	or DSC for a descriptor of any kind supported.
	A pointer to XD and D descriptor will free that descriptor.
	The type is given by the value at the record pointer.

	Ken Klare, LANL	P-4	(c)1990,1992
*/
#include <string.h>
#include <stdlib.h>
#include "tdirefcat.h"
#include "tdirefstandard.h"
#include <libroutines.h>
#include <strroutines.h>
#include <tdimessages.h>
#include <mdsshr.h>

static char *cvsrev = "@(#)$RCSfile$ $Revision$ $Date$";

extern unsigned short OpcDescr;
extern unsigned short OpcRef;
extern unsigned short OpcVal;
extern unsigned short OpcXd;

extern int TdiConcat();
extern int TdiData(  );
extern int TdiEvaluate(  );
extern int TdiFaultHandler(  );
extern int TdiFindImageSymbol(  );
extern int TdiGetLong(  );
extern int TdiPutIdent(  );

static int TdiInterlude  (int opcode, struct descriptor **newdsc, int (*routine)(  ), unsigned int *(*called)(  ),
                          void **result, int *max)
{
#if  defined(__ALPHA) && defined(__VMS)
        int f_regs = (*(int *)routine == 0x23FF0000) ? 0 : 1;
#else
        int f_regs = (opcode == 0);
#endif
	LibEstablish(TdiFaultHandler);
        switch (opcode)
        {
          case DTYPE_F:
            if (f_regs)
            { float (*called_f)() = (float (*)())called;
              float *result_f = (float *)result;
              *max = sizeof(float);
  	      *result_f = (*called_f)(newdsc, routine);
            }
            else
            { 
              *max = sizeof(float);
	      *result = (*called)(newdsc, routine);
            }
            break;
          case DTYPE_D:
          case DTYPE_G:
	  case DTYPE_T:
          case DTYPE_FC:
	  case DTYPE_FSC:
            if (f_regs)
            { double (*called_g)() = (double (*)())called;
              double *result_g = (double *)result;
              *max = sizeof(double);
  	      *result_g = (*called_g)(newdsc, routine);
              break;
            }
#if  defined(__ALPHA) && defined(__VMS)
            else
            { __int64 (*called_g)() = (__int64 (*)())called;
              __int64 *result_g = (__int64 *)result;
              *max = sizeof(double);
  	      *result_g = (*called_g)(newdsc, routine);
              break;
            }
#endif
          default:
            *max = sizeof(int);
	    *result = (*called)(newdsc, routine);
        }
	return 1;
}


TdiRefStandard(TdiCall)
struct descriptor_function	*pfun;
struct descriptor_xd	image = EMPTY_XD, entry = EMPTY_XD, tmp[255];
int				j, max, ntmp = 0, (*routine)(  );
struct descriptor *result[2] = {0,0};
unsigned short			code;
struct descriptor		*newdsc[256];
struct descriptor		dx = {0,0,CLASS_S,0};
unsigned char			origin[255];
        dx.dtype = (unsigned char)opcode;
        dx.pointer = (char *)result;
	LibEstablish(TdiFaultHandler);
	memset(newdsc,0,sizeof(newdsc));
	if (narg > 255+2) status = TdiNDIM_OVER;
	else status = TdiData(list[0], &image MDS_END_ARG);
	if (status & 1) status = TdiData(list[1], &entry MDS_END_ARG);
	if (status & 1) status = TdiFindImageSymbol(image.pointer, entry.pointer, &routine);
	MdsFree1Dx(&entry, NULL);
	MdsFree1Dx(&image, NULL);

	newdsc[0] = (struct descriptor *)(narg-2);
	for (j = 2; j < narg && status & 1; ++j) {
		for (pfun = (struct descriptor_function *)list[j];
                             pfun && pfun->dtype == DTYPE_DSC;) 
                  pfun = (struct descriptor_function *)pfun->pointer;
		if (pfun && pfun->dtype == DTYPE_FUNCTION) {
			code = *(unsigned short *)pfun->pointer;
			if (code == OpcDescr) {
				tmp[ntmp] = EMPTY_XD;
				status = TdiData(pfun->arguments[0], &tmp[ntmp] MDS_END_ARG);
				newdsc[j-1] = (struct descriptor *)tmp[ntmp].pointer;
				origin[ntmp++] = (unsigned char)j;
			}
			else if (code == OpcRef) {
				tmp[ntmp] = EMPTY_XD;
				status = TdiData(pfun->arguments[0], &tmp[ntmp] MDS_END_ARG);
				if (tmp[ntmp].pointer)
				{
                                  if (tmp[ntmp].pointer->dtype == DTYPE_T)
				  {
                                    DESCRIPTOR(zero,"\0");
                                    TdiConcat(&tmp[ntmp],&zero,&tmp[ntmp] MDS_END_ARG);
                                  }  
                                  newdsc[j-1] = (struct descriptor *)tmp[ntmp].pointer->pointer;
				}
                                origin[ntmp++] = (unsigned char)j;
			}
			else if (code == OpcVal) 
                                status = TdiGetLong(pfun->arguments[0], &newdsc[j-1]);
			else if (code == OpcXd) {
				tmp[ntmp] = EMPTY_XD;
				status = TdiEvaluate(pfun->arguments[0], 
                                        newdsc[j-1] = (struct descriptor *)&tmp[ntmp] MDS_END_ARG);
				origin[ntmp++] = (unsigned char)j;
			}
			else goto fort;
		}
		/********************************************
		Default is DESCR for text and REF for others.
		********************************************/
		else {
fort:			tmp[ntmp] = EMPTY_XD;
			if (list[j]) status = TdiData(list[j], &tmp[ntmp] MDS_END_ARG);
                        newdsc[j-1] = tmp[ntmp].pointer;
			if (newdsc[j-1])
                        {
                          if (newdsc[j-1]->dtype != DTYPE_T)
			    newdsc[j-1] = (struct descriptor *)newdsc[j-1]->pointer;
                          else
			  {
                            DESCRIPTOR(zero_dsc,"\0");
                            TdiConcat(&tmp[ntmp],&zero_dsc,&tmp[ntmp] MDS_END_ARG);
                            newdsc[j-1] = (struct descriptor *)tmp[ntmp].pointer->pointer;
                          }
                        }
			origin[ntmp++] = (unsigned char)j;
		}
	}
        if (status &1) status = TdiInterlude(opcode,newdsc, routine, (void *)LibCallg, (void **)result,&max);
	if (!out_ptr) goto skip;
	if (status & 1) switch (opcode) {
	case DTYPE_DSC :
                dx.pointer = (char *)result[0];
		if (result[0]) switch (result[0]->class) {
		case CLASS_XD :
			MdsFree1Dx(out_ptr, NULL);
			*out_ptr = *(struct descriptor_xd *)result[0];
			*(struct descriptor_xd *)result[0] = EMPTY_XD;
			goto skip;
		case CLASS_D :
			MdsFree1Dx(out_ptr, NULL);
			*(struct descriptor *)out_ptr = *result[0];
			result[0]->length = 0;
			result[0]->pointer = 0;
			goto skip;
		default :
			break;
		}
		break;
	case DTYPE_T :
	case DTYPE_PATH :
	case DTYPE_EVENT :
		dx.length = (unsigned short)strlen(dx.pointer = (char *)result[0]);
		break;
	case DTYPE_NID:
		dx.length = sizeof(int);
		break;
	default :
		if (opcode < TdiCAT_MAX) {
			if ((dx.length = TdiREF_CAT[opcode].length) > max) status = TdiTOO_BIG;
		}
		else status = TdiINVDTYDSC;
	}
	if (status & 1) status = MdsCopyDxXd(&dx, out_ptr);
skip:
	for (j = 0; j < ntmp; ++j) {
		for (pfun = (struct descriptor_function *)list[origin[j]];
                     pfun && pfun->dtype == DTYPE_DSC;) pfun = (struct descriptor_function *)pfun->pointer;
		if (pfun && pfun->dtype == DTYPE_FUNCTION) {
			code = *(unsigned short *)pfun->pointer;
			if (code == OpcDescr || code == OpcRef || code == OpcXd)
				pfun = (struct descriptor_function *)pfun->arguments[0];
			if (pfun && pfun->dtype == DTYPE_IDENT) TdiPutIdent(pfun, &tmp[j]);
		}
		if (tmp[j].pointer) MdsFree1Dx(&tmp[j], NULL);
	}
	return status;
}
