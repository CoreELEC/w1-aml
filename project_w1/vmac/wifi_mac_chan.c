#include "wifi_mac_com.h"
#include "rf_t9026/t9026_top.h"

#define ARR_SIZE_OF(arr) (sizeof(arr)/sizeof(arr[0]))
#define CHAN_2_FREQ_5G(chan) ((chan) * 5 + 5000)
#define CHAN_2_FREQ_2G(chan) ((chan) * 5 + 2407)

int support_idx[WIFI_country_MAX +1][20] = {
    {10,1,2,3,4,5,6,7,8,9,128,0,0,0,0,0,0,0,0,0},//WIFI_China -0
    {19,1,2,3,4,5,12,22,23,24,25,26,27,28,29,30,31,32,33,128},//WIFI_NorthAmerica
    {14,1,2,3,4,5,6,7,8,9,10,11,12,17,128,0,0,0,0,0},//WIFI_Europe
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},//WIFI_France
    {15,1,7,32,34,36,37,39,41,42,44,46,56,57,58,128,0,0,0,0},//WIFI_Japan
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},//WIFI_Israel
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},//WIFI_Mexico
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}//WIFI_country_MAX
};

struct class_chan_set global_chan_set [256] =
{
    {81, 20,13,{
                {CHAN_2_FREQ_2G(1), WIFINET_CHAN_2GHZ, 1, 4, 2, WIFINET_BWC_WIDTH20, 81},
                {CHAN_2_FREQ_2G(2), WIFINET_CHAN_2GHZ, 2, 4, 2, WIFINET_BWC_WIDTH20, 81},
                {CHAN_2_FREQ_2G(3), WIFINET_CHAN_2GHZ, 3, 4, 2, WIFINET_BWC_WIDTH20, 81},
                {CHAN_2_FREQ_2G(4), WIFINET_CHAN_2GHZ, 4, 4, 2, WIFINET_BWC_WIDTH20, 81},
                {CHAN_2_FREQ_2G(5), WIFINET_CHAN_2GHZ, 5, 4, 2, WIFINET_BWC_WIDTH20, 81},
                {CHAN_2_FREQ_2G(6), WIFINET_CHAN_2GHZ, 6, 4, 2, WIFINET_BWC_WIDTH20, 81},
                {CHAN_2_FREQ_2G(7), WIFINET_CHAN_2GHZ, 7, 4, 2, WIFINET_BWC_WIDTH20, 81},
                {CHAN_2_FREQ_2G(8), WIFINET_CHAN_2GHZ, 8, 4, 2, WIFINET_BWC_WIDTH20, 81},
                {CHAN_2_FREQ_2G(9), WIFINET_CHAN_2GHZ, 9, 4, 2, WIFINET_BWC_WIDTH20, 81},
                {CHAN_2_FREQ_2G(10), WIFINET_CHAN_2GHZ, 10, 4, 2, WIFINET_BWC_WIDTH20, 81},
                {CHAN_2_FREQ_2G(11), WIFINET_CHAN_2GHZ, 11, 4, 2, WIFINET_BWC_WIDTH20, 81},
                {CHAN_2_FREQ_2G(12), WIFINET_CHAN_2GHZ, 12, 4, 2, WIFINET_BWC_WIDTH20, 81},
                {CHAN_2_FREQ_2G(13), WIFINET_CHAN_2GHZ, 13, 4, 2, WIFINET_BWC_WIDTH20, 81}
               }
    }, //class 81
    {82,20,1,{
                 {CHAN_2_FREQ_2G(14), WIFINET_CHAN_2GHZ, 14, 4, 2, WIFINET_BWC_WIDTH20, 82}
              }
    },//82
    {83,40,9,{
                {CHAN_2_FREQ_2G(1 + 2), WIFINET_CHAN_2GHZ, 1, 4, 2, WIFINET_BWC_WIDTH40, 83},
                {CHAN_2_FREQ_2G(2 + 2), WIFINET_CHAN_2GHZ, 2, 4, 2, WIFINET_BWC_WIDTH40, 83},
                {CHAN_2_FREQ_2G(3 + 2), WIFINET_CHAN_2GHZ, 3, 4, 2, WIFINET_BWC_WIDTH40, 83},
                {CHAN_2_FREQ_2G(4 + 2), WIFINET_CHAN_2GHZ, 4, 4, 2, WIFINET_BWC_WIDTH40, 83},
                {CHAN_2_FREQ_2G(5 + 2), WIFINET_CHAN_2GHZ, 5, 4, 2, WIFINET_BWC_WIDTH40, 83},
                {CHAN_2_FREQ_2G(6 + 2), WIFINET_CHAN_2GHZ, 6, 4, 2, WIFINET_BWC_WIDTH40, 83},
                {CHAN_2_FREQ_2G(7 + 2), WIFINET_CHAN_2GHZ, 7, 4, 2, WIFINET_BWC_WIDTH40, 83},
                {CHAN_2_FREQ_2G(8 + 2), WIFINET_CHAN_2GHZ, 8, 4, 2, WIFINET_BWC_WIDTH40, 83},
                {CHAN_2_FREQ_2G(9 + 2), WIFINET_CHAN_2GHZ, 9, 4, 2, WIFINET_BWC_WIDTH40, 83}
               }
    },//83
   {84,40,9,{
                {CHAN_2_FREQ_2G(5 - 2), WIFINET_CHAN_2GHZ, 5, 4, 2, WIFINET_BWC_WIDTH40, 84},
                {CHAN_2_FREQ_2G(6 - 2), WIFINET_CHAN_2GHZ, 6, 4, 2, WIFINET_BWC_WIDTH40, 84},
                {CHAN_2_FREQ_2G(7 - 2), WIFINET_CHAN_2GHZ, 7, 4, 2, WIFINET_BWC_WIDTH40, 84},
                {CHAN_2_FREQ_2G(8 - 2), WIFINET_CHAN_2GHZ, 8, 4, 2, WIFINET_BWC_WIDTH40, 84},
                {CHAN_2_FREQ_2G(9 - 2), WIFINET_CHAN_2GHZ, 9, 4, 2, WIFINET_BWC_WIDTH40, 84},
                {CHAN_2_FREQ_2G(10 - 2), WIFINET_CHAN_2GHZ, 10, 4, 2, WIFINET_BWC_WIDTH40, 84},
                {CHAN_2_FREQ_2G(11 - 2), WIFINET_CHAN_2GHZ, 11, 4, 2, WIFINET_BWC_WIDTH40, 84},
                {CHAN_2_FREQ_2G(12 - 2), WIFINET_CHAN_2GHZ, 12, 4, 2, WIFINET_BWC_WIDTH40, 84},
                {CHAN_2_FREQ_2G(13 - 2), WIFINET_CHAN_2GHZ, 13, 4, 2, WIFINET_BWC_WIDTH40, 84}
               }
    },//84
    {112,20,3,{
                    {CHAN_2_FREQ_5G(8), WIFINET_CHAN_5GHZ, 8, 4, 2, WIFINET_BWC_WIDTH20, 112},
                    {CHAN_2_FREQ_5G(12), WIFINET_CHAN_5GHZ, 12, 4, 2, WIFINET_BWC_WIDTH20, 112},
                    {CHAN_2_FREQ_5G(16) , WIFINET_CHAN_5GHZ, 16, 4, 2, WIFINET_BWC_WIDTH20, 112}
                   }
    },//112
     {115,20,4,{
                    {CHAN_2_FREQ_5G(36), WIFINET_CHAN_5GHZ, 36, 4, 2, WIFINET_BWC_WIDTH20, 115},
                    {CHAN_2_FREQ_5G(40), WIFINET_CHAN_5GHZ, 40, 4, 2, WIFINET_BWC_WIDTH20, 115},
                    {CHAN_2_FREQ_5G(44), WIFINET_CHAN_5GHZ, 44, 4, 2, WIFINET_BWC_WIDTH20, 115},
                    {CHAN_2_FREQ_5G(48), WIFINET_CHAN_5GHZ, 48, 4, 2, WIFINET_BWC_WIDTH20, 115}
                   }
    },//115
     {116,40,2,{
                    {CHAN_2_FREQ_5G(36 + 2), WIFINET_CHAN_5GHZ, 36, 4, 2, WIFINET_BWC_WIDTH40, 116},
                    {CHAN_2_FREQ_5G(44 + 2), WIFINET_CHAN_5GHZ, 44, 4, 2, WIFINET_BWC_WIDTH40, 116}
                   }
    },//116
    {117,40,2,{
                    {CHAN_2_FREQ_5G(40 - 2), WIFINET_CHAN_5GHZ, 40, 4, 2, WIFINET_BWC_WIDTH40, 117},
                    {CHAN_2_FREQ_5G(48 - 2), WIFINET_CHAN_5GHZ, 48, 4, 2, WIFINET_BWC_WIDTH40, 117}
                   }
    },//117
    {118,20,4,{
                    {CHAN_2_FREQ_5G(52), WIFINET_CHAN_5GHZ, 52, 4, 2, WIFINET_BWC_WIDTH20, 118},
                    {CHAN_2_FREQ_5G(56), WIFINET_CHAN_5GHZ, 56, 4, 2, WIFINET_BWC_WIDTH20, 118},
                    {CHAN_2_FREQ_5G(60), WIFINET_CHAN_5GHZ, 60, 4, 2, WIFINET_BWC_WIDTH20, 118},
                    {CHAN_2_FREQ_5G(64), WIFINET_CHAN_5GHZ, 64, 4, 2, WIFINET_BWC_WIDTH20, 118}
                   }
    },//118
    {119,40,2,{
                    {CHAN_2_FREQ_5G(52 + 2), WIFINET_CHAN_5GHZ, 52, 4, 2, WIFINET_BWC_WIDTH40, 119},
                    {CHAN_2_FREQ_5G(60 + 2), WIFINET_CHAN_5GHZ, 60, 4, 2, WIFINET_BWC_WIDTH40, 119}
                   }
    },//119
    {120,40,2,{
                    {CHAN_2_FREQ_5G(56-2), WIFINET_CHAN_5GHZ, 56, 4, 2, WIFINET_BWC_WIDTH40, 120},
                    {CHAN_2_FREQ_5G(64-2), WIFINET_CHAN_5GHZ, 64, 4, 2, WIFINET_BWC_WIDTH40, 120}
                   }
    },//120
    {121,20,12,{
                   {CHAN_2_FREQ_5G(100), WIFINET_CHAN_5GHZ, 100, 4, 2, WIFINET_BWC_WIDTH20, 121},
                   {CHAN_2_FREQ_5G(104), WIFINET_CHAN_5GHZ, 104, 4, 2, WIFINET_BWC_WIDTH20, 121},
                   {CHAN_2_FREQ_5G(108), WIFINET_CHAN_5GHZ, 108, 4, 2, WIFINET_BWC_WIDTH20, 121},
                   {CHAN_2_FREQ_5G(112), WIFINET_CHAN_5GHZ, 112, 4, 2, WIFINET_BWC_WIDTH20, 121},
                   {CHAN_2_FREQ_5G(116), WIFINET_CHAN_5GHZ, 116, 4, 2, WIFINET_BWC_WIDTH20, 121},
                   {CHAN_2_FREQ_5G(120), WIFINET_CHAN_5GHZ, 120, 4, 2, WIFINET_BWC_WIDTH20, 121},
                   {CHAN_2_FREQ_5G(124), WIFINET_CHAN_5GHZ, 124, 4, 2, WIFINET_BWC_WIDTH20, 121},
                   {CHAN_2_FREQ_5G(128), WIFINET_CHAN_5GHZ, 128, 4, 2, WIFINET_BWC_WIDTH20, 121},
                   {CHAN_2_FREQ_5G(132), WIFINET_CHAN_5GHZ, 132, 4, 2, WIFINET_BWC_WIDTH20, 121},
                   {CHAN_2_FREQ_5G(136), WIFINET_CHAN_5GHZ, 136, 4, 2, WIFINET_BWC_WIDTH20, 121},
                   {CHAN_2_FREQ_5G(140), WIFINET_CHAN_5GHZ, 140, 4, 2, WIFINET_BWC_WIDTH20, 121},
                   {CHAN_2_FREQ_5G(144), WIFINET_CHAN_5GHZ, 144, 4, 2, WIFINET_BWC_WIDTH20, 121}
                   }
     },//121
     {122,40,6,{
                    {CHAN_2_FREQ_5G(100+2), WIFINET_CHAN_5GHZ, 100, 4, 2, WIFINET_BWC_WIDTH40, 122},
                    {CHAN_2_FREQ_5G(108+2), WIFINET_CHAN_5GHZ, 108, 4, 2, WIFINET_BWC_WIDTH40, 122},
                    {CHAN_2_FREQ_5G(116+2), WIFINET_CHAN_5GHZ, 116, 4, 2, WIFINET_BWC_WIDTH40, 122},
                    {CHAN_2_FREQ_5G(124+2), WIFINET_CHAN_5GHZ, 124, 4, 2, WIFINET_BWC_WIDTH40, 122},
                    {CHAN_2_FREQ_5G(132+2), WIFINET_CHAN_5GHZ, 132, 4, 2, WIFINET_BWC_WIDTH40, 122},
                    {CHAN_2_FREQ_5G(140+2), WIFINET_CHAN_5GHZ, 140, 4, 2, WIFINET_BWC_WIDTH40, 122}
                   }
    },//122
    {123,40,6,{
                    {CHAN_2_FREQ_5G(104-2), WIFINET_CHAN_5GHZ, 104, 4, 2, WIFINET_BWC_WIDTH40, 123},
                    {CHAN_2_FREQ_5G(112-2), WIFINET_CHAN_5GHZ, 112, 4, 2, WIFINET_BWC_WIDTH40, 123},
                    {CHAN_2_FREQ_5G(120-2), WIFINET_CHAN_5GHZ, 120, 4, 2, WIFINET_BWC_WIDTH40, 123},
                    {CHAN_2_FREQ_5G(128-2), WIFINET_CHAN_5GHZ, 128, 4, 2, WIFINET_BWC_WIDTH40, 123},
                    {CHAN_2_FREQ_5G(136-2), WIFINET_CHAN_5GHZ, 136, 4, 2, WIFINET_BWC_WIDTH40, 123},
                    {CHAN_2_FREQ_5G(144-2), WIFINET_CHAN_5GHZ, 144, 4, 2, WIFINET_BWC_WIDTH40, 123}
                   }
    },//123
    {124,20,4,{
                {CHAN_2_FREQ_5G(149), WIFINET_CHAN_5GHZ, 149, 4, 2, WIFINET_BWC_WIDTH20, 124},
                {CHAN_2_FREQ_5G(153), WIFINET_CHAN_5GHZ, 153, 4, 2, WIFINET_BWC_WIDTH20, 124},
                {CHAN_2_FREQ_5G(157), WIFINET_CHAN_5GHZ, 157, 4, 2, WIFINET_BWC_WIDTH20, 124},
                {CHAN_2_FREQ_5G(161), WIFINET_CHAN_5GHZ, 161, 4, 2, WIFINET_BWC_WIDTH20, 124}
               }
    },//124
    {125,20,5,{
                {CHAN_2_FREQ_5G(149), WIFINET_CHAN_5GHZ, 149, 4, 2, WIFINET_BWC_WIDTH20, 125},
                {CHAN_2_FREQ_5G(153), WIFINET_CHAN_5GHZ, 153, 4, 2, WIFINET_BWC_WIDTH20, 125},
                {CHAN_2_FREQ_5G(157), WIFINET_CHAN_5GHZ, 157, 4, 2, WIFINET_BWC_WIDTH20, 125},
                {CHAN_2_FREQ_5G(161), WIFINET_CHAN_5GHZ, 161, 4, 2, WIFINET_BWC_WIDTH20, 125},
                {CHAN_2_FREQ_5G(165), WIFINET_CHAN_5GHZ, 165, 4, 2, WIFINET_BWC_WIDTH20, 125}
               }
    },//125
    {126,40,2,{
                    {CHAN_2_FREQ_5G(149+2), WIFINET_CHAN_5GHZ, 149, 4, 2, WIFINET_BWC_WIDTH40, 126},
                    {CHAN_2_FREQ_5G(157+2), WIFINET_CHAN_5GHZ, 157, 4, 2, WIFINET_BWC_WIDTH40, 126}
                   }
     },//126
     {127,40,2,{
                {CHAN_2_FREQ_5G(153-2), WIFINET_CHAN_5GHZ, 153, 4, 2, WIFINET_BWC_WIDTH40, 127},
                {CHAN_2_FREQ_5G(161-2), WIFINET_CHAN_5GHZ, 161, 4, 2, WIFINET_BWC_WIDTH40, 127}
               }
    },//127
    {128,80,24,{
                {CHAN_2_FREQ_5G(42), WIFINET_CHAN_5GHZ, 36, 4, 2, WIFINET_BWC_WIDTH80, 128},
                {CHAN_2_FREQ_5G(42), WIFINET_CHAN_5GHZ, 40, 4, 2, WIFINET_BWC_WIDTH80, 128},
                {CHAN_2_FREQ_5G(42), WIFINET_CHAN_5GHZ, 44, 4, 2, WIFINET_BWC_WIDTH80, 128},
                {CHAN_2_FREQ_5G(42), WIFINET_CHAN_5GHZ, 48, 4, 2, WIFINET_BWC_WIDTH80, 128},
                {CHAN_2_FREQ_5G(58), WIFINET_CHAN_5GHZ, 52, 4, 2, WIFINET_BWC_WIDTH80, 128},
                {CHAN_2_FREQ_5G(58), WIFINET_CHAN_5GHZ, 56, 4, 2, WIFINET_BWC_WIDTH80, 128},
                {CHAN_2_FREQ_5G(58), WIFINET_CHAN_5GHZ, 60, 4, 2, WIFINET_BWC_WIDTH80, 128},
                {CHAN_2_FREQ_5G(58), WIFINET_CHAN_5GHZ, 64, 4, 2, WIFINET_BWC_WIDTH80, 128},
                {CHAN_2_FREQ_5G(106), WIFINET_CHAN_5GHZ, 100, 4, 2, WIFINET_BWC_WIDTH80, 128},
                {CHAN_2_FREQ_5G(106), WIFINET_CHAN_5GHZ, 104, 4, 2, WIFINET_BWC_WIDTH80, 128},
                {CHAN_2_FREQ_5G(106), WIFINET_CHAN_5GHZ, 108, 4, 2, WIFINET_BWC_WIDTH80, 128},
                {CHAN_2_FREQ_5G(106), WIFINET_CHAN_5GHZ, 112, 4, 2, WIFINET_BWC_WIDTH80, 128},
                {CHAN_2_FREQ_5G(122), WIFINET_CHAN_5GHZ, 116, 4, 2, WIFINET_BWC_WIDTH80, 128},
                {CHAN_2_FREQ_5G(122), WIFINET_CHAN_5GHZ, 120, 4, 2, WIFINET_BWC_WIDTH80, 128},
                {CHAN_2_FREQ_5G(122), WIFINET_CHAN_5GHZ, 124, 4, 2, WIFINET_BWC_WIDTH80, 128},
                {CHAN_2_FREQ_5G(122), WIFINET_CHAN_5GHZ, 128, 4, 2, WIFINET_BWC_WIDTH80, 128},
                {CHAN_2_FREQ_5G(138), WIFINET_CHAN_5GHZ, 132, 4, 2, WIFINET_BWC_WIDTH80, 128},
                {CHAN_2_FREQ_5G(138), WIFINET_CHAN_5GHZ, 136, 4, 2, WIFINET_BWC_WIDTH80, 128},
                {CHAN_2_FREQ_5G(138), WIFINET_CHAN_5GHZ, 140, 4, 2, WIFINET_BWC_WIDTH80, 128},
                {CHAN_2_FREQ_5G(138), WIFINET_CHAN_5GHZ, 144, 4, 2, WIFINET_BWC_WIDTH80, 128},
                {CHAN_2_FREQ_5G(155), WIFINET_CHAN_5GHZ, 149, 4, 2, WIFINET_BWC_WIDTH80, 128},
                {CHAN_2_FREQ_5G(155), WIFINET_CHAN_5GHZ, 153, 4, 2, WIFINET_BWC_WIDTH80, 128},
                {CHAN_2_FREQ_5G(155), WIFINET_CHAN_5GHZ, 157, 4, 2, WIFINET_BWC_WIDTH80, 128},
                {CHAN_2_FREQ_5G(155), WIFINET_CHAN_5GHZ, 161, 4, 2, WIFINET_BWC_WIDTH80, 128}
               }
    }//128
};

