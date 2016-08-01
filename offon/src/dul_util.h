
struct hlrdul_item{
    char hlrcode[4];
    int  dpt;
    int  rpt;
    char status;
    char dip[16];
    int  rcv_q_len;
    int  wfile_idx; /*保存的中间文件的当前序号*/
    int  rfile_idx; /*读取文件序号*/
    int  rfile_pos; /*读取位置*/ 
    char reserv[44];
};

extern struct hlrdul_item *local_item;

