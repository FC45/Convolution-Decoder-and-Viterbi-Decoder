#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

#define PI 3.1415926536


int g1[5] = {1,0,0,1,1};
int g2[5] = {1,1,1,0,1};
int state[16][4] = {{0,0,0,0},
                    {1,0,0,0},
                    {0,1,0,0},
                    {1,1,0,0},
                    {0,0,1,0},
                    {1,0,1,0},
                    {0,1,1,0},
                    {1,1,1,0},
                    {0,0,0,1},
                    {1,0,0,1},
                    {0,1,0,1},
                    {1,1,0,1},
                    {0,0,1,1},
                    {1,0,1,1},
                    {0,1,1,1},
                    {1,1,1,1}};



encoder(int a,int *b);//a=length b=input
double AWGN_generator();
int state_change(int c,int d);//c = current state, d = current input
int state_change_bit(int c_1,int c_2);//c_1=first state  c_2=next state
int codeword(int c_3,int c_4,int c_5);//c_3 = previous state c_4 = previous input c_5: 1 or 2
int hard_decision(float e);//Q=2
float soft_decision_4(float f);//Q=4
float soft_decision_8(float h);//Q=8
int viterbi_decoder(float *i,int j,int k);//i=decoder's input j=L     k=decision mode
bit_error_rate(int *l_0,int *l_1,int m);//l_0 = input l_1 = message m=output length=input length

int main()
{
    float *ptrout = NULL;
    int *ptrin = NULL;
    int *ptr_message = NULL;
    int length;//input length
    int outputlength;
    int c;
    int i;
    int j;
    int Q;
    float counter=0;
    float rate;
    float s;
    float noise;
    float SNR_dB;


    srand( (unsigned)time(NULL) );

    printf("input length(>32)=\n");
    scanf("%d",&length);
    if(length<=32){
        printf("length is to short\n");
        system("pause");
        return 0;
    }
    printf("SNR_dB=\n");
    scanf("%f",&SNR_dB);
    printf("Q(0:Q=2 1:Q=4 2:Q=8)=\n");
    scanf("%d",&Q);
    if(Q!=0 && Q!=1 && Q!=2){
        printf("error quantization!!!\n");
        system("pause");
        return 0;
    }


    s = pow(10,SNR_dB/10);
    noise = pow(1/s,0.5);
    outputlength = 2*(4+length); //output length = 2*(m+L)
    c = length + 4;//m+L

    ptrin = (int *)malloc(sizeof(int)*length);
    ptrout = (int *)malloc(sizeof(int)*outputlength);
    ptr_message = (int *)malloc(sizeof(int)*length);


    for(i=0;i<length;i++){
        if(rand()%2==1){
            *(ptrin+i)=1;
        }
        else {
            *(ptrin+i)=0;
        }
    }
    ptrout = encoder(length,ptrin);
    /*for(i=0;i<c;i++){
        printf("en=%f\n",*(ptrout+i));
    }*/

    for(i=0;i<outputlength;i++){
        if(*(ptrout+i)==0){
            *(ptrout+i) = 1 + noise*AWGN_generator();
        }
        else if(*(ptrout+i)==1){
            *(ptrout+i) = -1 + noise*AWGN_generator();
        }
        //printf("%f\n",*(ptrout+i));
        if(Q==0){
            *(ptrout+i) = hard_decision(*(ptrout+i));
        }
        else if(Q==1){
            *(ptrout+i) = soft_decision_4(*(ptrout+i));
        }
        else if(Q==2){
            *(ptrout+i) = soft_decision_8(*(ptrout+i));
        }
    }

    ptr_message = viterbi_decoder(ptrout,c,Q);
    /*for(i=0;i<length-32;i++){
        printf("(%d):%d %d\n",i,*(ptrin+i),*(ptr_message+i));
    }*/

    bit_error_rate(ptrin,ptr_message,length);

    system("pause");
    return 0;
}

encoder(int a,int *b)
{
    float *ptren = NULL;
    int *ptren1 = NULL;
    int *ptren2 = NULL;
    int q = 2*(4+a);
    int p = 4+a;
    int w;
    int s;
    int f;
    int r;
    ptren = (int *)malloc(sizeof(int)*q);
    ptren1 = (int *)malloc(sizeof(int)*p);
    ptren2 = (int *)malloc(sizeof(int)*p);
    for(w=0;w<p;w++){
        *(ptren1+w)=0;
        *(ptren2+w)=0;
        for(s=0;s<a;s++){
            for(f=0;f<5;f++){
                r = f + s;
                if(r==w){
                    *(ptren1+w) = (*(ptren1+w) + *(b+s)*g1[f])%2;
                    *(ptren2+w) = (*(ptren2+w) + *(b+s)*g2[f])%2;
                }
            }
        }
    }
    for(w=0;w<q;w++){
        if(w%2==0){
            *(ptren+w) = *(ptren1+(w/2));
        }
        else{
            *(ptren+w) = *(ptren2+((w-1)/2));
        }
    }
    return ptren;
}

