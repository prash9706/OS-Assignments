/*******************************************************************
 *File: disk_scheduling.c
 *Author: Prashant Yadav
 *Procedure:
 *uniform - It generates numbers uniformly between given high and low values.
 *is_unique - It validates for the uniqueness of generated track sector combination.
 *sort_io_data - It sorts generated io data by tracks in ascending order.
 *generate_io_data - Before each simulation it populates tracks and sectors array with arbitrary values with in specified ranges.
 *print_data - It prints track sector cobination with in a length.
 *get_seek_time - Given current track,sector combination and a target track, sector combination it evaluates total seek time.
 *fifo_disk_scheduling - For a given IO requests it evaluates seek time for FIFO disk scheduing policy.
 *sstf_disk_scheduling - For a given IO requests it evaluates seek time for SSTF disk scheduing policy.
 *scan_disk_scheduling - For a given IO requests it evaluates seek time for SCAN disk scheduing policy.
 *cscan_disk_scheduling - For a given IO requests it evaluates seek time for CSCAN disk scheduing policy.
 *simulate - This method simulates disk scheduling for all policies for a given set of IO requests 1000 times.
 *print_matrix - This routine is used to print seek time matrix.
 *main - Driver main routine from where program starts executing. In this routine we simulate disk scheduling for number of IO requests varies from 500 to 1000.
 *******************************************************************/
#include<stdio.h>
#include<stdlib.h>
#include<time.h>
#include<string.h>
#include<math.h>

int sectors[1000];   //Sector position to perform read
int tracks[1000];    //Track postions to perform read
int sorted_sectors[1000]; //Sorted values of sectors by track.
int sorted_tracks[1000];  //Sorted array by track in ascending order.
int length=0;    //Length of IO requests array. varies from 500-1000
//Each column stores seek time for FIFO, SSTF, SCAN, CSCAN disk scheduling policies.
//index0 - FIFO, index1 - SSTF, index2 - SCAN, index3 - CSCAN 
float seek_time[501][4];  
float tseek = 2,sseek=0.005; //Track seek time 2ms per track, Sector seek time 0.005ms per sector

/*******************************************************************
 *int uniform(int lo, int hi)
 *Author: Prashant Yadav
 *Date: 10/28/2019
 *Description: It geneates uniformly distributed random numbers between given range.
 *Parameters:
 *int lo I/P: It is lower bound on given ranges
 *int hi I/P: It is upper bound on given ranges
 *This routine does not return anything.
 *******************************************************************/
int uniform(int lo, int hi)
{
  int x, y=(hi-lo)+1,z=RAND_MAX/y;
  while(y<=(x=(random()/z)));
  return x+lo;
}

/*******************************************************************
 *int is_unique(int sec, int track, int index)
 *Author: Prashant Yadav
 *Date: 11/12/2019
 *Description: It validates whether given track sector in unique with in given index or not.
 *Parameters:
 *int sec I/P: sector value.
 *int track I/P: track value.
 *int index I/P: Index under which we have to check.
 *int O/P: It returns 0 if value is not unique, 1 otherwise.
 *******************************************************************/
int is_unique(int sec, int track, int index){
  int i;
  for(i=0;i<index;i++){
    if(sectors[i]==sec && tracks[i]==track){
      return 0;  //Return 0 if track sector combination already exists.
    }
  }
  return 1; //Return 1 otherwise.
}

/*******************************************************************
 *void sort_io_data()
 *Author: Prashant Yadav
 *Date: 11/12/2019
 *Description: It sorts track and sector combination by tracks.
 *Parameters:
 *This routine does not accept anything.
 *This routine does not return anything.
 *******************************************************************/
void sort_io_data(){
  int i,j;
  for(i=0;i<length;i++){
    for(j=0;j<length-i;j++){
      if(sorted_tracks[j]>sorted_tracks[j+1]){  //Sort track and sector arrays.
        int temp = sorted_tracks[j+1];
	sorted_tracks[j+1] = sorted_tracks[j];
	sorted_tracks[j] = temp;
	temp = sorted_sectors[j+1];
	sorted_sectors[j+1] = sorted_sectors[j];
	sorted_sectors[j] = temp;
      }
    }
  }
}

/*******************************************************************
 *void generate_io_data()
 *Author: Prashant Yadav
 *Date: 11/12/2019
 *Description: It generates unique track and sector combinations.
 *Parameters:
 *This routine does not accept anything.
 *This routine does not return anything.
 *******************************************************************/
