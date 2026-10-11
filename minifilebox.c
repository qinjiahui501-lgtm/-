#include "stdio.h"
#include "sys/types.h"
#include "sys/stat.h"
#include "fcntl.h"
#include "unistd.h"
#include "errno.h"
#include "string.h"
#include "stdlib.h"

int main(int argc,char const*argv[])
{
    if(argc!=3&&argc!=4)
    {
        printf("参数错误，请输入(./minifilebox <命令> <参数>)\n");
        printf("create <文件>：创建空文件，若已存在则报错\n");
        printf("write <文件> <内容>：以追加方式写入内容\n");
        printf("read <文件>：读取并打印文件全部内容\n");
        printf("copy <源> <目标>：复制文件\n");
        printf("size <文件>：输出文件字节大小\n");
        printf("info <文件>：统计并输出文件行数与字符数\n");
        printf("hole <文件> <MB>：生成指定大小的空洞文件\n");
        return -1;
    }
    int fd;
    if(argc==3)
    {
        if(strcmp(argv[1],"create")==0)
        {
            fd = open(argv[2],O_CREAT|O_EXCL|O_WRONLY,0766);
            if(fd==-1)
            {
                printf("创建%s空文件失败,错误原因：%s,错误位置：%d行\n",argv[2],strerror(errno),__LINE__);
                return -1;
            }
            close(fd);
            printf("创建%s空文件成功\n",argv[2]);
        }
        if(strcmp(argv[1],"read")==0)
        {
            FILE *fd =fopen(argv[2],"r");
            if(fd==NULL)
            {
                printf("打开%s文件失败,错误原因：%s,错误位置：%d行\n",argv[2],strerror(errno),__LINE__);
                return -1;
            }
            char buf[200]={0};
            int ret =0;
            while (1)
            {
                memset(buf,0,sizeof(buf));
                ret = fread(buf,1,sizeof(buf),fd);
                if(ret==-1) 
                {
                    printf("读取%s文件失败,错误原因：%s,错误位置：%d行\n",argv[2],strerror(errno),__LINE__);
                    return -1;
                }
                if(ret==0) break;
                printf("%s",buf);
            }
            printf("\n");
            fclose(fd);
            printf("读取并打印成功%s文件全部内容\n",argv[2]);
        }
        if(strcmp(argv[1],"size")==0)
        {
            fd=open(argv[2],O_RDONLY);
            if(fd==-1)
            {
                printf("打开%s文件失败,错误原因：%s,错误位置：%d行\n",argv[2],strerror(errno),__LINE__);
                return -1;
            }
            off_t ret_len = lseek(fd,0,SEEK_END);
            if(ret_len==-1)
            {
                printf("计算%s字节失败,错误原因：%s,错误位置：%d行\n",argv[2],strerror(errno),__LINE__);
                close(fd);
                return -1;
            }
            close(fd);
            printf("%s文件的大小为:%ld字节\n",argv[2],ret_len);
        }
        if(strcmp(argv[1],"info")==0)
        {
            fd =open(argv[2],O_RDONLY);
            if(fd==-1)
            {
                printf("打开%s文件失败,错误原因：%s,错误位置：%d行\n",argv[2],strerror(errno),__LINE__);
                return -1;
            }
            char buf[2000];
            size_t n;
            long lines=0,chars=0;
            int last_line=0;
            while((n=read(fd,buf,sizeof(buf)))>0)
            {
                for(long i=0;i<n;i++)
                {
                    chars++;
                    if(buf[i]=='\n')
                    {
                        lines++;
                    }
                }
                last_line = (buf[n-1]=='\n');
            }
            if(n==-1)
            {
                printf("读取%s文件失败,错误原因：%s,错误位置：%d行\n",argv[2],strerror(errno),__LINE__);
                close(fd);
                return -1;
            }
            if(last_line==0 && chars>0) lines++;
            close(fd);
            printf("文件行数：%ld\n",lines);
            printf("字符数：%ld\n",chars);
        }
    }
    else if(argc==4)
    {
        if(strcmp(argv[1],"write")==0)
        {
            fd=open(argv[2],O_WRONLY|O_APPEND);
            if(fd==-1)
            {
                printf("打开%s文件失败,错误原因：%s,错误位置：%d行\n",argv[2],strerror(errno),__LINE__);
                return -1;
            }
            size_t n =write(fd,argv[3],strlen(argv[3]));
            if(n==-1)
            {
                printf("写入%s文件失败,错误原因：%s,错误位置：%d行\n",argv[2],strerror(errno),__LINE__);
                close(fd);
                return -1;
            }
            close(fd);
            printf("追加写入%s文件成功\n",argv[2]);
        }
        if(strcmp(argv[1],"copy")==0)
        {
            fd=open(argv[2],O_RDONLY);
            if(fd==-1)
            {
                printf("打开%s文件失败,错误原因：%s,错误位置：%d行\n",argv[2],strerror(errno),__LINE__);
                return -1;
            }
            int fc=open(argv[3],O_TRUNC|O_CREAT|O_WRONLY,0644);
            if(fc==-1)
            {
                printf("打开%s文件失败,错误原因：%s,错误位置：%d行\n",argv[3],strerror(errno),__LINE__);
                return -1;
            }
            char buf[2024]={0};
            size_t re;
            while((re = read(fd,buf,sizeof(buf)))>0)
            {
                if(write(fc,buf,re)!=re)
                {
                    printf("写入%s文件失败,错误原因：%s,错误位置：%d行\n",argv[2],strerror(errno),__LINE__);
                    close(fd);
                    close(fc);
                    return -1;
                }
            }
            if(re==-1)
            {
                printf("读取%s文件失败,错误原因：%s,错误位置：%d行\n",argv[2],strerror(errno),__LINE__);
                close(fd);
                return -1;
            }
            close(fd);
            close(fc);
            printf("复制%s文件成功\n",argv[2]);
        }
        if(strcmp(argv[1],"hole")==0)
        {
            long long mb=atoll(argv[3]);
            if(mb<0)
            {
                printf("mb必须大于零\n");
                return -1;
            }
            long long MB=mb*1024*1024;
            fd =open(argv[2],O_WRONLY|O_TRUNC|O_CREAT,0644);
            if(fd==-1)
            {
                printf("生成%s文件失败,错误原因：%s,错误位置：%d行\n",argv[2],strerror(errno),__LINE__);
                return -1;
            }
            if(lseek(fd,MB-1,SEEK_SET)==-1)
            {
                printf("生成%s空洞文件失败,错误原因：%s,错误位置：%d行\n",argv[2],strerror(errno),__LINE__);
                close(fd);
                return -1;
            }
            if(write(fd,"\0",1)!=1)
            {
                printf("生成%s空洞文件失败,错误原因：%s,错误位置：%d行\n",argv[2],strerror(errno),__LINE__);
                close(fd);
                return -1;
            }
            close(fd);
            printf("生成%s空洞文件成功,大小%lld MB\n",argv[2],mb);
        }
    }
    return 0;
}