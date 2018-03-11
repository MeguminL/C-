#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<math.h>
#include <time.h>
struct Record  // 提交记录 
{
	int id;
	int problem_id;
	char date[20];
	char time[20];
} record[50000],record_2[50000];
struct number
{
	int Stu;
} All;
struct students  //  学生资料 
{
	int id;
	int grade;
	int mark;
	int sum;
	int IsCheat; 
	int problem_Rank[1000];
	int problem_id[1000];
	int qualified[1000];//  判 problem_id[1000]是否合格 
	char date[1000][12];
	char time[1000][12];
} stu[1000];

int cmp1(const void *a ,const void *b) //  将学生按积分从小到大排列 （结构体的一级排序） 
{ 
	struct students *pa = (struct students*)a; 
	struct students *pb = (struct students*)b;
	return pa->grade < pb->grade ? 1 : -1; 
} 
int cmp2(const void *a , const void *b) //  将提交记录按时间的先后顺序排列 （结构体的二级排序） 
{ 
	struct Record *pa = (struct Record*)a; 
	struct Record *pb = (struct Record*)b; 
    int v = strcmp(pa->date,pb->date);
    if( v != 0 )
        return v;
    return strcmp(pa->time,pb->time);
} 
int Grade(int a)	//  每一题的分值 
{
	if(a<=12) return 32;
	if(a<=24) return 16;
	if(a<=48) return 8;
	if(a<=96) return 4;
	if(a<=192) return 2;
	return 1;
}

