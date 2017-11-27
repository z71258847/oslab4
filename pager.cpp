#include <bits/stdc++.h>
#define lifo 0
#define random 1
#define lru 2

using namespace std;

ifstream randomin("random-numbers.txt");

int M, P, S, J, N, R, tot_frame, tot_page, tot_process, debug=0;
int tot_finished, cur_t = 0, last_in;

double get_random(){
	int x;
	double y;
	randomin >> x;
	//printf("used random number:%d\n", x);
	y = (double) x / 2147483648.0;
	return y;
}

struct Frame{
	int process_index, page_index, last_use;
	bool used;
	Frame():used(false){};
};

struct Process{
	int cur, fault_count = 0, evict_count = 0, recidency_time = 0;
	vector<int> load_time;
	double A, B, C;
	int remain;
	bool finished;
	vector<int> page_table;
	Process():fault_count(0), evict_count(0), recidency_time(0), load_time(0){};
	Process(double _A, double _B, double _C, int index){
		A = _A;
		B = _B;
		C = _C;
		cur = (index * 111) % S;
		page_table.clear();
		page_table.assign(tot_page, -1);
		load_time.clear();
		load_time.assign(tot_page, -1);
		remain = N;
		finished = false;
		fault_count = 0;
		evict_count = 0;
		recidency_time = 0;
	}
	
	void get_next(){
		double y = get_random();
		if (y<A) cur = (cur + 1) % S;
		else if (y<A+B) cur = (cur - 5 + S) % S;
		else if (y<A+B+C) cur = (cur + 4) % S;
		else{
			int x;
			randomin >> x;
			//printf("used random number:%d\n", x);
			cur = x % S;
		}
	}
};

Process pros[5];
vector<Frame> frame_table;

int check_free(){
	for (int i=tot_frame-1; i>=0; i--) {
		if (frame_table[i].used == false) {
			return i;
		}
	}
	return -1;
}

void lifo_replacement(int x, int page_number){
	int last_process = frame_table[last_in].process_index;
	int last_page = frame_table[last_in].page_index;
	pros[last_process].page_table[last_page] = -1;
	frame_table[last_in].process_index = x;
	frame_table[last_in].page_index = page_number;
	pros[x].page_table[page_number] = last_in;
	pros[last_process].evict_count++;
	pros[last_process].recidency_time += cur_t - pros[last_process].load_time[last_page];
	pros[x].load_time[page_number] = cur_t;
	//printf("%d evict %d on frame %d\n", x, last_process, last_in);
}

void random_replacement(int x, int page_number){
	int choose_frame;
	randomin >> choose_frame;
	//printf("%d used random number: %d", x, choose_frame);
	choose_frame %= tot_frame;
	int last_process = frame_table[choose_frame].process_index;
	int last_page = frame_table[choose_frame].page_index;
	pros[last_process].page_table[last_page] = -1;
	frame_table[choose_frame].process_index = x;
	frame_table[choose_frame].page_index = page_number;
	pros[x].page_table[page_number] = choose_frame;
	pros[last_process].evict_count++;
	pros[last_process].recidency_time += cur_t - pros[last_process].load_time[last_page];
	pros[x].load_time[page_number] = cur_t;
	//printf("%d evict %d on frame %d\n", x, last_process, choose_frame);
}

int choose_lru(){
	int minn, minx=999999999;
	for (int i=0; i<tot_frame; i++) {
		if (frame_table[i].last_use<minx) {
			minx = frame_table[i].last_use;
			minn = i;
		}
	}
	return minn;
}

void lru_replacement(int x, int page_number){
	int choose_frame = choose_lru();
	int last_process = frame_table[choose_frame].process_index;
	int last_page = frame_table[choose_frame].page_index;
	pros[last_process].page_table[last_page] = -1;
	frame_table[choose_frame].process_index = x;
	frame_table[choose_frame].page_index = page_number;
	pros[x].page_table[page_number] = choose_frame;
	pros[last_process].evict_count++;
	pros[last_process].recidency_time += cur_t - pros[last_process].load_time[last_page];
	pros[x].load_time[page_number] = cur_t;
	//printf("%d evict %d on frame %d\n", x, last_process, choose_frame);
}

void do_replacement(int x, int page_number){	
	if (R == lifo) {
		lifo_replacement(x, page_number);
		return;
	}
	if (R == random) {
		random_replacement(x, page_number);
		return;
	}
	if (R == lru) {
		lru_replacement(x, page_number);
		return;
	}
}

