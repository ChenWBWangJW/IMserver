#include <stdio.h>
#include <string.h>
#include <mysql.h>
#include <time.h>

//注册用户名
int insert(char szName[])		//szName为要注册的用户名
{
	MYSQL mysql;
	MYSQL_RES* res;
	MYSQL_ROW row;
	char* query;
	int ret;
	int t;
	int id = 12;
	char buf[512] = "";
	char cur_time[64] = "";

	mysql_init(&mysql);
	if ((mysql_real_connect(&mysql, "localhost", "root", "123456", "chatdb", 0, NULL, 0)) == NULL)
	{
		printf("connect error: %s\n", mysql_error(&mysql));
		return -1;
	}
	else
	{
		printf("Connected to Mysql successfully!\n");
	}

	sprintf(buf, "INSERT INTO qqnum(name) VALUES(\'%s\')", szName);
	if ((ret = mysql_query(&mysql, buf)) != 0)
	{
		printf("insert error: %s\n", mysql_error(&mysql));
		return -1;
	}
	else
	{
		printf("insert success!\n");
	}

	mysql_close(&mysql);
	return 0;
}

//判断用户名是否存在
int IsExist(char szName[])
{
	MYSQL mysql;
	MYSQL_RES* res;
	MYSQL_ROW row;
	char* query;
	char buf[521] = "";
	
	mysql_init(&mysql);
	if ((mysql_real_connect(&mysql, "localhost", "root", "123456", "chatdb", 0, NULL, 0)) == NULL)
	{
		printf("connect error: %s\n", mysql_error(&mysql));
		return -1;
	}
	else
	{
		printf("Connected to Mysql successfully!\n");
	}

	sprintf(buf, "select name from qqnum where name = '%s'", szName);
	if ((mysql_query(&mysql, buf)))
	{
		printf("select error: %s\n", mysql_error(&mysql));
		res = -1;
		goto end;
	}
	else
	{
		printf("Connected to Mysql successfully!\n");
	}

	res = mysql_store_result(&mysql);
	if (res == NULL)
	{
		printf("result error: %s\n", mysql_error(&mysql));
		res = -1;
		goto end;
	}
	else
	{
		printf("result success!\n");
	}

	row = mysql_fetch_row(res);
	if(row > 0)
	{
		printf("%s\n", row[0]);
		res = 1;
		goto end;
	}
	else res = 0;


end:
	mysql_close(&mysql);
	return res;
}

int showTable()
{
	MYSQL mysql;
	MYSQL_RES* res;
	MYSQL_ROW row;
	char* query;
	int flag;
	int t;

	mysql_init(&mysql);
	if ((mysql_real_connect(&mysql, "localhost", "root", "123456", "chatdb", 0, NULL, 0)) == NULL)
	{
		printf("connect error: %s\n", mysql_error(&mysql));
		return -1;
	}
	else
	{
		printf("Connected to Mysql successfully!\n");
	}

	query = "select * from qqnum";
	if ((flag = mysql_real_query(&mysql, query, (unsigned int)strlen(query))))
	{
		printf("Query failed!\n");
		return -1;
	}
	else
	{
		printf("[%s] made...\n", query);
	}

	res = mysql_store_result(&mysql);
	do
	{
		row = mysql_fetch_row(res);
		if (row == 0) break;
		for (t = 0; t < mysql_num_fields(res); t++)
		{
			printf("%s ", row[t]);
		}
	}while(1);

	mysql_free_result(res);
	mysql_close(&mysql);
	return 0;
}