double AWGN_generator()
{

  double temp1;
  double temp2;
  double result;
  int p;

  p = 1;

  while( p > 0 )
  {
	temp2 = ( rand() / ( (double)RAND_MAX ) );

    if ( temp2 == 0 )
    {
      p = 1;
    }
    else
    {
       p = -1;
    }

  }

  temp1 = cos( ( 2.0 * (double)PI ) * rand() / ( (double)RAND_MAX ) );
  result = sqrt( -2.0 * log( temp2 ) ) * temp1;

  return result;

}

int state_change(int c,int d)//c = current state, d = current input
{
    int a;
    a = (c%8)*2+d;
    return a;
}

int state_change_bit(int c_1,int c_2)  //state c_1 change to state c_2
{
    int a=0;/*
    if(state[c_1][0]==state[c_2][1] && state[c_1][1]==state[c_2][2] && state[c_1][2]==state[c_2][3]){
        a = state[c_2][0];
    }
    else{
        printf("errorbit %d to %d\n",c_1,c_2);
        system("pause");
        return 0;
    }*/
    a = state[c_2][0];


    return a;
}

int codeword(int c_3,int c_4,int c_5)//c_3 = previous state c_4 = previous input c_5 = 1 or 2
{
    int a;
    if(c_5==1){
        a = (c_4*g1[0] + state[c_3][0]*g1[1] + state[c_3][1]*g1[2] + state[c_3][2]*g1[3] + state[c_3][3]*g1[4])%2;
    }
    else if(c_5==2){
        a = (c_4*g2[0] + state[c_3][0]*g2[1] + state[c_3][1]*g2[2] + state[c_3][2]*g2[3] + state[c_3][3]*g2[4])%2;
    }
    else{
        printf("errorcodeword\n");
        system("pause");
        return 0;
    }
    return a;
}

int hard_decision(float e)
{
    int a;
    if(e>=0){
        a = 0;
    }
    else{
        a = 1;
    }
    return a;
}

float soft_decision_4(float f)
{
    float a;
    if(f>=1){
        a = 0;
    }
    else if(f>=0 && f<1){
        a = 0.25;
    }
    else if(f>=-1 && f<0){
        a = 0.75;
    }
    else{
        a = 1;
    }
    return a;
}

float soft_decision_8(float h)
{
    float a;
    if(h>=1.5){
        a = 0;
    }
    else if(h>=1 && h<1.5){
        a = 0.125;
    }
    else if(h>=0.5 && h<1){
        a = 0.25;
    }
    else if(h>=0 && h<0.5){
        a = 0.375;
    }
    else if(h>=-0.5 && h<0){
        a = 0.625;
    }
    else if(h>=-1 && h<-0.5){
        a = 0.75;
    }
    else if(h>=-1.5 && h<-1){
        a = 0.875;
    }
    else{
        a = 1;
    }
    return a;
}

