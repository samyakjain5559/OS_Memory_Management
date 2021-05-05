#include <stdio.h>
#include <stdlib.h>

int physicalmemsize;
char tempaddress[80];
int logicaladdress,pagenum,offset,logicaladdsave;
//int pmemory[physicalmemsize][256];  // this physical mem size vary
int framenumber;
struct node *head;
//File *store;
int thit = 0;
int pthit = 0;
int currphytablesize = 0;
int currpagetablesize = 0;
int currtlbtablesize = 0;
signed char signvalue;
int totalrequests;
float tolpagefaults;
float toltlbhits;
int done =0;

struct node {
    int llpagenumber;
    struct node *next;
};

// add a new task to the list of tasks
void insertmy(struct node **head, int llpagenumber2){     // insert at last of linked list code taken from EECS 2011 winter 2019
    struct node *newNode = (struct node*)malloc(sizeof(struct node));  // adding at the end of the linked list
    newNode->llpagenumber = llpagenumber2;
    struct node *end;
    end = *head;
    if(*head == NULL){
       *head = newNode;
    }else{
        while(end->next != NULL){
           end = end->next;
        }
        end->next = newNode;
        newNode->next = NULL;
    }
}
void ll_helper(struct node *temp,struct node *prev,int *track,int *llpagenumber2){
        *prev = *head;
        *temp = *temp->next;
        while (temp->llpagenumber != *llpagenumber2) {
            *prev = *temp;
            *temp = *temp->next;
            //if(temp->next == NULL){
                //break;
            //}
        }
        *track = temp->llpagenumber;
        *prev->next = *temp->next;
}
// delete the selected task from the list
int delete(struct node **head, int llpagenumber2) {    // delete function taken from project 2
    struct node *temp;
    struct node *prev;
    int track = 0;
    temp = *head;
    temper();
    if(llpagenumber2 == -1){
         //printf("last elmetn \n");
         track = (*head)->llpagenumber;
        *head = (*head)->next;
    }else if (temp->llpagenumber == llpagenumber2) {
        //printf("first elmetn \n");
        track = (*head)->llpagenumber;
        *head = (*head)->next;
    }else if((llpagenumber2 != -1) && (temp->llpagenumber != llpagenumber2)) {
        // interior or last element in the list
        //printf("interior elmetn \n");
        //ll_helper(&temp,&prev,&track,&llpagenumber2);
        prev = *head;
        temp = temp->next;
        while (temp->llpagenumber != llpagenumber2) {
            prev = temp;
            temp = temp->next;
            if(temp->next == NULL){
                break;
            }
        }
        track = temp->llpagenumber;
        prev->next = temp->next;
    }
    //printf("deleted pg %d\n",track);
    return track;
}

void temper(){
   struct node *temp;
   temp = head;
   int counn = 0;
   while(temp != NULL){
       //printf("%d, ",temp->llpagenumber);
       temp = temp->next;
       counn++;
   }
   //printf("\n %d \n",counn);
}

struct pagetable {
    int pagetnumber;
    int pagetfnumber;
};

struct tlbtable {
    int tlbtnumber;
    int tlbtfnumber;
};

struct tlbtable tlbtaccess[16];
struct pagetable pagetaccess[256];

void update_tlbtable(int i,int framenumber,int pagenum,int *currtlbtablesize){
    tlbtaccess[i].tlbtfnumber = framenumber;
    tlbtaccess[i].tlbtnumber = pagenum;
    if((*currtlbtablesize) < 15){
        (*currtlbtablesize) = (*currtlbtablesize) + 1;
    }
}

void update_pagetable(int pagenum,int currphytablesize){
   pagetaccess[currpagetablesize].pagetnumber = pagenum;
   pagetaccess[currpagetablesize].pagetfnumber = currphytablesize;
}

