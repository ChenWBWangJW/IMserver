#include <stdio.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/select.h>

#define MAXLINE 80
#define SERV_PORT 8000

#define CL_CMD_REG 'r'
#define CL_CMD_LOG 'l'
#define CL_CMD_CHAT 'c'

void perrorText(const char* str)
{
	fprintf(stderr, "%s error. reason: %s\n", str, strerror(errno));
	exit(-1);
}

/*******************************************
* 函数名称：GetName
* 功能描述：从字符串中获取用户名
* 参数列表：str:字符串 szName:用户名
* 执行结果：
*	读取如：r,chen, 从中读取r注册命令后，截
* 取出用户名chen
********************************************/
void getName(char str[], char szName[])
{
	//char str[] = "a,b,c,d*e";
	const char * split = ",";
	char * p;
	p = strtok(str, split);
	int i = 0;
	while (p != NULL)
	{
		printf("%s\n", p);
		if (i == 1) sprintf(szName, p);
		i++;
		p = strtok(NULL, split);
	}
}

// 查找字符串中某个字符出现的次数
int countChar(const char* p, const char chr)
{
	int count = 0;
	int i = 0;

	while (*(p + i))				//字符数组存放在一快内存区域中，按索引找字符，指针本身不变
	{
		if (p[i] == chr) ++count;
		++i;						//按数组的索引值找到对应指针变量的值
	}
	// printf("按字符串中w出现的次数：%d\n", count);
	return count;
}

int main(int argc, char* argv[])
{
	int i;
	int maxi;
	int maxfd;
	int listenfd, connfd, sockfd;
	int nready, client[FD_SETSIZE];
	ssize_t n;
	char szName[255] = "", szPwd[128] = "", repBuf[512] = "";

	//两个集合
	fd_set rset, allset;

	char buf[MAXLINE];
	char str[INET_ADDRSTRLEN];
	socklen_t cliaddr_len;
	struct sockaddr_in cliaddr, servaddr;

	//创建套接字
	if((listenfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
		perrorText("socket");
	else
		printf("Create socket success!\n");

	//初始化服务器地址结构
	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;					//地址族
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);	//IP地址
	servaddr.sin_port = htons(SERV_PORT);			//端口号

	//设置套接字选项
	int val = 1;
	if((setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, (void*)&val, sizeof(val)) == -1))
		perrorText("setsockopt");
	else
		printf("setsockopt success!\n");

	//绑定套接字和地址结构
	if((bind(listenfd, (struct sockaddr*)&servaddr, sizeof(servaddr)) == -1))
		perrorText("bind");
	else
		printf("bind success!\n");

	//将套接字转换为监听套接字
	if((listen(listenfd, 128) == -1))
		perrorText("listen");
	else
		printf("listen success!\n");
	
	//需要接收最大文件描述符
	maxfd = listenfd;

	//数组初始化为-1
	maxi = -1;
	for(i = 0; i < FD_SETSIZE; i++)
		client[i] = -1;

	//集合初始化
	FD_ZERO(&allset);

	//添加监听套接字
	FD_SET(listenfd, &allset);

	puts("Chat server is running......");

	for (; ; )
	{
		//关键点3
		rset = allset;	//每次循环都要重新设置select监控信号集

		//select返回rset集合中发生都时间的总是，参数1：最大文件描述符+1，参数2：读集合，参数3：写集合，参数4：异常集合，参数5：超时时间
		nready = select(maxfd + 1, &rset, NULL, NULL, NULL);
		if (nready < 0)
		{
			puts("select error");
		}

		if (FD_ISSET(listenfd, &rset))
		{
			//接收客户端连接
			cliaddr_len = sizeof(cliaddr);

			//accept返回已连接套接字,当前非阻塞，因为select已经发生读写事件
			if((connfd = accept(listenfd, (struct sockaddr*)&cliaddr, &cliaddr_len)) < 0)
				perrorText("accept");
			else
				printf("accept success!\n");

			printf("received from %s at PORT %d\n", 
				inet_ntop(AF_INET, &cliaddr.sin_addr, str, sizeof(str)), 
				ntohs(cliaddr.sin_port));

			//关键点1
			for (i = 0; i < FD_SETSIZE; i++)
			{
				if (client[i] < 0)
				{
					//保存accept返回的已连接套接字
					client[i] = connfd;
					break;
				}
			}

			//是否达到可控制套接字数量上限
			if (i == FD_SETSIZE)
			{
				fputs("too many clients\n", stderr);
				exit(1);
			}

			//关键点2
			FD_SET(connfd, &allset);	//将新的已连接套接字添加到集合中

			//更新最大文件描述符数
			if(connfd > maxfd)
				maxfd = connfd;		//select第一个参数需要
			if (i > maxi)
				maxi = i;			//更新client[]最大索引值，select第一个参数需要

			//没有更多的可读文件描述符，继续回到上面的select阻塞监听，负责处理已连接套接字
			if (--nready == 0)
			{
				continue;
			}
		}

		//处理已连接套接字
		for (i = 0; i <= maxi; i++)
		{
			//检测clients 哪个有数据就绪
			if((sockfd = client[i]) < 0)
				continue;

			//sockfd是否有数据就绪
			if (FD_ISSET(sockfd, &rset))
			{
				//读数据，不用阻塞立即读取(select已经帮忙处理阻塞环节)
				if ((n = read(sockfd, buf, MAXLINE)) == 0)
				{
					//无数据，client关闭连接，服务器端也关闭对应连接
					close(sockfd);
					FD_CLR(sockfd, &allset);	//清除集合中对应的文件描述符
					client[i] = -1;
				}
				else
				{
					char code = buf[0];
					switch (code)
					{
					case CL_CMD_REG:		//注册命令处理
						if (1 != countChar(buf, ','))
						{
							puts("invalid protocal!");
							break;
						}

						getName(buf, szName);

						//判断名字是否重复
						if (IsExist(szName))
						{
							sprintf(repBuf, "r,exist");
						}
						else
						{
							insert(szName);
							showTable();
							sprintf(repBuf, "r,ok");
							printf("reg ok, name = %s\n", szName);
						}
						
						write(sockfd, repBuf, strlen(repBuf));		//回复客户端
						break;		//登录命令处理

					case CL_CMD_LOG:
						if (1 != countChar(buf, ','))
						{
							puts("invalid protocal!");
							break;
						}

						getName(buf, szName);

						//判断名字是否存在
						if (IsExist(szName))
						{
							sprintf(repBuf, "l,ok");
							printf("login ok,%s\n", szName);
						}
						else
						{
							sprintf(repBuf, "l,noexist");
						}

						write(sockfd, repBuf, strlen(repBuf));		//回复客户端
						break;

					case CL_CMD_CHAT:
						puts("send all");

						//群发
						for (i = 0; i <= maxi; i++)
						{
							if (client[i] != -1)
								write(client[i], buf+2, n);		//写回客户端，"+2"表示去掉命令码和逗号
						}
						break;
					}
				}
				if (--nready == 0)
					break;
			}
		}
	}
	close(listenfd);
	return 0;
}