struct country_set all_cnty_set [8] =
{
    //CN
    {0, {
               {1,115},
               {2,118},
               {3,125},
               {4,116},
               {5,119},
               {6,126},
               {7,81},
               {8,83},
               {9,84},
               {128,128}
        }
    },
     //US
    {1, {
               {1,115},
               {2,118},
               {3,124},
               {4,121},
               {5,125},
               {12,81},
               {22,116},
               {23,119},
               {24,122},
               {25,126},
               {26,126},
               {27,117},
               {28,120},
               {29,123},
               {30,127},
               {31,127},
               {32,83},
               {33,84},
               {128,128}
          }
    },
    //E.R
     {2, {
               {1,115},
               {2,118},
               {3,121},
               {4,81},
               {5,116},
               {6,119},
               {7,122},
               {8,117},
               {9,120},
               {10,123},
               {11,83},
               {12,84},
               {17,125},
               {128,128}
            }
    },
//FR
      {3, {
               {0,0},
               {0,0},
               {0,0}
            }
       },
//JP
     {4, {
               {1,115},
               {7,109},
               {32,118},
               {34,121},
               {36,116},
               {37,119},
               {39,122},
               {41,117},
               {42,120},
               {44,123},
               {46,104},
               {56,83},
               {57,84},
               {58,121},
               {128,128}
            }
    },
    //ISR
     {5, {
               {0,0},
               {0,0},
               {0,0}
            }
    },
    //MEXI
    {6, {
               {0,0},
               {0,0},
               {0,0}
           }
    },
    //MAX
    {7, {
               {0,0},
               {0,0},
               {0,0}
           }
    }
};