void check_tlbtable(int pagenum){
   int i;
   for(i=0;i<16;i++){  // move it in function
        if(tlbtaccess[i].tlbtnumber == pagenum){
            thit = 1;  // found in tlb table
            if(physicalmemsize == 128){
                //printf("check tlb pg %d\n",pagenum);
                delete(&head,pagenum);
                insertmy(&head,pagenum);
            }
            framenumber = tlbtaccess[i].tlbtfnumber;
            toltlbhits = toltlbhits + 1;
            //printf("*************%f",toltlbhits);
        }
    }
}

void check_pagetable(int currpagetablesize,int pagenum){
    int i = 0;
    for(i=0;i<currpagetablesize;i++){
        if(pagetaccess[i].pagetnumber == pagenum){
            pthit = 1;
            if(physicalmemsize == 128){
                //printf("check pagetable pg %d\n",pagenum);
                delete(&head,pagenum);
                insertmy(&head,pagenum);
            }
            framenumber = pagetaccess[i].pagetfnumber;
        }
    }
}

void delete_shift(int del){
    int i = 0;
    //int u = 0;
    //printf("befor removing page%d\n",del);
    /*for(i = 0;i<currpagetablesize;i++){
        if((pagetaccess[i].pagetfnumber != -1) && (pagetaccess[i].pagetnumber != -1)){
             printf("%d, ",pagetaccess[i].pagetnumber);
        }
    }*/
    //printf("\n \n");
    for(i=0;i<currpagetablesize;i++){
       if(pagetaccess[i].pagetnumber == del){
           int c;
           //u = 1;
           for(c = i;c<(currpagetablesize-1);c++){
               int framnn , pagenn;
               framnn = pagetaccess[c+1].pagetfnumber;
               pagenn = pagetaccess[c+1].pagetnumber;
               pagetaccess[c].pagetfnumber = framnn ;
               pagetaccess[c].pagetnumber = pagenn;
           }
           pagetaccess[c].pagetfnumber = -1;
           pagetaccess[c].pagetnumber = -1;
       }
    }
     //printf("after removing page %d\n",del);
    /*for(i = 0;i<currpagetablesize;i++){
        if((pagetaccess[i].pagetfnumber != -1) && (pagetaccess[i].pagetnumber != -1)){
             printf("%d, ",pagetaccess[i].pagetnumber);
        }
    }*/
    //printf("\n \n");
}

