#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <time.h>
#include <sys/ipc.h>
#include <sys/msg.h>

#define PERMS 0644
struct my_msgbuf {
   long mtype;
   int mint; // int istället för string
};

int main(void) {
   srand(time(NULL));
   char mtext[200];
   struct my_msgbuf buf;
   int msqid;
   int len;
   key_t key;
   int i = 0;
   if(!system("touch msgq.txt")){
   }

   if ((key = ftok("msgq.txt", 'B')) == -1) {
      perror("ftok");
      exit(1);
   }

   if ((msqid = msgget(key, PERMS | IPC_CREAT)) == -1) {
      perror("msgget");
      exit(1);
   }
   printf("message queue: ready to send messages.\n");
   printf("Press enter to continue, ^D to quit:\n");
   buf.mtype = 1; /* we don't really care in this case */
   char* _ = fgets(mtext, sizeof mtext, stdin);
   while(i < 50) {
      buf.mint = rand(); // random siffra
      len = sizeof(buf.mint);
      printf("%i\n",buf.mint);
      if (msgsnd(msqid, &buf, len, 0) == -1)
         perror("msgsnd");
      i++;
   }
   printf("Press enter to continue, ^D to quit:\n");
   _ = fgets(mtext, sizeof mtext, stdin);
   buf.mint = -1;
   len = sizeof(buf.mint);
   if (msgsnd(msqid, &buf, len, 0) == -1) /* +1 for '\0' */
      perror("msgsnd");

   if (msgctl(msqid, IPC_RMID, NULL) == -1) {
      perror("msgctl");
      exit(1);
   }
   printf("message queue: done sending messages.\n");
   return 0;
}
