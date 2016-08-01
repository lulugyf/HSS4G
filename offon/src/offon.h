/**********************************************************
 *  FileName:
 *      offon.h
 *
 *  Version:
 *      1.00 2004.11.08
 *
 *  tabstop = 4
 *
 **********************************************************/
#ifndef __INCLUDE_OFFON_H__
#define __INCLUDE_OFFON_H__

#include "h.h"

#define	VERSION "2.0"

#define DBLOGIN				"/gdblogin.cfg"
#define CUSTDB				"/custdb.cfg"

#define TERM_HLR			'1'
#define TERM_AUC			'2'
#define TERM_DATACENTER		'3'
#define	TERM_ENTRY			'4'
#define	TERM_QUERY			'5'

#define	MSISDNLEN			15
#define	IMSILEN				20
#define	SSINFO1LEN			15
#define	SSINFO2LEN			20
#define	SSINFO3LEN			120
#define	QUERYINFOLEN		200

/* 用户资料管理 */
struct user_info
{
	char				user[12];			/* 用户名称 */
	char				pswd[12];			/* 用户密码 */
	char				hlrcode[4];			/* 交换机代码 */
	char				hlrport;			/* 交换机端口 */
	char				type;				/* 终端类型 */
	char				encrypt;			/* 加密标志 */
	char				termid[12];			/* 终端标识 */
	struct user_info	*next;
};

/* define login status */
#define		LOGIN_INIT			'0'			/* 连接建立，未注册 */
#define		LOGIN_REGISTER		'1'			/* 注册成功 */

/* 通信连接管理 */
struct comm_info
{
	char				login;				/* 用户状态标识 */
	char				termtype;			/* 终端类型 */
	char				encrypt;			/* 加密标志 */
	char				cipher[8];			/* 密钥 */
	int					sockid;				/* 通信描述字 */
	struct sockaddr_in	cliaddr;			/* 客户端地址 */
	time_t				begintime;			/* 登陆时间 */
	time_t				reqtime;			/* 命令请求时间 */
	time_t				sndtime;			/* 最晚发送时间 */
	long				cmdid;				/* 请求命令ID */
	struct user_info	userinfo;			/* 用户资料 */
	struct comm_info	*next;
};

/* 业务命令与交换机指令配置关系 */
struct cmdtoorder
{
	char				hlrcode[4];			/* 交换机代码 */
	int					cmdcode;			/* 业务命令代码 */
	int					priority;			/* 命令优先级 */
	int					ordercode;			/* 交换机指令代码 */
	char				type;				/* 指令类型 */
	int					serial;				/* 交换机指令序号 */
	struct cmdtoorder	*next;
};

#define		REDUCE_FIRST		'1'
#define		REDUCE_BACK			'2'
#define		REDUCE_ALL			'3'

/* 交换机指令剔重规则 */
struct order_reduce
{
	char				hlrcode[4];			/* 交换机代码 */
	int					old_order_code;		/* 旧指令代码 */
	int					new_order_code;		/* 新指令代码 */
	char				operate;			/* 操作代码 */
	struct order_reduce	*next;
};

/* 复制异常指令检查规则 */
struct order_resend
{
	char				hlrcode[4];			/* 交换机代码 */
	int					order_code;			/* 指令代码 */
	char				func_code[8];		/* 功能代码 */
	struct order_resend	*next;
};

/* 指令应答信息分析规则 */
struct reply_parse
{
	char				hlrcode[4];			/* 交换机代码 */
	int					cmd_code;			/* 业务命令代码 */
	int					order_code;			/* 原始指令 */
	int					reply_code;			/* 应答结果 */
	int					parse_result;		/* 分析结果 */
	char				revise;				/* 是否需要校正 */
	int					dest_order;			/* 目的指令 */
	char				order_type;			/* 目的指令类型 */
	int					serial;				/* 目的指令序号 */
	struct reply_parse	*next;
};