static int  wifi_mac_get_pos(struct wifi_channel in[], int a, int b)
{
    int i = a;
    int j = b;
    struct wifi_channel x ;

    if (!in || a > b)
    {
        return 0;
    }

    memcpy(&x,  &in[i], sizeof(struct wifi_channel));
    while (i < j)
    {
        while (i < j && in[j].chan_pri_num >= x.chan_pri_num)
        {
            j--;
        }
        if ( i < j)
        {
            memcpy(&in[i], &in[j], sizeof(struct wifi_channel));
            i++;
        }
        while (i < j && in[i].chan_pri_num < x.chan_pri_num)
        {
            i++;
        }
        if (i < j)
        {
            memcpy(&in[j], &in[i], sizeof(struct wifi_channel));
            j--;
        }
    }

    memcpy(&in[i], &x, sizeof(x));
    return i;
}

void wifi_mac_chan_order(struct wifi_channel in[], int a, int b)
{
    int pos = 0;
    if (a < b)
    {
        pos = wifi_mac_get_pos(in, a, b);
        wifi_mac_chan_order(in, a, pos-1);
        wifi_mac_chan_order(in, pos+1, b);
    }
}

void  wifi_mac_get_opt_set_by_country(int country_code, int support_opt[], int support_num,int out_opt[],int *out_num)
{
    int i = 0;
    int j = 0;
    int k = 0;
    int pos = 0;

    if ((!support_opt) || (!out_opt)) {
        printk("%s(%d) input err!\n", __func__, __LINE__);
        return;
    }

    for (i = 0; i < ARR_SIZE_OF(all_cnty_set);i++) {
        if (country_code == all_cnty_set[i].country) {
            pos = i;
            break; // get the country
        }
    }

    // get the global index
    for (i = 0; i < support_num; i++) {
        for (j = 0; j < MAX_CLASS_NUM; j++) {
            if (support_opt[i] == all_cnty_set[pos].opt_idx_map[j].operating_class) {
                out_opt[k] = all_cnty_set[pos].opt_idx_map[j].g_operating_class;
                k++;
            }
            *out_num = k;
        }
    }
}

