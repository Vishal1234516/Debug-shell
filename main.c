#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include<stddef.h>
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/gpio.h"
#include "driverlib/pin_map.h"
#include "driverlib/sysctl.h"
#include "driverlib/uart.h"
#include "inc/hw_ints.h"
#include "driverlib/interrupt.h"
#include"inc/tm4c123gh6pm.h"
int32_t test;
char command[20],k=0,check,reg_no=96,mem_address[10],no_byte,mem_flag=0;
unsigned char data,read_flag=0,write_flag=0;
volatile uint8_t conv_ascii;
long mem_address_store;
int memory;
char instruction[]={"*******************************************************UART BASED DEBBUGER********************************************************************* \n\r*******************************************************LIST OF COMMANDS************************************************************************\n\r\n\n suspend:         To suspend the main application and enter into debug mode \n\r resume:          To resume the main application  \n\r readxx:          To read any register value where xx can be R0-R14,lr,sp \n\r writexx:         To write into any register value xx can be R0-R14 \n\r readmem0x________To read any memory location \n\r Note:            All command related to read write work only in debug mode\n\r"};
char *words[] = {"readR0","readR1","readR2","readR3","readR4","readR5","readR6","readR7","readR8","readR9","readR10","readR11","readR12","readR13","readR14","readsp","readlr","writeR0","writeR1","writeR2","writeR3","writeR4","writeR5","writeR6","writeR7","writeR8","writeR9","writeR10","writeR11","writeR12","writeR13","writeR14"};
char  resume[]={"resume"};
char suspend[]={"suspend"};
char  readmem[]={"readmem0x"};
char entermem[]={"Enter The Memory Address : "};
unsigned char flag=1,task_status;
unsigned char reg;
volatile uint8_t *ptr1;
unsigned char txflag,flag1,i=4,print_reg,mem_read=0,l=0;
unsigned char rxflag,n=0;
#define NUMTHREADS 3
#define STACKSIZE 100
struct tcb
{
    long *sp;
    struct tcb *next;
};
//long count1,count2,count3;
typedef struct tcb tcbType;
tcbType tcbs[NUMTHREADS];
tcbType *RunPt;
void StartOS();
long Stacks[NUMTHREADS][STACKSIZE];
void enable_irq(void);
 int OS_AddThreads(void(*task0)(void),void(*task1)(void),void(*task2)(void))
{
  //  long status;
    __asm  ("CPSID I");
    tcbs[0].next=&tcbs[1];
    tcbs[1].next=&tcbs[0];
    //tcbs[2].next=&tcbs[0];
    SetInitialStack (0);
    SetInitialStack (1);
    SetInitialStack (2);
    Stacks[0][STACKSIZE-2]=(long)(task0);
    Stacks[1][STACKSIZE-2]=(long)(task1);
    Stacks[2][STACKSIZE-2]=(long)(task2);
    RunPt=&tcbs[0];
    __asm  ("CPSIE I");
    return 1;
}

