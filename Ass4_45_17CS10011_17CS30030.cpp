#include <bits/stdc++.h>
#include <pthread.h>
#include <signal.h>
#include <unistd.h>
using namespace std;
#define M 500
#define del .02
#define MX 20

/*
Threads : 
N worker 
Scheduler

Reporter

Data structure :
STATUS


*/
pthread_mutex_t lock_producer, lock_consumer;
void wake_up(int a)
{
	;
	// cout<<"waking "<<pthread_self() <<endl;
}
void sleep_bro(int a)
{
	// cout<<"sleeping "<<pthread_self() <<endl;
	pause();
}

struct STATUS
{
	int N;
	int terminated;
	int remain;
	pthread_t tid[MX];
	bool is_producer[MX];
	bool still_alive[MX];
	map<pthread_t, int> index;
	int start;
	int now;
	stack<int> ter_stk;
	stack<int> exe_stk;
	void ended(pthread_t ptid)
	{
		still_alive[index[ptid]] = 0;
		ter_stk.push(index[ptid]);
	}
	int get_next()
	{
		now = (now + 1) % N;
		while (still_alive[now] == 0)
		{
			now = (now + 1) % N;
		}
		return now;
	}
};
struct BUFFER
{
	stack<int> a;

	void init()
	{
		// a.clear();;
		;
	}
	bool is_full()
	{
		return int(a.size()) == M;
	}
	bool is_empty()
	{
		return int(a.size()) == 0;
	}
	void insert(int x)
	{
		a.push(x);
	}
	void pop()
	{
		// int ret=a[start];
		a.pop();
		// return ret;
	}
	int size()
	{
		return int(a.size());
	}
};
BUFFER buff;
STATUS status;
void *producer(void *arg)
{
	signal(SIGUSR1, sleep_bro);
	signal(SIGUSR2, wake_up);

	while (status.start == -1)
		;
	for (int i = 0; i < 1000; i++)
	{
		while (buff.is_full())
			;
		pthread_mutex_lock(&lock_producer);
		while (buff.is_full())
			;
		buff.insert(rand());
		pthread_mutex_unlock(&lock_producer);
	}
	status.terminated++;
	status.ended(pthread_self());
	pthread_exit(0);
}
void *consumer(void *arg)
{
	signal(SIGUSR1, sleep_bro);
	signal(SIGUSR2, wake_up);
	while (status.start == -1)
		;
	while (status.remain)
	{
		while (buff.is_empty())
		{
			if (status.remain == 0)
			{
				pthread_mutex_unlock(&lock_consumer);
				status.terminated++;
				status.ended(pthread_self());

				pthread_exit(0);
			}
		}
		pthread_mutex_lock(&lock_consumer);
		while (buff.is_empty())
		{
			if (status.remain == 0)
			{
				pthread_mutex_unlock(&lock_consumer);
				status.terminated++;
				status.ended(pthread_self());

				pthread_exit(0);
			}
		}
		buff.pop();
		status.remain--;
		pthread_mutex_unlock(&lock_consumer);
	}
	status.terminated++;
	status.ended(pthread_self());
	pthread_exit(0);
}

void *scheduler(void *arg)
{
	while (status.start == -1)
		;
	while (status.terminated < status.N)
	{

		int now = status.get_next();
		status.exe_stk.push(now);
		pthread_kill(status.tid[now], SIGUSR2); //send resume signal SIGUSR2
		// sleep(del);
		sleep(1);
		pthread_kill(status.tid[now], SIGUSR1); //send sleep signal SIGUSR1
	}
	pthread_exit(0);
}
void *reporter(void *arg)
{
	while (status.start == -1)
		;
	// pthread_exit(0);
	while ((status.terminated < status.N) or (int(status.ter_stk.size()) or int(status.exe_stk.size())))
	{
		while (int(status.ter_stk.size()))
		{
			int now = status.ter_stk.top();
			status.ter_stk.pop();
			if (status.is_producer[now])
			{
				cout << "producer with thread id " << status.tid[now] << " is terminated" << endl;
			}
			else
			{
				cout << "consumer with thread id " << status.tid[now] << " is terminated" << endl;
			}
			cout << "BUFFER SIZE = " << buff.size() << endl;
			cout << "remain " << status.remain << endl;
		}
		while (int(status.exe_stk.size()))
		{
			int now = status.exe_stk.top();
			status.exe_stk.pop();
			if (status.is_producer[now])
			{
				cout << "producer with thread id " << status.tid[now] << " is executing now" << endl;
			}
			else
			{
				cout << "consumer with thread id " << status.tid[now] << " is executing now" << endl;
			}
			cout << "BUFFER SIZE = " << buff.size() << endl;
			cout << "remain " << status.remain << endl;
		}
		// cout<<"remain "<<status.remain<<endl;
		// cout<<"BUFFER SIZE = "<<buff.size<<endl;
	}
	cout << "BUFFER SIZE = " << buff.size() << endl;
	cout << "remain " << status.remain << endl;
	pthread_exit(0);
}

int main()
{
	signal(SIGUSR1, sleep_bro);
	signal(SIGUSR2, wake_up);
	pthread_mutex_init(&lock_producer, NULL);
	pthread_mutex_init(&lock_consumer, NULL);
	cout << "Enter number of workers : ";
	int n;
	cin >> n;
	status.N = n;
	status.terminated = 0;
	status.remain = 0;
	status.start = -1;
	status.now = -1;
	buff.init();
	// pthread_attr_t attr; // what's this
	// pthread_attr_init(&attr);
	for (int i = 0; i < n; i++)
	{
		if (rand() % 2)
		{
			status.is_producer[i] = 1;
			pthread_create(&status.tid[i], NULL, producer, NULL);
			status.remain += 1000;
		}
		else
		{
			status.is_producer[i] = 0;
			pthread_create(&status.tid[i], NULL, consumer, NULL);
		}
		status.index[status.tid[i]] = i;
		pthread_kill(status.tid[i], SIGUSR1);
		status.still_alive[i] = 1;
	}
	bool flag = 0;
	for (int i = 0; i < n; i++)
		if (status.is_producer[i] == 0)
			flag = 1;
	if (flag == 0)
		return 0;
	flag = 0;
	for (int i = 0; i < n; i++)
		if (status.is_producer[i] == 1)
			flag = 1;
	if (flag == 0)
		return 0;
	pthread_t sch, rep;
	pthread_create(&sch, NULL, scheduler, NULL);
	pthread_create(&rep, NULL, reporter, NULL);
	status.start = 1;
	pthread_join(sch, NULL);
	pthread_join(rep, NULL);
	return 0;
}