int viterbi_decoder(float *i,int j,int k)//i=decoder's input j=L     k=decision mode
{
    int a_1;
    int s_0;
    int s_1;
    int bit_0;
    int bit_1;
    int weight_0 = 0;
    int weight_1 = 0;
    float weightx_0 = 0;
    float weightx_1 = 0;
    int m;
    int n;
    int o;
    int x;
    int y;
    int first_state=0;
    int next_state=0;
    int *b = NULL;
    int c[3][16] = {{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
                    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
                    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}};
    int c_temp[3][16] = {{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
                         {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
                         {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}};
    float cx[3][16] = {{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
                    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
                    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}};
    float cx_temp[3][16] = {{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
                         {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
                         {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}};

    a_1 = j+4; // a_1= L+m

    b = (int *)malloc(sizeof(int)*j);

    if(k==0){//hard decision
        for(m=0;m<j;m++){
            bit_0=state_change(first_state,0);
            bit_1=state_change(first_state,1);
            c[0][bit_0] = bit_0;
            c[2][bit_0] = 1;
            c[0][bit_1] = bit_1;
            c[2][bit_1] = 1;
            c[1][bit_0] = ((int)*(i+m*2) + codeword(first_state,0,1))%2 + ((int)*(i+m*2+1) + codeword(first_state,0,2))%2;
            c[1][bit_1] = ((int)*(i+m*2) + codeword(first_state,1,1))%2 + ((int)*(i+m*2+1) + codeword(first_state,1,2))%2;
            //printf("0=%d 1=%d\n",c[2][bit_0],c[2][bit_1]);
            if(m < j-32){ // truncation = 32
                for(n=1;n<32;n++){
                    for(o=0;o<16;o++){
                        if(c[2][o] != 0){
                            s_0 = state_change(o,0);
                            s_1 = state_change(o,1);
                            //printf("state=%d\n",c[0][o]);
                            weight_0 = c[1][o] + ((int)*(i+m*2+n*2) + codeword(o,0,1))%2 + ((int)*(i+m*2+1+n*2) + codeword(o,0,2))%2;
                            weight_1 = c[1][o] + ((int)*(i+m*2+n*2) + codeword(o,1,1))%2 + ((int)*(i+m*2+1+n*2) + codeword(o,1,2))%2;
                            if(c_temp[2][s_0] == 0){
                                c_temp[0][s_0] = c[0][o];
                                c_temp[1][s_0] = weight_0;
                                c_temp[2][s_0] = 1;
                            }
                            else{
                                if(c_temp[1][s_0]>weight_0){
                                    c_temp[0][s_0] = c[0][o];
                                    c_temp[1][s_0] = weight_0;

                                }
                            }

                            if(c_temp[2][s_1] == 0){
                                c_temp[0][s_1] = c[0][o];
                                c_temp[1][s_1] = weight_1;
                                c_temp[2][s_1] = 1;
                            }
                            else{
                                if(c_temp[1][s_1]>weight_1){
                                    c_temp[0][s_1] = c[0][o];
                                    c_temp[1][s_1] = weight_1;
                                }
                            }





                        }
                    }
                    for(x=0;x<3;x++){
                                for(y=0;y<16;y++){
                                    c[x][y]=c_temp[x][y];
                                    c_temp[x][y]=0;
                                }
                            }

                }




            }
            else{
                for(n=m;n<j;n++){
                    for(o=0;o<16;o++){
                        if(c[2][o] != 0){
                            s_0 = state_change(o,0);
                            s_1 = state_change(o,1);
                            weight_0 = c[1][o] + ((int)*(i+m*2+n*2) + codeword(o,0,1))%2 + ((int)*(i+m*2+1+n*2) + codeword(o,0,2))%2;
                            weight_1 = c[1][o] + ((int)*(i+m*2+n*2) + codeword(o,1,1))%2 + ((int)*(i+m*2+1+n*2) + codeword(o,1,2))%2;
                            if(c_temp[2][s_0] == 0){
                                c_temp[0][s_0] = c[0][o];
                                c_temp[1][s_0] = weight_0;
                                c_temp[2][s_0] = 1;
                            }
                            else{
                                if(c_temp[1][s_0]>weight_0){
                                    c_temp[0][s_0] = c[0][o];
                                    c_temp[1][s_0] = weight_0;
                                }
                            }

                            if(c_temp[2][s_1] == 0){
                                c_temp[0][s_1] = c[0][o];
                                c_temp[1][s_1] = weight_1;
                                c_temp[2][s_1] = 1;
                            }
                            else{
                                if(c_temp[1][s_1]>weight_1){
                                    c_temp[0][s_1] = c[0][o];
                                    c_temp[1][s_1] = weight_1;
                                }
                            }



                                }
                            }

                            for(x=0;x<3;x++){
                                for(y=0;y<16;y++){
                                    c[x][y]=c_temp[x][y];
                                    c_temp[x][y]=0;
                                }
                            }
                }
            }

            for(x=0;x<16;x++){
                if(x==0){
                    y=0;
                }
                else{
                    if(c[1][x]<c[1][y]){
                        y=x;
                    }
                }

            }
            next_state = c[0][y];
            printf("%d\n",next_state);
            *(b+m) = state_change_bit(first_state,next_state);
            first_state = next_state;
            for(x=0;x<3;x++){
                for(y=0;y<16;y++){
                    c[x][y]=0;
                }
            }
        }
    }
    else{//soft decision
        for(m=0;m<j;m++){
            bit_0=state_change(first_state,0);
            bit_1=state_change(first_state,1);
            cx[0][bit_0] = bit_0;
            cx[2][bit_0] = 1;
            cx[0][bit_1] = bit_1;
            cx[2][bit_1] = 1;
            cx[1][bit_0] = (pow(*(i+m*2)-codeword(first_state,0,1),2)  + pow(*(i+m*2+1)-codeword(first_state,0,2),2));
            cx[1][bit_1] = (pow(*(i+m*2)-codeword(first_state,1,1),2)  + pow(*(i+m*2+1)-codeword(first_state,1,2),2));
            //printf("0=%f 1=%f\n",cx[1][bit_0],cx[1][bit_1]);
            if(m < j-32){ // truncation = 32
                for(n=1;n<32;n++){
                    for(o=0;o<16;o++){
                        if(cx[2][o] != 0){
                            s_0 = state_change(o,0);
                            s_1 = state_change(o,1);
                            //printf("state=%f\n",cx[0][o]);
                            weightx_0 = cx[1][o] + (pow(*(i+m*2+n*2)-codeword(o,0,1),2) + pow(*(i+m*2+1+n*2)-codeword(o,0,2),2));
                            weightx_1 = cx[1][o] + (pow(*(i+m*2+n*2)-codeword(o,1,1),2) + pow(*(i+m*2+1+n*2)-codeword(o,1,2),2));
                            //printf("0=%d 1=%d\n",s_0,s_1);
                            if(cx_temp[2][s_0] == 0){
                                cx_temp[0][s_0] = cx[0][o];
                                cx_temp[1][s_0] = weightx_0;
                                cx_temp[2][s_0] = 1;
                            }
                            else{
                                if(cx_temp[1][s_0]>weightx_0){
                                    cx_temp[0][s_0] = cx[0][o];
                                    cx_temp[1][s_0] = weightx_0;

                                }
                            }

                            if(cx_temp[2][s_1] == 0){
                                cx_temp[0][s_1] = cx[0][o];
                                cx_temp[1][s_1] = weightx_1;
                                cx_temp[2][s_1] = 1;
                            }
                            else{
                                if(cx_temp[1][s_1]>weightx_1){
                                    cx_temp[0][s_1] = cx[0][o];
                                    cx_temp[1][s_1] = weightx_1;
                                }
                            }





                        }
                    }
                    for(x=0;x<3;x++){
                                for(y=0;y<16;y++){
                                    cx[x][y]=cx_temp[x][y];
                                    cx_temp[x][y]=0;
                                }
                            }

                }




            }
            else{
                for(n=m;n<j;n++){
                    for(o=0;o<16;o++){
                        if(cx[2][o] != 0){
                            s_0 = state_change(o,0);
                            s_1 = state_change(o,1);
                            weightx_0 = c[1][o] + (pow(*(i+m*2+n*2)-codeword(o,0,1),2) + pow(*(i+m*2+1+n*2)-codeword(o,0,2),2));
                            weightx_1 = c[1][o] + (pow(*(i+m*2+n*2)-codeword(o,1,1),2) + pow(*(i+m*2+1+n*2)-codeword(o,1,2),2));
                            if(cx_temp[2][s_0] == 0){
                                cx_temp[0][s_0] = cx[0][o];
                                cx_temp[1][s_0] = weightx_0;
                                cx_temp[2][s_0] = 1;
                            }
                            else{
                                if(cx_temp[1][s_0]>weightx_0){
                                    cx_temp[0][s_0] = cx[0][o];
                                    cx_temp[1][s_0] = weightx_0;
                                }
                            }

                            if(cx_temp[2][s_1] == 0){
                                cx_temp[0][s_1] = cx[0][o];
                                cx_temp[1][s_1] = weightx_1;
                                cx_temp[2][s_1] = 1;
                            }
                            else{
                                if(cx_temp[1][s_1]>weightx_1){
                                    cx_temp[0][s_1] = cx[0][o];
                                    cx_temp[1][s_1] = weightx_1;
                                }
                            }



                                }
                            }

                            for(x=0;x<3;x++){
                                for(y=0;y<16;y++){
                                    cx[x][y]=cx_temp[x][y];
                                    cx_temp[x][y]=0;
                        }
                    }
                }
            }

            for(x=0;x<16;x++){
                if(x==0){
                    y=0;
                }
                else{
                    if(cx[1][x]<cx[1][y]){
                        y=x;
                    }
                }

            }

            next_state = (int)cx[0][y];
            //printf("state=%f  %d\n",cx[0][y],next_state);
            for(x=0;x<3;x++){
                for(y=0;y<16;y++){
                    cx[x][y]=0;
                }
            }
            //printf("%d %d\n",first_state,next_state);
            *(b+m) = state_change_bit(first_state,next_state);
            first_state = next_state;
        }
    }


    return b;
}

bit_error_rate(int *l_0,int *l_1,int m){
    float a;
    int b;
    float c=0;

    for(b=0;b<m-32;b++){
        if( *(l_0+b) != *(l_1+b) ){
          c=c+1;
        }
    }
    a = c/(m-32);
    printf("BER=%f\n",a);

}


