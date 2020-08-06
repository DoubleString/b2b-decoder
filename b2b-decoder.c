/*
 * b2b-decoder.c
 *
 *  Created on: 2020/5/20/
 *      Author: juntao
 */
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
#define GLOBAL_B2B
#include "GNSS_PPP.h"
#undef GLOBAL_B2B
bool gnssinit(const char* ssrfile,const char* outfile) {
	int i;
	memset(ssr_orbits,0,sizeof(ssr_orbits));
	memset(ssr_clocks,0,sizeof(ssr_clocks));
	memset(ssr_masks,0,sizeof(ssr_masks));
	memset(ssr_codebias,0,sizeof(ssr_codebias));
	memset(&ssr_config,0,sizeof(ssr_config));
	ssr_orbit_count = 0;
	ssr_clock_count = 0;
	ssr_mask_count = 0;
	/* initilize the SSR type */
	for(i = 0; i < IF_MAXSSRTYPE;i++)
		ssr_codebias[i].SSR = -1;
	strcpy(ssr_config.CK_FILE[0],ssrfile);
	strcpy(ssr_config.Machine_number,"sz001");
	strcpy(ssr_config.Site_number,"BJ03");
	strcpy(ssr_config.DATA_PPP_FILENAME,"b2b_outfile");

	if(!(ssr_config.fp_output = fopen(outfile,"w"))){
		printf("file = %s,can't create output file to write\n",outfile);
		exit(1);
	}
	return true;
}
bool b2b_parse_ppp(unsigned char* datapackage,pppdata* PPPB2B){
	int b;
	unsigned int type = getbitu(datapackage, 0, 6);
	PPPB2B->mestype = type;
	PPPB2B->BDSsod = getbitu(datapackage, 6, 17);
	PPPB2B->nudata = getbitu(datapackage, 23, 4);
	PPPB2B->SSR = getbitu(datapackage, 27, 2);
	PPPB2B->CRC = getbitu(datapackage, 462, 24);
	int r = 29, a = 0;
	if (8 <= PPPB2B->mestype && PPPB2B->mestype <= 63) {
		return true;
	}
	switch (type) {
	case 1:
		PPPB2B->type.type1.IODP = getbitu(datapackage, 29, 4);
		for (a = 0; a < IF_MAXSAT; a++) {
			PPPB2B->type.type1.prn_make[a] = getbitu(datapackage, 33 + a, 1);
		}
		for (a = 0; a < 174; a++) {
			PPPB2B->type.type1.sub1NULL[a] = getbitu(datapackage, 288 + a, 1);
		}
		break;
	case 2:
		for (a = 0; a < 6; a++) {
			PPPB2B->type.type2.trasub[a].satslot = getbitu(datapackage, r, 9), r +=
					9;
			PPPB2B->type.type2.trasub[a].IODN = getbitu(datapackage, r, 10), r +=
					10;
			PPPB2B->type.type2.trasub[a].IODCorr = getbitu(datapackage, r, 3), r +=
					3;
			PPPB2B->type.type2.trasub[a].radial = getbits(datapackage, r, 15)
					* 0.0016, r += 15;
			PPPB2B->type.type2.trasub[a].Tangentialdir = getbits(datapackage, r,
					13) * 0.0064, r += 13;
			PPPB2B->type.type2.trasub[a].Normaldir = getbits(datapackage, r, 13)
					* 0.0064, r += 13;
			PPPB2B->type.type2.trasub[a].URAclass = getbitu(datapackage, r, 3), r +=
					3;
			PPPB2B->type.type2.trasub[a].URAvalue = getbitu(datapackage, r, 3), r +=
					3;
			PPPB2B->type.type2.trasub[a].ura = pow(3,
					PPPB2B->type.type2.trasub[a].URAclass)
					* (1 + 0.25 * PPPB2B->type.type2.trasub[a].URAvalue) - 1;
		}
		PPPB2B->type.type2.sub1NULL = getbitu(datapackage, r, 19);
		break;
	case 3:
		b = 0;
		PPPB2B->type.type3.num = getbitu(datapackage, r, 5), r += 5;
		for (a = 0; a < PPPB2B->type.type3.num; a++) {
			PPPB2B->type.type3.intersub3[a].satslot = getbitu(datapackage, r,
					9), r += 9;
			PPPB2B->type.type3.intersub3[a].num4 = getbitu(datapackage, r, 4), r +=
					4;
			for (b = 0; b < PPPB2B->type.type3.intersub3[a].num4; b++) {
				PPPB2B->type.type3.intersub3[a].pattern[b] = getbitu(
						datapackage, r, 4), r += 4;
				PPPB2B->type.type3.intersub3[a].deciation[b] = getbits(
						datapackage, r, 12) * 0.017, r += 12;
			}
		}
		break;
	case 4:
		PPPB2B->type.type4.IODP = getbitu(datapackage, r, 4), r += 4;
		PPPB2B->type.type4.subtupe1 = getbitu(datapackage, r, 5), r += 5;
		for (a = 0; a < 23; a++) {
			PPPB2B->type.type4.IDO_corr[a] = getbitu(datapackage, r, 3), r += 3;
			PPPB2B->type.type4.c[a] = getbits(datapackage, r, 15) * 0.0016, r +=
					15;
		}
		PPPB2B->type.type4.REV = getbitu(datapackage, r, 10);
		break;
	case 5:
		PPPB2B->type.type5.IODP = getbitu(datapackage, r, 4), r += 4;
		PPPB2B->type.type5.subtupe2 = getbitu(datapackage, r, 3), r += 3;
		for (a = 0; a < 70; a++) {
			PPPB2B->type.type5.URAclass[a] = getbitu(datapackage, r, 3), r += 3;
			PPPB2B->type.type5.URAvalue[a] = getbitu(datapackage, r, 3), r += 3;
		}
		break;
	case 6:
		PPPB2B->type.type6.NUMC = getbitu(datapackage, r, 5), r += 5;
		PPPB2B->type.type6.NUMO = getbitu(datapackage, r, 3), r += 3;

		PPPB2B->type.type6.sub6_clock.toc = getbitu(datapackage, r, 17), r += 17;
		PPPB2B->type.type6.sub6_clock.DEV = getbitu(datapackage, r, 4), r += 4;
		PPPB2B->type.type6.sub6_clock.IOD_SSR = getbitu(datapackage, r, 2), r +=
				2;
		PPPB2B->type.type6.sub6_clock.IOPD = getbitu(datapackage, r, 4), r += 4;
		PPPB2B->type.type6.sub6_clock.Slot_S = getbitu(datapackage, r, 9), r +=
				9;
		for (a = 0; a < PPPB2B->type.type6.NUMC; a++) {
			PPPB2B->type.type6.sub6_clock.clock_N_sub6[a].IOD_corr = getbitu(
					datapackage, r, 3), r += 3;
			PPPB2B->type.type6.sub6_clock.clock_N_sub6[a].Co = getbitu(
					datapackage, r, 15), r += 15;
		}
		PPPB2B->type.type6.sub6_tarck.tot = getbitu(datapackage, r, 17), r += 17;
		PPPB2B->type.type6.sub6_tarck.DEV = getbitu(datapackage, r, 4), r += 4;
		PPPB2B->type.type6.sub6_clock.IOD_SSR = getbitu(datapackage, r, 2), r += 2;
		for (a = 0; a < PPPB2B->type.type6.NUMO; a++) {
			PPPB2B->type.type6.sub6_tarck.numO[a].satslot = getbitu(datapackage,
					r, 9), r += 9;
			PPPB2B->type.type6.sub6_tarck.numO[a].IODN = getbitu(datapackage, r,
					10), r += 10;
			PPPB2B->type.type6.sub6_tarck.numO[a].IODCorr = getbitu(datapackage,
					r, 3), r += 3;
			PPPB2B->type.type6.sub6_tarck.numO[a].radial = getbits(datapackage,
					r, 15) * 0.0016, r += 15;
			PPPB2B->type.type6.sub6_tarck.numO[a].Tangentialdir = getbits(
					datapackage, r, 13) * 0.0064, r += 13;
			PPPB2B->type.type6.sub6_tarck.numO[a].Normaldir = getbits(
					datapackage, r, 13) * 0.0064, r += 13;
			PPPB2B->type.type6.sub6_tarck.numO[a].URAclass = getbitu(
					datapackage, r, 3), r += 3;
			PPPB2B->type.type6.sub6_tarck.numO[a].URAvalue = getbitu(
					datapackage, r, 3), r += 3;
		}
		break;
	case 7:
		PPPB2B->type.type7.NUMC = getbitu(datapackage, r, 5), r += 5;
		PPPB2B->type.type7.NUMO = getbitu(datapackage, r, 3), r += 3;
		PPPB2B->type.type7.sub7_clock.toc = getbitu(datapackage, r, 17), r += 17;
		PPPB2B->type.type7.sub7_clock.DEV = getbitu(datapackage, r, 4), r += 4;
		PPPB2B->type.type7.sub7_clock.IOD_SSR = getbitu(datapackage, r, 2), r +=
				2;
		for (a = 0; a < PPPB2B->type.type6.NUMC; a++) {
			PPPB2B->type.type7.sub7_clock.clock_sub7[a].satslot = getbitu(
					datapackage, r, 9), r += 9;
			PPPB2B->type.type7.sub7_clock.clock_sub7[a].IOD_corr = getbitu(
					datapackage, r, 3), r += 3;
			PPPB2B->type.type7.sub7_clock.clock_sub7[a].Co = getbitu(
					datapackage, r, 15), r += 15;
		}
		PPPB2B->type.type7.sub7_tarck.tot = getbitu(datapackage, r, 17), r += 17;
		PPPB2B->type.type7.sub7_tarck.DEV = getbitu(datapackage, r, 4), r += 4;
		PPPB2B->type.type7.sub7_tarck.IOD_SSR = getbitu(datapackage, r, 2), r +=
				2;
		for (a = 0; a < PPPB2B->type.type7.NUMO; a++) {
			PPPB2B->type.type7.sub7_tarck.numO[a].satslot = getbitu(datapackage,
					r, 9), r += 9;
			PPPB2B->type.type7.sub7_tarck.numO[a].IODN = getbitu(datapackage, r,
					10), r += 10;
			PPPB2B->type.type7.sub7_tarck.numO[a].IODCorr = getbitu(datapackage,
					r, 3), r += 3;
			PPPB2B->type.type7.sub7_tarck.numO[a].radial = getbits(datapackage,
					r, 15) * 0.0016, r += 15;
			PPPB2B->type.type7.sub7_tarck.numO[a].Tangentialdir = getbits(
					datapackage, r, 13) * 0.0064, r += 13;
			PPPB2B->type.type7.sub7_tarck.numO[a].Normaldir = getbits(
					datapackage, r, 13) * 0.0064, r += 13;
			PPPB2B->type.type7.sub7_tarck.numO[a].URAclass = getbitu(
					datapackage, r, 3), r += 3;
			PPPB2B->type.type7.sub7_tarck.numO[a].URAvalue = getbitu(
					datapackage, r, 3), r += 3;
		}
		break;
	default:
		printf("parse_PPPB2B  error\n");
		return false;
	}
	return true;
}
int b2b_updateiode(int SSR,int prn,int iodcrc){
	for(int i = ssr_orbit_count - 1; i >= (ssr_orbit_count - 8 >= 0 ? ssr_orbit_count - 8 : 0);i--){
		if(ssr_orbits[i].SSR != SSR) continue;
		if(ssr_orbits[i].iodcorr[prn] == iodcrc)
			return ssr_orbits[i].iode[prn];
	}
	return -1;
}
void b2b_fillmem(pppdata* p_sbas){
	int i,prn,ipos,j,nsum,bgot,ic;
	ppp_ssr_orbit *ptr_fill = NULL;
	ppp_ssr_mask* ptr_mask = NULL;
	ppp_ssr_clock* ptr_clk = NULL,ssr_clk = {0};;
	/*********** fill the memory here using ppp_sbas data *******************/
	switch(p_sbas->mestype){
	case 1:
		/** means a new mask mesage is received **/
		for(i = 0,bgot = false; i < ssr_mask_count;i++){
			if(ssr_masks[i].SSR != p_sbas->SSR) continue;
			if(ssr_masks[i].iodp == p_sbas->type.type1.IODP){
				bgot = true;
			}
		}
		if(bgot == false){
			/***a new mask type**/
			if (ssr_mask_count >= IF_MAXMASK) {
				for (i = 0; i < ssr_mask_count - 1; i++) {
					ssr_masks[i] = ssr_masks[i + 1];
				}
				ssr_mask_count--;
			}
			ptr_mask = ssr_masks + ssr_mask_count++;
			ptr_mask->SSR = p_sbas->SSR;
			ptr_mask->iodp = p_sbas->type.type1.IODP;
			memcpy(ptr_mask->cmake,p_sbas->type.type1.prn_make,sizeof(char) * IF_MAXSAT);
		}
		break;
	case 2:
		/** orbit information here **/
		ipos = -1;
		for(i = 0; i < ssr_orbit_count;i++){
			if(ssr_orbits[i].SSR != p_sbas->SSR || ssr_orbits[i].bweek != p_sbas->BDSweek || (int)ssr_orbits[i].bsow != (int)p_sbas->BDSsow) continue;
			ipos = i;
			break;
		}
		ptr_fill = ssr_orbits + ipos;
		if(-1 == ipos){
			/**** means this is a new information ***/
			if(ssr_orbit_count >= IF_MAXSSR){
				for(i = 0; i < ssr_orbit_count - 1;i++){
					ssr_orbits[i] = ssr_orbits[i + 1];
				}
				ssr_orbit_count--;
			}
			ptr_fill = ssr_orbits + ssr_orbit_count++;
			ptr_fill->bweek = p_sbas->BDSweek;
			ptr_fill->bsow = p_sbas->BDSsow;
			ptr_fill->SSR = p_sbas->SSR;
			for(i = 0; i < IF_MAXSAT;i++) ptr_fill->iodcorr[i] = -1; /**initialize the iode here***/
			for(i = 0; i < IF_MAXSAT;i++) ptr_fill->iode[i] = -1; /**initialize the iode here***/
			for(i = ssr_orbit_count - 1,nsum = 0; i >= 0;i--){
				if(ssr_orbits[i].SSR == p_sbas->SSR) if(++nsum == 2) ipos = i; /**do not get the last index,get the last second index**/
			}
			if(nsum >= 2) m_outorbit(&ssr_orbits[ipos]);
		}
		/**** just fill the structure ****/
		for(i = 0; i < 6;i++){
			if(p_sbas->type.type2.trasub[i].ura == 0)
				continue;
			prn = p_sbas->type.type2.trasub[i].satslot - 1;
			ptr_fill->RAC[prn][0]= p_sbas->type.type2.trasub[i].radial;
			ptr_fill->RAC[prn][1]= p_sbas->type.type2.trasub[i].Tangentialdir;
			ptr_fill->RAC[prn][2]= p_sbas->type.type2.trasub[i].Normaldir;
			ptr_fill->iodcorr[prn] = p_sbas->type.type2.trasub[i].IODCorr;
			ptr_fill->iode[prn] = p_sbas->type.type2.trasub[i].IODN;
			ptr_fill->ura[prn] = p_sbas->type.type2.trasub[i].ura;
		}
		break;
	case 3:
		/***** bias correction here *******/
		ipos = -1;
		/* mark the position in the memory */
		for(i = 0;i < IF_MAXSSRTYPE;i++){
			if(p_sbas->SSR != ssr_codebias[i].SSR) continue;
			ipos = i;
			break;
		}
		/* find the position here */
		if(ipos == -1){
			for(i = 0;i < IF_MAXSSRTYPE;i++){
				if(ssr_codebias[i].SSR != -1) continue;
				ipos = i;
				break;
			}
		}
		if(ipos != -1){
			ssr_codebias[ipos].SSR = p_sbas->SSR;
			/* save the corresponding code bias here */
			for(i = 0; i < p_sbas->type.type3.num ;i++){
				prn = p_sbas->type.type3.intersub3[i].satslot - 1;
				for(ic = 0;ic < p_sbas->type.type3.intersub3[i].num4;ic++){
					j = p_sbas->type.type3.intersub3[i].pattern[ic];
					ssr_codebias[ipos].codebias[prn][j] = p_sbas->type.type3.intersub3[i].deciation[ic];
				}
			}
		}
		break;
	case 4:
		/****** clock information *********/
		ipos = -1;
		for(i = 0; i < ssr_clock_count;i++){
			if(ssr_clocks[i].SSR != p_sbas->SSR || ssr_clocks[i].bweek != p_sbas->BDSweek || (int)ssr_clocks[i].bsow != (int)p_sbas->BDSsow) continue; /**not the same configure,the same time**/
			ipos = i;
			break;
		}
		ptr_clk = ssr_clocks + ipos;
		if(ipos == -1){
			if(ssr_clock_count >= IF_MAXSSR){
				for(i = 0; i < ssr_clock_count - 1;i++){
					ssr_clocks[i] = ssr_clocks[i + 1];
				}
				ssr_clock_count--;
			}
			ptr_clk = ssr_clocks + ssr_clock_count++;
			ptr_clk->SSR = p_sbas->SSR;
			ptr_clk->bweek = p_sbas->BDSweek;
			ptr_clk->bsow = p_sbas->BDSsow;
			ptr_clk->iodp = p_sbas->type.type4.IODP;
			for(i = 0; i < IF_MAXSAT;i++) ptr_clk->iode[i] = -1; /**initialize the iode here***/
			for(i = 0; i < IF_MAXSAT;i++) ptr_clk->iodcorr[i] = -1; /**initialize the iode here***/
			for(i = ssr_clock_count - 1,nsum = 0; i >= 0;i--){
				if(ssr_clocks[i].SSR == p_sbas->SSR)
					if(++nsum == 2)
						ipos = i;
			}
			if(nsum >= 2) m_outclock(&ssr_clocks[ipos]);
		}
		/*** find the mask ***/
		for(i = 0,ptr_mask = NULL; i < ssr_mask_count;i++){
			if(ssr_masks[i].iodp == ptr_clk->iodp){
				ptr_mask = ssr_masks + i;
				break;
			}
		}
		for(i = 0; i < 23 && ptr_mask != NULL;i++){
			prn = subtype_prn(ptr_mask->cmake,p_sbas->type.type4.subtupe1,i+1);
			if(prn == -1) continue;
			ptr_clk->C0[prn] = p_sbas->type.type4.c[i];
			if(fabs(fabs(ptr_clk->C0[prn]) - 26.2128) < 0.01 || fabs(ptr_clk->C0[prn]) < 0.001) continue;
			ptr_clk->iodcorr[prn] = p_sbas->type.type4.IDO_corr[i];
			ptr_clk->iode[prn] = b2b_updateiode(ptr_clk->SSR,prn,ptr_clk->iodcorr[prn]);
		}
		break;
	case 5:
		break;
	case 6:
		break;
	case 7:
		break;
	default:
		break;
	}
}
bool b2b_parsecorr(struct Message_header* mh){
	int mjd,week,i;
	double sow;
	pppdata p_sbas = {0};
	p_sbas.BDSsow = mh->current_week_second.BDSsecond;
	p_sbas.BDSweek = mh->current_week_second.BDSweek;
	wksow2mjd(p_sbas.BDSweek,p_sbas.BDSsow,&mjd,&p_sbas.BDSsod);
	p_sbas.prn = mh->current_PRN;
	if(b2b_parse_ppp(mh->data,&p_sbas)){
		for(i = -1;i < 1;i++){
			mjd2wksow(mjd + i,p_sbas.BDSsod,&week,&sow);
			double ddif = (week - p_sbas.BDSweek) * 86400 * 7 + sow - p_sbas.BDSsow;
			if(fabs(ddif) < 43200){
				break;
			}
		}
		mjd2wksow(mjd+i,p_sbas.BDSsod,&p_sbas.BDSweek,&p_sbas.BDSsow);
		b2b_fillmem(&p_sbas);
		/**output the log file here**/
		return true;
	}
	return false;
}
bool b2b_iffun(unsigned char* datapackage,struct datapackage* dp){
	sscanf(datapackage, "#%[^;]", dp->masthead);
	if (memcmp(dp->masthead, "BDRAWNAV", sizeof("BDRAWNAV")) != 0
			&& memcmp(dp->masthead, "GPRAWNAV", sizeof("GPRAWNAV")) != 0
			&& memcmp(dp->masthead, "GARAWNAV", sizeof("GARAWNAV")) != 0
			&& memcmp(dp->masthead, "GLRAWNAV", sizeof("GLRAWNAV")) != 0
			&& memcmp(dp->masthead, "SBRAWNAV", sizeof("SBRAWNAV")) != 0
			&& memcmp(dp->masthead, "PPRAWNAV", sizeof("PPRAWNAV")) != 0) {
		return -1;
	}
	if (dp->masthead[1] != 'L') {
		sscanf(datapackage, "#%[^;];%d,%f,%d,%d,%d,%[0-9a-zA-Z]", dp->masthead,
				&dp->BDSweek, &dp->BDSsecond, &dp->PRN, &dp->signalcomponent,
				&dp->len, dp->msg);
	}
	return dp->len;
}
bool b2b_ifsystem(struct datapackage* dp){
	unsigned char omsg[1024] = {0};
	struct Message_header mess_head;
	if(dp->PRN <= 0) return false;
	if(dp->PRN != 59) return true;
	memset(&mess_head, 0, sizeof(mess_head));
	mess_head.current_PRN = dp->PRN;
	mess_head.current_sin_s = dp->signalcomponent;
	if(dp->masthead[0] == 'P' && dp->masthead[1] == 'P'){
		if (dp->len != 122 && dp->len != 125) {
			printf("PPP len = %d error !\n", dp->len);
			return false;
		}
		if(dp->len == 125){
			char msg2[1024] = {0};
			memcpy(msg2,dp->msg + 3,1024 -3);
			memcpy(dp->msg,msg2,1024);
		}
		asco(dp->msg, omsg, "PPP");
		memcpy(mess_head.data, omsg, sizeof(omsg));
		mess_head.current_week_second.BDSsecond = dp->BDSsecond;
		mess_head.current_week_second.BDSweek = dp->BDSweek;
		mess_head.current_mess_sys = 'C';
		return b2b_parsecorr(&mess_head);
	}
	return true;
}
bool b2b_parse(unsigned char* data){
	struct datapackage dp = {0};
	if(b2b_iffun(data,&dp) < 0 || !b2b_ifsystem(&dp)) return false;
	return true;
}
void b2b_decoder_file(){
	int i;
	FILE* fp = NULL;
	char line[1024] = {0};
	if(!(fp = fopen(ssr_config.CK_FILE[0],"r"))){
		printf("file = %s,can't open file to read!\n",ssr_config.CK_FILE[0]);
		exit(1);
	}
	while(fgets(line,sizeof(line) - 1,fp)){
		if(line[0] == '$') continue;
		if(!b2b_parse(line)) printf("parse %s failed\n",line);
		memset(line,0,sizeof(line));
	}
	fclose(fp);
	/*** output the code bias here ***/
	for(i = 0; i < IF_MAXSSRTYPE;i++){
		if(ssr_codebias[i].SSR == -1) continue;
		m_outcodebias(&ssr_codebias[i]);
	}
}
int main(int argc,char* args[]){
	char ssrfile[1024] = {0},outfile[1024] = {0};
	if(argc < 5){
		printf("Usage: b2b-decoder -ssr ssrfile -out outifle");
		exit(1);
	}
	for(int iargc = 0;iargc < argc;iargc++){
		if(strstr(args[iargc],"-ssr"))
			strcpy(ssrfile,args[++iargc]);
		if(strstr(args[iargc],"-out"))
			strcpy(outfile,args[++iargc]);
	}
	gnssinit(ssrfile,outfile);
	b2b_decoder_file();
	printf("decode success!\n");
}
