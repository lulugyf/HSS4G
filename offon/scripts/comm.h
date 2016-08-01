
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

#define	MSISDNLEN			15
#define	IMSILEN				20
#define	SSINFO1LEN			15
#define	SSINFO2LEN			20
#define	SSINFO3LEN			120
#define	QUERYINFOLEN		200

/* 开关机业务命令数据格式【10000001】 */
#define		SRVCODE0001		10000001
struct op_data_req
{
	char				command_id[16];		/* 操作流水，业务命令的唯一标识 */
	char				phone_no[16];	/* 移动电话号码 */
	char				imsi_no[21];		/* IMSI号码 */
	char				op_code[6];				/* 业务代码 */
	char				command_code[6];		/* 操作命令代码 */
	char				cmd_status;				/* 业务状态 */
	char				phone_priority;			/* 号码优先级 */
	char				ss_info1[16];	/* 补充信息1 */
	char				ss_info2[21];	/* 补充信息2 */
	char				ss_info3[121];	/* 补充信息3 */
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

