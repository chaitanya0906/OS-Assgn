#include <fcntl.h>
#include <iostream>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <algorithm>
#include <cctype>
#include <fstream>
#include <functional>
#include <iostream>
#include <locale>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <sys/shm.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <vector>
using namespace std;

struct JOB
{
	int process_id;
	int producer_number;
	int priority;
	int compute_time;
	int job_id;
};

struct priority_qu
{
	JOB a[10];
	int sz;
	void init()
	{
		sz = 0;
	}
	JOB get_top()
	{
		int indexTop = 0;
		for (int i = 0; i < sz; i++)
		{
			if (a[i].priority > a[indexTop].priority)
				indexTop = i;
		}
		JOB ret = a[indexTop];
		sz--;
		for (int i = indexTop; i < sz; i++)
		{
			a[i] = a[i + 1];
		}
		return ret;
	}
	bool empty()
	{
		return sz == 0;
	}
	bool is_full()
	{
		return sz == 10;
	}
	void insert(JOB x)
	{
		a[sz] = x;
		sz++;
	}
};

struct shared_data
{
	int job_created;
	int job_completed;
	int NJ;
	priority_qu q;
	int consumer_update;
	int producer_update;
	sem_t semaphore_consumer;
	sem_t semaphore_producer;
	void init()
	{
		job_created = 0;
		job_completed = 0;
		NJ = -1;
		q.init();
		consumer_update = 0;
		producer_update = 0;
		sem_init(&semaphore_producer, 1, 0);
		sem_init(&semaphore_consumer, 1, 0);
	}
};

JOB generate_job(int pro_num, int job_cr)
{
	JOB ans;
	ans.job_id = ((rand()) % 100000) + 1;
	ans.compute_time = (rand() % 4) + 1;
	ans.producer_number = pro_num;
	ans.priority = (rand() % 10) + 1;
	ans.process_id = job_cr;
	return ans;
}
int main()
{

	key_t key = ftok("lavda", 65);
	int shmid = shmget(key, 1024, 0666 | IPC_CREAT);
	shared_data *s = (shared_data *)shmat(shmid, (void *)0, 0);
	(*s).init();

	cout << "Enter number of producer : ";
	int NP;
	cin >> NP;
	bool flag = 1; //1 if process is god
	int pro_num;
	for (pro_num = 1; pro_num <= NP; pro_num++)
	{
		int pid = fork();
		if (pid == 0)
		{
			flag = 0;
			break;
		}
		cout << "producer  " << pro_num << " :  " << pid << endl;
	}

	if (flag == 0)
	{
		// do producer thing
		//s = (shared_data*) shmat(shmid,NULL,0);

		while (((*s).NJ) == -1)
			;
		while ((*s).job_created < ((*s).NJ))
		{
			if ((*s).q.is_full())
				continue;
			sem_wait(&((*s).semaphore_producer));
			// int tt=10;
			// while(tt--) cout<<pro_num<<" ";cout<<endl;
			//cout<<"JOBS  : "<<(*s).NJ<<endl;

			if ((*s).job_created < ((*s).NJ) and (*s).q.is_full() == 0)
			{
				cout << "produced : " << (*s).job_created + 1 << " in  " << pro_num << endl;
				(*s).job_created++;
				(*s).q.insert(generate_job(pro_num, (*s).job_created));
			}
			// tt=10	;
			// while(tt--) cout<<pro_num<<" ";cout<<endl;
			// (*s).producer_update=0;

			sem_post(&((*s).semaphore_producer));
		}
		cout << "producer ended num : " << pro_num << endl;
		shmdt(s);
		exit(0);
	}
	cout << "Enter number of consumer : ";
	int NC;
	cin >> NC;
	int con_num;
	for (con_num = 1; con_num <= NC; con_num++)
	{
		int pid = fork();
		if (pid == 0)
		{
			flag = 0;
			break;
		}
		cout << "consumer  " << con_num << " :  " << pid << endl;
	}
	if (flag == 0)
	{
		// do consumer thing
		//s = (shared_data*) shmat(shmid,NULL,0);
		while (((*s).NJ) == -1)
			;

		while ((*s).job_completed < ((*s).NJ))
		{
			if ((*s).q.empty())
				continue;
			JOB temp;
			bool flag1 = 0;
			sem_wait(&((*s).semaphore_consumer));

			if ((*s).job_completed < ((*s).NJ) and (*s).q.empty() == 0)
			{
				temp = (*s).q.get_top();
				flag1 = 1;
				cout << "now executing : " << temp.process_id << " in  " << con_num << " produced by  " << temp.producer_number << endl;

				(*s).job_completed++;
			}

			sem_post(&((*s).semaphore_consumer));
			if (flag1)
				sleep(temp.compute_time);
		}

		cout << "consumer ended num : " << con_num << endl;
		//pthread_exit(NULL);
		shmdt(s);
		exit(0);
	}

	cout << "Enter number of jobs : ";
	cin >> (*s).NJ;
	cout << "JOBS  : " << (*s).NJ << endl;
	;
	sem_post(&((*s).semaphore_producer));
	sem_post(&((*s).semaphore_consumer));
	shmdt(s);
	exit(0);

	return 0;
}