void  wifi_mac_select_chan_from_global(int sub_set[], int num,struct wifi_mac *wifimac)
{
    int i = 0;
    int j = 0;
    int k = 0;
    int channel_num = 0;
    short hash_20M[200] = {0};
    short hash_40M[200][2] = {{-1}};
    short hash_80M[200] = {-1};
    int chn_idx_20M = 0;
    int chn_idx_40M = 0;
    int chn_idx_80M = 0;

    if ((!sub_set) || (!wifimac)) {
        printk("%s(%d) input or memory err!\n", __func__, __LINE__);
        return;
    }

    memset(hash_40M, -1, sizeof(hash_40M));
    for (i = 0; i < num; i++) {
        for (j = 0; j < ARR_SIZE_OF(global_chan_set); j++) {
            if (sub_set[i] == global_chan_set[j].opt_idx) {
                for (k = 0; k < global_chan_set[j].sub_num ; k++) {
                    if (global_chan_set[j].chan_sub_set[k].chan_bw == WIFINET_BWC_WIDTH20) {
                        chn_idx_20M = global_chan_set[j].chan_sub_set[k].chan_pri_num;

                        if (hash_20M[chn_idx_20M] == 0) {
                            memcpy(wifimac->wm_channels + channel_num++, &global_chan_set[j].chan_sub_set[k], sizeof(struct wifi_channel));
                            hash_20M[chn_idx_20M]++;
                        }

                    } else if (global_chan_set[j].chan_sub_set[k].chan_bw == WIFINET_BWC_WIDTH40) {
                        chn_idx_40M = global_chan_set[j].chan_sub_set[k].chan_pri_num;

                        if (hash_40M[chn_idx_40M][0] == -1) {
                            if (hash_40M[chn_idx_40M][1] == -1) {
                                memcpy(wifimac->wm_channels + channel_num++, &global_chan_set[j].chan_sub_set[k], sizeof(struct wifi_channel));
                                hash_40M[chn_idx_40M][1] = channel_num - 1;

                            } else if (hash_40M[chn_idx_40M][1] != -1) {
                                if (wifimac->wm_channels[hash_40M[chn_idx_40M][1]].chan_cfreq1 != global_chan_set[j].chan_sub_set[k].chan_cfreq1) {
                                    memcpy(wifimac->wm_channels + channel_num++, &global_chan_set[j].chan_sub_set[k], sizeof(struct wifi_channel));
                                    hash_40M[chn_idx_40M][0]++;//40M finish
                                }
                            }
                        }

                    } else if (global_chan_set[j].chan_sub_set[k].chan_bw == WIFINET_BWC_WIDTH80) {
                        chn_idx_80M = global_chan_set[j].chan_sub_set[k].chan_pri_num;

                        if (hash_80M[chn_idx_80M] == 0) {
                            memcpy(wifimac->wm_channels + channel_num++, &global_chan_set[j].chan_sub_set[k], sizeof(struct wifi_channel));
                            hash_80M[chn_idx_80M]++;
                        }
                    }
                }
            }
        }
    }

    wifimac->wm_nchans = channel_num;
    wifi_mac_chan_order(wifimac->wm_channels, 0, wifimac->wm_nchans - 1);

#if 0
    printk("wifimac->wm_nchans:%d\n", wifimac->wm_nchans);
    for (i = 0; i < wifimac->wm_nchans; ++i) {
        //printk("chan_cfreq1:%d\n", wifimac->wm_channels[i].chan_cfreq1);
        //printk("chan_bw:%d\n", wifimac->wm_channels[i].chan_bw);
        printk("chan_pri_num:%d\n", wifimac->wm_channels[i].chan_pri_num);
    }
    printk("i is %d\n", i);
#endif
}

