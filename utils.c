/*
 * utils.c
 *
 *  Created on: 2020年5月20日
 *      Author: juntao
 */
#include <time.h>
#include <string.h>
#include <sys/time.h>
#include <stdlib.h>
#include <math.h>
#include <stdio.h>
#include "GNSS_PPP.h"
/* get two component bits ----------------------------------------------------*/
extern unsigned int getbitu(const unsigned char *buff, int pos, int len) {
	unsigned int bits = 0;
	int i;
	for (i = pos; i < pos + len; i++)
		bits = (bits << 1) + ((buff[i / 8] >> (7 - i % 8)) & 1u);
	return bits;
}
extern int getbits(const unsigned char *buff, int pos, int len) {
	unsigned int bits = getbitu(buff, pos, len);
	if (len <= 0 || 32 <= len || !(bits & (1u << (len - 1))))
		return (int) bits;
	return (int) (bits | (~0u << len)); /* extend sign */
}
char prn_syssig(int sys) {
	char a = 'a';
	if (sys == 0)
		a = 'C';       //C
	else if (sys == 1)
		a = 'G';
	else if (sys == 2)
		a = 'E';
	else if (sys == 3)
		a = 'R';
	if (a == 'a') {
		printf("sys=='a' error \n");
		exit(1);
	}
	return a;
}
int satslot_prn(int prn) {
	int a = -1;
	if (1 <= prn && prn <= 63)
		a = prn;
	else if (64 <= prn && prn <= 100)
		a = prn - 63;
	else if (101 <= prn && prn <= 137)
		a = prn - 100;
	else if (138 <= prn && prn <= 174)
		a = prn - 137;
	if (a < 0) {
		printf("PRN<0 error \n");
		exit(1);
	}
	return a;
}
int syssig_prn(int prn) {
	int a = -1;
	if (1 <= prn && prn <= 63)
		a = 0;       //C
	else if (64 <= prn && prn < 100)
		a = 1;
	else if (101 <= prn && prn < 137)
		a = 2;
	else if (138 <= prn && prn < 174)
		a = 3;
	if (a < 0) {
		printf("sys<0 error \n");
		exit(1);
	}
	return a;
}
int put2(char *ascmsg, char *omsg, unsigned int b) {
	int a = 0, r = 0;
	if (b == 0) {
		printf("acc put2==1 error\n");
		return -1;
	}
	for (a = 0; a < b; a += 2) {
		omsg[r] = ascmsg[a] << 4;
		omsg[r] += ascmsg[a + 1];
		r++;
	}
	return 0;
}
bool asco(unsigned char *ascmsg, unsigned char *omsg, char *pd) {
	int b = 0;
	if (strcmp(pd, "B1C") == 0)
		b = 220;
	else if (strcmp(pd, "B2a") == 0)
		b = 72;
	else if (strcmp(pd, "B1A") == 0)
		b = 74;
	else if (strcmp(pd, "B3A") == 0)
		b = 74;
	else if (strcmp(pd, "B2b") == 0)
		b = 125;
	else if (strcmp(pd, "b3i") == 0)
		b = 75;
	else if (strcmp(pd, "E5A") == 0)
		b = 61;
	else if (strcmp(pd, "E5B") == 0)
		b = 32;
	else if (strcmp(pd, "L1C") == 0)
		b = 221;
	else if (strcmp(pd, "L5") == 0)
		b = 75;
	else if (strcmp(pd, "B1I") == 0 || strcmp(pd, "B3I") == 0)
		b = 75;
	else if (strcmp(pd, "B3Q") == 0)
		b = 75;
	else if (strcmp(pd, "SB1C") == 0 || strcmp(pd, "SB2A") == 0)
		b = 63;
	else if (strcmp(pd, "GLO") == 0)
		b = 20;
	else if (strcmp(pd, "PPP") == 0)
		b = 122;
	else if (strcmp(pd, "SB1A") == 0)
		b = 30;
	if (b == 0) {
		printf("asco fun error\n");
		exit(1);
	}
	int a = 0;
	for (a = 0; a < b; a++) {
		if ('0' <= ascmsg[a] && ascmsg[a] <= '9')
			ascmsg[a] -= 48;
		else if ('a' <= ascmsg[a] && ascmsg[a] <= 'z')
			ascmsg[a] -= 87;
		else if ('A' <= ascmsg[a] && ascmsg[a] <= 'Z')
			ascmsg[a] -= 55;
	}
	put2(ascmsg, omsg, b);
	return true;
}
int subtype_prn(char* make,int subtype,int ix){
	int i,ipos = subtype * 23 + ix,b = 0;
	for(i = 0 ; i < IF_MAXSAT;i++){
		if(make[i] == 1) b++;
		if(b == ipos) return i;
	}
	return -1;
}
void wksow2mjd(int week,double sow,int* mjd,double* sod){
	if(mjd!=NULL)
		*mjd = (int)(sow/86400.0) + week*7 + 44244;
	if(sod!=NULL)
		*sod = fmod(sow,86400.0);
}
void mjd2doy(int jd, int* iyear, int* idoy) {
	*iyear = (jd + 678940) / 365;
	*idoy = jd - md_julday(*iyear, 1, 1);
	while (*idoy <= 0) {
		(*iyear)--;
		*idoy = jd - md_julday(*iyear, 1, 1) + 1;
	}
}
void mjd2wksow(int mjd, double sod, int *week, double *sow) {
	*week = (int) ((mjd + sod / 86400.0 - 44244.0) / 7.0);
	*sow = (mjd - 44244.0 - *week * 7) * 86400.0 + sod;
}
void yeardoy2monthday(int iyear, int idoy, int* imonth, int* iday) {
	int days_in_month[12] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };
	int id, i;
	if ((iyear % 4 == 0 && iyear % 100 != 0) || iyear % 400 == 0)
		days_in_month[1] = 29;
	id = idoy;
	for (i = 0; i < 12; i++) {
		id = id - days_in_month[i];
		if (id > 0)
			continue;
		*iday = id + days_in_month[i];
		*imonth = i + 1;
		break;
	}
}
int md_julday(int iyear,int imonth,int iday){
	int iyr, result;
	int doy_of_month[12] = { 0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304,
			334 };
	if (iyear < 0 || imonth < 0 || iday < 0 || imonth > 12 || iday > 366
			|| (imonth != 0 && iday > 31)) {
		printf("iyear = %d,imonth = %d,iday = %d,incorrect argument",iyear,imonth,iday);
		exit(1);
	}
	iyr = iyear;
	if (imonth <= 2)
		iyr -= 1;
	result = 365 * iyear - 678941 + iyr / 4 - iyr / 100 + iyr / 400 + iday;
	if (imonth != 0)
		result = result + doy_of_month[imonth - 1];
	return result;
}
void mjd2date(int jd, double sod, int* iyear, int* imonth, int* iday, int* ih,
		int* imin, double* sec) {
	int doy = 0;
	mjd2doy(jd, iyear, &doy);
	yeardoy2monthday(*iyear, doy, imonth, iday);

	*ih =  (int)( sod / 3600.0);
	*imin = (int)((sod - (*ih) * 3600.0) / 60.0);
	*sec = sod - (*ih) * 3600.0 - (*imin) * 60.0;
}
void m_outorbit(ppp_ssr_orbit* orbit){
	int nsum = 0,week,iyear,imonth,iday,ih,im,mjd,nsat = 0;
	double dsec,sod;
	char SYS[4] = {'C','G','E','R'};
	week = orbit->bweek + 1356.0;
	wksow2mjd(week, orbit->bsow, &mjd, &sod);
	mjd2date(mjd,sod,&iyear,&imonth,&iday,&ih,&im,&dsec);
	for(int isat = 0;isat < IF_MAXSAT;isat++){
		if(orbit->iode[isat] == -1) continue;
		nsat++;
	}
	fprintf(ssr_config.fp_output,"> ORBIT %4d %02d %02d %02d %02d %4.1lf %d %d CLK01\n",iyear,imonth,iday,ih,im,dsec,2,nsat);
	for(int isat = 0;isat < IF_MAXSAT;isat++){
		if(orbit->iode[isat] == -1) continue;
		fprintf(ssr_config.fp_output,"%c%02d  %10d%11.4f%11.4f%11.4f%11.4f%11.4f%11.4f\n",SYS[syssig_prn(isat + 1)],satslot_prn(isat + 1),orbit->iode[isat],orbit->RAC[isat][0],orbit->RAC[isat][1],orbit->RAC[isat][2],0.0,0.0,0.0);
	}
}
void m_outclock(ppp_ssr_clock* clock){
	/** find the iode here **/
	int week,iyear,imonth,iday,ih,im,nsat = 0,mjd;
	double dsec,sod;
	char SYS[4] = {'C','G','E','R'};
	week = clock->bweek + 1356.0;
	wksow2mjd(week, clock->bsow, &mjd, &sod);
	mjd2date(mjd,sod,&iyear,&imonth,&iday,&ih,&im,&dsec);
	for(int isat = 0;isat < IF_MAXSAT;isat++){
		if(clock->iode[isat] == -1) continue;
		nsat++;
	}
	fprintf(ssr_config.fp_output,"> CLOCK %4d %02d %02d %02d %02d %4.1lf %d %d CLK01\n",iyear,imonth,iday,ih,im,dsec,2,nsat);
	for(int isat = 0;isat < IF_MAXSAT;isat++){
		if(clock->iode[isat] == -1) continue;
		fprintf(ssr_config.fp_output,"%c%02d  %10d%11.4f%11.4f%11.4f\n",SYS[syssig_prn(isat + 1)],satslot_prn(isat + 1),clock->iode[isat],clock->C0[isat],0.0,0.0);
	}
	fflush(ssr_config.fp_output);
}
void m_outcodebias(ppp_ssr_codebias* codebias){
	int i,isat,idx,gotdata;
	char filename[1024] = {0},SYS[4] = {"CGRE"},cprn[12] = {0};
	FILE* fp = NULL;
	char biascode[4][16][12] = {
			{"B1I","B1C(d)","B1C(p)","-","B2a(D)","B2a(P)","-","B2b-I","B2b-Q","-","","-","B3I","-","-","-"},
			{"L1 C/A","L1 P","","","L1C(P)","L1C(D+P)","","L2C(L)","L2C(M+L)","","","L5I","L5Q","L5I+Q","",""},
			{"G1 C/A","G1 P","G2 C/A","","","","","","","","","","","","",""},
			{"","E1 B","E1 C","","E5a Q","E5a I","","E5b I","E5b Q","","","E6 C","","","",""}
	};
	for(i = 0; i < sizeof(SYS) / sizeof(char);i++){
		sprintf(filename,"%s_%c_%d","codebias",SYS[i],codebias->SSR);
		fp = fopen(filename,"w");
		/* output the header here */
		fprintf(fp,"%12s"," ");
		for(idx = 0;idx < 16;idx++){
			fprintf(fp,"%12s",biascode[i][idx]);
		}
		fprintf(fp,"\n");
		for(isat = 0;isat < IF_MAXSAT;isat++){
			for(idx = 0,gotdata = 0;idx < 16;idx++){
				if(codebias->codebias[isat][idx] != 0) gotdata = 1;
			}
			if(gotdata != 1) continue;
			if(SYS[i] != SYS[syssig_prn(isat+1)]) continue;
			/* output the corresponding code bias here */

			sprintf(cprn,"%c%02d",SYS[syssig_prn(isat + 1)],satslot_prn(isat + 1));
			fprintf(fp,"%12s",cprn);
			for(idx = 0;idx < 16;idx++){
				fprintf(fp,"%12.3lf",codebias->codebias[isat][idx]);
			}
			fprintf(fp,"\n");
		}
	}
}