/* 查询指令结果分析 */
struct query_parse
{
	char				hlrcode[4];			/* 交换机代码 */
	int					cmd_code;			/* 业务命令代码 */
	int					order_code;			/* 指令代码 */
	int					src_beg;			/* 源字符串起始位 */
	int					data_len;			/* 数据长度 */
	int					dest_station;		/* 目的字符串位置 */
	int					dest_beg;			/* 目的字符串起始位 */
	struct query_parse	*next;
};

/* 查询同步规则 */
struct query_revise
{
	int					cmd_code;			/* 业务命令代码 */
	int					order_code;			/* 指令代码 */
	int					reply_code;			/* 指令完成代码 */
	char				func_code[8];		/* 功能代码 */
	char				status_flag;		/* 功能状态同步标志 */
	char				info_flag;			/* 补充信息同步标志 */
	int					status_addr;		/* 功能状态位置 */
	char				status_null;		/* 功能状态空值标识 */
	int					info_beg;			/* 补充信息起点 */
	int					info_len;			/* 补充信息长度 */
	struct query_revise	*next;
};

/* 交换机指令配置信息 */
struct order_info
{
	int					ordercode;			/* 交换机指令代码 */
	char				orderinfo[1024];	/* 交换机指令参数格式 */
	struct order_info	*next;
};

/* 交换机应答配置信息 */
struct reply_info
{
	int					replycode;			/* 交换机应答代码 */
	char				replyinfo[256];		/* 交换机应答参数格式 */
	struct reply_info	*next;
};

/* 主进程配置信息 */
struct mcfg_info
{
	char				hlrcode[4];			/* 交换机代码 */
	char				ipaddr[32];			/* IP地址 */
	int					commport;			/* 通信端口 */
	struct mcfg_info	*next;
};

/* 号段端口对照信息 */
struct phead_port
{
	int					phead;				/* 万号段 */
	int					port;				/* 通信端口 */
	char				hlrcode[4];			/* 交换机代码 */
};

#define		MAX_PHEAD		8192

/* 号段HLR归属信息 */
struct phead_hlr
{
	int					phead;				/* 万号段 */
	char				hlrcode[4];			/* 交换机代码 */
	struct phead_hlr	*next;
};

#define		MAXOPCODENUM	8192

/* 业务代码优先级定义 */
struct opcode_pri
{
	char				op_code[8];			/* 业务代码 */
	int					priority;			/* 优先级 */
	struct opcode_pri	*next;
};

/* 用户资料管理规则 */
struct usrdata_mgr
{
	int					order_code;
	char				func_code[6];
	char				func_status;
	char				ss_info[128];
	struct usrdata_mgr	*next;
};

/* 待发送交换机指令队列 */
#define		GENERAL_CMD			0
#define		REVISE_CMD			2
#define		REDUCE_CMD			1001

struct orderwait
{
	int					ordercode;			/* 指令代码 */
	char				type;				/* 指令类型 */
	int					serial;				/* 指令序号 */
	int					status;				/* 指令状态 */
	struct orderwait	*next;
};

/* 待发送业务命令队列 */
#define		BOSS_CMD			0
#define		PARSE_CMD			1
struct cmdwait
{
	long				streamid;					/* 操作流水号 */
	char				cmdid[20];					/* 业务命令流水号 */
	char				opcode[8];					/* 业务代码 */
	int					cmdcode;					/* 业务命令代码 */
	char				status;						/* 业务命令状态 */
	int					priority;					/* 命令优先级 */
	char				msisdn[MSISDNLEN+1];		/* 移动号码 */
	char				imsi[IMSILEN+1];			/* IMSI号码 */
	char				ss_info1[SSINFO1LEN+1];		/* 补充信息1 */
	char				ss_info2[SSINFO2LEN+1];		/* 补充信息2 */
	char				ss_info3[SSINFO3LEN+1];		/* 补充信息3 */
	char				req_time[16];				/* 命令请求时间 */
	char				queryinfo[QUERYINFOLEN+1];	/* 查询信息 */
	struct orderwait	orderhead;					/* 指令队列头节点 */
	struct cmdwait		*next;
};