void  wifi_mac_update_chan_list_by_country(int country_code, int support_opt[],int support_num,struct wifi_mac *wifimac )
{
    int opt_idx[32] = {0};
    int opt_num = 0;

    if ((!support_opt)||(support_num==0))
    {
        printk(" %s(%d) input is NULL!!!\n", __func__, __LINE__);
    }

    memset(wifimac->wm_channels, 0, sizeof(struct wifi_channel) * (WIFINET_CHAN_MAX * 2 + 1));
    wifimac->wm_nchans = 0;

    wifi_mac_get_opt_set_by_country(country_code, support_opt, support_num, opt_idx, &opt_num);
    wifi_mac_select_chan_from_global(opt_idx, opt_num,wifimac);
}

unsigned char wifi_mac_chan_num_availd (struct wifi_mac *wifimac, unsigned char channum)
{
    if (isset(wifimac->wm_chan_avail, channum))
    {
        return true;
    }
    else
    {
        return false;
    }
}

unsigned int wifi_mac_Mhz2ieee(unsigned int freq, unsigned int flags)
{
    if (flags & WIFINET_CHAN_2GHZ)
    {
        if (freq == 2484)
            return 14;

        if (freq < 2484)
            return (freq - 2407) / 5;
        else
            return 15 + ((freq - 2512) / 20);
    }
    else if (flags & WIFINET_CHAN_5GHZ)
    {
        return (freq - 5000) / 5;
    }
    else
    {
        if (freq == 2484)
            return 14;

        if (freq < 2484)
            return (freq - 2407) / 5;

        if (freq < 5000)
            return 15 + ((freq - 2512) / 20);

        return (freq - 5000) / 5;
    }
}

/*check struct WIFINET_S_CHANNEL *c and return primary channel num */
unsigned int wifi_mac_chan2ieee(struct wifi_mac *wifimac,
const struct wifi_channel *c)
{
    if (c == NULL)
    {
        printk("amlwifi,invalid channel (NULL)\n");
        return WIFINET_CHAN_INVALUE;
    }
    return c->chan_pri_num;
}

unsigned int wifi_mac_Ieee2mhz(unsigned int chan, unsigned int flags)
{
    if (flags & WIFINET_CHAN_2GHZ)
    {
        if (chan == 14)
            return 2484;

        if (chan < 14)
            return 2407 + chan*5;
        else
            return 2512 + ((chan-15)*20);
    }
    else if (flags & WIFINET_CHAN_5GHZ)
    {
        return 5000 + (chan*5);
    }
    else
    {
        if (chan == 14)
            return 2484;

        if (chan < 14)
            return 2407 + chan*5;

        if (chan < 27)
            return 2512 + ((chan-15)*20);

        return 5000 + (chan*5);
    }
}

struct wifi_channel * wifi_mac_find_chan(struct wifi_mac *wifimac, int chan, int bw, int center_chan)
{
    struct wifi_channel *c = NULL;
    int i = 0;