int main(int argc, char* argv[]){
	M = atoi(argv[1]);
	P = atoi(argv[2]);
	S = atoi(argv[3]);
	J = atoi(argv[4]);
	N = atoi(argv[5]);
	//string key = argv[6];
	debug = atoi(argv[7]);
	if (strcmp(argv[6],"lifo")==0) R=0;
	else if(strcmp(argv[6],"random")==0) R=1;
	else if(strcmp(argv[6],"lru")==0) R=2;
	//printf("%d\n", R);
	//M=20; P=10; S=10; J=2; N=10; R=2; debug=0;
	tot_frame = M / P;
	tot_page = S / P;
	frame_table.clear();
	frame_table.assign(tot_frame, Frame());
	if (J==1){
		tot_process = 1;
		pros[1] = Process(1,0,0,1);
	}
	else if (J==2){
		tot_process = 4;
		for (int i=1; i<=tot_process; i++) 
			pros[i] = Process(1,0,0,i);
	}
	else if (J==3){
		tot_process = 4;
		for (int i=1; i<=tot_process; i++) 
			pros[i] = Process(0,0,0,i);		
	}
	else{
		tot_process = 4;
		pros[1] = Process(0.75,0.25,0,1);
		pros[2] = Process(0.75,0,0.25,2);
		pros[3] = Process(0.75,0.125,0.125,3);
		pros[4] = Process(0.5,0.125,0.125,4);
	}
	tot_finished = 0;
	while (tot_finished < tot_process){
		for (int i=1; i<=tot_process; i++) {
			if (!pros[i].finished){
				for (int q=0; q<3; q++) {
					cur_t++;
					int cur_page = pros[i].cur / P;
					//printf("%d reference %d (page %d)\n", i, pros[i].cur, cur_page);
					if (pros[i].page_table[cur_page] == -1){
						pros[i].fault_count++;
						int free_frame = check_free();
						if (free_frame == -1){
							do_replacement(i, cur_page);
						}
						else {
							frame_table[free_frame].process_index = i;
							frame_table[free_frame].page_index = cur_page;
							frame_table[free_frame].last_use = cur_t;
							frame_table[free_frame].used = true;
							pros[i].page_table[cur_page] = free_frame;
							pros[i].load_time[cur_page] = cur_t;
							//printf("fault, using free frame %d\n", free_frame);
						}
						last_in = pros[i].page_table[cur_page];
						frame_table[last_in].last_use = cur_t;
					}
					else {
						int frame_used = pros[i].page_table[cur_page];
						frame_table[frame_used].last_use = cur_t;
						//printf("hit frame %d\n", frame_used);
					}
					if (--pros[i].remain == 0){
						pros[i].finished = true;
						tot_finished++;
						pros[i].get_next();
						break;
					}
					pros[i].get_next();
				}
			}
		}
	}
	printf("The machine size is %d.\n", M);
	printf("The page size is %d.\n", P);
	printf("The process size is %d.\n", S);
	printf("The job mix number is %d.\n", J);
	printf("The number of references per process is %d.\n", N);
	printf("The replacement algorithm is %s.\n", argv[6]);
	printf("The level of debugging output is %d\n\n", debug);
	int tot_fault = 0, tot_recidency = 0, tot_evict = 0;
	for (int i=1; i<=tot_process; i++) {
		if (pros[i].evict_count==0) {
			printf("Process %d had %d faults.\n     With no evictions, the average residence is undefined.\n", i, pros[i].fault_count);
			tot_fault += pros[i].fault_count;
		}
		else{
			printf("Process %d had %d faults and %.5lf average residency.\n", i, pros[i].fault_count, (double) pros[i].recidency_time/pros[i].evict_count);
			tot_fault += pros[i].fault_count;
			tot_recidency += pros[i].recidency_time;
			tot_evict += pros[i].evict_count;
		}
	}
	cout << endl;
	if (tot_evict == 0) {
		printf("The total number of faults is %d.\n     With no evictions, the overall average residence is undefined.\n", tot_fault);
	}
	else{
		printf("The total number of faults is %d and the overall average residency is %.5lf.\n", tot_fault, (double) tot_recidency/tot_evict);
	}
	randomin.close();
} 