/* 号码信息节点 */
struct phonewait
{
	time_t				status;				/* 号码状态 */
	char				type;				/* 指令类型 */
	struct cmdwait		cmdhead;			/* 命令队列头节点 */
};

#define		MAXPHEADNUM	 500

/* 号段管理队列 */
struct phonehead
{
	char				hlrcode[4];			/* 交换机代码 */
	int					phead;				/* 万号段 */
	struct phonewait	*ptail[10000];		/* 号码信息节点 */
	struct phonehead	*next;
};

/* 号码索引队列 */
struct phoneidx
{
	char				msisdn[MSISDNLEN+1];	/* 移动号码 */
	int					phead;					/* 万号段 */
	int					ptail;					/* 号码后四位 */
	struct phoneidx		*next;
};

/* 优先级索引队列 */
struct priorityidx
{
	int					priority;			/* 优先级 */
	int					num;				/* 号码数量 */
	struct phoneidx		phonehead;			/* 号码索引队列节点 */
	struct phoneidx     phonetail;          /* 号码索引队列尾部  2008-12新加 */
	struct priorityidx	*next;
};


struct query_info
{
	char	msisdn[11];
	char	imsi[15];
	char	toll;
	char	roam;
	char	gprs;
	char	sms_snd;
	char	sms_rcv;
	char	cfu;
	char	cfbusy;
	char	cfnreply;
	char	cfnreach;
	char	cw;
	char	ch;
	char	mpty;
	char	cboc;
	char	cbic;
	char	fax;
	char	bs;
	char	clip;
	char	soclip;
	char	clir;
	char	soclir;
	char	odboc;
	char	odbic;
	char	bs31;
	char	mrbt;
	char	ici;
	char	tcsi;
	char	ocsi;
	char	ocamel_srvkey[2];
	char	ocamel_gsmid[12];
	char	tcamel_srvkey[2];
	char	tcamel_gsmid[12];
};

#define		MSGFLAG			"`CD'"

#define		VER100			"100"

#define		REQ_TYPE		'1'
#define		ACK_TYPE		'2'

#define		ENCRYPT			'1'
#define		DECRYPT			'2'

#define		TX_BEG			'1'
#define		TX_CON			'2'
#define		TX_END			'3'

/* 基本通信消息格式 */
struct comm_data
{
	char				flag[4];			/* `CD' */
	char				msglen[4];			/* 消息总长度（含消息头） */
	char				srvtype[8];			/* 业务类型 */
	char				transid[8];			/* 事务ID（唯一标识） */
	char				finished;			/* 事务结束标志 */
	char				msgtype;			/* REQ & ACK */
	char				encrypt;			/* 加密类型 */
	char				reserve[5];			/* 保留 */
	char				data[10000];		/* 业务数据 */
};

#define		CONN_KEEP	00000010

#define		ACK0000		0000,"success"
#define		ACK0001		0001,"UNKNOW SRVTYPE"
#define		ACK0011		0011,"LOGIN FAILED"
#define		ACK0012		0012,"INCORRECT USERNAME"
#define		ACK0013		0013,"INCORRECT USERPSWD"
#define		ACK0021		0021,"UNREGISTERED"
#define		ACK0101		0101,"UNKNOW ERROR"
#define		ACK1001		1001,"UNKNOW PHONE_HEAD"
#define		ACK1002		1002,"UNKNOW COMMAND_CODE"
#define		ACK2001		2001,"UNKNOW QUERY_TYPE"
#define		ACK2002		2002,"PHONE_NO NOT EXIST"
#define		ACK2003		2003,"CAN'T OPEN QRY_FILE"
#define		ACK3001		3001,"NO DATAS"
#define		ACK3002		3002,"PHONE_NO NOT EXIST"
#define		ACK3003		3003,"STREAM_ID NOT EXIST"

/* 通用应答消息【00000000】 */
#define		GENERAL_ACK		00000000
struct general_ack
{
	char				retn[4];			/* 命令处理结果 */
	char				desc[60];			/* 结果描述 */
};