int duration(int k,int T1, int T2)		//计算时差 
{	
	struct tm time_cha;
	time_t t[2];
	int T[2] = {T1,T2};
	for(int i = 0; i < 2; i++) {
		time_cha.tm_year = (stu[k].date[T[i]][0]-'0')*1000+(stu[k].date[T[i]][1]-'0')*100+(stu[k].date[T[i]][2]-'0')*10+stu[k].date[T[i]][3]-'0' - 1900;
		time_cha.tm_mon = (stu[k].date[T[i]][5]-'0')*10+stu[k].date[T[i]][6]-'0' - 1;
		time_cha.tm_mday = (stu[k].date[T[i]][8]-'0')*10+stu[k].date[T[i]][9]-'0';
		time_cha.tm_hour = (stu[k].time[T[i]][0]-'0')*10+stu[k].time[T[i]][1]-'0';
		time_cha.tm_min = (stu[k].time[T[i]][3]-'0')*10+stu[k].time[T[i]][4]-'0';
		time_cha.tm_sec = (stu[k].time[T[i]][6]-'0')*10+stu[k].time[T[i]][7]-'0';
		t[i] = mktime(&time_cha);
	}
	return round((t[1]-t[0])*1.0/60.0);
}
int main()
{	
	memset(stu,0,sizeof(students)*1000);
	for(int k = 0; k < 1000 ; k++) 
	{
		stu[k].IsCheat = 0;
		for(int j = 0; j < 1000; j++)
			stu[k].qualified[j] = 1;
	}		
	FILE *fp;
	char name1[] = {"D:/C语言大作业/C语言大作业.txt"};
	fp = fopen(name1,"r");
	fseek(fp,0,0);
	int a1 = 0;
	char arr1[100];
	fgets(arr1,100,fp);
	if(fp == NULL)
	{
		printf("打开失败\n请确保提交记录文件位于%s\n",name1);
		exit(1);
	} 
	while(!feof(fp))
	{		
		fscanf(fp,"%d %d %s %s",&record[a1].id,&record[a1].problem_id,record[a1].date,record[a1].time);
		a1++;
	}
	qsort(record,a1,sizeof(record[0]),cmp2); 
	All.Stu=0;
	int h;
	for(int i = 0,j = 0; i < a1; i++) {
			while(1){
				if(stu[j].id==0)
				{
					All.Stu++;
					break;
				}
				else if(stu[j].id == record[i].id) break;
				j++;
			}	
			stu[j].id = record[i].id;
			stu[j].problem_id[stu[j].sum] = record[i].problem_id;
			strcpy(stu[j].date[stu[j].sum],record[i].date);
			strcpy(stu[j].time[stu[j].sum],record[i].time);
			stu[j].sum++;
			j = 0;
	}
	fclose(fp);
	printf("请问是否要对提交记录进行盘查（确认：1，否定：0）：");
	int order;
	scanf("%d",&order);
	while(order!=0)
	{
		if(order == 1){
			char filename[40];
			int N,L; 
			printf("请输入文件名： "); 
			scanf("%s",filename); 
			strcat(filename,".txt");	
			printf("请输入盘查时长： ");
			scanf("%d",&L); 
			printf("请输入最少题数： ");
			scanf("%d",&N); 
		
			FILE *fp;
			char name[50] = {"D:/C语言大作业/"};
			strcat(name,filename);
			fp = fopen(name,"w");
			for(int i = 0,k; i < All.Stu; i++)	
			{		
				for(k = 0; k < All.Stu; k++)
					if(i + 1== stu[k].id)
						break;
				for(int j = N-1,x=0; j < stu[k].sum; j++) 
				{	
							
					if(duration(k,x,j)<=L&&duration(k,x,j+1)>L){
						stu[k].IsCheat = 1;
						fprintf(fp,"ID\t\t开始时间\t结束时间\t\t时长\t题数\n");		
						fprintf(fp,"%d\t%s %s\t%s %s\t%d\t%d\n",stu[k].id,stu[k].date[x],stu[k].time[x],stu[k].date[j],stu[k].time[j],duration(k,x,j),j-x+1);
						for(int b = x; b <= j; b++) 
						{			
							 fprintf(fp,"\t\t%d\t\t%s %s\n",stu[k].problem_id[b],stu[k].date[b],stu[k].time[b]);
							 stu[k].qualified[b]=0;
						}
						fprintf(fp,"\n");
						x = j;
						j = x + N - 1;				
					}
					else if(duration(k,x,j)>L) x++;			
				}
			}
			printf("***文件生成成功***\n文件位置：%s\n\n",name);
			break;
		}
		else if(order != 0){
			printf("无效指令，请重新输入！\n");
			scanf("%d",&order);
		} 
	} 

	char filename2[40],filename3[40];
	printf("正在创建新的过题提交记录文件\n");	
	printf("请输入新放文件名： "); 
	scanf("%s",filename2); 
	strcat(filename2,".txt");
	FILE *fp2;
	char name3[50] = {"D:/C语言大作业/"},name_2[50] = {"D:/C语言大作业/"};
	strcat(name3,filename2);
	fp2 = fopen(name3,"w");
	fprintf(fp2,"ID\t题号\t\t时间\n"); 
	for(int i = 1,k; i <= All.Stu; i++)
	{
		for(k = 0; k < All.Stu; k++)
			if(i== stu[k].id)
				break;	
		for(int j = 0; j<stu[k].sum;j++)
			if(stu[k].qualified[j]==1) 
				fprintf(fp2,"%d\t%d\t%s %s\n",stu[k].id,stu[k].problem_id[j],stu[k].date[j],stu[k].time[j]);
	}
	fclose(fp2);
	printf("***文件生成成功***\n文件位置：%s\n\n",name3);
	
	FILE *fp4;
	fp4 = fopen(name3,"r");
	int a = 0;
	int Ac_proId[1000]={0};
	char arr[100];
	fseek(fp4,0,0);
	fgets(arr,100,fp4);
	while(!feof(fp4))
	{	
		fscanf(fp4,"%d %d %s %s",&record_2[a].id,&record_2[a].problem_id,record_2[a].date,record_2[a].time);
		a++;
	}
	fclose(fp);
	
	qsort(record_2,a,sizeof(record_2[0]),cmp2); 
	for(int i = 0; i < a; i++) {
		if(stu[record_2[i].id].qualified[record_2[i].problem_id-1000]==1)		
		{
			Ac_proId[record_2[i].problem_id - 1000]++;
			stu[record_2[i].id].id = record_2[i].id;
			stu[record_2[i].id].grade += Grade(Ac_proId[record_2[i].problem_id - 1000]);
			stu[record_2[i].id].problem_id[stu[record_2[i].id].sum] = record_2[i].problem_id;
			strcpy(stu[record_2[i].id].date[stu[record_2[i].id].sum],record_2[i].date);
			strcpy(stu[record_2[i].id].time[stu[record_2[i].id].sum],record_2[i].time);		
			stu[record_2[i].id].sum++;
		}
	}
	
	qsort(stu,All.Stu+1,sizeof(stu[0]),cmp1);
	int max = stu[0].grade; 
	printf("正在创建新的成绩排行文件\n");	
	printf("请输入新放文件名： ");  
	scanf("%s",filename3); 
	strcat(filename3,".txt");
	FILE *fp3;
	strcat(name_2,filename3);
	fp3 = fopen(name_2,"w");
	fprintf(fp3,"排行\t学生ID\t积分\t成绩");
	if(order == 1) fprintf(fp3,"  是否存在不良行为\n");
	else fprintf(fp3,"\n"); 
	for(int i = 0; i < All.Stu; i++) {
		char N = stu[i].IsCheat == 1 ? 'Y':'N';
		
		stu[i].mark = round(100+log(stu[i].grade*1.0/max*1.0)*10);
		fprintf(fp3,"%d\t%d\t%d\t%d",i+1,stu[i].id,stu[i].grade,stu[i].mark);
		if(order == 1) fprintf(fp3,"\t  %c\n",N);
		else fprintf(fp3,"\n");
	}
	
	printf("***文件生成成功***\n文件位置：%s\n\n",name_2);
	return 0;
} 
