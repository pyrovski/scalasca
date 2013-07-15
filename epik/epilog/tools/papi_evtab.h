/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2013                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  Copyright (c) 2003-2008                                                **
**  University of Tennessee, Innovative Computing Laboratory               **
**                                                                         **
**  See the file COPYRIGHT in the package base directory for details       **
****************************************************************************/

#ifndef _PAPI_EVTAB_H
#define _PAPI_EVTAB_H

typedef struct {
  const char* name;
  int   id;
} papi_ev_tab_t;

papi_ev_tab_t papi_ev_tab[] = {
  { "PAPI_BRU_IDL",  16 },
  { "PAPI_BR_CN",    43 },
  { "PAPI_BR_INS",   55 },
  { "PAPI_BR_MSP",   46 },
  { "PAPI_BR_NTK",   45 },
  { "PAPI_BR_PRC",   47 },
  { "PAPI_BR_TKN",   44 },
  { "PAPI_BR_UCN",   42 },
  { "PAPI_BTAC_M",   27 },
  { "PAPI_CA_CLN",   11 },
  { "PAPI_CA_INV",   12 },
  { "PAPI_CA_ITV",   13 },
  { "PAPI_CA_SHR",   10 },
  { "PAPI_CA_SNP",    9 },
  { "PAPI_CSR_FAL",  31 },
  { "PAPI_CSR_SUC",  32 },
  { "PAPI_CSR_TOT",  33 },
  { "PAPI_FAD_INS",  98 },
  { "PAPI_FDV_INS",  99 },
  { "PAPI_FMA_INS",  48 },
  { "PAPI_FML_INS",  97 },
  { "PAPI_FNV_INS", 101 },
  { "PAPI_FPU_IDL",  18 },
  { "PAPI_FP_INS",   52 },
  { "PAPI_FP_OPS",  102 },
  { "PAPI_FP_STAL",  58 },
  { "PAPI_FSQ_INS", 100 },
  { "PAPI_FUL_CCY",  40 },
  { "PAPI_FUL_ICY",  38 },
  { "PAPI_FXU_IDL",  17 },
  { "PAPI_HW_INT",   41 },
  { "PAPI_INT_INS",  51 },
  { "PAPI_L1_DCA",   64 },
  { "PAPI_L1_DCH",   62 },
  { "PAPI_L1_DCM",    0 },
  { "PAPI_L1_DCR",   67 },
  { "PAPI_L1_DCW",   70 },
  { "PAPI_L1_ICA",   76 },
  { "PAPI_L1_ICH",   73 },
  { "PAPI_L1_ICM",    1 },
  { "PAPI_L1_ICR",   79 },
  { "PAPI_L1_ICW",   82 },
  { "PAPI_L1_LDM",   23 },
  { "PAPI_L1_STM",   24 },
  { "PAPI_L1_TCA",   88 },
  { "PAPI_L1_TCH",   85 },
  { "PAPI_L1_TCM",    6 },
  { "PAPI_L1_TCR",   91 },
  { "PAPI_L1_TCW",   94 },
  { "PAPI_L2_DCA",   65 },
  { "PAPI_L2_DCH",   63 },
  { "PAPI_L2_DCM",    2 },
  { "PAPI_L2_DCR",   68 },
  { "PAPI_L2_DCW",   71 },
  { "PAPI_L2_ICA",   77 },
  { "PAPI_L2_ICH",   74 },
  { "PAPI_L2_ICM",    3 },
  { "PAPI_L2_ICR",   80 },
  { "PAPI_L2_ICW",   83 },
  { "PAPI_L2_LDM",   25 },
  { "PAPI_L2_STM",   26 },
  { "PAPI_L2_TCA",   89 },
  { "PAPI_L2_TCH",   86 },
  { "PAPI_L2_TCM",    7 },
  { "PAPI_L2_TCR",   92 },
  { "PAPI_L2_TCW",   95 },
  { "PAPI_L3_DCA",   66 },
  { "PAPI_L3_DCH",   29 },
  { "PAPI_L3_DCM",    4 },
  { "PAPI_L3_DCR",   69 },
  { "PAPI_L3_DCW",   72 },
  { "PAPI_L3_ICA",   78 },
  { "PAPI_L3_ICH",   75 },
  { "PAPI_L3_ICM",    5 },
  { "PAPI_L3_ICR",   81 },
  { "PAPI_L3_ICW",   84 },
  { "PAPI_L3_LDM",   14 },
  { "PAPI_L3_STM",   15 },
  { "PAPI_L3_TCA",   90 },
  { "PAPI_L3_TCH",   87 },
  { "PAPI_L3_TCM",    8 },
  { "PAPI_L3_TCR",   93 },
  { "PAPI_L3_TCW",   96 },
  { "PAPI_LD_INS",   53 },
  { "PAPI_LST_INS",  60 },
  { "PAPI_LSU_IDL",  19 },
  { "PAPI_MEM_RCY",  35 },
  { "PAPI_MEM_SCY",  34 },
  { "PAPI_MEM_WCY",  36 },
  { "PAPI_PRF_DM",   28 },
  { "PAPI_RES_STL",  57 },
  { "PAPI_SR_INS",   54 },
  { "PAPI_STL_CCY",  39 },
  { "PAPI_STL_ICY",  37 },
  { "PAPI_SYC_INS",  61 },
  { "PAPI_TLB_DM",   20 },
  { "PAPI_TLB_IM",   21 },
  { "PAPI_TLB_SD",   30 },
  { "PAPI_TLB_TL",   22 },
  { "PAPI_TOT_CYC",  59 },
  { "PAPI_TOT_IIS",  49 },
  { "PAPI_TOT_INS",  50 },
  { "PAPI_VEC_INS",  56 }
};

#endif
