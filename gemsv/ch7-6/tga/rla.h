/*
 * This software is copyrighted as noted below.  It may be freely copied,
 * modified, and redistributed, provided that the copyright notice is
 * preserved on all copies.
 *
 * There is no warranty or other guarantee of fitness for this software,
 * it is provided solely "as is".  Bug reports or fixes may be sent
 * to the author, who may or may not act on them as he desires.
 *
 * You may not include this software in a program or other software product
 * without supplying the source, or without informing the end-user that the
 * source is available for no extra charge.
 *
 * If you modify this software, you should include a notice giving the
 * name of the person performing the modification, the date of modification,
 * and the reason for such modification.
 */
/*
 * rla.h - type define to Wavefront's RLA format.
 *
 * Author:      Raul Rivero
 *              Mathematics Dept.
 *              University of Oviedo
 * Date:        Sat Jan 4 1992
 * Copyright (c) 1992, Raul Rivero
 *
 */

#ifndef MY_RLA

#define MY_RLA

typedef struct {
   short left, right, bottom, top;
} WINDOW_S;

typedef struct {
   WINDOW_S     window;
   WINDOW_S     act_window;
   short        frame;
   short        storage_type;
   short        num_chan;
   short        num_matte;
   short        num_aux;
   short        aux_mask;
   char         gamma[16];
   char         red_pri[24];
   char         green_pri[24];
   char         blue_pri[24];
   char         white_pt[24];
   long         job_num;
   char         name[128];
   char         desc[128];
   char         program[64];
   char         machine[32];
   char         user[32];
   char         date[20];
   char         aspect[32];
   char         chan[32];
   char         space[128];
} RLA_HEADER;

#ifdef MSDOS
#define cuserid(a)              "nouser"
#define gethostname(a, b)       strcpy( a, "my_PC_with_MSDOS" )
#endif

#endif       /* MY_RLA */