int main(int numin, char *inp[])
{
    physicalmemsize = atoi(inp[1]);
    int pmemory[physicalmemsize][256];  // this physical mem size vary
    //printf("%d",physicalmemsize);
    FILE *addressfile = fopen(inp[3],"r");
    FILE *store = fopen("BACKING_STORE.bin","rb");
    FILE *p;
    if(physicalmemsize == 256){
       p = fopen("output256.csv","w+");
    }else{
       p = fopen("output128.csv","w+");
    }

    while(fgets(tempaddress,sizeof(tempaddress),addressfile) != NULL){
          totalrequests = totalrequests + 1;
          logicaladdsave = atoi(tempaddress);
          logicaladdress = atoi(tempaddress);
          pagenum = ((logicaladdress >> 8) & 0XFFFF);  // here I updated logical address so I save it value before
          offset = (logicaladdress & 0XFF);
          //printf("%d \n",offset);
          // implement store read in diff funct else here
          int i = 0;
          thit = 0;
          pthit = 0;
          done =0;
          check_tlbtable(pagenum);
          if(thit == 0){
             // consult page table now
             check_pagetable(currpagetablesize,pagenum);
             if(pthit == 0){ // page fault
                //FILE *store = fopen("BACKING_STORE.bin","rb");
                if(currphytablesize >127 && physicalmemsize == 128){
                    tolpagefaults = tolpagefaults + 1;
                    int seek = fseek(store,pagenum*256,SEEK_SET);
                    if(seek != 0){
                       return 0;
                    }
                    char tt;
                    char storeout[256];
                    fread(storeout,sizeof(tt),256,store);
                    int cometrack = delete(&head,-1);  // delete comeback later
                    insertmy(&head,pagenum);
                    int fn = 0;
                    for(i=0;i<currpagetablesize;i++){
                       if(pagetaccess[i].pagetnumber == cometrack){
                           fn = pagetaccess[i].pagetfnumber;
                       }
                    }
                    framenumber = fn;
                    //framenumber = 0;
                    update_pagetable(pagenum,framenumber);
                    currpagetablesize = currpagetablesize + 1;
                    i = 0;
                    while(i != 256){
                          pmemory[framenumber][i] = storeout[i];  // store in physical memory
                          i++;
                    }
                     delete_shift(cometrack);
                     currpagetablesize = currpagetablesize - 1;
                }else{
                    insertmy(&head,pagenum);
                    tolpagefaults = tolpagefaults + 1;
                    int seek = fseek(store,pagenum*256,SEEK_SET);
                    if(seek != 0){
                       return 0;
                    }
                    char tt;
                    char storeout[256];
                    fread(storeout,sizeof(tt),256,store);
                    i = 0;
                    while(i != 256){
                      pmemory[currphytablesize][i] = storeout[i];  // store in physical memory
                      i++;
                    }
                    update_pagetable(pagenum,currphytablesize);
                    framenumber = currphytablesize;
                    i = 0;
                      thit = 0; // reseting for tlb inserting
                      int t = 0;
                      for(i=0;i<currtlbtablesize;i++){
                         if(tlbtaccess[i].tlbtnumber == pagenum){  // break if already in tlb
                            thit = 1;  // found in tlb table
                            break;
                         }
                         t = t + 1;
                      }
                      if(t < currtlbtablesize){ // if found then simply add to same point also we can remove it
                            update_tlbtable(i,currphytablesize,pagenum,&currtlbtablesize);
                      }
                      done = 1;
                      if(t == currtlbtablesize){  // not in tlb so add it after making making space
                            int p = 0;
                            for(p=0;p<i;p++){
                               tlbtaccess[p] = tlbtaccess[p+1]; // move it to last
                            }
                            update_tlbtable(i,currphytablesize,pagenum,&currtlbtablesize);  // this is to add if either in tlb or not
                      }
                      currpagetablesize = currpagetablesize + 1;
                      currphytablesize = currphytablesize + 1;
                 }
             }

          }
          if(done == 0){
                  i = 0;
                  thit = 0; // reseting for tlb inserting
                  int t = 0;
                  for(i=0;i<currtlbtablesize;i++){
                     if(tlbtaccess[i].tlbtnumber == pagenum){  // break if already in tlb
                        thit = 1;  // found in tlb table
                        break;
                     }
                     t = t + 1;
                  }
                  if(t < currtlbtablesize){ // if found then simply add to same point also we can remove it
                        update_tlbtable(i,framenumber,pagenum,&currtlbtablesize);
                  }
                  done = 1;
                  if(t == currtlbtablesize){  // not in tlb so add it after making making space
                        int p = 0;
                        for(p=0;p<i;p++){
                           tlbtaccess[p] = tlbtaccess[p+1]; // move it to last
                        }
                        update_tlbtable(i,framenumber,pagenum,&currtlbtablesize);  // this is to add if either in tlb or not
                  }
          }

          //printf("****%d \n",currtlbtablesize);
          //tlbtaccess[i].tlbtfnumber = framenumber;
          //tlbtaccess[i].tlbtnumber = pagenum;
          //if(currtlbtablesize < 15){
              //currtlbtablesize = currtlbtablesize + 1;
          //}

          signvalue = pmemory[framenumber][offset];
          //temper();
          //printf("logical add is --%d page num is %d framenum is -- %d offset is --%d \n",logicaladdsave,pagenum,framenumber,offset);
          //int physicaladdprint = (framenumber*physicalmemsize)+offset;
          int physicaladdprint = (framenumber*256)+offset;
          //FILE *p;
          //p = fopen("output256.csv","w+");
          fprintf(p,"%d,%d,%d\n",logicaladdsave,physicaladdprint,signvalue);
          //fclose(p);
    }
    fprintf(p,"Page Faults Rate, %0.2f%,\n",(tolpagefaults/totalrequests)*100);
    fprintf(p,"TLB Hits Rate, %0.2f%,",(toltlbhits/totalrequests)*100);
    return 0;
}