    WIFI_CHANNEL_LOCK(wifimac);
    for (i = 0; i < wifimac->wm_nchans; i++) {
        c = &wifimac->wm_channels[i];
        if ((c->chan_pri_num== chan) && (c->chan_bw == bw) && (c->chan_cfreq1 == wifi_mac_Ieee2mhz(center_chan, 0))) {
            DPRINTF(AML_DEBUG_BWC, "%s(%d): find a chan=%d, bw=%d and cntr chan=%d.\n", __func__, __LINE__, chan, bw ,center_chan);
            WIFI_CHANNEL_UNLOCK(wifimac);
            return c;
        }
    }
    WIFI_CHANNEL_UNLOCK(wifimac);

    DPRINTF(AML_DEBUG_BWC, "%s(%d) !!!: NOT find a chan=%d, bw=%d and center chan=%d.\n", __func__, __LINE__,  chan, bw ,center_chan);
    return NULL;
}

void wifi_mac_update_country_chan_list(struct wifi_mac *wifimac)
{
    struct wifi_channel *c;
    int i;

    memset(wifimac->wm_chan_avail, 0, sizeof(wifimac->wm_chan_avail));
    wifi_mac_get_country(wifimac, &wifimac->wm_country);

    for (i = 0; i < wifimac->wm_nchans; i++)
    {
        c = &wifimac->wm_channels[i];
        setbit(wifimac->wm_chan_avail, c->chan_pri_num);
    }
}

int wifi_mac_recv_bss_intol_channelCheck(struct wifi_mac *wifimac, struct wifi_mac_ie_intolerant_report *intol_ie)
{
    int i, j;
    unsigned char intol_chan  = 0;
    unsigned char *chan_list = &intol_ie->chan_list[0];

    if (intol_ie->elem_len <= 1)
        return false;

    WIFI_CHANNEL_LOCK(wifimac);
    for (i = 0; i < intol_ie->elem_len-1; i++)
    {
        intol_chan = *chan_list++;
        for (j = 0; j < wifimac->wm_nchans; j++)
        {
            if (intol_chan == wifimac->wm_channels[j].chan_pri_num)
            {
                WIFI_CHANNEL_UNLOCK(wifimac);
                return true;
            }
        }
    }
    WIFI_CHANNEL_UNLOCK(wifimac);

    return false;
}

struct wifi_channel * wifi_mac_scan_sta_get_ap_channel(struct wlan_net_vif *wnet_vif, struct wifi_mac_scan_param *sp)
{
    unsigned char *htinfoie = sp->htinfo;
    unsigned char chan = sp->chan;
    struct wifi_mac_ie_htinfo *xhtinfo = (struct wifi_mac_ie_htinfo*)htinfoie;
    struct wifi_mac_ie_htinfo_cmn *htinfo = NULL;
    struct wifi_mac *wifimac = wnet_vif->vm_wmac;
    struct wifi_mac_ie_vht_opt *vht_opt_ie =  (struct wifi_mac_ie_vht_opt *)sp->vht_opt;
    int center_chan = 0, bw = 0;
    static struct wifi_channel *apchan =NULL;

    if (xhtinfo != NULL)
    {
        htinfo = &xhtinfo->hi_ie;
    }

    DPRINTF(AML_DEBUG_BWC, "%s(%d) %s, chan %d htinfo %p,chwidth %x \n", __func__, __LINE__,
        ssidie_sprintf(sp->ssid), sp->chan, htinfo, htinfo?htinfo->hi_txchwidth : 0);

    if ((htinfo != NULL) && (htinfo->hi_txchwidth == WIFINET_HTINFO_TXWIDTH_2040))
    {
        DPRINTF(AML_DEBUG_BWC, "%s pri_chn %d\n",__func__, htinfo->hi_ctrlchannel);

        switch (htinfo->hi_extchoff)
        {
            case WIFINET_HTINFO_EXTOFFSET_ABOVE:
                DPRINTF(AML_DEBUG_BWC, "%s %d  WIFINET_HTINFO_EXTOFFSET_ABOVE \n", __func__,__LINE__);
                bw = WIFINET_BWC_WIDTH40;
                center_chan = htinfo->hi_ctrlchannel + 2;
                break;

                case WIFINET_HTINFO_EXTOFFSET_BELOW:
                DPRINTF(AML_DEBUG_BWC, "%s %d  WIFINET_HTINFO_EXTOFFSET_BELOW\n", __func__,__LINE__);
                bw = WIFINET_BWC_WIDTH40;
                center_chan = htinfo->hi_ctrlchannel - 2;
                break;

            case WIFINET_HTINFO_EXTOFFSET_NA:
            default:
                DPRINTF(AML_DEBUG_BWC, "%s %d 20M only mode\n",__func__,__LINE__);
                bw = WIFINET_BWC_WIDTH20;
                center_chan = htinfo->hi_ctrlchannel;
                break;
        }
    }
    else
    {
        bw = WIFINET_BWC_WIDTH20;
        center_chan = chan;
    }
    if((vht_opt_ie != NULL) &&
    (vht_opt_ie->vht_op_chwidth >= WIFINET_VHTOP_CHWIDTH_80))
    {
        center_chan = vht_opt_ie->vht_op_ch_freq_seg1;
        DPRINTF(AML_DEBUG_BWC, "%s center chn %d\n",__func__, center_chan);

        switch(vht_opt_ie->vht_op_chwidth)
        {
            case WIFINET_VHTOP_CHWIDTH_80:
                bw = WIFINET_BWC_WIDTH80;
                break;
            case WIFINET_VHTOP_CHWIDTH_160:
                bw = WIFINET_BWC_WIDTH160;
                break;
            case WIFINET_VHTOP_CHWIDTH_80_80:
                bw = WIFINET_BWC_WIDTH80P80;
                break;
            default:
                bw = WIFINET_BWC_WIDTH80;
                break;
        }
    }

    DPRINTF(1, "%s chan %d, bw %d, center_chan %d \n", __func__, chan, bw, center_chan);
    apchan = wifi_mac_find_chan(wifimac, chan, bw, center_chan);
    if ((apchan == NULL) && (htinfo != NULL)) {
        if ((htinfo->hi_txchwidth == WIFINET_HTINFO_TXWIDTH_2040) && (bw == WIFINET_BWC_WIDTH40)) {
            bw = WIFINET_BWC_WIDTH20;
            center_chan = chan;
            apchan = wifi_mac_find_chan(wifimac, chan, bw, center_chan);
        }
    }
    return apchan;
}

