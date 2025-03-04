#include <stdio.h>
#include <sys/types.h>
#include <sys/msg.h>
#include <sys/ipc.h>
#include <stdlib.h>
#include <unistd.h>
#include <wait.h>
#include <string.h>
#include <signal.h>

#define MSGKEY 75

struct msgform
{
    long mtype;
    char msgtext[1030];
}msg;

int msgqid, pid;

int wait_mark = 1;

void stop()
{
   wait_mark=0;
}

void waiting()
{
    while(wait_mark!=0);
}

void CLIENT() //ͨ���������Ʒ�����Ϣ
{
    int i = 100;
    char string_i[15]; // �����Ϣ
    msgqid = msgget(MSGKEY, 0777); // ��һ����Ϣ���У�0777 ���ļ��Ĵ�ȡȨ��
    signal(SIGQUIT,stop);
    for (i; i >= 1; i--) 
    {
        wait_mark=1;
        waiting();

        msg.mtype = i;
        printf("(client) sent #%d\n", i);
        sprintf(msg.msgtext, "the content of message is: ");
        sprintf(string_i, "message %d", i);
        strcat(msg.msgtext, string_i);
        strcat(msg.msgtext, "\n");
        msgsnd(msgqid, &msg, sizeof(msg.msgtext), 0);

        // �����źŸ������
        kill(pid, 16);
    }
    exit(0);
}

void SERVER() 
{
    signal(SIGQUIT,SIG_IGN);
    while (1) 
    {
        wait_mark=1;
        signal(16,stop);
        waiting();
        msgrcv(msgqid, &msg, sizeof(msg.msgtext), 0, 0);
        printf("(server) received message %d, ", msg.mtype);
        printf("%s", msg.msgtext);
        if (msg.mtype == 1) break; // �����յ�����Ϊ1����Ϣʱ�˳�
    }
    msgctl(msgqid, IPC_RMID, 0);
    exit(0);
}

int main() {
    printf("����quit\n");
    msgqid = msgget(MSGKEY, 0777 | IPC_CREAT);
    pid = fork();
    if (pid == 0)   //�ӽ���
    {
        SERVER();
    }
   if(pid > 0)       //������
    {
        CLIENT(); 
        wait(0);
    } 
    return 0;
 
}