void generate_io_data(){
  int i=0;
  srand(time(0));
  while(i<length){
    int sector = uniform(1,12000);  //Random value for sector
    int track  = uniform(1,5000);   //Random value for track
    if(is_unique(sector, track,i)==1){ //If it is unique populate the array
      sectors[i] = sector;
      sorted_sectors[i] = sector;
      tracks[i] = track;
      sorted_tracks[i] = track;
      i++;
    }
 }
 sort_io_data();  //Sort the data by track
}

/*******************************************************************
 *void print_data()
 *Author: Prashant Yadav
 *Date: 11/12/2019
 *Description: It prints IO requests i.e track and sector combinations.
 *Parameters:
 *This routine does not accept anything.
 *This routine does not return anything.
 *******************************************************************/
void print_data(){
  int i;
  for(i=0;i<length;i++){
    printf("(%d,%d)\n",sorted_tracks[i],sorted_sectors[i]);
  }
}

/*******************************************************************
 *float get_seek_time(int tr, int sec, int totr, int tosec)
 *Author: Prashant Yadav
 *Date: 11/12/2019
 *Description: It calculates seek time to move from one position to another
 *Parameters:
 *int tr I/P:Source track
 *int sec I/P:Source sector
 *int totr I/P:Destination track
 *int tosec I/P:Destination source
 *float O/P:It returns time taken to move from source to destination
 *******************************************************************/
float get_seek_time(int tr, int sec, int totr, int tosec){
    float ttime = abs(tr-totr)*tseek; //Track seek time
    int isec = sec - ((int)(ttime/sseek)%12000);  //Sectors moved with in that time
    if(isec<0){
      isec+=12000;
    }
    return ttime + abs(tosec - isec)*sseek; //Total time
}

/*******************************************************************
 *float fifo_disk_scheduling()
 *Author: Prashant Yadav
 *Date: 11/12/2019
 *Description: It calculates total time to server all I/O requests by FIFO Policy.
 *Parameters:
 *This method doesn't accept anything.
 *float O/P:It returns time taken to complete all I/O requests
 *******************************************************************/
float fifo_disk_scheduling(){
  int track_pos=2500, sector_pos=0,i;
  float time=0;
  float ttime,sstime;
  for(i=0;i<length;i++){ //Iterate over all positions
    time = time + get_seek_time(track_pos, sector_pos, tracks[i], sectors[i]);
    track_pos = tracks[i];
    sector_pos = sectors[i];
  }
  return time;
}

/*******************************************************************
 *float sstf_disk_scheduling()
 *Author: Prashant Yadav
 *Date: 11/12/2019
 *Description: It calculates total time to server all I/O requests by SSTF Policy.
 *Parameters:
 *This method doesn't accept anything.
 *float O/P:It returns time taken to complete all I/O requests
 *******************************************************************/
float sstf_disk_scheduling(){
  int track_pos=2500, sector_pos=0,i;
  float time=0;
  float ttime,sstime;
  int p=0,curr, pre, post;
  while(p<length && sorted_tracks[p]<track_pos){p++;}
  if(p==length){
    curr = p-1;
  }else if(p==0){
    curr = 0;
  }else {
    curr = abs(sorted_tracks[p]-track_pos)>abs(sorted_tracks[p-1]-track_pos)?p-1:p;
  }
  pre = curr-1;
  post = curr+1;
  while(1){ //Iterate over Sorted arrays and check with is closer to the current position
    //printf("%d %d %d\n",pre,curr,post);
    time = time + get_seek_time(track_pos, sector_pos, sorted_tracks[curr], sorted_sectors[curr]);
    track_pos = sorted_tracks[curr];
    sector_pos = sorted_sectors[curr];
    if(pre<0 &&  post>=length){
      break;
    }else if(pre<0){
      curr = post;
      post++;
    }else if(post>=length){
      curr = pre;
      pre--;
    }else{
      curr = abs(sorted_tracks[pre]-track_pos)>abs(sorted_tracks[post]-track_pos)?post:pre;
      pre = pre==curr?pre-1:pre;
      post = post==curr?post+1:post;
    }
  }
  return time;
}