int wifi_mac_chan_attach (struct wifi_mac *wifimac)
{
    struct wifi_channel *c;
    int i;
    KASSERT(0 < wifimac->wm_nchans && wifimac->wm_nchans < WIFINET_CHAN_MAX,
            ("invalid number of channels specified: %u", wifimac->wm_nchans));
    memset(wifimac->wm_chan_avail, 0, sizeof(wifimac->wm_chan_avail));

    WIFI_CHANNEL_LOCK(wifimac);
    for (i = 0; i < wifimac->wm_nchans; i++)
    {
        c = &wifimac->wm_channels[i];
        KASSERT(c->chan_pri_num < WIFINET_CHAN_MAX,
                ("channel with bogus ieee number %u", c->chan_pri_num));
        setbit(wifimac->wm_chan_avail, c->chan_pri_num);
    }
    WIFI_CHANNEL_UNLOCK(wifimac);

    wifimac->wm_curchan = WIFINET_CHAN_ERR;

    return 0;
}

void wifi_mac_chan_setup(void * ieee, unsigned int wMode, int countrycode_ex)
{
    struct wifi_mac *wifimac = NET80211_HANDLE(ieee);
    int *support_ptr = NULL;
    int support_num = 0;
    int i = 0;

    switch (countrycode_ex)
    {
        case WIFI_NorthAmerica:
            support_num = support_idx[WIFI_NorthAmerica][0];
            support_ptr = &support_idx[WIFI_NorthAmerica][1];
            break;

        case WIFI_Europe:
            support_num = support_idx[WIFI_Europe][0];
            support_ptr = &support_idx[WIFI_Europe][1];
            break;

        case WIFI_France:
            support_num = support_idx[WIFI_France][0];
            support_ptr = &support_idx[WIFI_France][1];
            break;

        case WIFI_Israel:
            support_num = support_idx[WIFI_Israel][0];
            support_ptr = &support_idx[WIFI_Israel][1];
            break;

        case WIFI_Mexico:
            support_num = support_idx[WIFI_Mexico][0];
            support_ptr = &support_idx[WIFI_Mexico][1];
            break;

        case WIFI_China:/*channel 1~14*/
            support_num = support_idx[WIFI_China][0];
            support_ptr = &support_idx[WIFI_China][1];
            break;

        case WIFI_Japan:
            support_num = support_idx[WIFI_Japan][0];
            support_ptr = &support_idx[WIFI_Japan][1];
             break;

        default:
           printk("%s(%d) NOT support this country 0x%x\n", __func__, __LINE__, countrycode_ex);
         break;
    }

    printk("%s(%d) country code 0x%x, support num %d\n", __func__, __LINE__, countrycode_ex, support_num);
    wifi_mac_update_chan_list_by_country(countrycode_ex,support_ptr, support_num, wifimac);

    for(i= 0; i < wifimac->wm_nchans; i++) {
        chan_dbg(&wifimac->wm_channels[i], "chan info", __LINE__);
    }
}

void wifi_mac_ChangeChannel(void * ieee, struct wifi_channel *chan, unsigned char flag, unsigned char vid)
{
    struct wifi_mac *wifimac = NET80211_HANDLE(ieee);
    struct hal_channel hchan;

    if (chan == WIFINET_CHAN_ERR) {
        printk("%s chan not valid !!\n", __func__);
        return;
    }

    wifimac->wm_curchan = chan;
    hchan.channel = chan->chan_cfreq1;
    hchan.cchan_num = wifi_mac_mhz2chan(chan->chan_cfreq1);
    hchan.pchan_num = chan->chan_pri_num;
    hchan.chan_bw = chan->chan_bw;

    KASSERT(hchan.channel != 0,("bogus channel %u\n", hchan.channel));
#ifdef FW_RF_CALIBRATION
    wifimac->drv_priv->drv_ops.drv_hal_tx_frm_pause(wifimac->drv_priv, 1);
#endif
    wifimac->drv_priv->drv_ops.set_channel(wifimac->drv_priv, &hchan, flag, vid);

    if (!(wifimac->wm_flags & WIFINET_F_SCAN)) {
        DPRINTF(AML_DEBUG_WARNING, "%s: %.3d %d\n", __func__, chan->chan_pri_num, chan->chan_bw);
    }
}

void wifi_mac_set_wnet_vif_chan_ex(SYS_TYPE param1,SYS_TYPE param2, SYS_TYPE param3,
    SYS_TYPE param4,SYS_TYPE param5)
{
    struct wifi_mac *wifimac = (struct wifi_mac * )param1;
    struct wifi_channel *wnet_vif_chan = (struct wifi_channel * )param2;
    struct wlan_net_vif *wnet_vif = (struct wlan_net_vif *)param3;

    wifi_mac_ChangeChannel(wifimac, wnet_vif_chan, 3, wnet_vif->wnet_vif_id);
}

struct wifi_channel * wifi_mac_get_wm_chan (struct wifi_mac *wifimac)
{
    if (wifimac->wm_curchan != WIFINET_CHAN_ERR)
    {
        DPRINTF(AML_DEBUG_ERROR,  "wm_curchan != WIFINET_CHAN_ERR\n");
        return wifimac->wm_curchan;
    }
    else
    {
        return WIFINET_CHAN_ERR;
    }
}

int wifi_mac_set_wnet_vif_channel(struct wlan_net_vif *wnet_vif,  int chan, int bw, int center_chan)
{
    struct wifi_mac *wifimac = wnet_vif->vm_wmac;
    struct wifi_channel * c = NULL;

    printk("%s(%d)\n", __func__, __LINE__);
    c = wifi_mac_find_chan(wifimac, chan, bw, center_chan);

    if (c == NULL) {
        DPRINTF(AML_DEBUG_ERROR, "WARNING<%s>:: %s %d can't support set this channel\n",
            (wnet_vif)->vm_ndev->name, __func__, __LINE__);
        return false;
    }

    DPRINTF(AML_DEBUG_ERROR, "%s(%d),chan_pri_num =%d, chan_cfreq1 =%d,chan_flags =0x%x\n",
        __func__,__LINE__, c->chan_pri_num, c->chan_cfreq1, c->chan_flags);
    wnet_vif->vm_curchan = c;
    wifi_mac_add_work_task(wnet_vif->vm_wmac, wifi_mac_set_wnet_vif_chan_ex, NULL,
        (SYS_TYPE)(wnet_vif->vm_wmac), (SYS_TYPE)c, (SYS_TYPE)wnet_vif, 0, 0);
    return true;
}

