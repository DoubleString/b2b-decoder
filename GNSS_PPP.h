#ifndef GNSS_PPP_H
#define GNSS_PPP_H
#include <stdbool.h>
typedef unsigned char  		INT8U;
typedef signed   char  		INT8S;
typedef unsigned short 		INT16U;
typedef signed   short 		INT16S;
typedef unsigned int   		INT32U;
typedef signed   int   		INT32S;
typedef unsigned long long 	INT64U;
typedef signed   long long  INT64S;
#define IF_MAXSAT 255
#define IF_MAXSSR 120
#define IF_MAXMASK 16
typedef struct {
//	char IP[1024];
//	int Port;
	char CK_FILE[255][255];
	//站点标号
	char Site_number[20];
	//终端编号
	char Machine_number[20];
//	double CACHE_TIME;
//	char R_FILE_DIR[1024];
//	char LOG_FILE[1024];
//	char redis_password[1024];
//	char mysql_port[1024];
//	char mysql_pass[1024];
//	char mysql_user[1024];
//	char mysql_name[1024];
	FILE* fp_output;
	char DATA_PPP_FILENAME[250];
}config_t;

typedef struct {            /* ftp download control type */
	int state;              /* state (0:unavailable,1:available) */
	int proto;              /* protocol (0:mysql,1:mssql) */
	/* int error;              error code provided by database server */
	char addr[1024];        /* address of database */
	char user[256];         /* user for database */
	char passwd[256];       /* password for database */
	char dbname[256];	    /* name of database */
	int  port;				/* database port */
	int  topts[4];          /* time options {poff,tint,toff,tretry} (s) */
} db_t;

typedef struct ppp_data_sub1
{
unsigned char  IODP;
/*
BDS  1-63       prn_make[0]--prn_make[62]
GPS  1-37       prn_make[63]--prn_make[99]
galileo  1-37   prn_make[100]--prn_make[136]
GLONSDD  1-37   prn_make[137]--prn_make[173]
NULL            prn_make[174]--prn_make[254]
*/
char prn_make[IF_MAXSAT];
char sub1NULL[200];
}ppp_sub1;


typedef struct
{
short satslot;
unsigned char IODN;
unsigned char IODCorr;
double radial;
double Tangentialdir;
double Normaldir;
unsigned char URAclass;
unsigned char URAvalue;
double ura;
}Trackcp;

typedef struct ppp_data_sub2
{
    int sub1NULL;
    Trackcp trasub[6];
}ppp_sub2;

typedef struct
{
short satslot;
unsigned char num4;
unsigned char pattern[16];
double deciation[16];
}Intersymbol_dev;

typedef struct ppp_data_sub3
{
    unsigned char num;
    Intersymbol_dev intersub3[31];
}ppp_sub3;

typedef struct ppp_data_sub4
{
unsigned char IODP;
unsigned char subtupe1;
unsigned char IDO_corr[23];
double c [23];
int REV;
}ppp_sub4;

typedef struct ppp_data_sub5
{
unsigned char IODP;
unsigned char subtupe2;
unsigned char URAclass[70];
unsigned char URAvalue[70];
int REV;

}ppp_sub5;

typedef struct
{
    unsigned char IOD_corr;
    int Co;
}clock_NUMC_sub6;
typedef struct
{
unsigned int toc;
short DEV;
unsigned char IOD_SSR;
unsigned char IOPD;
unsigned char Slot_S;
clock_NUMC_sub6 clock_N_sub6 [22];
}clock_su6;

typedef struct
{
unsigned int tot;
short DEV;
unsigned char IOD_SSR;
Trackcp numO[6];

}track_su6;

typedef struct ppp_data_sub6
{
unsigned char NUMC;
unsigned char NUMO;
 track_su6 sub6_tarck;
 clock_su6 sub6_clock;

}ppp_sub6;

typedef struct
{
    short satslot;
    unsigned char IOD_corr;
    int Co;
}clock_NUMC_sub7;
typedef struct
{
unsigned int toc;
short DEV;
unsigned char IOD_SSR;
unsigned char IOPD;
unsigned char Slot_S;
clock_NUMC_sub7 clock_sub7[22];
}clock_su7;

typedef struct ppp_data_sub7
{

unsigned char NUMC;
unsigned char NUMO;
track_su6 sub7_tarck;
clock_su7 sub7_clock;

}ppp_sub7;

union ppp_sub_union
{
     ppp_sub1 type1;
     ppp_sub2 type2;
     ppp_sub3 type3;
     ppp_sub4 type4;
     ppp_sub5 type5;
     ppp_sub6 type6;
     ppp_sub7 type7;
};
typedef struct ppp_data
{
short mestype;
int CRC;

short nudata;
unsigned char SSR;
short prn;

int  BDSweek;			//当前消息北斗周
double   BDSsod;					//当前数据北斗天内秒
double   BDSsow;					//当前数据北斗周内秒
//sub1
union ppp_sub_union type;
unsigned char sta [10240];
}pppdata;
typedef struct
{
    unsigned int sow[2];
    // ----time 0
    Trackcp trasub;
    //clock----time  1
    unsigned char IDO_corr;
    unsigned char IODC;
    double c;
    int coun;
    int IOD_SSR[2];

}ppp_clwait;

struct week_second
{
	unsigned short  BDSweek;
	INT32U  BDSsecond;
};