/* 登陆建立会话【00000001】 */
#define		SRV_LOGIN		00000001
struct login_req
{
	char				username[8];		/* 用户名称 */
	char				userpswd[8];		/* 用户密码 */
	char				hlrcode[4];			/* 交换机代码 */
	char				hlrport;			/* 通信端口 */
	char				termtype;			/* 终端类型 */
	char				reserve[2];			/* 保留 */
};
#define		LOGINREQLEN		sizeof(struct login_req)

struct login_ack
{
	char				retn[4];			/* 返回代码 */
	char				encrypt;			/* 加密标志 */
	char				reserve[3];			/* 保留 */
	char				cipher[8];			/* 密钥 */
};
#define		LOGINACKLEN		sizeof(struct login_ack)

/* 开关机业务命令数据格式【10000001】 */
#define		SRVCODE0001		10000001
struct op_data_req
{
	char				command_id[16];		/* 操作流水，业务命令的唯一标识 */
	char				phone_no[MSISDNLEN+1];	/* 移动电话号码 */
	char				imsi_no[IMSILEN+1];		/* IMSI号码 */
	char				op_code[6];				/* 业务代码 */
	char				command_code[6];		/* 操作命令代码 */
	char				cmd_status;				/* 业务状态 */
	char				phone_priority;			/* 号码优先级 */
	char				ss_info1[SSINFO1LEN+1];	/* 补充信息1 */
	char				ss_info2[SSINFO2LEN+1];	/* 补充信息2 */
	char				ss_info3[SSINFO3LEN+1];	/* 补充信息3 */
	char				req_time[16];			/* 命令请求时间 */
	char				login_no[8];			/* 工号 */
};
#define		SRVLEN0001		sizeof(struct op_data_req)

struct op_data_ack
{
	char				retn[4];			/* 命令处理结果 */
	char				desc[60];			/* 结果描述 */
};

/* 开关机指令数据格式【10000002】 */
#define		GETORDER		10000002
#define		ONLY_GET		'1'
#define		REPLY_GET		'2'
struct cmd_data_req
{
	char				retn[4];				/* 返回代码 */
	char				type;					/* 指令类型 */
	char				phone_no[MSISDNLEN+1];	/* 移动号码 */
	char				stream_id[16];		/* 操作流水，业务命令的唯一标识 */
	char				ordercode[4];			/* 指令代码 */
	char				info[QUERYINFOLEN+1];	/* 查询信息 */
};
#define		CMDREQLEN		sizeof(struct cmd_data_req)

struct cmd_data_ack
{
	char				retn[4];				/* 返回代码 */
	char				stream_id[16];		/* 操作流水，业务命令的唯一标识 */
	char				ordercode[4];			/* 指令代码 */
	char				phone_no[MSISDNLEN+1];	/* 移动电话号码 */
	char				imsi_no[IMSILEN+1];		/* IMSI号码 */
	char				ss_info1[SSINFO1LEN+1];	/* 补充信息1 */
	char				ss_info2[SSINFO2LEN+1];	/* 补充信息2 */
	char				ss_info3[SSINFO3LEN+1];	/* 补充信息3 */
	/*char				queryinfo[QUERYINFOLEN+1];	 查询信息 */
};
#define		CMDACKLEN		sizeof(struct cmd_data_ack)

/* 号码状态查询数据格式【20000001】 */
#define		QUERY0001		20000001
struct query0001_req
{
	char				type;				/* 查询类型 */
	char				datas[64];			/* 查询参数 */
};
#define		QRYLEN0001		sizeof(struct query0001_req)

struct query0001_ack
{
	char				retn[4];			/* 查询处理结果 */
	char				fname[60];			/* 查询结果文件名称 */
};

struct attr
{
    char        id[64];
    char        val[1024];
    struct attr *next;
};

struct RongZai
{
	char				tname[64];
	char				suff[32];
	char				tmpdir[256];
	char				snddir[256];
	char				bakdir[256];
	struct RongZai		*next;
};

#endif /*__INCLUDE_OFFON_H__ */