struct wifi_channel *wifi_mac_get_connect_wnet_vif_channel(struct wlan_net_vif *wnet_vif)
{
    struct wifi_mac *wifimac = wnet_vif->vm_wmac;
    struct drv_private* drv_priv = wifimac->drv_priv;
    struct wlan_net_vif *selected_wnet_vif = wnet_vif;

    if (wifimac->wm_nrunning == 1) {
        if (drv_priv->drv_wnet_vif_table[NET80211_P2P_VMAC]->vm_state == WIFINET_S_CONNECTED) {
            selected_wnet_vif = drv_priv->drv_wnet_vif_table[NET80211_P2P_VMAC];
        } else {
            selected_wnet_vif = drv_priv->drv_wnet_vif_table[NET80211_MAIN_VMAC];
        }
    }
    return selected_wnet_vif->vm_curchan;
}

void wifi_mac_restore_wnet_vif_channel(struct wlan_net_vif *wnet_vif)
{
    struct wifi_mac *wifimac = wnet_vif->vm_wmac;
    struct drv_private* drv_priv = wifimac->drv_priv;
    struct wlan_net_vif *selected_wnet_vif = NULL;

    if (wifimac->wm_nrunning > 1)
    {
    #ifdef CONFIG_CONCURRENT_MODE
        if (wifimac->wm_vsdb_slot == CONCURRENT_SLOT_P2P) {
            selected_wnet_vif = drv_priv->drv_wnet_vif_table[NET80211_P2P_VMAC];

        } else if (wifimac->wm_vsdb_slot == CONCURRENT_SLOT_STA) {
            selected_wnet_vif = drv_priv->drv_wnet_vif_table[NET80211_MAIN_VMAC];

        } else {
            selected_wnet_vif = drv_priv->drv_wnet_vif_table[NET80211_P2P_VMAC];

            if (selected_wnet_vif->vm_curchan->chan_bw < drv_priv->drv_wnet_vif_table[NET80211_MAIN_VMAC]->vm_curchan->chan_bw) {
                selected_wnet_vif = drv_priv->drv_wnet_vif_table[NET80211_MAIN_VMAC];
            }
        }
    #endif
    } else if (wifimac->wm_nrunning == 1) {
        if (drv_priv->drv_wnet_vif_table[NET80211_P2P_VMAC]->vm_state == WIFINET_S_CONNECTED) {
            selected_wnet_vif = drv_priv->drv_wnet_vif_table[NET80211_P2P_VMAC];
        } else {
            selected_wnet_vif = drv_priv->drv_wnet_vif_table[NET80211_MAIN_VMAC];
        }
    }

    if (selected_wnet_vif == NULL) {
        return;
    }

    if ((selected_wnet_vif->vm_curchan != WIFINET_CHAN_ERR) && (wifimac->wm_curchan != selected_wnet_vif->vm_curchan)) {
        DPRINTF(AML_DEBUG_SCAN, "%s vid:%d, prichan:%d, bw:%d\n",  __func__, selected_wnet_vif->wnet_vif_id,
            selected_wnet_vif->vm_curchan->chan_pri_num, selected_wnet_vif->vm_curchan->chan_bw);
        wifi_mac_ChangeChannel(wifimac, selected_wnet_vif->vm_curchan, 1, selected_wnet_vif->wnet_vif_id);
    }

    wifimac->drv_priv->drv_ops.drv_set_is_mother_channel(wifimac->drv_priv, selected_wnet_vif->wnet_vif_id, 1);
    wifi_mac_scan_notify_leave_or_back(selected_wnet_vif, 0);
    wifimac->drv_priv->drv_ops.drv_flush_normal_buffer_queue(wifimac->drv_priv, selected_wnet_vif->wnet_vif_id);
    tasklet_schedule(&wifimac->drv_priv->ampdu_tasklet);
}

void wifi_mac_restore_wnet_vif_channel_task_ex(void * data)
{
    struct wlan_net_vif *wnet_vif = (struct wlan_net_vif *)data;
    wifi_mac_restore_wnet_vif_channel(wnet_vif);
}

void wifi_mac_restore_wnet_vif_channel_task(struct wlan_net_vif *wnet_vif)
{
    struct wifi_mac *wifimac = (struct wifi_mac *)(wnet_vif->vm_wmac);
    wifi_mac_add_work_task(wifimac, wifi_mac_restore_wnet_vif_channel_task_ex, NULL, (SYS_TYPE)wnet_vif,0,0,0,0);
}

struct wifi_channel *wifi_mac_get_main_vmac_channel(struct wifi_mac *wifimac)
{
    struct drv_private *drv_priv = wifimac->drv_priv;
    if (drv_priv->drv_wnet_vif_table[NET80211_MAIN_VMAC]) {
        return drv_priv->drv_wnet_vif_table[NET80211_MAIN_VMAC]->vm_curchan;

    } else {
        return WIFINET_CHAN_ERR;
    }
}

struct wifi_channel *wifi_mac_get_p2p_vmac_channel(struct wifi_mac *wifimac)
{
    struct drv_private *drv_priv = wifimac->drv_priv;
    return drv_priv->drv_wnet_vif_table[NET80211_P2P_VMAC]->vm_curchan;
}

void chan_dbg(struct wifi_channel *chan, char* str, int line)
{
    if (chan == WIFINET_CHAN_ERR) {
        return;
    }

    DPRINTF(AML_DEBUG_BWC, "%s(%d)->curr: freq1 %d flags 0x%x prim %d, maxpw %d, minpw %d bw %d\n",
        str, line, chan->chan_cfreq1,chan->chan_flags,chan->chan_pri_num,
        chan->chan_maxpower,chan->chan_minpower,chan->chan_bw);
}