typedef struct
{
    struct week_second ws;
    unsigned int toc;
}ppp_ws_ts;


struct  datapackage
{
	char  masthead[10];
	unsigned short BDSweek;
	float BDSsecond;
	float BDSsow;
	//INT32U BDSsecond;
	unsigned char PRN;
	unsigned short glofreq;
	unsigned char signalcomponent;
	unsigned short len;
    char msg[1024];
	unsigned char ys_msg[1024];
	unsigned char type;
};

struct Message_header
{
	struct week_second current_week_second;
	unsigned short	current_PRN;
	unsigned char current_sin_s;
	char current_mess_sys;
	unsigned char data[1024];
};

typedef struct{
	int SSR; /**for matching different configure**/
	int bweek; /** week in bds**/
	double bsow; /** seconds of week in bds **/
	double RAC[IF_MAXSAT][3]; /** orbit corrections **/
	double ura[IF_MAXSAT]; /** ura parameters **/
	int iode[IF_MAXSAT]; /** for matching ephemeris**/
	int iodcorr[IF_MAXSAT]; /** for mathcing the clock**/
}ppp_ssr_orbit;
typedef struct{
	int SSR; /** for matching different configure **/
	int iodp; /** for matching  the type 1**/
	int bweek; /** week in bds**/
	double bsow; /** seconds of week in bds **/
	double C0[IF_MAXSAT]; /** clock corrections **/
	int iode[IF_MAXSAT];
	int iodcorr[IF_MAXSAT]; /**for matching the orbit**/
}ppp_ssr_clock;
typedef struct{
	int SSR; /** for matching different configure **/
	int iodp; /** for matching  the type 1**/
	char cmake[IF_MAXSAT]; /**signal mask collected from type 1**/
}ppp_ssr_mask;

#ifdef GLOBAL_B2B
#define GLOBAL_EXTERN
#else
#define GLOBAL_EXTERN extern
#endif
GLOBAL_EXTERN config_t ssr_config;
GLOBAL_EXTERN int ssr_clock_count;
GLOBAL_EXTERN int ssr_orbit_count;
GLOBAL_EXTERN int ssr_mask_count;
GLOBAL_EXTERN ppp_ssr_orbit ssr_orbits[IF_MAXSSR]; /**/
GLOBAL_EXTERN ppp_ssr_clock ssr_clocks[IF_MAXSSR]; /**/
GLOBAL_EXTERN ppp_ssr_mask ssr_masks[IF_MAXMASK]; /**/

unsigned int getbitu(const unsigned char *buff, int pos, int len);
unsigned int getbitu2(const unsigned char *buff, int p1, int l1, int p2, int l2);
int getbits2(const unsigned char *buff, int p1, int l1, int p2, int l2);
int getbits(const unsigned char *buff, int pos, int len);
unsigned int getbitu3(const unsigned char *buff, int p1, int l1, int p2, int l2, int p3, int l3);
int getbits3(const unsigned char *buff, int p1, int l1, int p2, int l2, int p3, int l3);
int PPPand(int sys,int prn);
void  ts2ts(double ep[],int cprn);
char prn_syssig(int sys);
int satslot_prn(int prn);
int syssig_prn(int prn);
int put2(char *ascmsg,char* omsg,unsigned int b);
bool debug_perror(char *fferror);
bool asco(unsigned char *ascmsg,unsigned char* omsg,char*pd);
bool cwaitif(int prn,int sys,int cprn);
bool write2_file(FILE *fi2w,int sig2w,int sys,int prn,int cprn);
bool write_file(char *filename,int cprn);
int  iffun(unsigned char* datapackage, struct datapackage *dp);
bool strategy_PPP(struct  Message_header mh,char* pd);
bool PPP_init(pppdata PPPB2B);
bool ppp_cwait(pppdata  PPPB2B);
bool ifsystem(struct datapackage dp) ;
bool parse(unsigned char *datapackage);
int subtype_prn(char* make,int subtype,int ix);
void m_outorbit(ppp_ssr_orbit* orbit);
void m_outclock(ppp_ssr_clock* clock);
int md_julday(int iyear,int imonth,int iday);
void wksow2mjd(int week,double sow,int* mjd,double* sod);
void mjd2doy(int jd, int* iyear, int* idoy);
void yeardoy2monthday(int iyear, int idoy, int* imonth, int* iday);
void mjd2date(int jd, double sod, int* iyear, int* imonth, int* iday, int* ih,
		int* imin, double* sec);
void mjd2wksow(int mjd, double sod, int *week, double *sow);
bool parse_PPP(unsigned char *datapackage, pppdata* PPPB2B);
bool strpro(char *str1, char *str2);
char* parse_api(unsigned char *datapackage);
int  sysUsecTime(char *str);
bool read_file_nav_2(unsigned char *dw_data);
bool strpro_sig(char *str1, char *str2, char fsig, char *sta);
bool gnssinit(const char*,const char*);


int b2b_updateiode(int,int prn,int iodcrc);
bool b2b_parse_ppp(unsigned char* datapackage,pppdata* PPPB2B);
void b2b_fillmem(pppdata* p_sbas);
bool b2b_parsecorr(struct Message_header* mh);
bool b2b_iffun(unsigned char* datapackage,struct datapackage* dp);
bool b2b_ifsystem(struct datapackage* dp);
bool b2b_parse(unsigned char* data);
void b2b_decoder_file();
#endif