__attribute__((naked)) void app(void)
{

    for(;;)
    {
  //     flag=1;
        GPIO_PORTF_DATA_R^=0x02;

    }
}
__attribute__((naked)) void debug(void)
{
    volatile uint8_t *ptr1;



    for(;;)
    {
        ptr1=(uint8_t *)(tcbs[0].sp);
    //    flag=1;
        GPIO_PORTF_DATA_R^=0x08;
        if(check==1)
        {
        for (i = 0; i <32; i++)
        {
               if (!strcmp(command, words[i]))
               {
                  reg_no=i;
                  mem_read=0;
               }
        }
        check=0;
        i=0;
        if(reg_no<=16)
       {
           read_flag=1;
           write_flag=0;
       }
        if(reg_no>16)
        {
            write_flag=1;
            read_flag=0;
            reg_no=reg_no-17;
        }

        }
        if(reg_no==15 || reg_no==16)
        {
            reg_no=reg_no-2;

        }
        if(reg_no>=0 && reg_no<=3)
        {
            ptr1=ptr1+32;
            ptr1=ptr1+(4*reg_no)+3;
            reg=1;
            reg_no=96;
        }
         if(reg_no>=4 && reg_no<=11)
        {
            ptr1=ptr1+(4*(reg_no-4))+3;
            reg=1;
            reg_no=96;
        }
         if(reg_no==12)
         {
             ptr1=ptr1+(12*4)+3;
             reg=1;
             reg_no=96;
         }
         if(reg_no==13)
          {
              ptr1=ptr1+(14*4)+3;
              reg=1;
              reg_no=96;
          }
         if(reg_no==14)
          {
              ptr1=ptr1+(13*4)+3;
              reg=1;
              reg_no=96;
          }
         if(read_flag==1)
         {
        while(i<4 && reg==1 )
      {
        flag1=UART0_FR_R;
        txflag=(flag1) & (0x20);
        while (txflag == 1);
        if(txflag==0)
         {
            if((*ptr1)<=15)
            {
            UART0_DR_R=*ptr1+48;
            ptr1=ptr1-1;
            i++;
            }
            else
            {
                UART0_DR_R=((*ptr1)&0x0F)+48;
                flag1=UART0_FR_R;
                txflag=(flag1) & (0x20);
                while (txflag == 1);
                UART0_DR_R=((*ptr1)&0xF0)+48;
                i++;
                ptr1=ptr1-1;
            }

         }
     }
        read_flag=0;
 }


        if(write_flag==1)
    {
            __asm  ("CPSID I");

        while(i<4 && reg==1)
      {
        flag1=UART0_FR_R;
        txflag=(flag1) & (0x20);
        flag1=UART0_FR_R;
        rxflag=(flag1) & (0x10);
        if(rxflag==0)
        {
            *ptr1=UART0_DR_R-48;
            ptr1=ptr1-1;
            i++;
        }

       }
        __asm  ("CPSIE I");
        write_flag=0;
    }

        reg=0;
        if(mem_read==1 && check==0)
        {
            for(l=0;l<9;l++)
            {
                if(command[l]==readmem[l])
                {
                    mem_flag++;
                }
            }
            mem_read=0;

         }

       if(mem_flag==9)
        {
            for(l=7;l<=16;l++)
            {
                mem_address[l-7]=command[l];
            }
            no_byte=10*(command[17]-48)+(command[18]-48);
            sscanf(mem_address,"0x%x",&memory);
            ptr1=(volatile uint8_t *)memory;
            for(l=0;l<no_byte;l++)
            {
                flag1=UART0_FR_R;
                txflag=(flag1) & (0x20);
                while (txflag == 1);
                if(txflag==0)
                 {
                   UART0_DR_R=*ptr1;
                   ptr1=ptr1+1;
                   l++;
                 }
            }
        }

        mem_flag=0;
    }
}
__attribute__((naked)) void Task3(void)
{

    for(;;)
    {
        GPIO_PORTF_DATA_R^=0x08;
    }
}

 void SetInitialStack(int i)
{
tcbs[i].sp=&Stacks[i][STACKSIZE-16];
Stacks[i][STACKSIZE-1]=0x01000000;
Stacks[i][STACKSIZE-3]=0x14141414;
Stacks[i][STACKSIZE-4]=0x12121212;
Stacks[i][STACKSIZE-5]=0x03030303;
Stacks[i][STACKSIZE-6]=0x02020202;
Stacks[i][STACKSIZE-7]=0x01010101;
Stacks[i][STACKSIZE-8]=0x00000000;
Stacks[i][STACKSIZE-9]=0x11111111;
Stacks[i][STACKSIZE-10]=0x10101010;
Stacks[i][STACKSIZE-11]=0x09090909;
Stacks[i][STACKSIZE-12]=0x08080808;
Stacks[i][STACKSIZE-13]=0x07070707;
Stacks[i][STACKSIZE-14]=0x06060606;
Stacks[i][STACKSIZE-15]=0x05050505;
Stacks[i][STACKSIZE-16]=0x04040404;
}
 void StartOS()
{

__asm  (" LDR R0,=RunPt\n");
__asm  (" LDR R2,[R0]\n");
__asm  ("LDR SP,[R2]\n");
__asm  ("POP {R4-R11}\n");
__asm  ("POP {R0-R3}\n");
__asm  ("POP {R12}\n");
__asm  ("POP {LR}\n");
__asm  ("POP {LR}\n");
__asm  ("POP {R1} \n");
__asm  ("CPSIE I \n");
__asm  ("BX LR \n");
}
__attribute__((naked)) void UARTIntHandler(void)
{

    unsigned char rxflag,flag1,m,n=0,l=0,s=0,txflag;
    char d;
    __asm  ("CPSID I");
    __asm  ("PUSH {R4-R11}");
     flag1=UART0_FR_R;
     rxflag=(flag1) & (0x10);
     if(rxflag==0)
     {
         test=UART0_DR_R;
         if(test!=13)
         {
             command[k]=test;
              k++;
         }
         else
         {
             command[k]='\0';
             k=0;
             mem_read=1;
             check=1;
         }

     }
     flag1=UART0_FR_R;
     txflag=(flag1) & (0x20);
     while (txflag == 1);
     if(txflag==0)
     {
       UART0_DR_R=test;
     }

     if(test==48)
     {
        task_status=1;
     }
     if(check==1)
     {
         for(m=0;m<=5;m++)
         {
             if(resume[m]==command[m])
             {
                 n++;
             }
         }

     }

     if(check==1)
          {
              for(m=0;m<=6;m++)
              {
                  if(suspend[m]==command[m])
                  {
                      s++;
                  }
              }

          }
     if(n==6 || s==7)
         {
             flag=0;
           check=0;
           n=0;
         }
     if(test>=53 && flag==1)
          {
              i=0;
              print_reg=test;
          }

       __asm  ("LDR R0,=RunPt");
       __asm  ("LDR R1,[R0]");
       __asm  ("STR SP,[R1]");
       if(flag==1 )
       {
       __asm  ("LDR R1,[R1,#4]");
       __asm  ("LDR R1,[R1,#4]");
       }
       if(flag==0)
       {
       __asm  ("LDR R1,[R1,#4]");
       flag=1;
       }
       __asm  ("STR R1,[R0]");
       __asm  ("LDR SP,[R1]");
       __asm  ("POP {R4-R11}");
       __asm  ("CPSIE I \n");
       __asm  ("BX LR");
}
  int main(void)
{
      int p=0;

    SysCtlClockSet(SYSCTL_SYSDIV_4 | SYSCTL_USE_PLL | SYSCTL_OSC_MAIN | SYSCTL_XTAL_16MHZ);

    SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
    GPIOPinConfigure(GPIO_PA0_U0RX);
    GPIOPinConfigure(GPIO_PA1_U0TX);
    GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);
    SysCtlPeripheralEnable( SYSCTL_PERIPH_GPIOF );
    GPIOPinTypeGPIOOutput( GPIO_PORTF_BASE, GPIO_PIN_2 );
    GPIOPinTypeGPIOOutput( GPIO_PORTF_BASE, GPIO_PIN_1 );
    GPIOPinTypeGPIOOutput( GPIO_PORTF_BASE, GPIO_PIN_3 );
    SYSCTL_RCGC2_R |= 0x00000020;     /* enable clock to GPIOF at clock gating control register */
    GPIO_PORTF_DIR_R = 0x0E;          /* enable the GPIO pins for the LED (PF3, 2, 1) as output */
    GPIO_PORTF_DEN_R = 0x0E;
    GPIO_PORTF_DATA_R=0x00;
    OS_AddThreads(&app,&debug,&Task3);


    UARTConfigSetExpClk(UART0_BASE, SysCtlClockGet(), 115200,(UART_CONFIG_WLEN_8 |
        UART_CONFIG_STOP_ONE | UART_CONFIG_PAR_NONE));
    IntMasterEnable();
    IntEnable( INT_UART0 );
    UARTIntEnable( UART0_BASE, UART_INT_RX | UART_INT_RT );

while(instruction[p]!='\0')
{
    UARTCharPut( UART0_BASE, instruction[p]);
    p++;
}
    StartOS();
}