/*******************************************************************
 *float scan_disk_scheduling()
 *Author: Prashant Yadav
 *Date: 11/12/2019
 *Description: It calculates total time to server all I/O requests by SCAN Policy.
 *Parameters:
 *This method doesn't accept anything.
 *float O/P:It returns time taken to complete all I/O requests
 *******************************************************************/
float scan_disk_scheduling(){
  int track_pos=2500, sector_pos=0,i;
  float time=0;
  float ttime,sstime;
  int p=0;
  while(p<length && sorted_tracks[p]<track_pos){p++;}
  for(i=p;i<length;i++){
    time = time + get_seek_time(track_pos, sector_pos, sorted_tracks[i], sorted_sectors[i]);
    track_pos = sorted_tracks[i];
    sector_pos = sorted_sectors[i];
  }
  for(i=p-1;i>=0;i--){
    time = time + get_seek_time(track_pos, sector_pos, sorted_tracks[i], sorted_sectors[i]);
    track_pos = sorted_tracks[i];
    sector_pos = sorted_sectors[i];
  }
  return time;
}

/*******************************************************************
 *float csan_disk_scheduling()
 *Author: Prashant Yadav
 *Date: 11/12/2019
 *Description: It calculates total time to server all I/O requests by CSCAN Policy.
 *Parameters:
 *This method doesn't accept anything.
 *float O/P:It returns time taken to complete all I/O requests
 *******************************************************************/
float csan_disk_scheduling(){
  int track_pos=2500, sector_pos=0,i;
  float time=0;float ttime,sstime;
  int p=0;
  while(p<length && sorted_tracks[p]<track_pos){p++;}
  for(i=p;i<length;i++){
    time = time + get_seek_time(track_pos, sector_pos, sorted_tracks[i], sorted_sectors[i]);
    track_pos = sorted_tracks[i];
    sector_pos = sorted_sectors[i];
  }
  time = time + 1.5; //Snap the head back to 0
  for(i=0;i<p;i++){
    time = time + get_seek_time(track_pos, sector_pos, sorted_tracks[i], sorted_sectors[i]);
    track_pos = sorted_tracks[i];
    sector_pos = sorted_sectors[i];
  }
  return time;
}

/*******************************************************************
 *void simulate()
 *Author: Prashant Yadav
 *Date: 11/12/2019
 *Description: It simulates disk scheduling for given I\O requests by four policies namely FIFO,SSTF,SCAN and CSCAN.
 *Parameters:
 *This method doesn't accept anything.
 *This method doesn't return anything.
 *******************************************************************/
void simulate(){
  int i;
  for(i=0;i<1000;i++){
    generate_io_data(); //Generate random I/O Requests
    //print_data();
    seek_time[length-500][0]+=(float)(fifo_disk_scheduling()); //Simulate for FIFO
    seek_time[length-500][1]+=(float)(sstf_disk_scheduling()); //Simulate for SSTF
    seek_time[length-500][2]+=(float)(scan_disk_scheduling()); //Simulate for SCAN
    seek_time[length-500][3]+=(float)(csan_disk_scheduling()); //Simulate for CSCAN
  }
}

/*******************************************************************
 *void print_matrix()
 *Author: Prashant Yadav
 *Date: 11/12/2019
 *Description: It prints final matrix for all seek times
 *Parameters:
 *This method doesn't accept anything.
 *This method doesn't return anything.
 *******************************************************************/
void print_matrix(){
  printf("I/O Request\tFIFO\t\tSSTF\t\tSCAN\t\tCSCAN\n");
  int i;
  for(i=0;i<501;i++){
    seek_time[i][0]/=(1000*1000);
    seek_time[i][1]/=(1000*1000);
    seek_time[i][2]/=(1000*1000);
    seek_time[i][3]/=(1000*1000);
    printf("%d\t\t%.2f\t\t%.2f\t\t%.2f\t\t%.2f\n",i+500,seek_time[i][0],seek_time[i][1],seek_time[i][2],seek_time[i][3]);
  }
}

/*******************************************************************
 *void main()
 *Author: Prashant Yadav
 *Date: 11/12/2019
 *Description: This is the main driver method. It simulats process for length from 500 to 1000
 *Parameters:
 *This method doesn't accept anything.
 *This method doesn't return anything.
 *******************************************************************/
void main(){
 for(length=500;length<=1000;length++){
   printf("Simulating for %d \n",length);
   simulate();
 }
 print_matrix();
}
