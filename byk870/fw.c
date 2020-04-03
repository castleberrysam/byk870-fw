#include <stdint.h>

uint8_t ram_int[0x100];
uint8_t ram_ext[0x900];
uint8_t eeprom[0x800];
uint8_t prog[0x10000];

uint8_t SP;

const uint8_t init_data[] = {
  0xc1, // set 1 bit-addressable register
  0x19, // set 0x23[1] = 0
  0x41, // set 1 external ram value
  0x02,
  0x1d,
  0x64, // 0x021d = 0x64
  0xc1, // set 1 bit-addressable register
  0xd3, // set 0x2a[3] = 1
  0xc1, // set 1 bit-addressable register
  0xd5, // set 0x2a[5] = 1
  0x41, // set 1 external ram value
  0x03,
  0xc0,
  0x01, // 0x03c0 = 0x01
  0x00
};

// address bee1
void reset(void)
{
  for(int i=0;i<sizeof(ram_int);++i) {ram_int[i] = 0;}
  for(int i=0;i<sizeof(ram_ext);++i) {ram_ext[i] = 0;}
  
  uint8_t *ptr = init_data;
  while(true) {
    int r6 = 1;
    
    int a = *(ptr++);
    if(a == 0) {break;}

    int r7 = a;
    a &= 0x3f;
    if((a & 0x20) != 0) {
      r6 = a & 0x1f;
      a = *(ptr++);
      if(a != 0) {++r6;}
    }

    xchg(a, r7);
    a &= 0xc0;
    a += a;
    if(a == 0) {
      int r0 = *(ptr++);

      do {
        a = *(ptr++);
        if(!carry) {
          ram_int[r0] = a;
        } else {
          ram_ext[r0] = a;
        }
        ++r0;
      } while(r7 times);
    } else if(carry) {
      do {
        int a = *(ptr++);
        int r0 = a & 7;
        a <<= 1;
        swap(a);
        a &= 0x0f;
        a |= 0x20;
        r0 = {0x01,0x02,0x04,0x08,0x10,0x20,0x40,0x80}[r0];
        if(!carry) {
          r0 = ~r0;
          r0 &= ram_int[a];
        } else {
          r0 |= ram_int[a];
        }
        ram_int[a] = r0;
      } while(r7 times);
    } else {
      int r2 = *(ptr++);
      int r0 = *(ptr++);

      do {
        ram_ext[((r2 << 8) | r0)++] = *(ptr++);
      } ((r6 << 8) | r7 times);
    }
  }

  // jump to 8a4e
  asm("CLR EA"); // disable interrupts
  
  ram_int[0xb1] = 0; // clear the reset flag
  ram_int[0x86] &= 0x3f; // select SFR bank 0
  
  init();

  ram_int[0xe9] &= 0xef; // disable pullup on P0.4
  ram_int[0xe1] &= 0xef; // set P0.4 as input
  ram_int[0xeb] &= 0xfe; // disable pullup on P2.0
  ram_int[0xe3] &= 0xfe; // set P2.0 as input

  sub_d9ef(1, 0x00);
  sub_d9ef(2, 0x00);
  sub_d9ef(3, 0x00);

  // initialize some more stuff
  clr_row_color_all();
  set_col_drive_all();
  sub_c9c8();
  sub_de53();
  
  start_timer3();
  start_pwm();

  asm("SETB 22.0");
  asm("CLR 2c.4");
  ram_ext[0x03ca] = 0;
  asm("CLR 2b.2");
  asm("SETB 2b.0");
  ram_ext[0x039f] = 0x15;
  ram_ext[0x039b] = 0x1f;
  ram_ext[0x0395] = 0x29;
  ram_ext[0x0393] = 0x33;
  asm("MOV RB1R1, #4");
  
  read_eeprom_1();
  read_eeprom_2();
  read_eeprom_3();
  read_eeprom_4();
  read_eeprom_5();
  
  start_usb();

  asm("SETB EA"); // enable interrupts

  // address 8acb
 start:
  ram_int[0x0c] = 0x00;
  ram_int[0x0d] = 0xc8;

  if(2b.2) {
    asm("CLR 2b.2");
    suspend();
  }

  if(ram_int[0x2d] != 0 && !23.1) {
    delay(0xfffa); // delay for ~50ms
    ram_int[0x0c] = 0x00;
    ram_int[0x0d] = 0xc8;
    ram_int[0xb1] = 0x00; // clear watchdog timer
    
    delay(0xfffa); // delay for ~50ms
    ram_int[0x0c] = 0x00;
    ram_int[0x0d] = 0xc8;
    ram_int[0xb1] = 0x00; // clear watchdog timer

    delay(0xfffa); // delay for ~50ms
    ram_int[0x0c] = 0x00;
    ram_int[0x0d] = 0xc8;
    ram_int[0xb1] = 0x00; // clear watchdog timer

    if(!20.0) {
      asm("SETB 23.1");
      asm("SETB 20.1");
      start_timer2();
    }
  }

  if(!20.0) {
    sub_dd70();

    if(!20.4 && !20.3 && !27.2) {
      // windows lock LED
      if(23.3) {
        asm("CLR P3.4");
      } else {
        asm("SETB P3.4");
      }

      // scroll lock LED
      if(ram_int[0x46].2) {
        asm("CLR P3.5");
      } else {
        asm("SETB P3.5");
      }

      // caps lock LED
      if(ram_int[0x46].1) {
        asm("CLR P3.6");
      } else {
        asm("SETB P3.6");
      }

      // num lock LED
      if(ram_int[0x46].0) {
        asm("CLR P3.7");
      } else {
        asm("SETB P3.7");
      }
    }

    goto label3:
  } else {
    if(++ram_ext[0x039a] == 0) {++ram_ext[0x0399];}
    
    if(ram_ext[{0x0399,0x039a}] - 0xfff0 borrowed) {
      ram_ext[0x0399] = 0;
      ram_ext[0x039a] = 0;
      asm("SETB 2b.2");
    }

  label3:
    if(27.2) {
      if(ram_ext[0x0398] == 0) {
      label1:
        sub_7833();
      }
    label2:
      if(2a.0) {
        sub_c25e();
        if(!20.0) {
          if(21.5) {sub_d04d();}
          if(23.2) {sub_b4d2();}
        }

        asm("CLR 2a.0");
      }

      if(!2a.1) {
        goto start;
      } else {
        asm("CLR 2a.1");
        
        sub_b091();
        sub_9dca();
        sub_7a9c();
        sub_de0e();
        sub_9414();
        sub_6c17();
        sub_a70a();
        sub_2fb7();
        sub_dea5();
        if(!22.0) {
          sub_d54b();
          sub_7580();
          sub_a09c();
          sub_b7fd();
          sub_c4e8();
          sub_b73e();
          sub_b672();
          sub_a5c9();
          sub_df51();
        }
        sub_0042();
        sub_d27c();
        sub_a949();
        sub_b3ff();
        update_indicators();
        if(((20.7 || 22.4) && (21.3 || 21.4 || 21.6)) || 20.6 || 2b.3) {
          sub_005e();
        }
        sub_18be();

        ram_ext[0x0000] = 0xff;
        if(ram_ext[0x00e1] != 0) {ram_ext[0x0000] &= 0xfe;}
        if(ram_ext[0x001d] != 0) {ram_ext[0x0000] &= 0xfd;}
        if(ram_ext[0x0029] != 0) {ram_ext[0x0000] &= 0xfb;}
        if(ram_ext[0x0035] != 0) {ram_ext[0x0000] &= 0xf7;}
        if(ram_ext[0x0041] != 0) {ram_ext[0x0000] &= 0xef;}
        
        handle_ep1_in();
        handle_ep2_in();
        goto start;
      }
    } else {
      if(29.4) {
        asm("CLR 29.4");
        goto label1;
      } else {
        goto label2;
      }
    }
  }
}

// address 6f4a
void handle_ep2_in(void)
{
  ram_int[0x86] |= 0x40; // select SFR bank 1
  if(ram_int[0xd8].2) { // if EP2 IN is already ready
    ram_int[0x86] &= 0x3f; // select SFR bank 0
    return;
  }

  if(21.6) {
    asm("CLR 21.6");
    asm("CLR EA"); // disable interrupts

    if(23.3) {
      ram_int[0x90] &= 0xf7; // indirect high RAM access
      ram_int[0x90] &= 0x7f; // indirect high RAM access
    }

    ram_int[0x86] |= 0x40; // select SFR bank 1
    ram_int[0x9f] = 0x08; // set IEP2CNT to 8 bytes
    ram_int[0x86] &= 0x3f; // select SFR bank 0
    
    int addr = 0x0a70; // held in pair 3d:3e; address of EP2 IN buffer
    ram_ext[addr++] = 0x08;
    ram_ext[addr++] = ram_ext[0x02b5];
    ram_ext[addr++] = ram_ext[0x02b6];
    ram_ext[addr++] = ram_ext[0x02b7];
    ram_ext[addr++] = ram_ext[0x02b8];
    ram_ext[addr++] = ram_ext[0x02b9];
    ram_ext[addr++] = ram_ext[0x02ba];
    ram_ext[addr] = ram_ext[0x02bb];

    ram_int[0x86] |= 0x40; // select SFR bank 1
    ram_int[0xd8] |= 0x04; // set IEP2RDY (data is ready to be sent)
    ram_int[0x86] &= 0x3f; // select SFR bank 0

    asm("SETB EA"); // enable interrupts
    
    ram_ext[0x02b5] = 0;
    ram_ext[0x02b6] = 0;
    ram_ext[0x02b7] = 0;
    ram_ext[0x02b8] = 0;
    ram_ext[0x02b9] = 0;
    ram_ext[0x02ba] = 0;
    ram_ext[0x02bb] = 0;
  } else if(21.3) {
    asm("CLR 21.3");
    asm("CLR EA"); // disable interrupts

    ram_int[0x86] |= 0x40; // select SFR bank 1
    ram_int[0x9f] = 0x08; // set IEP2CNT to 8 bytes
    ram_int[0x86] &= 0x3f; // select SFR bank 0

    int addr = 0x0a70; // held in pair 3d:3e; address of EP2 IN buffer
    ram_ext[addr++] = 0x06;
    ram_ext[addr++] = ram_int[0x90]; // indirect high RAM access
    ram_ext[addr++] = ram_int[0x91]; // indirect high RAM access
    ram_ext[addr++] = ram_int[0x92]; // indirect high RAM access
    ram_ext[addr++] = ram_int[0x93]; // indirect high RAM access
    ram_ext[addr++] = ram_int[0x94]; // indirect high RAM access
    ram_ext[addr++] = ram_int[0x95]; // indirect high RAM access
    ram_ext[addr] = ram_int[0x96]; // indirect high RAM access

    ram_int[0x86] |= 0x40; // select SFR bank 1
    ram_int[0xd8] |= 0x04; // set IEP2RDY (data is ready to be sent)
    ram_int[0x86] &= 0x3f; // select SFR bank 0

    asm("SETB EA"); // enable interrupts

    ram_int[0x90] = 0; // indirect high RAM access
    ram_int[0x91] = 0; // indirect high RAM access
    ram_int[0x92] = 0; // indirect high RAM access
    ram_int[0x93] = 0; // indirect high RAM access
    ram_int[0x94] = 0; // indirect high RAM access
    ram_int[0x95] = 0; // indirect high RAM access
    ram_int[0x96] = 0; // indirect high RAM access
  } else if(21.4) {
    asm("CLR 21.4");
    asm("CLR EA"); // disable interrupts

    ram_int[0x86] |= 0x40; // select SFR bank 1
    ram_int[0x9f] = 0x08; // set IEP2CNT to 8 bytes
    ram_int[0x86] &= 0x3f; // select SFR bank 0

    int addr = 0x0a70; // held in pair 3d:3e; address of EP2 IN buffer
    ram_ext[addr++] = 0x07;
    ram_ext[addr++] = ram_int[0x98]; // indirect high RAM access
    ram_ext[addr++] = ram_int[0x99]; // indirect high RAM access
    ram_ext[addr++] = ram_int[0x9a]; // indirect high RAM access
    ram_ext[addr++] = ram_int[0x9b]; // indirect high RAM access
    ram_ext[addr++] = ram_int[0x9c]; // indirect high RAM access
    ram_ext[addr++] = ram_int[0x9d]; // indirect high RAM access
    ram_ext[addr] = ram_int[0x9e]; // indirect high RAM access

    ram_int[0x86] |= 0x40; // select SFR bank 1
    ram_int[0xd8] |= 0x04; // set IEP2RDY (data is ready to be sent)
    ram_int[0x86] &= 0x3f; // select SFR bank 0

    asm("SETB EA"); // enable interrupts

    ram_int[0x98] = 0; // indirect high RAM access
    ram_int[0x99] = 0; // indirect high RAM access
    ram_int[0x9a] = 0; // indirect high RAM access
    ram_int[0x9b] = 0; // indirect high RAM access
    ram_int[0x9c] = 0; // indirect high RAM access
    ram_int[0x9d] = 0; // indirect high RAM access
    ram_int[0x9e] = 0; // indirect high RAM access
  } else if(21.2) {
    asm("CLR 21.2");
    asm("CLR EA"); // disable interrupts

    ram_int[0x86] |= 0x40; // select SFR bank 1
    ram_int[0x9f] = 0x03; // set IEP2CNT to 3 bytes
    ram_int[0x86] &= 0x3f; // select SFR bank 0

    int addr = 0x0a70; // held in pair 3d:3e; address of EP2 IN buffer
    ram_ext[addr++] = 0x02;
    ram_ext[addr++] = ram_int[0x8d]; // indirect high RAM access
    ram_ext[addr] = ram_int[0x8e]; // indirect high RAM access

    ram_int[0x86] |= 0x40; // select SFR bank 1
    ram_int[0xd8] |= 0x04; // set IEP2RDY (data is ready to be sent)
    ram_int[0x86] &= 0x3f; // select SFR bank 0

    asm("SETB EA"); // enable interrupts

    ram_int[0x8d] = 0; // indirect high RAM access
    ram_int[0x8e] = 0; // indirect high RAM access
  } else if(2b.5) {
    asm("CLR 2b.5");
    asm("CLR EA"); // disable interrupts

    ram_int[0x86] |= 0x40; // select SFR bank 1
    ram_int[0x9f] = 0x02; // set IEP2CNT to 2 bytes
    ram_int[0x86] &= 0x3f; // select SFR bank 0

    int addr = 0x0a70; // held in pair 3d:3e; address of EP2 IN buffer
    ram_ext[addr++] = 0x01;
    ram_ext[addr] = ram_ext[0x03a9];

    ram_int[0x86] |= 0x40; // select SFR bank 1
    ram_int[0xd8] |= 0x04; // set IEP2RDY (data is ready to be sent)
    ram_int[0x86] &= 0x3f; // select SFR bank 0

    asm("SETB EA"); // enable interrupts

    ram_ext[0x03aa] = 0;
    ram_int[0x03a9] = 0;
  }
  ram_int[0x86] &= 0x3f; // select SFR bank 0
}

// address 9f3f
void handle_ep1_in(void)
{
  ram_int[0x86] |= 0x40; // select SFR bank 1
  if(ram_int[0xc0].2) { // if EP1 IN is already ready
    ram_int[0x86] &= 0x3f; // select SFR bank 0
    return;
  }

  if(23.3) {
    ram_ext[0x02af] &= 0xf7;
    ram_ext[0x02af] &= 0x7f;
  }

  if(20.6) {
    asm("CLR 20.6");
    asm("CLR EA"); // disable interrupts

    ram_int[0x86] |= 0x40; // select SFR bank 1
    ram_int[0x9e] = 0x08; // set IEP1CNT to 8 bytes
    ram_int[0x86] &= 0x3f; // select SFR bank 0

    int addr = 0x0a20; // held in pair 3d:3e; address of EP1 IN buffer
    if(22.2) {
      ram_ext[addr++] = 0x00;
      ram_ext[addr++] = 0x00;
      ram_ext[addr++] = 0x00;
      ram_ext[addr++] = 0x00;
      ram_ext[addr++] = 0x00;
      ram_ext[addr++] = 0x00;
      ram_ext[addr++] = 0x00;
      ram_ext[addr] = 0x00;
    } else {
      ram_ext[addr++] = ram_ext[0x02af];
      ram_ext[addr++] = 0x00;
      ram_ext[addr++] = ram_ext[0x02b0];
      ram_ext[addr++] = ram_ext[0x02b1];
      ram_ext[addr++] = ram_ext[0x02b2];
      ram_ext[addr++] = ram_ext[0x02bc];
      ram_ext[addr++] = ram_ext[0x02bd];
      ram_ext[addr] = ram_ext[0x02be];
    }

    ram_ext[0x86] |= 0x40; // select SFR bank 1
    ram_ext[0xc0] |= 0x04; // set IEP1RDY (data is ready to be sent)
    ram_ext[0x86] &= 0x3f; // select SFR bank 0

    asm("SETB EA"); // enable interrupts
  }
  ram_ext[0x86] &= 0x3f; // select SFR bank 0
}

// address 18be
void sub_18be(void)
{
  ram_int[0x4c] = 0x01;

  // address 18c1
  if((ram_int[0x64] & ram_int[0x4c]) == 0 && // indirect internal RAM access
     ram_ext[0x0215] != 0 && --ram_ext[0x0215] == 0) {
    if(!20.7) {
      sub_ddf6(0x04, 0);
    } else {
      sub_ddf6(0x08, 0);
      sub_ddf6(0x0e, 3); 
   }
  }

  // address 18ee
  if((ram_int[0x65] & ram_int[0x4c]) == 0 && // indirect internal RAM access
     ram_ext[0x0216] != 0 && --ram_ext[0x0216] == 0) {
    if(!20.7) {
      sub_ddf6(0x08, 0);
    } else {
      sub_ddf6(0x14, 0);
      sub_ddf6(0x1c, 3);
    }
  }

  // address 191b
  if((ram_int[0x66] & ram_int[0x4c]) == 0 && // indirect internal RAM access
     ram_ext[0x0217] != 0 && --ram_ext[0x0217] == 0) {
    if(!20.7) {
      sub_ddf6(0x10, 0);
    } else {
      sub_ddf6(0x28, 0);
      sub_ddf6(0x38, 3);
    }
  }

  // address 1948
  if((ram_int[0x67] & ram_int[0x4c]) == 0 && // indirect internal RAM access
     ram_ext[0x0218] != 0 && --ram_ext[0x0218] == 0) {
    if(!20.7) {
      sub_ddf6(0x20, 0);
    } else {
      sub_ddf6(0x50, 0);
      sub_ddf6(0x70, 3);
    }
  }

  // address 1975
  if((ram_int[0x68] & ram_int[0x4c]) == 0 && // indirect internal RAM access
     ram_ext[0x0219] != 0 && --ram_ext[0x0219] == 0) {
    if(!20.7) {
      sub_ddf6(0x40, 0);
    } else {
      sub_ddf6(0x20, 0);
      sub_ddf6(0x01, 1);
      sub_ddf6(0xe0, 3);
    }
  }

  // address 19ab
  if((ram_int[0x69] & ram_int[0x4c]) == 0 && // indirect internal RAM access
     ram_ext[0x021a] != 0 && --ram_ext[0x021a] == 0) {
    sub_ddf6(0x80, 0);
  }

  ram_int[0x4b] = 0x04;

  // address 19c7
  if((ram_int[0x6a] & ram_int[0x4c]) == 0 && // indirect internal RAM access
     ram_ext[0x021b] != 0 && --ram_ext[0x021b] == 0) {
    if(!20.7) {
      sub_ddf6(0x01, 1);
    } else {
      sub_ddf6(0x02, 1);
      sub_ddf6(0x40, 0);
      sub_ddf6(0xc0, 3);
      sub_ddf6(0x01, 4);
    }
  }

  // address 1a07
  if((ram_int[0x6b] & ram_int[0x4c]) == 0 && // indirect internal RAM access
     ram_ext[0x021c] != 0 && --ram_ext[0x021c] == 0) {
    if(!20.7) {
      sub_ddf6(0x02, 1);
    } else {
      sub_ddf6(0x05, 1);
      sub_ddf6(0x03, 4);
    }
  }

  // address 1a35
  if((ram_int[0x6c] & ram_int[0x4c]) == 0 && // indirect internal RAM access
     ram_ext[0x021e] != 0 && --ram_ext[0x021e] == 0) {
    if(!20.7) {
      sub_ddf6(0x04, 1);
    } else {
      sub_ddf6(0x0a, 1);
      sub_ddf6(0x06, 4);
    }
  }

  // address 1a63
  if((ram_int[0x6d] & ram_int[0x4c]) == 0 && // indirect internal RAM access
     ram_ext[0x0220] != 0 && --ram_ext[0x0220] == 0) {
    if(!20.7) {
      sub_ddf6(0x08, 1);
    } else {
      sub_ddf6(0x14, 1);
      sub_ddf6(0x0c, 4);
    }
  }

  // address 1a91
  if((ram_int[0x6e] & ram_int[0x4c]) == 0 && // indirect internal RAM access
     ram_ext[0x0223] != 0 && --ram_ext[0x0223] == 0) {
    if(!20.7) {
      sub_ddf6(0x10, 1);
    } else {
      sub_ddf6(0x28, 1);
      sub_ddf6(0x38, 4);
    }
  }

  // address 1abf
  if((ram_int[0x6f] & ram_int[0x4c]) == 0 && // indirect internal RAM access
     ram_ext[0x0227] != 0 && --ram_ext[0x0227] == 0) {
    if(!20.7) {
      sub_ddf6(0x20, 1);
    } else {
      sub_ddf6(0x50, 1);
      sub_ddf6(0x20, 4);
    }
  }

  // address 1aed
  if((ram_int[0x70] & ram_int[0x4c]) == 0 && // indirect internal RAM access
     ram_ext[0x022c] != 0 && --ram_ext[0x022c] == 0) {
    if(!20.7) {
      sub_ddf6(0x40, 1);
    } else {
      sub_ddf6(0xa0, 1);
      sub_ddf6(0x60, 4);
    }
  }

  // address 1b1b
  if((ram_int[0x71] & ram_int[0x4c]) == 0 && // indirect internal RAM access
     ram_ext[0x0232] != 0 && --ram_ext[0x0232] == 0) {
    if(!20.7) {
      sub_ddf6(0x80, 1);
    } else {
      sub_ddf6(0x40, 1);
      sub_ddf6(0x01, 2);
      sub_ddf6(0xc0, 4);
      sub_ddf6(0x01, 5);
    }
  }

  ram_int[0x4b] = 0x05;

  // address 1b5e
  if((ram_int[0x72] & ram_int[0x4c]) == 0 && // indirect internal RAM access
     ram_ext[0x0239] != 0 && --ram_ext[0x0239] == 0) {
    if(!20.7) {
      sub_ddf6(0x01, 2);
    } else {
      sub_ddf6(0x02, 2);
      sub_ddf6(0x80, 1);
      sub_ddf6(0x03, 5);
    }
  }

  // address 1b95
  if((ram_int[0x73] & ram_int[0x4c]) == 0 && // indirect internal RAM access
     ram_ext[0x0241] != 0 && --ram_ext[0x0241] == 0) {
    if(!20.7) {
      sub_ddf6(0x02, 2);
    } else {
      sub_ddf6(0x05, 2);
      sub_ddf6(0x07, 5);
    }
  }

  // address 1bc3
  if((ram_int[0x74] & ram_int[0x4c]) == 0 && // indirect internal RAM access
     ram_ext[0x024a] != 0 && --ram_ext[0x024a] == 0) {
    if(!20.7) {
      sub_ddf6(0x04, 2);
    } else {
      sub_ddf6(0x0a, 2);
      sub_ddf6(0x0f, 5);
    }
  }

  // address 1bf1
  if((ram_int[0x75] & ram_int[0x4c]) == 0 && // indirect internal RAM access
     ram_ext[0x0253] != 0 && --ram_ext[0x0253] == 0) {
    if(!20.7) {
      sub_ddf6(0x08, 2);
    } else {
      sub_ddf6(0x14, 2);
      sub_ddf6(0x1c, 5);
    }
  }

  // address 1c1f
  if((ram_int[0x76] & ram_int[0x4c]) == 0 && // indirect internal RAM access
     ram_ext[0x021f] != 0 && --ram_ext[0x021f] == 0) {
    if(!20.7) {
      sub_ddf6(0x10, 2);
    } else {
      sub_ddf6(0x08, 2);
      sub_ddf6(0x38, 5);
    }
  }

  ram_int[0x4c] = 0x02;
  ram_int[0x4b] = 0x03;

  // address 1c53
  if((ram_int[0x63] & ram_int[0x4c]) == 0 && // indirect internal RAM access
     ram_ext[0x0238] != 0 && --ram_ext[0x0238] == 0) {
    if(!20.7) {
      sub_ddf6(0x02, 3);
    } else {
      sub_ddf6(0x04, 0);
      sub_ddf6(0x04, 3);
      sub_ddf6(0x02, 6);
    }
  }

  // address 1c8a
  if((ram_int[0x64] & ram_int[0x4c]) == 0 && // indirect internal RAM access
     ram_ext[0x0240] != 0 && --ram_ext[0x0240] == 0) {
    if(!20.7) {
      sub_ddf6(0x04, 3);
    } else {
      sub_ddf6(0x0c, 0);
      sub_ddf6(0x0a, 3);
      sub_ddf6(0x06, 6);
    }
  }

  // address 1cc1
  if((ram_int[0x65] & ram_int[0x4c]) == 0 && // indirect internal RAM access
     ram_ext[0x0249] != 0 && --ram_ext[0x0249] == 0) {
    if(!20.7) {
      sub_ddf6(0x08, 3);
    } else {
      sub_ddf6(0x18, 0);
      sub_ddf6(0x14, 3);
      sub_ddf6(0x0c, 6);
    }
  }

  // address 1cf8
  if((ram_int[0x66] & ram_int[0x4c]) == 0 && // indirect internal RAM access
     ram_ext[0x0252] != 0 && --ram_ext[0x0252] == 0) {
    if(!20.7) {
      sub_ddf6(0x10, 3);
    } else {
      sub_ddf6(0x18, 0);
      sub_ddf6(0x28, 3);
      sub_ddf6(0x18, 6);
    }
  }

  // address 1d2f
  if((ram_int[0x67] & ram_int[0x4c]) == 0 && // indirect internal RAM access
     ram_ext[0x025b] != 0 && --ram_ext[0x025b] == 0) {
    if(!20.7) {
      sub_ddf6(0x20, 3);
    } else {
      sub_ddf6(0x30, 0);
      sub_ddf6(0x50, 3);
      sub_ddf6(0x30, 6);
    }
  }

  // address 1d66
  if((ram_int[0x68] & ram_int[0x4c]) == 0 && // indirect internal RAM access
     ram_ext[0x0221] != 0 && --ram_ext[0x0221] == 0) {
    if(!20.7) {
      sub_ddf6(0x40, 3);
    } else {
      sub_ddf6(0x60, 0);
      sub_ddf6(0xa0, 3);
      sub_ddf6(0x60, 6);
    }
  }

  // address 1d9d
  if((ram_int[0x69] & ram_int[0x4c]) == 0 && // indirect internal RAM access
     ram_ext[0x0225] != 0 && --ram_ext[0x0225] == 0) {
    if(!20.7) {
      sub_ddf6(0x80, 3);
    } else {
      sub_ddf6(0x40, 0);
      sub_ddf6(0x01, 1);
      sub_ddf6(0x40, 3);
      sub_ddf6(0x01, 4);
      sub_ddf6(0xc0, 6);
    }
  }

  ram_int[0x4b] = 0x04;

  // address 1de9
  if((ram_int[0x6a] & ram_int[0x4c]) == 0 && // indirect internal RAM access
     ram_ext[0x022a] != 0 && --ram_ext[0x022a] == 0) {
    if(!20.7) {
      sub_ddf6(0x01, 4);
    } else {
      sub_ddf6(0x03, 1);
      sub_ddf6(0x80, 3);
      sub_ddf6(0x40, 3);
      sub_ddf6(0x02, 4);
      sub_ddf6(0x80, 6);
      sub_ddf6(0x01, 7);
    }
  }

  // address 1e32
  if((ram_int[0x6b] & ram_int[0x4c]) == 0 && // indirect internal RAM access
     ram_ext[0x0230] != 0 && --ram_ext[0x0230] == 0) {
    if(!20.7) {
      sub_ddf6(0x02, 4);
    } else {
      sub_ddf6(0x06, 1);
      sub_ddf6(0x05, 4);
      sub_ddf6(0x03, 7);
    }
  }

  // address 1e69
  if((ram_int[0x6c] & ram_int[0x4c]) == 0 && // indirect internal RAM access
     ram_ext[0x0237] != 0 && --ram_ext[0x0237] == 0) {
    if(!20.7) {
      sub_ddf6(0x04, 4);
    } else {
      sub_ddf6(0x0c, 1);
      sub_ddf6(0x0a, 4);
      sub_ddf6(0x06, 7);
    }
  }

  // address 1ea0
  if((ram_int[0x6d] & ram_int[0x4c]) == 0 && // indirect internal RAM access
     ram_ext[0x023f] != 0 && --ram_ext[0x023f] == 0) {
    if(!20.7) {
      sub_ddf6(0x08, 4);
    } else {
      sub_ddf6(0x18, 1);
      sub_ddf6(0x14, 4);
      sub_ddf6(0x0c, 7);
    }
  }

  // address 1ed7
  if((ram_int[0x6e] & ram_int[0x4c]) == 0 && // indirect internal RAM access
     ram_ext[0x0248] != 0 && --ram_ext[0x0248] == 0) {
    if(!20.7) {
      sub_ddf6(0x10, 4);
    } else {
      sub_ddf6(0x30, 1);
      sub_ddf6(0x28, 4);
      sub_ddf6(0x18, 7);
    }
  }

  // address 1f0e
  if((ram_int[0x6f] & ram_int[0x4c]) == 0 && // indirect internal RAM access
     ram_ext[0x0251] != 0 && --ram_ext[0x0251] == 0) {
    if(!20.7) {
      sub_ddf6(0x20, 4);
    } else {
      sub_ddf6(0x60, 1);
      sub_ddf6(0x50, 4);
      sub_ddf6(0x30, 7);
    }
  }

  // address 1f45
  if((ram_int[0x70] & ram_int[0x4c]) == 0 && // indirect internal RAM access
     ram_ext[0x025a] != 0 && --ram_ext[0x025a] == 0) {
    if(!20.7) {
      sub_ddf6(0x40, 4);
    } else {
      sub_ddf6(0xc0, 1);
      sub_ddf6(0x40, 7);
      sub_ddf6(0x01, 5);
      sub_ddf6(0x20, 7);
      sub_ddf6(0x20, 4);
    }
  }

  // addresss 1f8c
  if((ram_int[0x71] & ram_int[0x4c]) == 0 && // indirect internal RAM access
     ram_ext[0x0262] != 0 && --ram_ext[0x0262] == 0) {
    sub_ddf6(0x80, 4);
  }

  ram_int[0x4b] = 0x05;  

  // address 1fa9
  if((ram_int[0x72] & ram_int[0x4c]) == 0 && // indirect internal RAM access
     ram_ext[0x0224] != 0 && --ram_ext[0x0224] == 0) {
    if(!20.7) {
      sub_ddf6(0x01, 5);
    } else {
      sub_ddf6(0x80, 1);
      sub_ddf6(0x03, 2);
      sub_ddf6(0x40, 4);
      sub_ddf6(0x02, 5);
      sub_ddf6(0x03, 8);
    }
  }

  // address 1ff2
  if((ram_int[0x73] & ram_int[0x4c]) == 0 && // indirect internal RAM access
     ram_ext[0x0229] != 0 && --ram_ext[0x0229] == 0) {
    if(!20.7) {
      sub_ddf6(0x02, 5);
    } else {
      sub_ddf6(0x07, 2);
      sub_ddf6(0x05, 5);
      sub_ddf6(0x07, 8);
    }
  }

  // address 2029
  if((ram_int[0x74] & ram_int[0x4c]) == 0 && // indirect internal RAM access
     ram_ext[0x022f] != 0 && --ram_ext[0x022f] == 0) {
    if(!20.7) {
      sub_ddf6(0x04, 5);
    } else {
      sub_ddf6(0x0e, 2);
      sub_ddf6(0x0a, 5);
      sub_ddf6(0x0e, 8);
    }
  }

  // address 2060
  if((ram_int[0x75] & ram_int[0x4c]) == 0 && // indirect internal RAM access
     ram_ext[0x0236] != 0 && --ram_ext[0x0236] == 0) {
    if(!20.7) {
      sub_ddf6(0x08, 5);
    } else {
      sub_ddf6(0x1c, 2);
      sub_ddf6(0x14, 5);
      sub_ddf6(0x1c, 8);
    }
  }

  // address 2097
  if((ram_int[0x76] & ram_int[0x4c]) == 0 && // indirect internal RAM access
     ram_ext[0x023e] != 0 && --ram_ext[0x023e] == 0) {
    if(!20.7) {
      sub_ddf6(0x10, 5);
    } else {
      sub_ddf6(0x28, 5);
      sub_ddf6(0x38, 8);
    }
  }

  // address 20c5
  if((ram_int[0x77] & ram_int[0x4c]) == 0 && // indirect internal RAM access
     ram_ext[0x0247] != 0 && --ram_ext[0x0247] == 0) {
    if(!20.7) {
      sub_ddf6(0x20, 5);
    } else {
      sub_ddf6(0x50, 5);
      sub_ddf6(0x20, 8);
    }
  }

  // address 20f3
  if((ram_int[0x78] & ram_int[0x4c]) == 0 && // indirect internal RAM access
     ram_ext[0x0250] != 0 && --ram_ext[0x0250] == 0) {
    if(!20.7) {
      sub_ddf6(0x40, 5);
    } else {
      sub_ddf6(0x20, 5);
      sub_ddf6(0x60, 8);
    }
  }

  ram_int[0x4c] = 0x04;
  ram_int[0x4b] = 0x06;

  // address 2127
  if((ram_int[0x62] & ram_int[0x4c]) != 0 && // indirect internal RAM access
     ram_ext[0x0261] == 0 || --ram_ext[0x0261] == 0) {
    sub_ddf6(0x01, 6);
  }

  // address 2144
  if((ram_int[0x63] & ram_int[0x4c]) == 0 && // indirect internal RAM access
     ram_ext[0x0268] != 0 && --ram_ext[0x0268] == 0) {
    if(!20.7) {
      sub_ddf6(0x02, 6);
    } else {
      sub_ddf6(0x06, 3);
      sub_ddf6(0x04, 6);
      sub_ddf6(0x06, 9);
    }
  }

  // address 217b
  if((ram_int[0x64] & ram_int[0x4c]) == 0 && // indirect internal RAM access
     ram_ext[0x0228] != 0 && --ram_ext[0x0228] == 0) {
    if(!20.7) {
      sub_ddf6(0x04, 6);
    } else {
      sub_ddf6(0x0c, 3);
      sub_ddf6(0x0a, 6);
      sub_ddf6(0x06, 9);
    }
  }

  // address 21b2
  if((ram_int[0x65] & ram_int[0x4c]) == 0 && // indirect internal RAM access
     ram_ext[0x022e] != 0 && --ram_ext[0x022e] == 0) {
    if(!20.7) {
      sub_ddf6(0x08, 6);
    } else {
      sub_ddf6(0x18, 3);
      sub_ddf6(0x14, 6);
      sub_ddf6(0x0c, 9);
    }
  }

  // address 21e9
  if((ram_int[0x66] & ram_int[0x4c]) == 0 && // indirect internal RAM access
     ram_ext[0x0235] != 0 && --ram_ext[0x0235] == 0) {
    if(!20.7) {
      sub_ddf6(0x10, 6);
    } else {
      sub_ddf6(0x30, 3);
      sub_ddf6(0x28, 6);
      sub_ddf6(0x18, 9);
    }
  }

  // address 2220
  if((ram_int[0x67] & ram_int[0x4c]) == 0 && // indirect internal RAM access
     ram_ext[0x023d] != 0 && --ram_ext[0x023d] == 0) {
    if(!20.7) {
      sub_ddf6(0x20, 6);
    } else {
      sub_ddf6(0x60, 3);
      sub_ddf6(0x50, 6);
      sub_ddf6(0x30, 9);
    }
  }

  // address 2257
  if((ram_int[0x68] & ram_int[0x4c]) == 0 && // indirect internal RAM access
     ram_ext[0x0246] != 0 && --ram_ext[0x0246] == 0) {
    if(!20.7) {
      sub_ddf6(0x40, 6);
    } else {
      sub_ddf6(0xc0, 3);
      sub_ddf6(0xa0, 6);
      sub_ddf6(0x60, 9);
    }
  }

  // address 228e
  if((ram_int[0x69] & ram_int[0x4c]) == 0 && // indirect internal RAM access
     ram_ext[0x024f] != 0 && --ram_ext[0x024f] == 0) {
    if(!20.7) {
      sub_ddf6(0x80, 6);
    } else {
      sub_ddf6(0x80, 3);
      sub_ddf6(0x01, 4);
      sub_ddf6(0x40, 6);
      sub_ddf6(0x01, 7);
      sub_ddf6(0xc0, 9);
    }
  }

  ram_int[0x4b] = 0x07;

  // address 22d8
  if((ram_int[0x6a] & ram_int[0x4c]) == 0 && // indirect internal RAM access
     ram_ext[0x0258] != 0 && --ram_ext[0x0258] == 0) {
    if(!20.7) {
      sub_ddf6(0x01, 7);
    } else {
      sub_ddf6(0x03, 4);
      sub_ddf6(0x80, 6);
      sub_ddf6(0x02, 7);
      sub_ddf6(0x80, 9);
      sub_ddf6(0x01, 10);
    }
  }

  // address 2321
  if((ram_int[0x6b] & ram_int[0x4c]) == 0 && // indirect internal RAM access
     ram_ext[0x0260] != 0 && --ram_ext[0x0260] == 0) {
    if(!20.7) {
      sub_ddf6(0x02, 7);
    } else {
      sub_ddf6(0x06, 4);
      sub_ddf6(0x05, 7);
      sub_ddf6(0x03, 10);
    }
  }

  // address 2358
  if((ram_int[0x6c] & ram_int[0x4c]) == 0 && // indirect internal RAM access
     ram_ext[0x0267] != 0 && --ram_ext[0x0267] == 0) {
    if(!20.7) {
      sub_ddf6(0x04, 7);
    } else {
      sub_ddf6(0x0c, 4);
      sub_ddf6(0x0a, 7);
      sub_ddf6(0x06, 10);
    }
  }

  // address 238f
  if((ram_int[0x6d] & ram_int[0x4c]) == 0 && // indirect internal RAM access
     ram_ext[0x026d] != 0 && --ram_ext[0x026d] == 0) {
    if(!20.7) {
      sub_ddf6(0x08, 7);
    } else {
      sub_ddf6(0x18, 4);
      sub_ddf6(0x14, 7);
      sub_ddf6(0x0c, 10);
    }
  }

  // address 23c6
  if((ram_int[0x6e] & ram_int[0x4c]) == 0 && // indirect internal RAM access
     ram_ext[0x022d] != 0 && --ram_ext[0x022d] == 0) {
    if(!20.7) {
      sub_ddf6(0x10, 7);
    } else {
      sub_ddf6(0x30, 4);
      sub_ddf6(0x28, 7);
      sub_ddf6(0x18, 10);
    }
  }

  // address 23fd
  if((ram_int[0x6f] & ram_int[0x4c]) == 0 && // indirect internal RAM access
     ram_ext[0x0234] != 0 && --ram_ext[0x0234] == 0) {
    if(!20.7) {
      sub_ddf6(0x20, 7);
    } else {
      sub_ddf6(0x60, 4);
      sub_ddf6(0xd0, 7);
      sub_ddf6(0x50, 10);
    }
  }

  // address 2321
  if((ram_int[0x70] & ram_int[0x4c]) == 0 && // indirect internal RAM access
     ram_ext[0x023c] != 0 && --ram_ext[0x023c] == 0) {
    if(!20.7) {
      sub_ddf6(0x40, 7);
    } else {
      sub_ddf6(0x40, 4);
      sub_ddf6(0x20, 7);
      sub_ddf6(0x40, 10);
      sub_ddf6(0x01, 8);
    }
  }

  ram_int[0x4b] = 0x08;

  // address 2475
  if((ram_int[0x72] & ram_int[0x4c]) == 0 && // indirect internal RAM access
     ram_ext[0x0260] != 0 && --ram_ext[0x0260] == 0) {
    if(!20.7) {
      sub_ddf6(0x01, 8);
    } else {
      sub_ddf6(0x01, 5);
      sub_ddf6(0x02, 8);
      sub_ddf6(0x20, 10);
      sub_ddf6(0x40, 7);
    }
  }

  // address 24b3
  if((ram_int[0x73] & ram_int[0x4c]) == 0 && // indirect internal RAM access
     ram_ext[0x0257] != 0 && --ram_ext[0x0257] == 0) {
    if(!20.7) {
      sub_ddf6(0x02, 8);
    } else {
      sub_ddf6(0x07, 5);
      sub_ddf6(0x05, 8);
    }
  }

  // address 24df
  if((ram_int[0x74] & ram_int[0x4c]) == 0 && // indirect internal RAM access
     ram_ext[0x025f] != 0 && --ram_ext[0x025f] == 0) {
    if(!20.7) {
      sub_ddf6(0x04, 8);
    } else {
      sub_ddf6(0x0e, 5);
      sub_ddf6(0x0a, 8);
    }
  }

  // address 250b
  if((ram_int[0x75] & ram_int[0x4c]) == 0 && // indirect internal RAM access
     ram_ext[0x0266] != 0 && --ram_ext[0x0266] == 0) {
    if(!20.7) {
      sub_ddf6(0x08, 8);
    } else {
      sub_ddf6(0x1c, 5);
      sub_ddf6(0x14, 8);
      sub_ddf6(0x18, 11);
    }
  }

  // address 2542
  if((ram_int[0x76] & ram_int[0x4c]) == 0 && // indirect internal RAM access
     ram_ext[0x026c] != 0 && --ram_ext[0x026c] == 0) {
    if(!20.7) {
      sub_ddf6(0x10, 8);
    } else {
      sub_ddf6(0x38, 5);
      sub_ddf6(0x28, 8);
      sub_ddf6(0x38, 11);
    }
  }

  // address 2579
  if((ram_int[0x77] & ram_int[0x4c]) == 0 && // indirect internal RAM access
     ram_ext[0x0271] != 0 && --ram_ext[0x0271] == 0) {
    if(!20.7) {
      sub_ddf6(0x20, 8);
    } else {
      sub_ddf6(0x70, 5);
      sub_ddf6(0x50, 8);
      sub_ddf6(0x70, 11);
    }
  }

  // address 25b0
  if((ram_int[0x78] & ram_int[0x4c]) == 0 && // indirect internal RAM access
     ram_ext[0x0233] != 0 && --ram_ext[0x0233] == 0) {
    if(!20.7) {
      sub_ddf6(0x40, 8);
    } else {
      sub_ddf6(0x60, 5);
      sub_ddf6(0x20, 8);
      sub_ddf6(0x60, 11);
      sub_ddf6(0x40, 14);
    }
  }

  ram_int[0x4c] = 0x08;
  ram_int[0x4b] = 0x09;

  // address 25f6
  if((ram_int[0x63] & ram_int[0x4c]) == 0 && // indirect internal RAM access
     ram_ext[0x024d] != 0 && --ram_ext[0x024d] == 0) {
    if(!20.7) {
      sub_ddf6(0x02, 9);
    } else {
      sub_ddf6(0x06, 6);
      sub_ddf6(0x04, 9);
      sub_ddf6(0x02, 12);
    }
  }

  // address 262d
  if((ram_int[0x64] & ram_int[0x4c]) == 0 && // indirect internal RAM access
     ram_ext[0x0256] != 0 && --ram_ext[0x0256] == 0) {
    if(!20.7) {
      sub_ddf6(0x04, 9);
    } else {
      sub_ddf6(0x0c, 6);
      sub_ddf6(0x0a, 9);
      sub_ddf6(0x06, 12);
    }
  }

  // address 2664
  if((ram_int[0x65] & ram_int[0x4c]) == 0 && // indirect internal RAM access
     ram_ext[0x025e] != 0 && --ram_ext[0x025e] == 0) {
    if(!20.7) {
      sub_ddf6(0x08, 9);
    } else {
      sub_ddf6(0x18, 6);
      sub_ddf6(0x14, 9);
      sub_ddf6(0x0c, 12);
    }
  }

  // address 269b
  if((ram_int[0x66] & ram_int[0x4c]) == 0 && // indirect internal RAM access
     ram_ext[0x0265] != 0 && --ram_ext[0x0265] == 0) {
    if(!20.7) {
      sub_ddf6(0x10, 9);
    } else {
      sub_ddf6(0x30, 6);
      sub_ddf6(0x28, 9);
      sub_ddf6(0x18, 12);
    }
  }

  // address 262d
  if((ram_int[0x67] & ram_int[0x4c]) == 0 && // indirect internal RAM access
     ram_ext[0x026b] != 0 && --ram_ext[0x026b] == 0) {
    if(!20.7) {
      sub_ddf6(0x20, 9);
    } else {
      sub_ddf6(0x60, 6);
      sub_ddf6(0x50, 9);
      sub_ddf6(0x30, 12);
    }
  }

  // address 2709
  if((ram_int[0x68] & ram_int[0x4c]) == 0 && // indirect internal RAM access
     ram_ext[0x0270] != 0 && --ram_ext[0x0270] == 0) {
    if(!20.7) {
      sub_ddf6(0x40, 9);
    } else {
      sub_ddf6(0xc0, 6);
      sub_ddf6(0xa0, 9);
      sub_ddf6(0x60, 12);
    }
  }

  // address 2740
  if((ram_int[0x63] & ram_int[0x4c]) == 0 && // indirect internal RAM access
     ram_ext[0x024d] != 0 && --ram_ext[0x024d] == 0) {
    if(!20.7) {
      sub_ddf6(0x80, 9);
    } else {
      sub_ddf6(0x80, 6);
      sub_ddf6(0x01, 7);
      sub_ddf6(0x40, 9);
      sub_ddf6(0x01, 10);
      sub_ddf6(0xc0, 12);
    }
  }

  ram_int[0x4b] = 0x0a;

  // address 278a
  if((ram_int[0x6a] & ram_int[0x4c]) == 0 && // indirect internal RAM access
     ram_ext[0x023a] != 0 && --ram_ext[0x023a] == 0) {
    if(!20.7) {
      sub_ddf6(0x01, 10);
    } else {
      sub_ddf6(0x03, 7);
      sub_ddf6(0x80, 9);
      sub_ddf6(0x02, 10);
      sub_ddf6(0x80, 12);
      sub_ddf6(0x01, 13);
    }
  }

  // address 27d3
  if((ram_int[0x6b] & ram_int[0x4c]) == 0 && // indirect internal RAM access
     ram_ext[0x0243] != 0 && --ram_ext[0x0243] == 0) {
    if(!20.7) {
      sub_ddf6(0x02, 10);
    } else {
      sub_ddf6(0x06, 7);
      sub_ddf6(0x05, 10);
      sub_ddf6(0x03, 13);
    }
  }

  // address 280a
  if((ram_int[0x6c] & ram_int[0x4c]) == 0 && // indirect internal RAM access
     ram_ext[0x024c] != 0 && --ram_ext[0x024c] == 0) {
    if(!20.7) {
      sub_ddf6(0x04, 10);
    } else {
      sub_ddf6(0x0c, 7);
      sub_ddf6(0x0a, 10);
      sub_ddf6(0x06, 13);
    }
  }

  // address 2841
  if((ram_int[0x6d] & ram_int[0x4c]) == 0 && // indirect internal RAM access
     ram_ext[0x0255] != 0 && --ram_ext[0x0255] == 0) {
    if(!20.7) {
      sub_ddf6(0x08, 10);
    } else {
      sub_ddf6(0x18, 7);
      sub_ddf6(0x14, 10);
      sub_ddf6(0x0c, 13);
    }
  }

  // address 2878
  if((ram_int[0x6e] & ram_int[0x4c]) == 0 && // indirect internal RAM access
     ram_ext[0x025d] != 0 && --ram_ext[0x025d] == 0) {
    if(!20.7) {
      sub_ddf6(0x10, 10);
    } else {
      sub_ddf6(0x30, 7);
      sub_ddf6(0x48, 10);
      sub_ddf6(0x48, 13);
    }
  }

  // address 28af
  if((ram_int[0x6f] & ram_int[0x4c]) == 0 && // indirect internal RAM access
     ram_ext[0x0264] != 0 && --ram_ext[0x0264] == 0) {
    if(!20.7) {
      sub_ddf6(0x20, 10);
    } else {
      sub_ddf6(0x40, 4);
      sub_ddf6(0x20, 7);
      sub_ddf6(0x40, 10);
    }
  }

  // address 28e4
  if((ram_int[0x70] & ram_int[0x4c]) == 0 && // indirect internal RAM access
     ram_ext[0x026a] != 0 && --ram_ext[0x026a] == 0) {
    if(!20.7) {
      sub_ddf6(0x40, 10);
    } else {
      sub_ddf6(0xe0, 7);
      sub_ddf6(0x30, 10);
      sub_ddf6(0x40, 13);
    }
  }

  // address 291b
  if((ram_int[0x75] & ram_int[0x4c]) == 0 && // indirect internal RAM access
     ram_ext[0x024b] != 0 && --ram_ext[0x024b] == 0) {
    if(!20.7) {
      sub_ddf6(0x08, 11);
    } else {
      sub_ddf6(0x1c, 8);
      sub_ddf6(0x10, 11);
      sub_ddf6(0x18, 14);
    }
  }

  // address 2952
  if((ram_int[0x76] & ram_int[0x4c]) == 0 && // indirect internal RAM access
     ram_ext[0x0254] != 0 && --ram_ext[0x0254] == 0) {
    if(!20.7) {
      sub_ddf6(0x10, 11);
    } else {
      sub_ddf6(0x38, 8);
      sub_ddf6(0x28, 11);
      sub_ddf6(0x38, 14);
    }
  }

  // address 2989
  if((ram_int[0x77] & ram_int[0x4c]) == 0 && // indirect internal RAM access
     ram_ext[0x025c] != 0 && --ram_ext[0x025c] == 0) {
    if(!20.7) {
      sub_ddf6(0x20, 11);
    } else {
      sub_ddf6(0x70, 8);
      sub_ddf6(0x50, 11);
      sub_ddf6(0x70, 14);
    }
  }

  ram_int[0x4c] = 0x10;
  ram_int[0x4b] = 0x0c;
  
  // address 29c6
  if((ram_int[0x63] & ram_int[0x4c]) == 0 && // indirect internal RAM access
     ram_ext[0x0272] != 0 && --ram_ext[0x0272] == 0) {
    if(!20.7) {
      sub_ddf6(0x02, 12);
    } else {
      sub_ddf6(0x06, 9);
      sub_ddf6(0x04, 12);
      sub_ddf6(0x06, 15);
    }
  }

  // address 29fd
  if((ram_int[0x64] & ram_int[0x4c]) == 0 && // indirect internal RAM access
     ram_ext[0x0275] != 0 && --ram_ext[0x0275] == 0) {
    if(!20.7) {
      sub_ddf6(0x04, 12);
    } else {
      sub_ddf6(0x0c, 9);
      sub_ddf6(0x0a, 12);
      sub_ddf6(0x80, 16);
      sub_ddf6(0x04, 15);
    }
  }

  // address 2a3d
  if((ram_int[0x65] & ram_int[0x4c]) == 0 && // indirect internal RAM access
     ram_ext[0x0277] != 0 && --ram_ext[0x0277] == 0) {
    if(!20.7) {
      sub_ddf6(0x08, 12);
    } else {
      sub_ddf6(0x18, 9);
      sub_ddf6(0x14, 12);
      sub_ddf6(0x04, 15);
    }
  }

  // address 2a74
  if((ram_int[0x66] & ram_int[0x4c]) == 0 && // indirect internal RAM access
     ram_ext[0x0278] != 0 && --ram_ext[0x0278] == 0) {
    if(!20.7) {
      sub_ddf6(0x10, 12);
    } else {
      sub_ddf6(0x30, 9);
      sub_ddf6(0x28, 12);
      sub_ddf6(0x08, 15);
    }
  }

  // address 2aab
  if((ram_int[0x67] & ram_int[0x4c]) == 0 && // indirect internal RAM access
     ram_ext[0x027a] != 0 && --ram_ext[0x027a] == 0) {
    if(!20.7) {
      sub_ddf6(0x20, 12);
    } else {
      sub_ddf6(0x60, 9);
      sub_ddf6(0x50, 12);
    }
  }

  // address 2ad7
  if((ram_int[0x68] & ram_int[0x4c]) == 0 && // indirect internal RAM access
     ram_ext[0x01ec] != 0 && --ram_ext[0x01ec] == 0) {
    if(!20.7) {
      sub_ddf6(0x40, 12);
    } else {
      sub_ddf6(0xc0, 9);
      sub_ddf6(0xa0, 12);
      sub_ddf6(0x40, 15);
    }
  }

  // address 2b0e
  if((ram_int[0x69] & ram_int[0x4c]) == 0 && // indirect internal RAM access
     ram_ext[0x01f0] != 0 && --ram_ext[0x01f0] == 0) {
    if(!20.7) {
      sub_ddf6(0x80, 12);
    } else {
      sub_ddf6(0x80, 9);
      sub_ddf6(0x01, 10);
      sub_ddf6(0x40, 12);
      sub_ddf6(0x01, 13);
      sub_ddf6(0x40, 15);
    }
  }

  ram_int[0x4b] = 0x0d;

  // address 2b58
  if((ram_int[0x6a] & ram_int[0x4c]) == 0 && // indirect internal RAM access
     ram_ext[0x01f5] != 0 && --ram_ext[0x01f5] == 0) {
    if(!20.7) {
      sub_ddf6(0x01, 13);
    } else {
      sub_ddf6(0x03, 10);
      sub_ddf6(0x80, 12);
      sub_ddf6(0x02, 13);
    }
  }

  // address 2b8d
  if((ram_int[0x6b] & ram_int[0x4c]) == 0 && // indirect internal RAM access
     ram_ext[0x01fa] != 0 && --ram_ext[0x01fa] == 0) {
    if(!20.7) {
      sub_ddf6(0x02, 13);
    } else {
      sub_ddf6(0x06, 10);
      sub_ddf6(0x02, 16);
      sub_ddf6(0x05, 13);
    }
  }

  // address 2bc2
  if((ram_int[0x6c] & ram_int[0x4c]) == 0 && // indirect internal RAM access
     ram_ext[0x01ff] != 0 && --ram_ext[0x01ff] == 0) {
    if(!20.7) {
      sub_ddf6(0x04, 13);
    } else {
      sub_ddf6(0x0c, 10);
      sub_ddf6(0x0a, 13);
      sub_ddf6(0x06, 16);
    }
  }

  // address 2bf9
  if((ram_int[0x6d] & ram_int[0x4c]) == 0 && // indirect internal RAM access
     ram_ext[0x0204] != 0 && --ram_ext[0x0204] == 0) {
    if(!20.7) {
      sub_ddf6(0x08, 13);
    } else {
      sub_ddf6(0x18, 10);
      sub_ddf6(0x44, 13);
      sub_ddf6(0x0c, 16);
    }
  }

  // address 2c30
  if((ram_int[0x70] & ram_int[0x4c]) == 0 && // indirect internal RAM access
     ram_ext[0x0279] != 0 && --ram_ext[0x0279] == 0) {
    if(!20.7) {
      sub_ddf6(0x40, 13);
    } else {
      sub_ddf6(0x50, 10);
      sub_ddf6(0x08, 13);
      sub_ddf6(0x28, 16);
    }
  }

  // address 2c67
  if((ram_int[0x73] & ram_int[0x4c]) == 0 && // indirect internal RAM access
     ram_ext[0x01f4] != 0 && --ram_ext[0x01f4] == 0) {
    if(!20.7) {
      sub_ddf6(0x02, 14);
    } else {
      sub_ddf6(0x07, 17);
    }
  }

  // address 2c8c
  if((ram_int[0x75] & ram_int[0x4c]) == 0 && // indirect internal RAM access
     ram_ext[0x01fe] != 0 && --ram_ext[0x01fe] == 0) {
    if(!20.7) {
      sub_ddf6(0x08, 14);
    } else {
      sub_ddf6(0x18, 11);
      sub_ddf6(0x10, 14);
      sub_ddf6(0x1c, 17);
    }
  }

  // address 2cc3
  if((ram_int[0x76] & ram_int[0x4c]) == 0 && // indirect internal RAM access
     ram_ext[0x0203] != 0 && --ram_ext[0x0203] == 0) {
    if(!20.7) {
      sub_ddf6(0x10, 14);
    } else {
      sub_ddf6(0x38, 11);
      sub_ddf6(0x28, 14);
      sub_ddf6(0x38, 17);
    }
  }

  // address 2cfa
  if((ram_int[0x77] & ram_int[0x4c]) == 0 && // indirect internal RAM access
     ram_ext[0x0207] != 0 && --ram_ext[0x0207] == 0) {
    if(!20.7) {
      sub_ddf6(0x20, 14);
    } else {
      sub_ddf6(0x30, 11);
      sub_ddf6(0x50, 14);
      sub_ddf6(0x20, 17);
    }
  }

  // address 2d31
  if((ram_int[0x78] & ram_int[0x4c]) == 0 && // indirect internal RAM access
     ram_ext[0x020b] != 0 && --ram_ext[0x020b] == 0) {
    if(!20.7) {
      sub_ddf6(0x40, 14);
    } else {
      sub_ddf6(0x40, 8);
      sub_ddf6(0x20, 11);
      sub_ddf6(0x20, 14);
      sub_ddf6(0x20, 17);
    }
  }

  ram_int[0x4c] = 0x20;
  ram_int[0x4b] = 0x0f;

  // address 2d75
  if((ram_int[0x63] & ram_int[0x4c]) == 0 && // indirect internal RAM access
     ram_ext[0x01ee] != 0 && --ram_ext[0x01ee] == 0) {
    if(!20.7) {
      sub_ddf6(0x02, 15);
    } else {
      sub_ddf6(0x02, 12);
      sub_ddf6(0x80, 16);
    }
  }

  // address 2da1
  if((ram_int[0x64] & ram_int[0x4c]) == 0 && // indirect internal RAM access
     ram_ext[0x01f3] != 0 && --ram_ext[0x01f3] == 0) {
    if(!20.7) {
      sub_ddf6(0x04, 15);
    } else {
      sub_ddf6(0x12, 12);
      sub_ddf6(0x80, 16);
    }
  }


  // address 2d75
  if((ram_int[0x68] & ram_int[0x4c]) == 0 && // indirect internal RAM access
     ram_ext[0x0206] != 0 && --ram_ext[0x0206] == 0) {
    if(!20.7) {
      sub_ddf6(0x40, 15);
    } else {
      sub_ddf6(0xe0, 12);
    }
  }

  ram_int[0x4b] = 0x10;

  // address 2df7
  if((ram_int[0x6b] & ram_int[0x4c]) == 0 && // indirect internal RAM access
     ram_ext[0x0211] != 0 && --ram_ext[0x0211] == 0) {
    if(!20.7) {
      sub_ddf6(0x02, 16);
    } else {
      sub_ddf6(0x0e, 13);
      sub_ddf6(0x04, 16);
    }
  }

  // address 2e23
  if((ram_int[0x6c] & ram_int[0x4c]) == 0 && // indirect internal RAM access
     ram_ext[0x01ed] != 0 && --ram_ext[0x01ed] == 0) {
    if(!20.7) {
      sub_ddf6(0x04, 16);
    } else {
      sub_ddf6(0x08, 13);
      sub_ddf6(0x0a, 16);
    }
  }

  // address 2e4f
  if((ram_int[0x6d] & ram_int[0x4c]) == 0 && // indirect internal RAM access
     ram_ext[0x01f2] != 0 && --ram_ext[0x01f2] == 0) {
    if(!20.7) {
      sub_ddf6(0x08, 16);
    } else {
      sub_ddf6(0x48, 13);
      sub_ddf6(0x24, 16);
    }
  }

  // address 2e7b
  if((ram_int[0x6f] & ram_int[0x4c]) == 0 && // indirect internal RAM access
     ram_ext[0x01fc] != 0 && --ram_ext[0x01fc] == 0) {
    if(!20.7) {
      sub_ddf6(0x20, 16);
    } else {
      sub_ddf6(0x40, 13);
      sub_ddf6(0x08, 16);
      sub_ddf6(0x01, 17);
    }
  }

  // address 2eb2
  if((ram_int[0x71] & ram_int[0x4c]) == 0 && // indirect internal RAM access
     ram_ext[0x0205] != 0 && --ram_ext[0x0205] == 0) {
    if(!20.7) {
      sub_ddf6(0x80, 16);
    } else {
      sub_ddf6(0x06, 12);
      sub_ddf6(0x06, 15);
    }
  }

  ram_int[0x4b] = 0x11;

  // address 2ee3
  if((ram_int[0x72] & ram_int[0x4c]) == 0 && // indirect internal RAM access
     ram_ext[0x0209] != 0 && --ram_ext[0x0209] == 0) {
    if(!20.7) {
      sub_ddf6(0x01, 17);
    } else {
      sub_ddf6(0x20, 16);
      sub_ddf6(0x02, 17);
    }
  }

  // address 2f0f
  if((ram_int[0x73] & ram_int[0x4c]) == 0 && // indirect internal RAM access
     ram_ext[0x020d] != 0 && --ram_ext[0x020d] == 0) {
    if(!20.7) {
      sub_ddf6(0x02, 17);
    } else {
      sub_ddf6(0x02, 14);
      sub_ddf6(0x05, 17);
    }
  }

  // address 2f39
  if((ram_int[0x74] & ram_int[0x4c]) == 0 && // indirect internal RAM access
     ram_ext[0x0210] != 0 && --ram_ext[0x0210] == 0) {
    if(!20.7) {
      sub_ddf6(0x04, 17);
    } else {
      sub_ddf6(0x0a, 17);
    }
  }

  // address 2f5c
  if((ram_int[0x75] & ram_int[0x4c]) == 0 && // indirect internal RAM access
     ram_ext[0x0212] != 0 && --ram_ext[0x0212] == 0) {
    if(!20.7) {
      sub_ddf6(0x08, 17);
    } else {
      sub_ddf6(0x24, 17);
      sub_ddf6(0x18, 14);
    }
  }

  // address 2f8a
  if((ram_int[0x77] & ram_int[0x4c]) == 0 && // indirect internal RAM access
     ram_ext[0x01f6] != 0 && --ram_ext[0x01f6] == 0) {
    if(!20.7) {
      sub_ddf6(0x20, 17);
    } else {
      sub_ddf6(0x70, 14);
      sub_ddf6(0x08, 17);
    }
  }
}

// address ddf6
void sub_ddf6(uint8_t mask, uint8_t off)
{
  ram_int[0xc3+off] &= ~mask;
  ram_int[0xb1+off] &= ~mask;
  ram_int[0x9f+off] &= ~mask;
}

// address 005e
void sub_005e(void)
{
  // address 005e
  if(ram_int[0x64].0) { // indirect internal RAM access
    if(ram_ext[0x0215] == 0) {
      if(!20.7) {
        sub_a485(0x04, 0);
      } else {
        sub_a485(0x08, 0);
        sub_a485(0x0e, 3);
      }
    }
    ram_ext[0x0215] = ram_ext[0x021d];
  }

  // address 008d
  if(ram_int[0x65].0) { // indirect internal RAM access
    if(ram_ext[0x0216] == 0) {
      if(!20.7) {
        sub_a485(0x08, 0);
      } else {
        sub_a485(0x14, 0);
        sub_a485(0x1c, 3);
      }
    }
    ram_ext[0x0216] = ram_ext[0x021d];
  }

  // address 00bc
  if(ram_int[0x66].0) { // indirect internal RAM access
    if(ram_ext[0x0217] == 0) {
      if(!20.7) {
        sub_a485(0x10, 0);
      } else {
        sub_a485(0x28, 0);
        sub_a485(0x38, 3);
      }
    }
    ram_ext[0x0217] = ram_ext[0x021d];
  }

  // address 00eb
  if(ram_int[0x67].0) { // indirect internal RAM access
    if(ram_ext[0x0218] == 0) {
      if(!20.7) {
        sub_a485(0x20, 0);
      } else {
        sub_a485(0x50, 0);
        sub_a485(0x70, 3);
      }
    }
    ram_ext[0x0218] = ram_ext[0x021d];
  }

  // address 011a
  if(ram_int[0x68].0) { // indirect internal RAM access
    if(ram_ext[0x0219] == 0) {
      if(!20.7) {
        sub_a485(0x40, 0);
      } else {
        sub_a485(0x20, 0);
        sub_a485(0x01, 1);
        sub_a485(0xe0, 3);
      }
    }
    ram_ext[0x0219] = ram_ext[0x021d];
  }

  // address 0152
  if(ram_int[0x69].0) { // indirect internal RAM access
    if(ram_ext[0x021a] == 0) {
      sub_a485(0x80, 0);
    }
    ram_ext[0x021a] = ram_ext[0x021d];
  }

  // address 016d
  if(ram_int[0x6a].0) { // indirect internal RAM access
    if(ram_ext[0x021b] == 0) {
      if(!20.7) {
        sub_a485(0x01, 1);
      } else {
        sub_a485(0x02, 1);
        sub_a485(0x40, 0);
        sub_a485(0xc0, 3);
        sub_a485(0x01, 4);
      }
    }
    ram_ext[0x021b] = ram_ext[0x021d];
  }

  // address 01af
  if(ram_int[0x6b].0) { // indirect internal RAM access
    if(ram_ext[0x021c] == 0) {
      if(!20.7) {
        sub_a485(0x02, 1);
      } else {
        sub_a485(0x05, 1);
        sub_a485(0x03, 4);
      }
    }
    ram_ext[0x021c] = ram_ext[0x021d];
  }

  // address 01df
  if(ram_int[0x6c].0) { // indirect internal RAM access
    if(ram_ext[0x021e] == 0) {
      if(!20.7) {
        sub_a485(0x04, 1);
      } else {
        sub_a485(0x0a, 1);
        sub_a485(0x06, 4);
      }
    }
    ram_ext[0x021e] = ram_ext[0x021d];
  }

  // address 020d
  if(ram_int[0x6d].0) { // indirect internal RAM access
    if(ram_ext[0x0220] == 0) {
      if(!20.7) {
        sub_a485(0x08, 1);
      } else {
        sub_a485(0x14, 1);
        sub_a485(0x0c, 4);
      }
    }
    ram_ext[0x0220] = ram_ext[0x021d];
  }

  // address 023d
  if(ram_int[0x6e].0) { // indirect internal RAM access
    if(ram_ext[0x0223] == 0) {
      if(!20.7) {
        sub_a485(0x10, 1);
      } else {
        sub_a485(0x28, 1);
        sub_a485(0x38, 4);
      }
    }
    ram_ext[0x0223] = ram_ext[0x021d];
  }

  // address 026d
  if(ram_int[0x6f].0) { // indirect internal RAM access
    if(ram_ext[0x0227] == 0) {
      if(!20.7) {
        sub_a485(0x20, 1);
      } else {
        sub_a485(0x50, 1);
        sub_a485(0x20, 4);
      }
    }
    ram_ext[0x0227] = ram_ext[0x021d];
  }

  // address 029d
  if(ram_int[0x70].0) { // indirect internal RAM access
    if(ram_ext[0x022c] == 0) {
      if(!20.7) {
        sub_a485(0x40, 1);
      } else {
        sub_a485(0xa0, 1);
        sub_a485(0x60, 4);
      }
    }
    ram_ext[0x022c] = ram_ext[0x021d];
  }

  // address 02cd
  if(ram_int[0x71].0) { // indirect internal RAM access
    if(ram_ext[0x0232] == 0) {
      if(!20.7) {
        sub_a485(0x80, 1);
      } else {
        sub_a485(0x40, 1);
        sub_a485(0x01, 2);
        sub_a485(0xc0, 4);
        sub_a485(0x01, 5);
      }
    }
    ram_ext[0x0232] = ram_ext[0x021d];
  }

  // address 030f
  if(ram_int[0x72].0) { // indirect internal RAM access
    if(ram_ext[0x0239] == 0) {
      if(!20.7) {
        sub_a485(0x01, 2);
      } else {
        sub_a485(0x02, 2);
        sub_a485(0x80, 1);
        sub_a485(0x03, 5);
      }
    }
    ram_ext[0x0239] = ram_ext[0x021d];
  }

  // address 0348
  if(ram_int[0x73].0) { // indirect internal RAM access
    if(ram_ext[0x0241] == 0) {
      if(!20.7) {
        sub_a485(0x02, 2);
      } else {
        sub_a485(0x05, 2);
        sub_a485(0x07, 5);
      }
    }
    ram_ext[0x0241] = ram_ext[0x021d];
  }

  // address 0378
  if(ram_int[0x74].0) { // indirect internal RAM access
    if(ram_ext[0x024a] == 0) {
      if(!20.7) {
        sub_a485(0x04, 2);
      } else {
        sub_a485(0x0a, 2);
        sub_a485(0x0e, 5);
      }
    }
    ram_ext[0x024a] = ram_ext[0x021d];
  }

  // address 03a8
  if(ram_int[0x75].0) { // indirect internal RAM access
    if(ram_ext[0x0253] == 0) {
      if(!20.7) {
        sub_a485(0x08, 2);
      } else {
        sub_a485(0x14, 2);
        sub_a485(0x1c, 5);
      }
    }
    ram_ext[0x0253] = ram_ext[0x021d];
  }

  // address 03d8
  if(ram_int[0x76].0) { // indirect internal RAM access
    if(ram_ext[0x021f] == 0) {
      if(!20.7) {
        sub_a485(0x10, 2);
      } else {
        sub_a485(0x08, 2);
        sub_a485(0x38, 5);
      }
    }
    ram_ext[0x021f] = ram_ext[0x021d];
  }

  ram_int[0x4b] = 0x02;
  ram_int[0x4c] = 0x03;

  // address 040e
  if(ram_int[0x63] & 0x4b != 0) { // indirect internal RAM access
    if(ram_ext[0x0238] == 0) {
      if(!20.7) {
        sub_a485(0x02, 3);
      } else {
        sub_a485(0x04, 0);
        sub_a485(0x04, 3);
        sub_a485(0x02, 6);
      }
    }
    ram_ext[0x0238] = ram_ext[0x021d];
  }

  // address 0448
  if(ram_int[0x64] & 0x4b != 0) { // indirect internal RAM access
    if(ram_ext[0x0240] == 0) {
      if(!20.7) {
        sub_a485(0x04, 3);
      } else {
        sub_a485(0x0c, 0);
        sub_a485(0x0a, 3);
        sub_a485(0x06, 6);
      }
    }
    ram_ext[0x0240] = ram_ext[0x021d];
  }

  // address 0482
  if(ram_int[0x65] & 0x4b != 0) { // indirect internal RAM access
    if(ram_ext[0x0249] == 0) {
      if(!20.7) {
        sub_a485(0x08, 3);
      } else {
        sub_a485(0x18, 0);
        sub_a485(0x14, 3);
        sub_a485(0x0c, 6);
      }
    }
    ram_ext[0x0249] = ram_ext[0x021d];
  }

  // address 04f6
  if(ram_int[0x67] & 0x4b != 0) { // indirect internal RAM access
    if(ram_ext[0x025b] == 0) {
      if(!20.7) {
        sub_a485(0x20, 3);
      } else {
        sub_a485(0x30, 0);
        sub_a485(0x50, 3);
        sub_a485(0x30, 6);
      }
    }
    ram_ext[0x025b] = ram_ext[0x021d];
  }

  // address 0530
  if(ram_int[0x68] & 0x4b != 0) { // indirect internal RAM access
    if(ram_ext[0x0221] == 0) {
      if(!20.7) {
        sub_a485(0x40, 3);
      } else {
        sub_a485(0x60, 0);
        sub_a485(0xa0, 3);
        sub_a485(0x60, 6);
      }
    }
    ram_ext[0x0221] = ram_ext[0x021d];
  }

  // address 056a
  if(ram_int[0x69] & 0x4b != 0) { // indirect internal RAM access
    if(ram_ext[0x0225] == 0) {
      if(!20.7) {
        sub_a485(0x80, 3);
      } else {
        sub_a485(0x40, 0);
        sub_a485(0x01, 1);
        sub_a485(0x40, 3);
        sub_a485(0x01, 4);
        sub_a485(0xc0, 6);
      }
    }
    ram_ext[0x0225] = ram_ext[0x021d];
  }

  ram_int[0x4c] = 0x04;

  // address 05b9
  if(ram_int[0x6a] & 0x4b != 0) { // indirect internal RAM access
    if(ram_ext[0x022a] == 0) {
      if(!20.7) {
        sub_a485(0x01, 4);
      } else {
        sub_a485(0x03, 1);
        sub_a485(0x80, 3);
        sub_a485(0x02, 4);
        sub_a485(0x80, 6);
        sub_a485(0x01, 7);
      }
    }
    ram_ext[0x022a] = ram_ext[0x021d];
  }

  // address 0605
  if(ram_int[0x6b] & 0x4b != 0) { // indirect internal RAM access
    if(ram_ext[0x0230] == 0) {
      if(!20.7) {
        sub_a485(0x02, 4);
      } else {
        sub_a485(0x06, 1);
        sub_a485(0x05, 4);
        sub_a485(0x03, 7);
      }
    }
    ram_ext[0x0230] = ram_ext[0x021d];
  }

  // address 0605
  if(ram_int[0x6b] & 0x4b != 0) { // indirect internal RAM access
    if(ram_ext[0x0230] == 0) {
      if(!20.7) {
        sub_a485(0x02, 4);
      } else {
        sub_a485(0x06, 1);
        sub_a485(0x05, 4);
        sub_a485(0x03, 7);
      }
    }
    ram_ext[0x0230] = ram_ext[0x021d];
  }

  // address 063f
  if(ram_int[0x6c] & 0x4b != 0) { // indirect internal RAM access
    if(ram_ext[0x0237] == 0) {
      if(!20.7) {
        sub_a485(0x04, 4);
      } else {
        sub_a485(0x0c, 1);
        sub_a485(0x0a, 4);
        sub_a485(0x06, 7);
      }
    }
    ram_ext[0x0237] = ram_ext[0x021d];
  }

  // address 0679
  if(ram_int[0x6d] & 0x4b != 0) { // indirect internal RAM access
    if(ram_ext[0x023f] == 0) {
      if(!20.7) {
        sub_a485(0x08, 4);
      } else {
        sub_a485(0x18, 1);
        sub_a485(0x14, 4);
        sub_a485(0x0c, 7);
      }
    }
    ram_ext[0x023f] = ram_ext[0x021d];
  }

  // address 06b3
  if(ram_int[0x6e] & 0x4b != 0) { // indirect internal RAM access
    if(ram_ext[0x0248] == 0) {
      if(!20.7) {
        sub_a485(0x10, 4);
      } else {
        sub_a485(0x30, 1);
        sub_a485(0x28, 4);
        sub_a485(0x18, 7);
      }
    }
    ram_ext[0x0248] = ram_ext[0x021d];
  }

  // address 06ed
  if(ram_int[0x6f] & 0x4b != 0) { // indirect internal RAM access
    if(ram_ext[0x0251] == 0) {
      if(!20.7) {
        sub_a485(0x20, 4);
      } else {
        sub_a485(0x60, 1);
        sub_a485(0x50, 4);
        sub_a485(0x30, 7);
      }
    }
    ram_ext[0x0251] = ram_ext[0x021d];
  }

  // address 0727
  if(ram_int[0x70] & 0x4b != 0) { // indirect internal RAM access
    if(ram_ext[0x025a] == 0) {
      if(!20.7) {
        sub_a485(0x40, 4);
      } else {
        sub_a485(0xc0, 1);
        sub_a485(0x40, 7);
        sub_a485(0x01, 5);
        sub_a485(0x20, 7);
        sub_a485(0x20, 4);
      }
    }
    ram_ext[0x025a] = ram_ext[0x021d];
  }

  // address 0771
  if(ram_int[0x71] & 0x4b != 0) { // indirect internal RAM access
    if(ram_ext[0x0262] == 0) {
      if(!20.7) {
        sub_a485(0x80, 4);
      }
    }
    ram_ext[0x0262] = ram_ext[0x021d];
  }

  ram_int[0x4c] = 0x05;

  // address 0794
  if(ram_int[0x72] & 0x4b != 0) { // indirect internal RAM access
    if(ram_ext[0x0224] == 0) {
      if(!20.7) {
        sub_a485(0x01, 5);
      } else {
        sub_a485(0x80, 1);
        sub_a485(0x03, 2);
        sub_a485(0x40, 4);
        sub_a485(0x02, 5);
        sub_a485(0x03, 8);
      }
    }
    ram_ext[0x0224] = ram_ext[0x021d];
  }

  // address 07e0
  if(ram_int[0x73] & 0x4b != 0) { // indirect internal RAM access
    if(ram_ext[0x0229] == 0) {
      if(!20.7) {
        sub_a485(0x02, 5);
      } else {
        sub_a485(0x07, 2);
        sub_a485(0x05, 5);
        sub_a485(0x07, 8);
      }
    }
    ram_ext[0x0229] = ram_ext[0x021d];
  }

  // address 081a
  if(ram_int[0x74] & 0x4b != 0) { // indirect internal RAM access
    if(ram_ext[0x022f] == 0) {
      if(!20.7) {
        sub_a485(0x04, 5);
      } else {
        sub_a485(0x0e, 2);
        sub_a485(0x0a, 5);
        sub_a485(0x0e, 8);
      }
    }
    ram_ext[0x022f] = ram_ext[0x021d];
  }

  // address 0854
  if(ram_int[0x75] & 0x4b != 0) { // indirect internal RAM access
    if(ram_ext[0x0236] == 0) {
      if(!20.7) {
        sub_a485(0x08, 5);
      } else {
        sub_a485(0x1c, 2);
        sub_a485(0x14, 5);
        sub_a485(0x1c, 8);
      }
    }
    ram_ext[0x0236] = ram_ext[0x021d];
  }

  // address 088e
  if(ram_int[0x76] & 0x4b != 0) { // indirect internal RAM access
    if(ram_ext[0x023e] == 0) {
      if(!20.7) {
        sub_a485(0x10, 5);
      } else {
        sub_a485(0x28, 5);
        sub_a485(0x38, 8);
      }
    }
    ram_ext[0x023e] = ram_ext[0x021d];
  }

  // address 08bf
  if(ram_int[0x77] & 0x4b != 0) { // indirect internal RAM access
    if(ram_ext[0x0247] == 0) {
      if(!20.7) {
        sub_a485(0x20, 5);
      } else {
        sub_a485(0x50, 5);
        sub_a485(0x20, 8);
      }
    }
    ram_ext[0x0247] = ram_ext[0x021d];
  }

  // address 08f0
  if(ram_int[0x78] & 0x4b != 0) { // indirect internal RAM access
    if(ram_ext[0x0250] == 0) {
      if(!20.7) {
        sub_a485(0x40, 5);
      } else {
        sub_a485(0x20, 5);
        sub_a485(0x60, 8);
      }
    }
    ram_ext[0x0250] = ram_ext[0x021d];
  }

  ram_int[0x4b] = 0x04;
  ram_int[0x4c] = 0x06;

  // address 0927
  if(ram_int[0x62] & 0x4b != 0) { // indirect internal RAM access
    if(ram_ext[0x0261] == 0) {
      if(!20.7) {
        sub_a485(0x01, 6);
      }
    }
    ram_ext[0x0261] = ram_ext[0x021d];
  }

  // address 0947
  if(ram_int[0x63] & 0x4b != 0) { // indirect internal RAM access
    if(ram_ext[0x0268] == 0) {
      if(!20.7) {
        sub_a485(0x02, 6);
      } else {
        sub_a485(0x06, 3);
        sub_a485(0x04, 6);
        sub_a485(0x06, 9);
      }
    }
    ram_ext[0x0268] = ram_ext[0x021d];
  }

  // address 0981
  if(ram_int[0x64] & 0x4b != 0) { // indirect internal RAM access
    if(ram_ext[0x0228] == 0) {
      if(!20.7) {
        sub_a485(0x04, 6);
      } else {
        sub_a485(0x0c, 3);
        sub_a485(0x0a, 6);
        sub_a485(0x06, 9);
      }
    }
    ram_ext[0x0228] = ram_ext[0x021d];
  }

  // address 09bb
  if(ram_int[0x65] & 0x4b != 0) { // indirect internal RAM access
    if(ram_ext[0x022e] == 0) {
      if(!20.7) {
        sub_a485(0x08, 6);
      } else {
        sub_a485(0x18, 3);
        sub_a485(0x14, 6);
        sub_a485(0x0c, 9);
      }
    }
    ram_ext[0x022e] = ram_ext[0x021d];
  }

  // address 09f5
  if(ram_int[0x66] & 0x4b != 0) { // indirect internal RAM access
    if(ram_ext[0x0235] == 0) {
      if(!20.7) {
        sub_a485(0x10, 6);
      } else {
        sub_a485(0x30, 3);
        sub_a485(0x28, 6);
        sub_a485(0x18, 9);
      }
    }
    ram_ext[0x0235] = ram_ext[0x021d];
  }

  // address 0a2f
  if(ram_int[0x67] & 0x4b != 0) { // indirect internal RAM access
    if(ram_ext[0x023d] == 0) {
      if(!20.7) {
        sub_a485(0x20, 6);
      } else {
        sub_a485(0x60, 3);
        sub_a485(0x50, 6);
        sub_a485(0x30, 9);
      }
    }
    ram_ext[0x023d] = ram_ext[0x021d];
  }

  // address 0a69
  if(ram_int[0x68] & 0x4b != 0) { // indirect internal RAM access
    if(ram_ext[0x0246] == 0) {
      if(!20.7) {
        sub_a485(0x40, 6);
      } else {
        sub_a485(0xc0, 3);
        sub_a485(0xa0, 6);
        sub_a485(0x60, 9);
      }
    }
    ram_ext[0x0246] = ram_ext[0x021d];
  }

  // address 0aa3
  if(ram_int[0x69] & 0x4b != 0) { // indirect internal RAM access
    if(ram_ext[0x024f] == 0) {
      if(!20.7) {
        sub_a485(0x80, 6);
      } else {
        sub_a485(0x80, 3);
        sub_a485(0x01, 4);
        sub_a485(0x40, 6);
        sub_a485(0x01, 7);
        sub_a485(0xc0, 9);
      }
    }
    ram_ext[0x024f] = ram_ext[0x021d];
  }

  ram_int[0x4c] = 0x07;

  // address 0af0
  if(ram_int[0x6a] & 0x4b != 0) { // indirect internal RAM access
    if(ram_ext[0x0258] == 0) {
      if(!20.7) {
        sub_a485(0x01, 7);
      } else {
        sub_a485(0x03, 4);
        sub_a485(0x80, 6);
        sub_a485(0x02, 7);
        sub_a485(0x80, 9);
        sub_a485(0x01, 10);
      }
    }
    ram_ext[0x0258] = ram_ext[0x021d];
  }

  // address 0b3c
  if(ram_int[0x6b] & 0x4b != 0) { // indirect internal RAM access
    if(ram_ext[0x0260] == 0) {
      if(!20.7) {
        sub_a485(0x02, 7);
      } else {
        sub_a485(0x06, 4);
        sub_a485(0x05, 7);
        sub_a485(0x03, 10);
      }
    }
    ram_ext[0x0260] = ram_ext[0x021d];
  }

  // address 0b76
  if(ram_int[0x6c] & 0x4b != 0) { // indirect internal RAM access
    if(ram_ext[0x0267] == 0) {
      if(!20.7) {
        sub_a485(0x04, 7);
      } else {
        sub_a485(0x0c, 4);
        sub_a485(0x0a, 7);
        sub_a485(0x06, 10);
      }
    }
    ram_ext[0x0267] = ram_ext[0x021d];
  }

  // address 0bb0
  if(ram_int[0x6d] & 0x4b != 0) { // indirect internal RAM access
    if(ram_ext[0x026d] == 0) {
      if(!20.7) {
        sub_a485(0x08, 7);
      } else {
        sub_a485(0x18, 4);
        sub_a485(0x14, 7);
        sub_a485(0x0c, 10);
      }
    }
    ram_ext[0x026d] = ram_ext[0x021d];
  }

  // address 0bea
  if(ram_int[0x6e] & 0x4b != 0) { // indirect internal RAM access
    if(ram_ext[0x022d] == 0) {
      if(!20.7) {
        sub_a485(0x10, 7);
      } else {
        sub_a485(0x30, 4);
        sub_a485(0x28, 7);
        sub_a485(0x18, 10);
      }
    }
    ram_ext[0x022d] = ram_ext[0x021d];
  }

  // address 0c24
  if(ram_int[0x6f] & 0x4b != 0) { // indirect internal RAM access
    if(ram_ext[0x0234] == 0) {
      if(!20.7) {
        sub_a485(0x20, 7);
      } else {
        sub_a485(0x60, 4);
        sub_a485(0xd0, 7);
        sub_a485(0x50, 10);
      }
    }
    ram_ext[0x0234] = ram_ext[0x021d];
  }

  // address 0c5e
  if(ram_int[0x70] & 0x4b != 0) { // indirect internal RAM access
    if(ram_ext[0x023c] == 0) {
      if(!20.7) {
        sub_a485(0x40, 7);
      } else {
        sub_a485(0x40, 4);
        sub_a485(0x20, 7);
        sub_a485(0x40, 10);
        sub_a485(0x01, 8);
      }
    }
    ram_ext[0x023c] = ram_ext[0x021d];
  }

  // address 0c9f
  if(ram_int[0x71] & 0x4b != 0) { // indirect internal RAM access
    if(ram_ext[0x0245] == 0) {
      if(!20.7) {
        sub_a485(0x20, 10);
      } else {
        sub_a485(0x40, 4);
        sub_a485(0x20, 7);
        sub_a485(0x40, 10);
      }
    }
    ram_ext[0x0245] = ram_ext[0x021d];
  }

  ram_int[0x4c] = 0x08;

  // address 0cda
  if(ram_int[0x72] & 0x4b != 0) { // indirect internal RAM access
    if(ram_ext[0x024e] == 0) {
      if(!20.7) {
        sub_a485(0x01, 8);
      } else {
        sub_a485(0x01, 5);
        sub_a485(0x02, 8);
        sub_a485(0x20, 10);
        sub_a485(0x40, 7);
      }
    }
    ram_ext[0x024e] = ram_ext[0x021d];
  }

  // address 0d1b
  if(ram_int[0x73] & 0x4b != 0) { // indirect internal RAM access
    if(ram_ext[0x0257] == 0) {
      if(!20.7) {
        sub_a485(0x02, 8);
      } else {
        sub_a485(0x07, 5);
        sub_a485(0x05, 8);
      }
    }
    ram_ext[0x0257] = ram_ext[0x021d];
  }

  // address 0d4a
  if(ram_int[0x74] & 0x4b != 0) { // indirect internal RAM access
    if(ram_ext[0x025f] == 0) {
      if(!20.7) {
        sub_a485(0x04, 8);
      } else {
        sub_a485(0x0e, 5);
        sub_a485(0x0a, 8);
      }
    }
    ram_ext[0x025f] = ram_ext[0x021d];
  }

  // address 0d79
  if(ram_int[0x75] & 0x4b != 0) { // indirect internal RAM access
    if(ram_ext[0x0266] == 0) {
      if(!20.7) {
        sub_a485(0x08, 8);
      } else {
        sub_a485(0x1c, 5);
        sub_a485(0x14, 8);
        sub_a485(0x18, 11);
      }
    }
    ram_ext[0x0266] = ram_ext[0x021d];
  }

  // address 0db3
  if(ram_int[0x76] & 0x4b != 0) { // indirect internal RAM access
    if(ram_ext[0x026c] == 0) {
      if(!20.7) {
        sub_a485(0x10, 8);
      } else {
        sub_a485(0x38, 5);
        sub_a485(0x28, 8);
        sub_a485(0x38, 11);
      }
    }
    ram_ext[0x026c] = ram_ext[0x021d];
  }

  // address 0ded
  if(ram_int[0x77] & 0x4b != 0) { // indirect internal RAM access
    if(ram_ext[0x0271] == 0) {
      if(!20.7) {
        sub_a485(0x20, 8);
      } else {
        sub_a485(0x70, 5);
        sub_a485(0x50, 8);
        sub_a485(0x70, 11);
      }
    }
    ram_ext[0x0271] = ram_ext[0x021d];
  }

  // address 0e27
  if(ram_int[0x78] & 0x4b != 0) { // indirect internal RAM access
    if(ram_ext[0x0233] == 0) {
      if(!20.7) {
        sub_a485(0x40, 8);
      } else {
        sub_a485(0x60, 5);
        sub_a485(0x20, 8);
        sub_a485(0x60, 11);
        sub_a485(0x40, 14);
      }
    }
    ram_ext[0x0233] = ram_ext[0x021d];
  }

  ram_int[0x4b] = 0x08;
  ram_int[0x4c] = 0x09;

  // address 0e70
  if(ram_int[0x63] & 0x4b != 0) { // indirect internal RAM access
    if(ram_ext[0x024d] == 0) {
      if(!20.7) {
        sub_a485(0x02, 9);
      } else {
        sub_a485(0x06, 6);
        sub_a485(0x04, 9);
        sub_a485(0x02, 12);
      }
    }
    ram_ext[0x024d] = ram_ext[0x021d];
  }

  // address 0eaa
  if(ram_int[0x64] & 0x4b != 0) { // indirect internal RAM access
    if(ram_ext[0x0256] == 0) {
      if(!20.7) {
        sub_a485(0x04, 9);
      } else {
        sub_a485(0x0c, 6);
        sub_a485(0x0a, 9);
        sub_a485(0x06, 12);
      }
    }
    ram_ext[0x0256] = ram_ext[0x021d];
  }

  // address 0ee4
  if(ram_int[0x65] & 0x4b != 0) { // indirect internal RAM access
    if(ram_ext[0x025e] == 0) {
      if(!20.7) {
        sub_a485(0x08, 9);
      } else {
        sub_a485(0x18, 6);
        sub_a485(0x14, 9);
        sub_a485(0x0c, 12);
      }
    }
    ram_ext[0x025e] = ram_ext[0x021d];
  }

  // address 0f58
  if(ram_int[0x67] & 0x4b != 0) { // indirect internal RAM access
    if(ram_ext[0x026b] == 0) {
      if(!20.7) {
        sub_a485(0x20, 9);
      } else {
        sub_a485(0x60, 6);
        sub_a485(0x50, 9);
        sub_a485(0x30, 12);
      }
    }
    ram_ext[0x026b] = ram_ext[0x021d];
  }

  // address 0f92
  if(ram_int[0x68] & 0x4b != 0) { // indirect internal RAM access
    if(ram_ext[0x0270] == 0) {
      if(!20.7) {
        sub_a485(0x40, 9);
      } else {
        sub_a485(0xc0, 6);
        sub_a485(0xa0, 9);
        sub_a485(0x60, 12);
      }
    }
    ram_ext[0x0270] = ram_ext[0x021d];
  }

  // address 0fcc
  if(ram_int[0x69] & 0x4b != 0) { // indirect internal RAM access
    if(ram_ext[0x0274] == 0) {
      if(!20.7) {
        sub_a485(0x80, 9);
      } else {
        sub_a485(0x80, 6);
        sub_a485(0x01, 7);
        sub_a485(0x40, 9);
        sub_a485(0x01, 10);
        sub_a485(0xc0, 12);
      }
    }
    ram_ext[0x0274] = ram_ext[0x021d];
  }

  ram_int[0x4c] = 0x0a;

  // address 1019
  if(ram_int[0x6a] & 0x4b != 0) { // indirect internal RAM access
    if(ram_ext[0x023a] == 0) {
      if(!20.7) {
        sub_a485(0x01, 10);
      } else {
        sub_a485(0x03, 7);
        sub_a485(0x80, 9);
        sub_a485(0x02, 10);
        sub_a485(0x80, 12);
        sub_a485(0x01, 13);
      }
    }
    ram_ext[0x023a] = ram_ext[0x021d];
  }

  // address 1065
  if(ram_int[0x6b] & 0x4b != 0) { // indirect internal RAM access
    if(ram_ext[0x0243] == 0) {
      if(!20.7) {
        sub_a485(0x02, 10);
      } else {
        sub_a485(0x06, 7);
        sub_a485(0x05, 10);
        sub_a485(0x03, 13);
      }
    }
    ram_ext[0x0243] = ram_ext[0x021d];
  }

  // address 109f
  if(ram_int[0x6c] & 0x4b != 0) { // indirect internal RAM access
    if(ram_ext[0x024c] == 0) {
      if(!20.7) {
        sub_a485(0x04, 10);
      } else {
        sub_a485(0x0c, 7);
        sub_a485(0x0a, 10);
        sub_a485(0x06, 13);
      }
    }
    ram_ext[0x024c] = ram_ext[0x021d];
  }

  // address 10d9
  if(ram_int[0x6d] & 0x4b != 0) { // indirect internal RAM access
    if(ram_ext[0x0255] == 0) {
      if(!20.7) {
        sub_a485(0x08, 10);
      } else {
        sub_a485(0x18, 7);
        sub_a485(0x14, 10);
        sub_a485(0x0c, 13);
      }
    }
    ram_ext[0x0255] = ram_ext[0x021d];
  }

  // address 1113
  if(ram_int[0x6e] & 0x4b != 0) { // indirect internal RAM access
    if(ram_ext[0x025d] == 0) {
      if(!20.7) {
        sub_a485(0x10, 10);
      } else {
        sub_a485(0x30, 7);
        sub_a485(0x48, 10);
        sub_a485(0x48, 13);
      }
    }
    ram_ext[0x025d] = ram_ext[0x021d];
  }

  // address 114d
  if(ram_int[0x6f] & 0x4b != 0) { // indirect internal RAM access
    if(ram_ext[0x0264] == 0) {
      if(!20.7) {
        sub_a485(0x20, 10);
      } else {
        sub_a485(0x40, 4);
        sub_a485(0x20, 7);
        sub_a485(0x40, 10);
      }
    }
    ram_ext[0x0264] = ram_ext[0x021d];
  }

  // address 1185
  if(ram_int[0x70] & 0x4b != 0) { // indirect internal RAM access
    if(ram_ext[0x026a] == 0) {
      if(!20.7) {
        sub_a485(0x40, 10);
      } else {
        sub_a485(0xe0, 7);
        sub_a485(0x30, 10);
        sub_a485(0x40, 13);
      }
    }
    ram_ext[0x026a] = ram_ext[0x021d];
  }

  // address 11bf
  if(ram_int[0x75] & 0x4b != 0) { // indirect internal RAM access
    if(ram_ext[0x024b] == 0) {
      if(!20.7) {
        sub_a485(0x08, 11);
      } else {
        sub_a485(0x1c, 8);
        sub_a485(0x10, 11);
        sub_a485(0x18, 14);
      }
    }
    ram_ext[0x024b] = ram_ext[0x021d];
  }

  // address 11f9
  if(ram_int[0x76] & 0x4b != 0) { // indirect internal RAM access
    if(ram_ext[0x0254] == 0) {
      if(!20.7) {
        sub_a485(0x10, 11);
      } else {
        sub_a485(0x38, 8);
        sub_a485(0x28, 11);
        sub_a485(0x38, 14);
      }
    }
    ram_ext[0x0254] = ram_ext[0x021d];
  }

  // address 1233
  if(ram_int[0x77] & 0x4b != 0) { // indirect internal RAM access
    if(ram_ext[0x025c] == 0) {
      if(!20.7) {
        sub_a485(0x20, 11);
      } else {
        sub_a485(0x70, 8);
        sub_a485(0x50, 11);
        sub_a485(0x70, 14);
      }
    }
    ram_ext[0x025c] = ram_ext[0x021d];
  }

  ram_int[0x4b] = 0x10;
  ram_int[0x4c] = 0x0c;

  // address 1273
  if(ram_int[0x63] & 0x4b != 0) { // indirect internal RAM access
    if(ram_ext[0x0272] == 0) {
      if(!20.7) {
        sub_a485(0x02, 12);
      } else {
        sub_a485(0x06, 9);
        sub_a485(0x04, 12);
        sub_a485(0x06, 15);
      }
    }
    ram_ext[0x0272] = ram_ext[0x021d];
  }

  // address 12ad
  if(ram_int[0x64] & 0x4b != 0) { // indirect internal RAM access
    if(ram_ext[0x0275] == 0) {
      if(!20.7) {
        sub_a485(0x04, 12);
      } else {
        sub_a485(0x0c, 9);
        sub_a485(0x0a, 12);
        sub_a485(0x80, 10);
        sub_a485(0x04, 15);
      }
    }
    ram_ext[0x0275] = ram_ext[0x021d];
  }

  // address 12f0
  if(ram_int[0x65] & 0x4b != 0) { // indirect internal RAM access
    if(ram_ext[0x0277] == 0) {
      if(!20.7) {
        sub_a485(0x08, 12);
      } else {
        sub_a485(0x18, 9);
        sub_a485(0x14, 12);
        sub_a485(0x04, 15);
      }
    }
    ram_ext[0x0277] = ram_ext[0x021d];
  }

  // address 132a
  if(ram_int[0x66] & 0x4b != 0) { // indirect internal RAM access
    if(ram_ext[0x0278] == 0) {
      if(!20.7) {
        sub_a485(0x10, 12);
      } else {
        sub_a485(0x30, 9);
        sub_a485(0x28, 12);
        sub_a485(0x08, 15);
      }
    }
    ram_ext[0x0278] = ram_ext[0x021d];
  }

  // address 1364
  if(ram_int[0x67] & 0x4b != 0) { // indirect internal RAM access
    if(ram_ext[0x027a] == 0) {
      if(!20.7) {
        sub_a485(0x20, 12);
      } else {
        sub_a485(0x60, 9);
        sub_a485(0x50, 12);
      }
    }
    ram_ext[0x027a] = ram_ext[0x021d];
  }

  // address 1393
  if(ram_int[0x68] & 0x4b != 0) { // indirect internal RAM access
    if(ram_ext[0x01ec] == 0) {
      if(!20.7) {
        sub_a485(0x40, 12);
      } else {
        sub_a485(0xc0, 9);
        sub_a485(0xa0, 12);
        sub_a485(0x40, 15);
      }
    }
    ram_ext[0x01ec] = ram_ext[0x021d];
  }

  // address 13cd
  if(ram_int[0x69] & 0x4b != 0) { // indirect internal RAM access
    if(ram_ext[0x01f0] == 0) {
      if(!20.7) {
        sub_a485(0x80, 12);
      } else {
        sub_a485(0x80, 9);
        sub_a485(0x01, 10);
        sub_a485(0x40, 12);
        sub_a485(0x01, 13);
        sub_a485(0x40, 15);
      }
    }
    ram_ext[0x01f0] = ram_ext[0x021d];
  }

  ram_int[0x4c] = 0x0d;
  
  // address 141a
  if(ram_int[0x6a] & 0x4b != 0) { // indirect internal RAM access
    if(ram_ext[0x01f5] == 0) {
      if(!20.7) {
        sub_a485(0x01, 13);
      } else {
        sub_a485(0x03, 10);
        sub_a485(0x80, 12);
        sub_a485(0x02, 13);
      }
    }
    ram_ext[0x01f5] = ram_ext[0x021d];
  }

  // address 1452
  if(ram_int[0x6b] & 0x4b != 0) { // indirect internal RAM access
    if(ram_ext[0x01fa] == 0) {
      if(!20.7) {
        sub_a485(0x02, 13);
      } else {
        sub_a485(0x06, 10);
        sub_a485(0x02, 16);
        sub_a485(0x05, 13);
      }
    }
    ram_ext[0x01fa] = ram_ext[0x021d];
  }

  // address 148a
  if(ram_int[0x6c] & 0x4b != 0) { // indirect internal RAM access
    if(ram_ext[0x01ff] == 0) {
      if(!20.7) {
        sub_a485(0x04, 13);
      } else {
        sub_a485(0x0c, 10);
        sub_a485(0x0a, 13);
        sub_a485(0x06, 16);
      }
    }
    ram_ext[0x01ff] = ram_ext[0x021d];
  }

  // address 14c4
  if(ram_int[0x6d] & 0x4b != 0) { // indirect internal RAM access
    if(ram_ext[0x0204] == 0) {
      if(!20.7) {
        sub_a485(0x08, 13);
      } else {
        sub_a485(0x18, 10);
        sub_a485(0x44, 13);
        sub_a485(0x0c, 16);
      }
    }
    ram_ext[0x0204] = ram_ext[0x021d];
  }

  // address 14fe
  if(ram_int[0x70] & 0x4b != 0) { // indirect internal RAM access
    if(ram_ext[0x0279] == 0) {
      if(!20.7) {
        sub_a485(0x40, 13);
      } else {
        sub_a485(0x50, 10);
        sub_a485(0x08, 13);
        sub_a485(0x28, 16);
      }
    }
    ram_ext[0x0279] = ram_ext[0x021d];
  }

  // address 1538
  if(ram_int[0x73] & 0x4b != 0) { // indirect internal RAM access
    if(ram_ext[0x01f4] == 0) {
      if(!20.7) {
        sub_a485(0x02, 14);
      } else {
        sub_a485(0x07, 17);
      }
    }
    ram_ext[0x01f4] = ram_ext[0x021d];
  }

  // address 1560
  if(ram_int[0x75] & 0x4b != 0) { // indirect internal RAM access
    if(ram_ext[0x01fe] == 0) {
      if(!20.7) {
        sub_a485(0x08, 14);
      } else {
        sub_a485(0x18, 11);
        sub_a485(0x10, 14);
        sub_a485(0x1c, 17);
      }
    }
    ram_ext[0x01fe] = ram_ext[0x021d];
  }

  // address 159a
  if(ram_int[0x76] & 0x4b != 0) { // indirect internal RAM access
    if(ram_ext[0x0203] == 0) {
      if(!20.7) {
        sub_a485(0x10, 14);
      } else {
        sub_a485(0x38, 11);
        sub_a485(0x28, 14);
        sub_a485(0x38, 17);
      }
    }
    ram_ext[0x0203] = ram_ext[0x021d];
  }

  // address 15d4
  if(ram_int[0x77] & 0x4b != 0) { // indirect internal RAM access
    if(ram_ext[0x0207] == 0) {
      if(!20.7) {
        sub_a485(0x20, 14);
      } else {
        sub_a485(0x30, 11);
        sub_a485(0x50, 14);
        sub_a485(0x20, 17);
      }
    }
    ram_ext[0x0207] = ram_ext[0x021d];
  }

  // address 160e
  if(ram_int[0x78] & 0x4b != 0) { // indirect internal RAM access
    if(ram_ext[0x020b] == 0) {
      if(!20.7) {
        sub_a485(0x40, 14);
      } else {
        sub_a485(0x40, 8);
        sub_a485(0x20, 11);
        sub_a485(0x20, 14);
        sub_a485(0x20, 17);
      }
    }
    ram_ext[0x020b] = ram_ext[0x021d];
  }

  ram_int[0x4b] = 0x20;
  ram_int[0x4c] = 0x0f;

  // address 1655
  if(ram_int[0x63] & 0x4b != 0) { // indirect internal RAM access
    if(ram_ext[0x01ee] == 0) {
      if(!20.7) {
        sub_a485(0x02, 15);
      } else {
        sub_a485(0x02, 12);
        sub_a485(0x80, 16);
      }
    }
    ram_ext[0x01ee] = ram_ext[0x021d];
  }

  // address 1684
  if(ram_int[0x64] & 0x4b != 0) { // indirect internal RAM access
    if(ram_ext[0x01f3] == 0) {
      if(!20.7) {
        sub_a485(0x04, 15);
      } else {
        sub_a485(0x0c, 12);
        sub_a485(0x80, 16);
      }
    }
    ram_ext[0x01f3] = ram_ext[0x021d];
  }

  // address 16b5
  if(ram_int[0x68] & 0x4b != 0) { // indirect internal RAM access
    if(ram_ext[0x0206] == 0) {
      if(!20.7) {
        sub_a485(0x40, 15);
      } else {
        sub_a485(0xe0, 12);
      }
    }
    ram_ext[0x0206] = ram_ext[0x021d];
  }

  ram_int[0x4c] = 0x10;

  // address 16e0
  if(ram_int[0x6b] & 0x4b != 0) { // indirect internal RAM access
    if(ram_ext[0x0211] == 0) {
      if(!20.7) {
        sub_a485(0x02, 16);
      } else {
        sub_a485(0x0e, 13);
        sub_a485(0x04, 16);
      }
    }
    ram_ext[0x0211] = ram_ext[0x021d];
  }

  // address 170f
  if(ram_int[0x6c] & 0x4b != 0) { // indirect internal RAM access
    if(ram_ext[0x01ed] == 0) {
      if(!20.7) {
        sub_a485(0x04, 16);
      } else {
        sub_a485(0x08, 13);
        sub_a485(0x0a, 16);
      }
    }
    ram_ext[0x01ed] = ram_ext[0x021d];
  }

  // address 173e
  if(ram_int[0x6d] & 0x4b != 0) { // indirect internal RAM access
    if(ram_ext[0x01f2] == 0) {
      if(!20.7) {
        sub_a485(0x08, 16);
      } else {
        sub_a485(0x48, 13);
        sub_a485(0x24, 16);
      }
    }
    ram_ext[0x01f2] = ram_ext[0x021d];
  }

  // address 176d
  if(ram_int[0x6f] & 0x4b != 0) { // indirect internal RAM access
    if(ram_ext[0x01fc] == 0) {
      if(!20.7) {
        sub_a485(0x20, 16);
      } else {
        sub_a485(0x40, 13);
        sub_a485(0x08, 16);
        sub_a485(0x01, 17);
      }
    }
    ram_ext[0x01fc] = ram_ext[0x021d];
  }

  // address 17a7
  if(ram_int[0x71] & 0x4b != 0) { // indirect internal RAM access
    if(ram_ext[0x0205] == 0) {
      if(!20.7) {
        sub_a485(0x80, 16);
      } else {
        sub_a485(0x06, 12);
        sub_a485(0x06, 15);
      }
    }
    ram_ext[0x0205] = ram_ext[0x021d];
  }

  ram_int[0x4c] = 0x11;

  // address 17db
  if(ram_int[0x72] & 0x4b != 0) { // indirect internal RAM access
    if(ram_ext[0x0209] == 0) {
      if(!20.7) {
        sub_a485(0x01, 17);
      } else {
        sub_a485(0x20, 16);
        sub_a485(0x02, 17);
      }
    }
    ram_ext[0x0209] = ram_ext[0x021d];
  }

  // address 180a
  if(ram_int[0x73] & 0x4b != 0) { // indirect internal RAM access
    if(ram_ext[0x020d] == 0) {
      if(!20.7) {
        sub_a485(0x02, 17);
      } else {
        sub_a485(0x02, 14);
        sub_a485(0x05, 17);
      }
    }
    ram_ext[0x020d] = ram_ext[0x021d];
  }

  // address 1837
  if(ram_int[0x74] & 0x4b != 0) { // indirect internal RAM access
    if(ram_ext[0x0210] == 0) {
      if(!20.7) {
        sub_a485(0x04, 17);
      } else {
        sub_a485(0x0a, 17);
      }
    }
    ram_ext[0x0210] = ram_ext[0x021d];
  }

  // address 185d
  if(ram_int[0x75] & 0x4b != 0) { // indirect internal RAM access
    if(ram_ext[0x0212] == 0) {
      if(!20.7) {
        sub_a485(0x08, 17);
      } else {
        sub_a485(0x24, 17);
        sub_a485(0x18, 14);
      }
    }
    ram_ext[0x0212] = ram_ext[0x021d];
  }

  // address 188e
  if(ram_int[0x77] & 0x4b != 0) { // indirect internal RAM access
    if(ram_ext[0x01f6] == 0) {
      if(!20.7) {
        sub_a485(0x20, 17);
      } else {
        sub_a485(0x70, 14);
        sub_a485(0x08, 17);
      }
    }
    ram_ext[0x01f6] = ram_ext[0x021d];
  }
}

// address a485
void sub_a485(uint8_t mask, uint8_t off)
{
  uint8_t r6 = ram_ext[0x0397];
  if(r6 == 1) {
    ram_int[0xc3+off] |= mask;
    return;
  } else if(r6 == 2) {
    ram_int[0xc3+off] |= mask;
    ram_int[0x9f+off] |= mask;
    return;
  }
  
  r6 = ram_ext[0x0397];
  if(r6 == 3) {
    ram_int[0x9f+off] |= mask;
    return;
  } else if(r6 == 4) {
    ram_int[0xb1+off] |= mask;
    ram_int[0x9f+off] |= mask;
    return;
  }

  r6 = ram_ext[0x0397];
  if(r6 == 5) {
    ram_int[0xb1+off] |= mask;
    return;
  } else if(r6 == 6) {
    ram_int[0xc3+off] |= mask;
    ram_int[0xb1+off] |= mask;
  }

  r6 = ram_ext[0x0397];
  if(r6 == 7) {
    ram_int[0xc3+off] |= mask;
    ram_int[0xb1+off] |= mask;
    ram_int[0x9f+off] |= mask;
  } else if(r6 == 8) {
    if(++ram_ext[0x03ce] >= 7) {ram_ext[0x03ce] = 1;}
    
    uint8_t r6 = ram_ext[0x03ce];
    if(r6 == 1) {
      ram_int[0xc3+off] |= mask;
      ram_int[0xb1+off] &= ~mask;
      ram_int[0x9f+off] &= ~mask;
      return;
    } else if(r6 == 2) {
      ram_int[0xc3+off] |= mask;
      ram_int[0x9f+off] |= mask;
      ram_inf[0xb1+off] &= ~mask;
      return;
    }

    r6 = ram_ext[0x03ce];
    if(r6 == 3) {
      ram_int[0x9f+off] |= mask;
      ram_int[0xc3+off] &= ~mask;
      ram_int[0xb1+off] &= ~mask;
      return;
    } else if(r6 == 4) {
      ram_int[0xb1+off] |= mask;
      ram_int[0x9f+off] |= mask;
      ram_int[0xc3+off] &= ~mask;
      return;
    }

    r6 = ram_ext[0x03ce];
    if(r6 == 5) {
      ram_int[0xb1+off] |= mask;
      ram_int[0xc3+off] &= ~mask;
      ram_int[0x9f+off] &= ~mask;
      return;
    } else if(r6 == 6) {
      ram_int[0xc3+off] |= mask;
      ram_int[0xb1+off] |= mask;
      ram_int[0x9f+off] &= ~mask;
      return;
    }

    r6 = ram_ext[0x03ce];
    if(r6 == 7) {
      ram_int[0xc3+off] |= mask;
      ram_int[0xb1+off] |= mask;
      ram_int[0x9f+off] |= mask;
    }
  }
}

// address dda9
void update_indicators(void)
{
  if(!20.4 && !20.3) {return;}

  if(++ram_ext[0x01dc] >= 0x3c) {
    ram_ext[0x01dc] = 0;
    asm("CPL P3.7");
    asm("CPL P3.6");
    asm("CPL P3.5");
  }
}

// address b3ff
void sub_b3ff(void)
{
  if(!23.0 || !20.1) {return;}

  uint8_t r7 = ram_ext[0x03c6];
  if(r7 == 0) {
    ram_int[0x4b] = 0x08;
  } else if(r7 == 1) {
    ram_int[0x4b] = 0x14;
  } else if(r7 == 2) {
    ram_int[0x4b] = 0x2d;
  } else if(r7 == 3) {
    ram_int[0x4b] = 0x46;
  } else if(r7 == 4) {
    ram_int[0x4b] = 0xff;
  }

  if(++ram_ext[0x03c9] >= ram_int[0x4b]) {
    ram_ext[0x03c9] = 0;
    
    if(!2c.5) {
      if(++ram_ext[0x03c8] >= 0x18) {
        ram_ext[0x03c8] = 0;
      }
    } else {
      if(ram_ext[0x03c8] >= 0) {
        if(--ram_ext[0x03c8] == 0) {
          ram_ext[0x03c8] = 0x18;
        }
      } else {
        ram_ext[0x03c8] = 0x18;
      }
    }

    // address b485
    uint8_t r4, r5, r5, r7;
    read_prog_4(0xa1ef + (ram_ext[0x03c8] * 4), &r4, &r5, &r6, &r7);
    write_ext_4(0x03b7, r4, r5, r6, r7);
    sub_a836(1);

    read_prog_4(0xa20f + (ram_ext[0x03c8] * 4), &r4, &r5, &r6, &r7);
    write_ext_4(0x03b7, r4, r5, r6, r7);
    sub_a836(2);

    read_prog_4(0xa22f + (ram_ext[0x03c8] * 4), &r4, &r5, &r6, &r7);
    write_ext_4(0x03b7, r4, r5, r6, r7);
    sub_a836(3);
  }
}

// address a836
void sub_a836(uint8_t arg)
{
  byte_reverse_3();

  // ALL ram_int accesses below are indirect internal RAM accesses
  if(arg == 1) {
    uint8_t r7 = ram_ext[0x03bc];
    ram_int[0xc9] = r7;
    ram_int[0xc6] = r7;
    ram_int[0xc3] = r7;
    ram_int[0xd2] = r7;
    ram_int[0xcf] = r7;
    ram_int[0xcc] = r7;

    r7 = ram_ext[0x03bd];
    ram_int[0xca] = r7;
    ram_int[0xc7] = r7;
    ram_int[0xc4] = r7;
    ram_int[0xd3] = r7;
    ram_int[0xd0] = r7;
    ram_int[0xcd] = r7;

    r7 = ram_ext[0x03bb];
    ram_int[0xcb] = r7;
    ram_int[0xc8] = r7;
    ram_int[0xc5] = r7;
    ram_int[0xd4] = r7;
    ram_int[0xd1] = r7;
    ram_int[0xce] = r7;

    if(ram_int[0xd2].1) {
      ram_int[0xd3] |= 0x80;
    } else {
      ram_int[0xd3] &= 0x7f;
    }
  } else if(arg == 2) {
    uint8_t r7 = ram_ext[0x03bc];
    ram_int[0xb7] = r7;
    ram_int[0xb4] = r7;
    ram_int[0xb1] = r7;
    ram_int[0xc0] = r7;
    ram_int[0xbd] = r7;
    ram_int[0xba] = r7;

    r7 = ram_ext[0x03bd];
    ram_int[0xb8] = r7;
    ram_int[0xb5] = r7;
    ram_int[0xb2] = r7;
    ram_int[0xc1] = r7;
    ram_int[0xbe] = r7;
    ram_int[0xbb] = r7;

    r7 = ram_ext[0x03bb];
    ram_int[0xb9] = r7;
    ram_int[0xb6] = r7;
    ram_int[0xb3] = r7;
    ram_int[0xc2] = r7;
    ram_int[0xbf] = r7;
    ram_int[0xbc] = r7;

    if(ram_int[0xc0].1) {
      ram_int[0xc1] |= 0x80;
    } else {
      ram_int[0xc1] &= 0x7f;
    }
  } else if(arg == 3) {
    uint8_t r7 = ram_ext[0x03bc];
    ram_int[0xa5] = r7;
    ram_int[0xa2] = r7;
    ram_int[0x9f] = r7;
    ram_int[0xae] = r7;
    ram_int[0xab] = r7;
    ram_int[0xa8] = r7;

    r7 = ram_ext[0x03bd];
    ram_int[0xa6] = r7;
    ram_int[0xa3] = r7;
    ram_int[0xa0] = r7;
    ram_int[0xaf] = r7;
    ram_int[0xac] = r7;
    ram_int[0xa9] = r7;

    r7 = ram_ext[0x03bb];
    ram_int[0xa7] = r7;
    ram_int[0xa4] = r7;
    ram_int[0xa1] = r7;
    ram_int[0xb0] = r7;
    ram_int[0xad] = r7;
    ram_int[0xaa] = r7;

    if(ram_int[0xae].1) {
      ram_int[0xaf] |= 0x80;
    } else {
      ram_int[0xaf] &= 0x7f;
    }
  }
}

// address 905a
void byte_reverse_3(void)
{
  ram_ext[0x03bb] = 0;
  ram_ext[0x03bd] = 0;
  ram_ext[0x03bc] = 0;

  if(ram_ext[0x03b7+1] & 0x80 != 0) {ram_ext[0x03bc] |= 0x01;}
  if(ram_ext[0x03b7+1] & 0x40 != 0) {ram_ext[0x03bc] |= 0x02;}
  if(ram_ext[0x03b7+1] & 0x20 != 0) {ram_ext[0x03bc] |= 0x04;}
  if(ram_ext[0x03b7+1] & 0x10 != 0) {ram_ext[0x03bc] |= 0x08;}
  if(ram_ext[0x03b7+1] & 0x08 != 0) {ram_ext[0x03bc] |= 0x10;}
  if(ram_ext[0x03b7+1] & 0x04 != 0) {ram_ext[0x03bc] |= 0x20;}
  if(ram_ext[0x03b7+1] & 0x02 != 0) {ram_ext[0x03bc] |= 0x40;}
  if(ram_ext[0x03b7+1] & 0x01 != 0) {ram_ext[0x03bc] |= 0x80;}

  if(ram_ext[0x03b7+2] & 0x80 != 0) {ram_ext[0x03bd] |= 0x01;}
  if(ram_ext[0x03b7+2] & 0x40 != 0) {ram_ext[0x03bd] |= 0x02;}
  if(ram_ext[0x03b7+2] & 0x20 != 0) {ram_ext[0x03bd] |= 0x04;}
  if(ram_ext[0x03b7+2] & 0x10 != 0) {ram_ext[0x03bd] |= 0x08;}
  if(ram_ext[0x03b7+2] & 0x08 != 0) {ram_ext[0x03bd] |= 0x10;}
  if(ram_ext[0x03b7+2] & 0x04 != 0) {ram_ext[0x03bd] |= 0x20;}
  if(ram_ext[0x03b7+2] & 0x02 != 0) {ram_ext[0x03bd] |= 0x40;}
  if(ram_ext[0x03b7+2] & 0x01 != 0) {ram_ext[0x03bd] |= 0x80;}

  if(ram_ext[0x03b7+3] & 0x80 != 0) {ram_ext[0x03bb] |= 0x01;}
  if(ram_ext[0x03b7+3] & 0x40 != 0) {ram_ext[0x03bb] |= 0x02;}
  if(ram_ext[0x03b7+3] & 0x20 != 0) {ram_ext[0x03bb] |= 0x04;}
  if(ram_ext[0x03b7+3] & 0x10 != 0) {ram_ext[0x03bb] |= 0x08;}
  if(ram_ext[0x03b7+3] & 0x08 != 0) {ram_ext[0x03bb] |= 0x10;}
  if(ram_ext[0x03b7+3] & 0x04 != 0) {ram_ext[0x03bb] |= 0x20;}
  if(ram_ext[0x03b7+3] & 0x02 != 0) {ram_ext[0x03bb] |= 0x40;}
  if(ram_ext[0x03b7+3] & 0x01 != 0) {ram_ext[0x03bb] |= 0x80;}
}

// address c985
void write_ext_4(uint16_t dptr, uint8_t r4, uint8_t r5, uint8_t r6, uint8_t r7)
{
  [dptr++] = r4;
  prog[dptr++] = r5;
  prog[dptr++] = r6;
  prog[dptr++] = r7;
}

// address c975
void read_prog_4(uint16_t dptr, uint8_t *r4, uint8_t *r5, uint8_t *r6, uint8_t *r7)
{
  *r4 = prog[dptr];
  *r5 = prog[dptr+1];
  *r6 = prog[dptr+2];
  *r7 = prog[dptr+3];
}

// address a949
void sub_a949(void)
{
  if(!22.0 || !20.1) {return;}
  
  if(++ram_ext[0x039d] >= 2) {
    ram_ext[0x039d] = 0;
    ram_ext[0x0371] = 0;
    
    ram_ext[0x0371] = prog[0xa2af + ram_ext[0x0354]];
    sub_52c4();

    ram_ext[0x0371] = prog[0xa2c7 + ram_ext[0x0354]];
    sub_52c4();

    ram_ext[0x0371] = prog[0xa2df + ram_ext[0x0354]];
    sub_52c4();

    ram_ext[0x0371] = prog[0xa2f7 + ram_ext[0x0354]];
    sub_52c4();

    ram_ext[0x0371] = prog[0xa30f + ram_ext[0x0354]];
    sub_52c4();

    ram_ext[0x0371] = prog[0xa327 + ram_ext[0x0354]];
    sub_52c4();
    
    uint8_t r7 = ram_ext[0x03c8];
    if(r7 == 0 || r7 == 4) {
      if(++ram_ext[0x0354] > 23) {
        asm("SETB 2c.4");
        ram_ext[0x0354] = 0;
      }
    } else if(r7 == 1 || r7 == 5) {
      if(++ram_ext[0x0354] > 23) {
        asm("CLR 2c.4");
        if(++ram_ext[0x03c8] >= 5) {
          ram_ext[0x03c8] = 0;
          asm("CLR 22.0");
          sub_7f5a();
        }
      }
    } else if(r7 == 2) {
      if(--ram_ext[0x0354] == 0) {
        ram_ext[0x0354] = 0x18;
        asm("SETB 2c.4");
        ++ram_ext[0x03c8];
      }
    } else if(r7 == 3) {
      if(--ram_ext[0x0354] == 0) {
        asm("CLR 2c.4");
        ++ram_ext[0x03c8];
      }
    }
  }
}

// address 7f5a
void sub_7f5a(void)
{
  sub_c9c8();
  set_col_drive_all();
  clr_row_color_all();

  ram_int[0xa7] |= 0x01; // set MOVC access to EEPROM

  ram_int[0x4b] = eeprom[0x0600];
  ram_int[0x09] = eeprom[0x0601];
  ram_ext[0x03b6] = eeprom[0x0602];
  ram_ext[0x03cb] = eeprom[0x0603];
  ram_int[0x4c] = eeprom[0x0604];
  ram_ext[0x0331] = eeprom[0x0605];
  ram_ext[0x0356] = eeprom[0x0606];
  ram_ext[0x0392] = eeprom[0x0607];
  ram_ext[0x0397] = eeprom[0x0608];
  ram_ext[0x03b4] = eeprom[0x0609];
  ram_ext[0x03cc] = eeprom[0x060a];
  ram_ext[0x021d] = eeprom[0x060b];
  ram_ext[0x0370] = eeprom[0x060c];
  ram_ext[0x0332] = eeprom[0x060d];
  ram_ext[0x038d] = eeprom[0x060e];

  ram_int[0xa7] &= 0xfe; // set MOVC access to program memory

  if(ram_ext[0x03b6] < 1) {ram_ext[0x03b6] = 5;}
  if(ram_ext[0x03b6] > 5) {ram_ext[0x03b6] = 5;}
  if(ram_ext[0x021d] > 0xa0) {ram_ext[0x021d] = 0x3c;}
  if(ram_ext[0x021d] < 0x1e) {ram_ext[0x021d] = 0x3c;}
  if(ram_ext[0x0370] > 8) {ram_ext[0x0370] = 2;}
  if(ram_ext[0x0370] < 1) {ram_ext[0x0370] = 2;}
  if(ram_ext[0x0332] > 5) {ram_ext[0x0332] = 2;}
  if(ram_ext[0x0332] < 1) {ram_ext[0x0332] = 2;}
  if(ram_ext[0x038d] > 8) {ram_ext[0x038d] = 1;}
  if(ram_ext[0x038d] < 1) {ram_ext[0x038d] = 1;}
  if(ram_ext[0x0356] == 0) {ram_ext[0x0356] = 8;}
  if(ram_ext[0x0392] == 0) {ram_ext[0x0392] = 8;}
  if(ram_ext[0x0397] == 0) {ram_ext[0x0397] = 8;}
  if(ram_ext[0x03cb] == 0) {ram_ext[0x03cb] = 4;}
  if(ram_ext[0x03b4] == 0) {ram_ext[0x03b4] = 4;}

  ram_ext[0x03c4] = 3;
  if(ram_int[0x4b] == 1) {
    asm("SETB 21.0");
    ram_ext[0x03cd] = 1;
    sub_c0a6(ram_ext[0x03cb]);
  } else if(ram_int[0x4b] == 2) {
    asm("SETB 21.5");
    ram_ext[0x03cd] = 2;
    sub_c0a6(ram_ext[0x03b4]);
    sub_de53();
  } else if(ram_int[0x4b] == 3) {
    asm("SETB 22.7");
    ram_int[0x03cd] = 2;
    sub_a33f();
  } else if(ram_int[0x4b] == 4) {
    asm("SETB 23.2");
    ram_ext[0x03cd] = 4;
    sub_d9ef(1, 0xff);
    sub_d9ef(2, 0xff);
    sub_d9ef(3, 0xff);
    ram_int[0xdd] = 0;
    ram_int[0xde] = 0; // set PWM2 duty cycle to 0x0000
    ram_int[0xdb] = 0;
    ram_int[0xdc] = 0; // set PWM1 duty cycle to 0x0000
    ram_int[0xd9] = 0;
    ram_int[0xda] = 0; // set PWM0 duty cycle to 0x0000
    sub_de53();
  } else if(ram_int[0x4b] == 5) {
    asm("SETB 22.4");
    ram_ext[0x03cd] = 5;
  } else if(ram_int[0x4b] == 6) {
    asm("SETB 22.3");
    ram_ext[0x03cd] = 6;
    sub_bc55();
  } else if(ram_int[0x4b] == 7) {
    asm("SETB 23.4");
    ram_ext[0x03cd] = 7;
  } else if(ram_int[0x4b] == 8) {
    asm("SETB 22.6");
    ram_ext[0x03cd] = 8;
  } else if(ram_int[0x4b] == 9) {
    asm("SETB 20.7");
    ram_ext[0x03cd] = 9;
  } else if(ram_int[0x4b] == 10) {
    sub_c9c8();
    asm("SETB 23.0");
    ram_ext[0x03cd] = 10;
  } else if(ram_int[0x4b] >= 11) {
    asm("SETB 20.7");
    ram_ext[0x03cd] = 0;
    sub_c9c8();
    asm("SETB 22.1");
    clr_row_color_all();
    set_col_drive_all();
  } else if(ram_int[0x4b] == 0) {
    asm("SETB 21.0");
    ram_ext[0x03cd] = 1;
    sub_c0a6(ram_ext[0x03cb]);
    ram_int[0x09] = 4;
  }
  
  asm("CLR 2c.5");
  if(ram_int[0x4c] != 0) {asm("SETB 2c.5");}
  sub_d506(ram_int[0x09]);
  delay(0x0212);
  sub_d506(ram_int[0x09]);
  delay(0x0212);
}

// address d506
void sub_d506(uint8_t r7)
{
  if(23.2) {return;}
  
  if(22.3) {
    ram_int[0xdd] = prog[0xdd6b+r7];
    ram_int[0xde] = 0x00; // set PWM2 duty cycle
    
    ram_int[0xdb] = prog[0xdd66+r7];
    ram_int[0xdc] = 0x00; // set PWM1 duty cycle
    
    ram_int[0xd9] = prog[0xdd61+r7];
    ram_int[0xda] = 0x00; // set PWM0 duty cycle
  } else {
    ram_int[0xdd] = prog[0xdd5c+r7];
    ram_int[0xde] = 0x00; // set PWM2 duty cycle
    
    ram_int[0xdb] = prog[0xdd57+r7];
    ram_int[0xdc] = 0x00; // set PWM1 duty cycle
    
    ram_int[0xd9] = prog[0xdd52+r7];
    ram_int[0xda] = 0x00; // set PWM0 duty cycle
  }
  
  delay(0x001e);
}

// address a33f
void sub_a33f(void)
{
  if(ram_ext[0x03cc] <= 1) {
    sub_baf0(0x0000);
    sub_d35e();
    if(ram_ext[0x02fe] == 0) {
      ram_int[0xc3] = 4;
      ram_int[0x9f] = 4;
      ram_int[0xb7] = 8;
      ram_int[0xa5] = 8;
      ram_int[0xba] = 0x1c;
      ram_int[0xa8] = 0x1c;
      ram_int[0xbf] = 2;
      ram_int[0xc2] = 7;
    }
  } else if(ram_ext[0x03cc] == 2) {
    sub_baf0(0x0050);
    sub_d35e();
    if(ram_ext[0x02fe] == 0) {
      ram_int[0xc3] = 0x38;
      ram_int[0x9f] = 0x38;
      ram_int[0x9f] = 1; // double assignment?
      ram_int[0xb4] = 0x7c;
      ram_int[0xa4] = 6;
      ram_int[0xb7] = 0x3e;
      ram_int[0xa7] = 6;
      ram_int[0xba] = 0x5c;
      ram_int[0xbd] = 0x42;
      ram_int[0xc0] = 0x46;
    }
  } else if (ram_ext[0x03cc] == 3) {
    sub_baf0(0x0100);
    sub_d35e();
    if(ram_ext[0x02fe] == 0) {
      ram_int[0xa2] = 0xfc;
      ram_int[0xa3] = 1;
      ram_int[0xa5] = 0x7c;
      ram_int[0xa8] = 0x7c;
      ram_int[0xab] = 0x32;
      ram_int[0xae] = 2;
      ram_int[0xd2] = 2;
      ram_int[0xc0] = 2;
      ram_int[0xcf] = 2;
      ram_int[0xbd] = 2;
    }
  } else if(ram_ext[0x03cc] = 4) {
    sub_baf0(0x0150);
    sub_d35e();
    if(ram_ext[0x02fe] == 0) {
      ram_int[0xa2] = 0xfc;
      ram_int[0xc6] = 0xfc;
      ram_int[0xa3] = 1;
      ram_int[0xc7] = 1;
      ram_int[0xc9] = 0x3c;
      ram_int[0xb7] = 0x3c;
      ram_int[0xcc] = 0x3c;
      ram_int[0xba] = 0x3c;
    }
  } else if(ram_ext[0x03cc] == 5) {
    sub_baf0(0x0200);
    sub_d35e();
    if(ram_ext[0x02fe] == 0) {
      ram_int[0xc9] = 0x28;
      ram_int[0xa5] = 0x28;
      ram_int[0xcc] = 0x1c;
      ram_int[0xba] = 0x1c;
      ram_int[0xab] = 2;
      ram_int[0xbd] = 2;
      ram_int[0xad] = 2;
      ram_int[0xbf] = 2;
      ram_int[0xd1] = 2;
      ram_int[0xae] = 6;
      ram_int[0xc0] = 6;
      ram_int[0xb0] = 7;
      ram_int[0xc2] = 7;
      ram_int[0xd4] = 7;
    }
  }
}

// address d35e
void sub_d35e(void)
{
  for(int i=0;i<18;++i) {ram_int[0xc3+i] = ram_ext[0x02eb+i];}
  for(int i=0;i<18;++i) {ram_int[0xb1+i] = ram_ext[0x02d8+i];}
  for(int i=0;i<18;++i) {ram_int[0x9f+i] = ram_ext[0x02c6+i];}
}

// address baf0
void sub_baf0(uint16_t r6r7) // held in 4d:4e
{
  uint8_t r7 = 0;
  ram_ext[0x02fe] = 0;
  
  ram_int[0xa7] |= 0x01; // set MOVC access to EEPROM

  for(int i=0;i<18;++i) { // r6
    ram_ext[0x02eb+i] = eeprom[r6r7+i];
    r7 |= ram_ext[0x02eb+i];
  }
  for(int i=0;i<18;++i) { // r6
    ram_ext[0x02d8+i] = eeprom[r6r7+18+i];
    r7 |= ram_ext[0x02d8+i];
  }
  for(int i=0;i<18;++i) { // r6
    ram_ext[0x02c6+i] = eeprom[r6r7+18+18+i];
    r7 |= ram_ext[0x02c6+i];
  }

  ram_int[0xa7] &= 0xfe; // set MOVC access to program memory

  ram_ext[0x02fe] = r7;
}

// address c0a6
void sub_c0a6(uint8_t r7)
{
  ram_int[0x4d] = r7;
  
  sub_d9ef(1, 0x00);
  sub_d9ef(2, 0x00);
  sub_d9ef(3, 0x00);

  if(ram_int[0x4d] <= 1) {
    sub_d9ef(1, 0xff);
  } else if(ram_int[0x4d] == 2) {
    sub_d9ef(1, 0xff);
    sub_d9ef(3, 0xff);
  } else if(ram_int[0x4d] == 3) {
    sub_d9ef(3, 0xff);
  } else if(ram_int[0x4d] == 4) {
    sub_d9ef(2, 0xff);
    sub_d9ef(3, 0xff);
  } else if(ram_int[0x4d] == 5) {
    sub_d9ef(2, 0xff);
  } else if(ram_int[0x4d] == 6) {
    sub_d9ef(2, 0xff);
    sub_d9ef(1, 0xff);
  } else if(ram_int[0x4d] == 7) {
    sub_d9ef(2, 0xff);
    sub_d9ef(3, 0xff);
    sub_d9ef(1, 0xff);
  }
}

// address 52c4
void sub_52c4(void)
{
  if(22.6) {
    if(ram_ext[0x0371] == 0x0f) {asm("NOP");}
    if(ram_ext[0x0371] == 0x87) {asm("NOP");}
  }

  uint8_t r7 = ram_ext[0x0371];
  if(r7 < 8) {
    ram_int[0x59] = 0;
  } else if(r7 >= 8 && r7 < 16) {
    ram_int[0x59] = 1;
  } else if(r7 >= 16 && r7 < 24) {
    ram_int[0x59] = 2;
  } else if(r7 >= 24 && r7 < 32) {
    ram_int[0x59] = 3;
  } else if(r7 >= 32 && r7 < 40) {
    ram_int[0x59] = 4;
  } else if(r7 >= 40 && r7 < 48) {
    ram_int[0x59] = 5;
  } else if(r7 >= 48 && r7 < 56) {
    ram_int[0x59] = 6;
  } else if(r7 >= 56 && r7 < 64) {
    ram_int[0x59] = 7;
  } else if(r7 >= 64 && r7 < 72) {
    ram_int[0x59] = 8;
  } else if(r7 >= 72 && r7 < 80) {
    ram_int[0x59] = 9;
  } else if(r7 >= 80 && r7 < 88) {
    ram_int[0x59] = 10;
  } else if(r7 >= 88 && r7 < 96) {
    ram_int[0x59] = 11;
  } else if(r7 >= 96 && r7 < 104) {
    ram_int[0x59] = 12;
  } else if(r7 >= 104 && r7 < 112) {
    ram_int[0x59] = 13;
  } else if(r7 >= 112 && r7 < 120) {
    ram_int[0x59] = 14;
  } else if(r7 >= 120 && r7 < 128) {
    ram_int[0x59] = 15;
  } else if(r7 >= 128 && r7 < 136) {
    ram_int[0x59] = 16;
  } else if(r7 >= 136 && r7 < 144) {
    ram_int[0x59] = 17;
  }

  if(r7 == 0 || r7 == 8 || r7 == 16 || r7 == 24 ||
     r7 == 32 || r7 == 40 || r7 == 48 || r7 == 56 ||
     r7 == 64 || r7 == 72 || r7 == 80 || r7 == 88 ||
     r7 == 96 || r7 == 104 || r7 == 112 || r7 == 120 ||
     r7 == 128 || r7 == 136) {
    ram_int[0x5a] = 1;
  } else if(r7 == 1 || r7 == 9 || r7 == 17 || r7 == 25 ||
            r7 == 33 || r7 == 41 || r7 == 49 || r7 == 57 ||
            r7 == 65 || r7 == 73 || r7 == 81 || r7 == 89 ||
            r7 == 97 || r7 == 105 || r7 == 113 || r7 == 121 ||
            r7 == 129 || r7 == 137) {
    ram_int[0x5a] = 2;
  } else if(r7 == 2 || r7 == 10 || r7 == 18 || r7 == 26 ||
            r7 == 34 || r7 == 42 || r7 == 50 || r7 == 58 ||
            r7 == 66 || r7 == 74 || r7 == 82 || r7 == 90 ||
            r7 == 98 || r7 == 106 || r7 == 114 || r7 == 122 ||
            r7 == 130 || r7 == 138) {
    ram_int[0x5a] = 4;
  } else if(r7 == 3 || r7 == 11 || r7 == 19 || r7 == 27 ||
            r7 == 35 || r7 == 43 || r7 == 51 || r7 == 59 ||
            r7 == 67 || r7 == 75 || r7 == 83 || r7 == 91 ||
            r7 == 99 || r7 == 107 || r7 == 115 || r7 == 123 ||
            r7 == 131 || r7 == 139) {
    ram_int[0x5a] = 8;
  } else if(r7 == 4 || r7 == 12 || r7 == 20 || r7 == 28 ||
            r7 == 35 || r7 == 43 || r7 == 52 || r7 == 60 ||
            r7 == 68 || r7 == 76 || r7 == 84 || r7 == 92 ||
            r7 == 100 || r7 == 108 || r7 == 116 || r7 == 124 ||
            r7 == 132 || r7 == 140) {
    ram_int[0x5a] = 16;
  } else if(r7 == 5 || r7 == 13 || r7 == 21 || r7 == 29 ||
            r7 == 36 || r7 == 44 || r7 == 53 || r7 == 61 ||
            r7 == 69 || r7 == 77 || r7 == 85 || r7 == 93 ||
            r7 == 101 || r7 == 109 || r7 == 117 || r7 == 125 ||
            r7 == 133 || r7 == 141) {
    ram_int[0x5a] = 32;
  } else if(r7 == 6 || r7 == 14 || r7 == 22 || r7 == 30 ||
            r7 == 37 || r7 == 45 || r7 == 54 || r7 == 62 ||
            r7 == 70 || r7 == 78 || r7 == 86 || r7 == 94 ||
            r7 == 102 || r7 == 110 || r7 == 118 || r7 == 126 ||
            r7 == 134 || r7 == 142) {
    ram_int[0x5a] = 64;
  } else if(r7 == 7 || r7 == 15 || r7 == 23 || r7 == 31 ||
            r7 == 38 || r7 == 46 || r7 == 55 || r7 == 63 ||
            r7 == 71 || r7 == 79 || r7 == 87 || r7 == 95 ||
            r7 == 103 || r7 == 111 || r7 == 119 || r7 == 127 ||
            r7 == 135 || r7 == 143) {
    ram_int[0x5a] = 128;
  }

  if(23.4) {
    if(2c.4) {
      uint8_t off = ram_int[0x59];
      uint8_t mask = ram_int[0x5a];
      
      ram_int[0x9f+off] &= ~mask;
      ram_int[0xc3+off] &= ~mask;
      ram_int[0xb1+off] &= ~mask;
    } else {
      sub_c13c(0x0356);
    }
  }

  if(22.0) {
    if(2c.4) {
      uint8_t off = ram_int[0x59];
      uint8_t mask = ram_int[0x5a];
      
      ram_int[0x9f+off] &= ~mask;
      ram_int[0xc3+off] &= ~mask;
      ram_int[0xb1+off] &= ~mask;
    } else if(ram_ext[0x3c8] == 1) {
      ram_int[0x9f+off] |= mask;
    } else if(ram_ext[0x03c8] == 2) {
      ram_int[0xc3+off] |= mask;
    } else if(ram_ext[0x03c8] == 4) {
      ram_int[0xb1+off] |= mask;
    }
  }

  if(22.6) {
    if(ram_ext[0x0371] != 0x87) {
      sub_c139();
    }
    if(ram_ext[0x0371] == 0x79) {
      ram_int[0x59] = 0x10;
      ram_int[0x5a] = 0x80;
      sub_c139();
    }
  }
}

// address c139
void sub_c139(void)
{
  sub_c13c(0x0392);
}

// address c13c
void sub_c13c(uint16_t dptr)
{
  uint8_t r7 = ram_ext[dptr];
  uint8_t off = ram_int[0x59];
  uint8_t mask = ram_int[0x5a];

  if(r7 == 1) {
    ram_int[0xc3+off] |= mask;
  } else if(r7 == 2) {
    ram_int[0xc3+off] |= mask;
    ram_int[0xb1+off] |= mask;
  } else if(r7 == 3) {
    ram_int[0xb1+off] |= mask;
  } else if(r7 == 4) {
    ram_int[0x9f+off] |= mask;
    ram_int[0xb1+off] |= mask;
  } else if(r7 == 5) {
    ram_int[0x9f+off] |= mask;
  } else if(r7 == 6) {
    ram_int[0xc3+off] |= mask;
    ram_int[0x9f+off] |= mask;
  } else if(r7 == 7) {
    ram_int[0xc3+off] |= mask;
    ram_int[0x9f+off] |= mask;
    ram_int[0xb1+off] |= mask;
  } else if(r7 == 8) {
    uint8_t r1 = ram_ext[0x036f];
    if(r1 & 1) {ram_int[0xc3+off] |= mask;}
    if(r2 & 2) {ram_int[0x9f+off] |= mask;}
    if(r2 & 4) {ram_int[0xb1+off] |= mask;}
  }
}

// address d27c
void sub_d27c(void)
{
  if(!23.4) {return;}
  
  if(++ram_ext[0x039d] > ram_ext[0x0370]) {
    ram_ext[0x039d] = 0;
    ram_ext[0x0371] = 0;
    
    ram_ext[0x0371] = prog[0xc73e + ram_ext[0x0354]];
    sub_52c4();

    if(!2c.4) {
      if(++ram_ext[0x0354] > 113) {asm("SETB 2c.4");}
    } else {
      if(--ram_ext[0x0354] < 1) {asm("CLR 2c.4");}
    }
  }
}

// address 0042
void sub_0042(void)
{
  return;
}

// address df51
void sub_df51(void)
{
  uint8_t a = ram_int[0x7b]; // indirect internal RAM access
  if((a & 2) == 0) {
    asm("SETB 23.6");
  } else if(23.6) {
    asm("CLR 23.6");
    asm("CPL 22.2");
  }
}

// address a5c9
void sub_a5c9(void)
{
  if(!22.6) {return;}
  
  if(++ram_ext[0x0391] > ram_ext[0x038d]+5) {
    ram_ext[0x0391] = 0;

    sub_d9ef(1, 0x00);
    sub_d9ef(2, 0x00);
    sub_d9ef(3, 0x00);

    if(26.0) {
      if(++ram_ext[0x0304] > 0x18) {
        ram_ext[0x0304] = 0;
        asm("CLR 26.0");
        ram_ext[0x030e] = 0;
      } else {
        sub_5d17(ram_ext[0x0304], 0x030e);
      }
    }

    if(26.1) {
      if(++ram_ext[0x0305] > 0x18) {
        ram_ext[0x0305] = 0;
        asm("CLR 26.1");
        ram_ext[0x030f] = 0;
      } else {
        sub_5d17(ram_ext[0x0305], 0x030f);
      }
    }

    if(26.2) {
      if(++ram_ext[0x0306] > 0x18) {
        ram_ext[0x0306] = 0;
        asm("CLR 26.2");
        ram_ext[0x0310] = 0;
      } else {
        sub_5d17(ram_ext[0x0305], 0x0310);
      }
    }

    if(26.3) {
      if(++ram_ext[0x0307] > 0x18) {
        ram_ext[0x0307] = 0;
        asm("CLR 26.3");
        ram_ext[0x0311] = 0;
      } else {
        sub_5d17(ram_ext[0x0307], 0x0311);
      }
    }

    if(26.4) {
      if(++ram_ext[0x0308] > 0x18) {
        ram_ext[0x0308] = 0;
        asm("CLR 26.4");
        ram_ext[0x0313] = 0;
      } else {
        sub_5d17(ram_ext[0x0308], 0x0313);
      }
    }

    if(26.5) {
      if(++ram_ext[0x0309] > 0x18) {
        ram_ext[0x0309] = 0;
        asm("CLR 26.5");
        ram_ext[0x0315] = 0;
      } else {
        sub_5d17(ram_ext[0x0309], 0x0315);
      }
    }

    if(26.6) {
      if(++ram_ext[0x030a] > 0x18) {
        ram_ext[0x030a] = 0;
        asm("CLR 26.6");
        ram_ext[0x0317] = 0;
      } else {
        sub_5d17(ram_ext[0x030a], 0x0317);
      }
    }

    if(26.7) {
      if(++ram_ext[0x030b] > 0x18) {
        ram_ext[0x030b] = 0;
        asm("CLR 26.7");
        ram_ext[0x031a] = 0;
      } else {
        sub_5d17(ram_ext[0x030b], 0x031a);
      }
    }
  }
}

// address 5d17
void sub_5d17(uint8_t r7, uint16_t dptr)
{
  ram_ext[0x0330] = ram_ext[dptr];
  ram_ext[0x0318] = r7;

  uint8_t r6 = ram_ext[0x0330];
  if(r6 == 0 || r6 == 6 || r6 == 12 || r6 == 18 ||
     r6 == 24 || r6 == 30 || r6 == 36 || r6 == 42 ||
     r6 == 48 || r6 == 54 || r6 == 60 || r6 == 66 ||
     r6 == 72 || r6 == 78 || r6 == 84 || r6 == 90 ||
     r6 == 96 || r6 == 102 || r6 == 108) {
    uint8_t a = ram_ext[0x0318];
    if(a == 1) {
      sub_dae7(r6);
    } else if(a == 2) {
      sub_d5d4(r6);
    } else if(a == 3) {
      sub_cd3d(r6);
    } else if(a == 4) {
      sub_cbbe(r6);
    } else if(a == 5) {
      sub_bba7(r6);
    }

    if(a >= 6) {sub_8e67(r6, a-6, 0);}
    
    return;
  }

  if(r6 == 1 || r6 == 7 || r6 == 13 || r6 == 19 ||
     r6 == 25 || r6 == 31 || r6 == 37 || r6 == 43 ||
     r6 == 49 || r6 == 55 || r6 == 61 || r6 == 67 ||
     r6 == 73 || r6 == 79 || r6 == 85 || r6 == 91 ||
     r6 == 97 || r6 == 103 || r6 == 109 || r6 == 115 ||
     r6 == 121 || r6 == 127 || r6 == 133) {
    uint8_t a = ram_ext[0x0318];
    if(a == 1) {
      sub_da23(r6);
    } else if(a == 2) {
      sub_ca93(r6);
    } else if(a == 3) {
      sub_caf7(r6);
    } else if(a == 4) {
      sub_c012(r6);
    }

    if(a >= 5) {sub_8e67(r6, a-5, 1);}

    return;
  }

  if(r6 == 2 || r6 == 8 || r6 == 14 || r6 == 20 ||
     r6 == 26 || r6 == 32 || r6 == 38 || r6 == 44 ||
     r6 == 50 || r6 == 56 || r6 == 62 || r6 == 68 ||
     r6 == 74 || r6 == 80 || r6 == 86 || r6 == 92 ||
     r6 == 98 || r6 == 104 || r6 == 110 || r6 == 116 ||
     r6 == 122 || r6 == 128 || r6 == 134) {
    uint8_t a = ram_ext[0x0318];
    if(a == 1) {
      sub_db68(r6);
    } else if(a == 2) {
      sub_c7b1(r6);
    } else if(a == 3) {
      sub_c656(r6);
    }

    if(a >= 4) {sub_8e67(r6, a-4, 2);}

    return;
  }

  if(r6 == 3 || r6 == 9 || r6 == 15 || r6 == 21 ||
     r6 == 27 || r6 == 33 || r6 == 39 || r6 == 45 ||
     r6 == 51 || r6 == 57 || r6 == 63 || r6 == 69 ||
     r6 == 75 || r6 == 81 || r6 == 87 || r6 == 93 ||
     r6 == 99 || r6 == 105 || r6 == 111 || r6 == 117 ||
     r6 == 123 || r6 == 129 || r6 == 135) {
    uint8_t a = ram_ext[0x0318];
    if(a == 1) {
      sub_d656(r6);
    } else if(a == 2) {
      sub_c81f(r6);
    } else if(a == 3) {
      sub_d0a0(r6);
    }

    if(a >= 4) {sub_8e67(r6, a-4, 3);}

    return;
  }

  if(r6 == 4 || r6 == 10 || r6 == 16 || r6 == 22 ||
     r6 == 28 || r6 == 34 || r6 == 40 || r6 == 46 ||
     r6 == 52 || r6 == 58 || r6 == 64 || r6 == 70 ||
     r6 == 76 || r6 == 82 || r6 == 88 || r6 == 94 ||
     r6 == 100 || r6 == 106 || r6 == 112 || r6 == 118 ||
     r6 == 124 || r6 == 130 || r6 == 136) {
    uint8_t a = ram_ext[0x0318];
    if(a == 1) {
      sub_d2c8(r6);
    } else if(a == 2) {
      sub_cce2(r6);
    } else if(a == 3) {
      sub_c88c(r6);
    } else if(a == 4) {
      sub_c368(r6);
    }

    if(a >= 5) {sub_8e67(r6, a-5, 4);}

    return;
  }

  if(r6 == 5 || r6 == 11 || r6 == 17 || r6 == 23 ||
     r6 == 29 || r6 == 35 || r6 == 41 || r6 == 47 ||
     r6 == 53 || r6 == 59 || r6 == 65 || r6 == 71 ||
     r6 == 77 || r6 == 83 || r6 == 89 || r6 == 95 ||
     r6 == 101 || r6 == 107 || r6 == 113 || r6 == 119 ||
     r6 == 125 || r6 == 131 || r6 == 137) {
    uint8_t a = ram_ext[0x0218];
    if(a == 1) {
      sub_da57(r6);
    } else if(a == 2) {
      sub_d7bf(r6);
    } else if(a == 3) {
      sub_c1cc(r6);
    } else if(a == 4) {
      sub_d0f2(r6);
    } else if(a == 5) {
      sub_cef8(r6);
    } else if(a == 6) {
      sub_c8f6(r6);
    } else if(a == 7) {
      sub_c5de(r6);
    }

    if(a >= 7) {sub_8e67(r6, a-5, 5);}

    return;
  }
}

// address 83f4
void sub_83f4(uint8_t r7, uint16_t r4r5)
{
  ram_ext[0x0302] = 0xaa;
  
  if(r7 == r4r5+0x0042) {
    ram_ext[0x0302] = 0;
  } else if(r7 == r4r5+0x0048) {
    ram_ext[0x0302] = 1;
  } else if(r7 == r4r5+0x004e) {
    ram_ext[0x0302] = 2;
  } else if(r7 == r4r5+0x0054) {
    ram_ext[0x0302] = 3;
  } else if(r7 == r4r5+0x005a) {
    ram_ext[0x0302] = 4;
  } else if(r7 == r4r5+0x0060) {
    ram_ext[0x0302] = 5;
  } else if(r7 == r4r5+0x0066) {
    ram_ext[0x0302] = 6;
  } else if(r7 == r4r5+0x006c) {
    ram_ext[0x0302] = 7;
  } else if(r7 == r4r5+0x0072) {
    ram_ext[0x0302] = 8;
  } else if(r7 == r4r5+0x0078) {
    ram_ext[0x0302] = 9;
  } else if(r7 == r4r5+0x007e) {
    ram_ext[0x0302] = 10;
  } else if(r7 == r4r5+0x0084) {
    ram_ext[0x0302] = 11;
  } else if(r7 == r4r5+0x008a) {
    ram_ext[0x0302] = 12;
  } else if(r7 == r4r5+0x003c) {
    ram_ext[0x0302] = -1;
  } else if(r7 == r4r5+0x0036) {
    ram_ext[0x0302] = -2;
  } else if(r7 == r4r5+0x0030) {
    ram_ext[0x0302] = -3;
  } else if(r7 == r4r5+0x002a) {
    ram_ext[0x0302] = -4;
  } else if(r7 == r4r5+0x0024) {
    ram_ext[0x0302] = -5;
  } else if(r7 == r4r5+0x001e) {
    ram_ext[0x0302] = -6;
  } else if(r7 == r4r5+0x0018) {
    ram_ext[0x0302] = -7;
  } else if(r7 == r4r5+0x0012) {
    ram_ext[0x0302] = -8;
  } else if(r7 == r4r5+0x000c) {
    ram_ext[0x0302] = -9;
  } else if(r7 == r4r5+0x0006) {
    ram_ext[0x0302] = -10;
  }
}

// address 83f0
void sub_83f0(uint8_t r7)
{
  sub_83f4(r7, 0x0005);
}

// address dc73
void sub_dc73(void)
{
  ram_ext[0x0371] += ram_ext[0x0302];
  if(ram_ext[0x0371] > 1 && ram_ext[0x0371] < 23) {sub_52c4();}
}

// address dc6d
void sub_dc6d(void)
{
  ram_ext[0x0371] = 12;
  sub_dc73();
}

// address dc94
void sub_dc94(void)
{
  ram_ext[0x0371] += ram_ext[0x0302];
  if(ram_ext[0x0371] < 47 && ram_ext[0x0371] > 24) {sub_52c4();}
}

// address dcb5
void sub_dcb5(void)
{
  ram_ext[0x0371] += ram_ext[0x0302];
  if(ram_ext[0x0371] < 71 && ram_ext[0x0371] > 48) {sub_52c4();}
}

// address dcd6
void sub_dcd6(void)
{
  ram_ext[0x0371] += ram_ext[0x0302];
  if(ram_ext[0x0371] < 95 && ram_ext[0x0371] > 72) {sub_52c4();}
}

// address dcd0
void sub_dcd0(void)
{
  ram_ext[0x0371] = 82;
  sub_dcd6();
}

// address dcf7
void sub_dcf7(void)
{
  ram_ext[0x0371] += ram_ext[0x0302];
  if(ram_ext[0x0371] < 119 && ram_ext[0x0371] > 96) {sub_52c4();}
}

// address dcf1
void sub_dcf1(void)
{
  ram_ext[0x0371] = 104;
  sub_dcf7();
}

// address dd18
void sub_dd18(void)
{
  ram_ext[0x0371] += ram_ext[0x0302];
  if(ram_ext[0x0371] < 143 && ram_ext[0x0371] > 120) {sub_52c4();}
}

// address dd12
void sub_dd12(void)
{
  ram_ext[0x0371] = 130;
  sub_dd18();
}

// address bba7
void sub_bba7(uint8_t r7)
{
  sub_83f4(r7, 0x0000);
  
  if(ram_ext[0x0302] != 0xaa) {
    ram_ext[0x0371] = 7;
    sub_dc73();

    ram_ext[0x0371] = 15;
    sub_dc73();

    ram_ext[0x0371] = 30;
    sub_dc94();

    ram_ext[0x0371] = 39;
    sub_dc94();

    ram_ext[0x0371] = 54;
    sub_dcb5();

    ram_ext[0x0371] = 62;
    sub_dcb5();

    ram_ext[0x0371] = 79;
    sub_dcd6();

    ram_ext[0x0371] = 85;
    sub_dcd6();

    ram_ext[0x0371] = 86;
    sub_dcd6();

    ram_ext[0x0371] = 103;
    sub_dcf7();

    ram_ext[0x0371] = 108;
    sub_dcf7();

    ram_ext[0x0371] = 109;
    sub_dcf7();

    ram_ext[0x0371] = 127;
    sub_dd18();

    ram_ext[0x0371] = 128;
    sub_dd18();

    ram_ext[0x0371] = 129;
    sub_dd18();

    sub_dd12();

    ram_ext[0x0371] = 131;
    sub_dd18();

    ram_ext[0x0371] = 132;
    sub_dd18();
  }
}

// address c012
void sub_c012(uint8_t r7)
{
  sub_83f4(r7, 0x0001);
  
  if(ram_ext[0x0302] != 0xaa) {
    ram_ext[0x0371] = 9;
    sub_dc73();

    ram_ext[0x0371] = 14;
    sub_dc73();

    ram_ext[0x0371] = 32;
    sub_dc94();

    ram_ext[0x0371] = 38;
    sub_dc94();

    ram_ext[0x0371] = 55;
    sub_dcb5();

    ram_ext[0x0371] = 62;
    sub_dcb5();

    ram_ext[0x0371] = 79;
    sub_dcd6();

    ram_ext[0x0371] = 86;
    sub_dcd6();

    ram_ext[0x0371] = 103;
    sub_dcf7();

    ram_ext[0x0371] = 109;
    sub_dcf7();

    ram_ext[0x0371] = 128;
    sub_dd18();

    ram_ext[0x0371] = 129;
    sub_dd18();

    sub_dd12();

    ram_ext[0x0371] = 131;
    sub_dd18();

    ram_ext[0x0371] = 132;
    sub_dd18();
  }
}

// address c1cc
void sub_c1cc(uint8_t r7)
{
  sub_83f0(r7);

  if(ram_ext[0x0302] != 0xaa) {
    ram_ext[0x0371] = 59 + ram_ext[0x0302];
    if(ram_ext[0x0371] < 71 && ram_ext[0x0371] > 48) {sub_52c4();}

    ram_ext[0x0371] = 60 + ram_ext[0x0302];
    if(ram_ext[0x0371] < 71 && ram_ext[0x0371] > 48) {sub_52c4();}

    ram_ext[0x0371] = 61 + ram_ext[0x0302];
    if(ram_ext[0x0371] < 71 && ram_ext[0x0371] > 48) {sub_52c4();}

    sub_dcd0();

    ram_ext[0x0371] = 86;
    sub_dcd6();

    ram_ext[0x0371] = 105;
    sub_dcf7();

    ram_ext[0x0371] = 110;
    sub_dcf7();

    sub_dd12();

    ram_ext[0x0317] = 133;
    sub_dd18();
  }
}

// address c368
void sub_c368(uint8_t r7)
{
  sub_83f4(r7, 0x0004);

  if(ram_ext[0x0302] != 0xaa) {
    ram_ext[0x0371] = 11;
    sub_dc73();

    sub_dc6d();

    ram_ext[0x0371] = 13;
    sub_dc73();

    ram_ext[0x0371] = 14;
    sub_dc73();

    ram_ext[0x0371] = 34;
    sub_dc94();

    ram_ext[0x0371] = 39;
    sub_dc94();

    ram_ext[0x0371] = 57;
    sub_dcb5();

    ram_ext[0x0371] = 63;
    sub_dcb5();

    ram_ext[0x0371] = 81;
    sub_dcd6();

    ram_ext[0x0371] = 87;
    sub_dcd6();

    sub_dcf1();

    ram_ext[0x0371] = 111;
    sub_dcf7();

    ram_ext[0x0371] = 128;
    sub_dd18();

    ram_ext[0x0371] = 133;
    sub_dd18();
  }
}

// address c5de
void sub_c5de(uint8_t r7)
{
  sub_83f0(r7);

  if(ram_ext[0x0302] != 0xaa) {
    ram_ext[0x0371] = 9;
    sub_dc73();

    ram_ext[0x0371] = 16;
    sub_dc73();

    ram_ext[0x0371] = 32;
    sub_dc94();

    ram_ext[0x0371] = 41;
    sub_dc94();

    ram_ext[0x0371] = 55;
    sub_dcb5();

    ram_ext[0x0371] = 65;
    sub_dcb5();

    ram_ext[0x0371] = 79;
    sub_dcd6();

    ram_ext[0x0371] = 89;
    sub_dcd6();

    ram_ext[0x0371] = 103;
    sub_dcf7();

    ram_ext[0x0371] = 112;
    sub_dcf7();

    ram_ext[0x0371] = 128;
    sub_dd18();

    ram_ext[0x0371] = 135;
    sub_dd18();
  }
}

// address c656
void sub_c656(uint8_t r7)
{
  sub_83f4(r7, 0x0002);

  if(ram_ext[0x0302] != 0xaa) {
    ram_ext[0x0371] = 10;
    sub_dc73();

    ram_ext[0x0371] = 14;
    sub_dc73();

    ram_ext[0x0371] = 33;
    sub_dc94();

    ram_ext[0x0371] = 38;
    sub_dc94();

    ram_ext[0x0371] = 56;
    sub_dcb5();

    ram_ext[0x0371] = 62;
    sub_dcb5();

    ram_ext[0x0371] = 80;
    sub_dcd6();

    ram_ext[0x0371] = 85;
    sub_dcd6();

    sub_dcf1();

    ram_ext[0x0371] = 108;
    sub_dcf7();

    sub_dd14(129);

    sub_dd12();

    ram_ext[0x0371] = 131;
    sub_dd18();
  }
}

// address c7b1
void sub_c7b1(uint8_t r7)
{
  sub_83f4(r7, 0x0002);

  if(ram_ext[0x0302] != 0xaa) {
    sub_dc6f(11);

    sub_dc6d();

    ram_ext[0x0371] = 13;
    sub_dc73();

    ram_ext[0x0371] = 34;
    sub_dc94();

    ram_ext[0x0371] = 37;
    sub_dc94();

    ram_ext[0x0371] = 57;
    sub_dcb5();

    ram_ext[0x0371] = 61;
    sub_dcb5();

    ram_ext[0x0371] = 81;
    sub_dcd6();

    ram_ext[0x0371] = 84;
    sub_dcd6();

    sub_dcf3(105);

    ram_ext[0x0371] = 106;
    sub_dcf7();

    ram_ext[0x0371] = 107;
    sub_dcf7();
  }
}

// address c81f
void sub_c81f(uint8_t r7)
{
  sub_83f4(r7, 0x0003);

  if(ram_ext[0x0302] != 0xaa) {
    ram_ext[0x0371] = 35;
    sub_dc94();

    sub_dc8e();

    ram_ext[0x0371] = 37;
    sub_dc94();

    sub_dcaf();

    ram_ext[0x0371] = 61;
    sub_dcb5();

    ram_ext[0x0371] = 81;
    sub_dcd6();

    ram_ext[0x0371] = 85;
    sub_dcd6();

    sub_dcf1();

    ram_ext[0x0371] = 105;
    sub_dcf7();

    ram_ext[0x0371] = 108;
    sub_dcf7();

    ram_ext[0x0371] = 109;
    sub_dcf7();

    ram_ext[0x0371] = 129;
    sub_dd18();

    sub_dd12();
  }
}

// address c88c
void sub_c88c(uint8_t r7)
{
  sub_83f4(0x0004);

  if(ram_ext[0x0302] != 0xaa) {
    ram_ext[0x0371] = 35;
    sub_dc94();

    sub_dc8e();

    ram_ext[0x0371] = 37;
    sub_dc94();

    ram_ext[0x0371] = 38;
    sub_dc94();

    sub_dcaf();

    ram_ext[0x0371] = 62;
    sub_dcb5();

    ram_ext[0x0371] = 81;
    sub_dcd6();

    ram_ext[0x0371] = 86;
    sub_dcd6();

    sub_dcf1();

    ram_ext[0x0371] = 110;
    sub_dcf7();

    ram_ext[0x0371] = 127;
    sub_dd18();

    ram_ext[0x0371] = 132;
    sub_dd18();
  }
}

// address c8f6
void sub_c8f6(uint8_t r7)
{
  sub_83f0(r7);

  if(ram_ext[0x0320] != 0xaa) {
    ram_ext[0x0371] = 10;
    sub_dc73();

    ram_ext[0x0371] = 33;
    sub_dc94();

    ram_ext[0x0371] = 40;
    sub_dc94();

    ram_ext[0x0371] = 56;
    sub_dcb5();

    ram_ext[0x0371] = 64;
    sub_dcb5();

    ram_ext[0x0371] = 80;
    sub_dcd6();

    ram_ext[0x0371] = 88;
    sub_dcd6();

    sub_dcf1();

    ram_ext[0x0371] = 111;
    sub_dcf7();

    ram_ext[0x0371] = 129;
    sub_dd18();

    ram_ext[0x0371] = 134;
    sub_dd18();
  }
}

// address ca93
void sub_ca93(uint8_t r7)
{
  sub_83f4(r7, 0x0001);

  if(ram_ext[0x0302] != 0xaa) {
    ram_ext[0x0371] = 10;
    sub_dc73();

    ram_ext[0x0371] = 13;
    sub_dc73();
    
    ram_ext[0x0371] = 33;
    sub_dc94();

    ram_ext[0x0371] = 37;
    sub_dc94();

    ram_ext[0x0371] = 57;
    sub_dcb5();

    ram_ext[0x0371] = 60;
    sub_dcb5();

    ram_ext[0x0371] = 81;
    sub_dcd6();

    sub_dcd0();

    ram_ext[0x0371] = 83;
    sub_dcd6();

    ram_ext[0x0371] = 84;
    sub_dcd6();
  }
}

// address caf7
void sub_caf7(uint8_t r7)
{
  sub_83f4(r7, 0x0001);

  if(ram_ext[0x0302] != 0xaa) {
    ram_ext[0x0371] = 56;
    sub_dcb5();

    ram_ext[0x0371] = 61;
    sub_dcb5();

    ram_ext[0x0371] = 80;
    sub_dcd6();

    ram_ext[0x0371] = 85;
    sub_dcd6();

    ram_ext[0x0371] = 84;
    sub_dcd6();

    sub_dcf1();

    ram_ext[0x0371] = 105;
    sub_dcf7();

    ram_ext[0x0371] = 106;
    sub_dcf7();

    ram_ext[0x0371] = 107;
    sub_dcf7();

    ram_ext[0x0371] = 108;
    sub_dcf7();
  }
}

// address cbbe
void sub_cbbe(uint8_t r7)
{
  sub_83f4(r7, 0x0000);

  if(ram_ext[0x0302] != 0xaa) {
    ram_ext[0x0371] = 31;
    sub_dc94();

    ram_ext[0x0371] = 38;
    sub_dc94();

    ram_ext[0x0371] = 55;
    sub_dcb5();

    ram_ext[0x0371] = 61;
    sub_dcb5();

    ram_ext[0x0371] = 80;
    sub_dcd6();

    ram_ext[0x0371] = 84;
    sub_dcd6();

    sub_dcf1();

    ram_ext[0x0371] = 105;
    sub_dcf7();

    ram_ext[0x0371] = 106;
    sub_dcf7();

    ram_ext[0x0371] = 107;
    sub_dcf7();
  }
}

// address cce2
void sub_cce2(uint8_t r7)
{
  sub_83f4(r7, 0x0004);

  if(ram_ext[0x0302] != 0xaa) {
    ram_ext[0x0371] = 59;
    sub_dcb5();

    ram_ext[0x0371] = 60;
    sub_dcb5();

    ram_ext[0x0371] = 61;
    sub_dcb5();

    sub_dcd0();

    ram_ext[0x0371] = 85;
    sub_dcd6();

    ram_ext[0x0371] = 105;
    sub_dcf7();

    ram_ext[0x0371] = 109;
    sub_dcf7();

    ram_ext[0x0371] = 126;
    sub_dd18();

    ram_ext[0x0371] = 131;
    sub_dd18();
  }
}

// address cd3d
void sub_cd3d(uint8_t r7)
{
  sub_83f4(r7, 0x0000);

  if(ram_ext[0x0302] != 0xaa) {
    ram_ext[0x0371] = 8;
    sub_dc73();

    ram_ext[0x0371] = 14;
    sub_dc73();

    ram_ext[0x0371] = 32;
    sub_dc94();

    ram_ext[0x0371] = 37;
    sub_dc94();

    ram_ext[0x0371] = 56;
    sub_dcb5();

    ram_ext[0x0371] = 60;
    sub_dcb5();

    ram_ext[0x0371] = 81;
    sub_dcd6();

    sub_dcd0();

    ram_ext[0x0371] = 83;
    sub_dcd6();
  }
}

// address cef8
void sub_cef8(uint8_t r7)
{
  sub_83f0(r7);

  if(ram_ext[0x0302] != 0xaa) {
    ram_ext[0x0371] = 11;
    sub_dc73();

    sub_dc6d();

    ram_ext[0x0371] = 13;
    sub_dc73();

    ram_ext[0x0371] = 14;
    sub_dc73();

    ram_ext[0x0371] = 15;
    sub_dc73();

    ram_ext[0x0371] = 34;
    sub_dc94();

    ram_ext[0x0371] = 39;
    sub_dc94();

    ram_ext[0x0371] = 57;
    sub_dcb5();

    ram_ext[0x0371] = 63;
    sub_dcb5();
  }
}

// address d0a0
void sub_d0a0(uint8_t r7)
{
  sub_83f4(r7, 0x0003);

  if(ram_ext[0x0302] != 0xaa) {
    ram_ext[0x0371] = 11;
    sub_dc73();

    sub_dc6d();

    ram_ext[0x0371] = 13;
    sub_dc73();

    ram_ext[0x0371] = 14;
    sub_dc73();

    ram_ext[0x0371] = 34;
    sub_dc94();

    ram_ext[0x0371] = 38;
    sub_dc94();

    ram_ext[0x0371] = 57;
    sub_dcb5();

    ram_ext[0x0371] = 62;
    sub_dcb5();
  }
}

// address d0f2
void sub_d0f2(uint8_t r7)
{
  sub_83f0(r7);

  if(ram_ext[0x0302] != 0xaa) {
    ram_ext[0x0371] = 35;
    sub_dc94();

    sub_dc8e();

    ram_ext[0x0371] = 37;
    sub_dc94();

    ram_ext[0x0371] = 38;
    sub_dc94();

    sub_dcaf();

    ram_ext[0x0371] = 62;
    sub_dcb5();

    ram_ext[0x0371] = 81;
    sub_dcd6();

    ram_ext[0x0371] = 87;
    sub_dcd6();

    ram_ext[0x0371] = 111;
    sub_dcf7();
  }
}

// address d2c8
void sub_d2c8(uint8_t r7)
{
  sub_83f4(r7, 0x0004);

  if(ram_ext[0x0302] != 0xaa) {
    sub_dcd0();

    ram_ext[0x0371] = 83;
    sub_dcd6();

    ram_ext[0x0371] = 84;
    sub_dcd6();

    ram_ext[0x0371] = 106;
    sub_dcf7();

    ram_ext[0x0371] = 108;
    sub_dcf7();

    ram_ext[0x0371] = 129;
    sub_dd18();

    sub_dd12();

    ram_ext[0x0371] = 131;
    sub_dd18();
  }
}

// address d5d4
void sub_d5d4(uint8_t r7)
{
  sub_83f4(r7, 0x0000);

  if(ram_ext[0x0302] != 0xaa) {
    ram_ext[0x0371] = 9;
    sub_dc73();

    ram_ext[0x0371] = 13;
    sub_dc73();

    ram_ext[0x0371] = 33;
    sub_dc94();

    sub_dc8e();

    ram_ext[0x0371] = 57;
    sub_dcb5();

    sub_dcaf();

    ram_ext[0x0371] = 59;
    sub_dcb5();
  }
}

// address d656
void sub_d656(uint8_t r7)
{
  sub_83f4(r7, 0x0003);

  if(ram_ext[0x0302] != 0xaa) {
    ram_ext[0x0371] = 59;
    sub_dcb5();

    ram_ext[0x0371] = 60;
    sub_dcb5();

    sub_dcd0();

    ram_ext[0x0371] = 84;
    sub_dcd6();

    ram_ext[0x0371] = 106;
    sub_dcf7();

    ram_ext[0x0371] = 107;
    sub_dcf7();
  }
}

// address d7bf
void sub_d7bf(uint8_t r7)
{
  sub_83f0(r7);

  if(ram_ext[0x0302] != 0xaa) {
    ram_ext[0x0371] = 83;
    sub_dcd6();

    ram_ext[0x0371] = 84;
    sub_dcd6();

    ram_ext[0x0371] = 85;
    sub_dcd6();

    ram_ext[0x0371] = 106;
    sub_dcf7();

    ram_ext[0x0371] = 109;
    sub_dcf7();
  }
}

// address da23
void sub_da23(uint8_t r7)
{
  sub_83f4(r7, 0x0001);

  if(ram_ext[0x0302] != 0xaa) {
    ram_ext[0x0371] = 11;
    sub_dc73();

    sub_dc6d();

    ram_ext[0x0371] = 34;
    sub_dc94();

    sub_dc8e();

    sub_dcaf();

    ram_ext[0x0371] = 59;
    sub_dcb5();
  }
}

// address da57
void sub_da57(uint8_t r7)
{
  sub_83f0(r7);

  if(ram_ext[0x0371] != 0xaa) {
    ram_ext[0x0371] = 107;
    sub_dcf7();

    ram_ext[0x0371] = 108;
    sub_dcf7();

    sub_dd12();

    ram_ext[0x0371] = 131;
    sub_dd18();

    ram_ext[0x0371] = 132;
    sub_dd18();
  }
}

// address dae7
void sub_dae7(uint8_t r7)
{
  sub_83f4(r7, 0x0000);

  if(ram_ext[0x0302] != 0xaa) {
    ram_ext[0x0371] = 10;
    sub_dc73();

    sub_dc6d();

    ram_ext[0x0371] = 34;
    sub_dc94();

    ram_ext[0x0371] = 35;
    sub_dc94();
  }
}

// address db68
void sub_db68(uint8_t r7)
{
  sub_83f4(r7, 0x0002);

  if(ram_ext[0x0302] != 0xaa) {
    sub_dc8e();

    sub_dc90(35);

    sub_dcb1(60);

    sub_dcaf();

    sub_dcd0();

    sub_dcd2(83);
  }
}

// address 8e67
void sub_8e67(uint8_t r5, uint8_t r2r3, uint8_t r7) // r2r3 held in 4b:4c
{
  ram_int[0x4b] = r2r3 >> 8;
  ram_int[0x4c] = r2r3 & 0xff;
  
  if(r7 == 0) {
    ram_int[0x4d] = 14;
    ram_int[0x4e] = 6;
    ram_int[0x4f] = 0x27;
    ram_int[0x50] = 0x1d;
    ram_int[0x51] = 0x3f;
    ram_int[0x52] = 0x35;
    ram_int[0x53] = 0x57;
    ram_int[0x54] = 0x4e;
    ram_int[0x55] = 0x6e;
    ram_int[0x56] = 0x66;
    ram_int[0x57] = 0x85;
    ram_int[0x58] = 0x7e;
  } else if(r7 == 1) {
    ram_int[0x4d] = 15;
    ram_int[0x4e] = 8;
    ram_int[0x4f] = 0x27;
    ram_int[0x50] = 0x1f;
    ram_int[0x51] = 0x3f;
    ram_int[0x52] = 0x36;
    ram_int[0x53] = 0x57;
    ram_int[0x54] = 0x4e;
    ram_int[0x55] = 0x6e;
    ram_int[0x56] = 0x66;
    ram_int[0x57] = 0x85;
    ram_int[0x58] = 0x7f;
  } else if(r7 == 2) {
    ram_int[0x4d] = 14;
    ram_int[0x4e] = 9;
    ram_int[0x4f] = 0x27;
    ram_int[0x50] = 0x20;
    ram_int[0x51] = 0x3f;
    ram_int[0x52] = 0x37;
    ram_int[0x53] = 0x56;
    ram_int[0x54] = 0x4f;
    ram_int[0x55] = 0x6d;
    ram_int[0x56] = 0x67;
    ram_int[0x57] = 0x84;
    ram_int[0x58] = 0x80;
  } else if(r7 == 3) {
    ram_int[0x4d] = 15;
    ram_int[0x4e] = 10;
    ram_int[0x4f] = 0x27;
    ram_int[0x50] = 0x21;
    ram_int[0x51] = 0x3f;
    ram_int[0x52] = 0x38;
    ram_int[0x53] = 0x56;
    ram_int[0x54] = 0x50;
    ram_int[0x55] = 0x6c;
    ram_int[0x56] = 0x67;
    ram_int[0x57] = 0x83;
    ram_int[0x58] = 0x80;
  } else if(r7 == 4 || r7 == 5) {
    ram_int[0x4d] = 15;
    ram_int[0x4e] = 10;
    ram_int[0x4f] = 0x28;
    ram_int[0x50] = 0x21;
    ram_int[0x51] = 0x40;
    ram_int[0x52] = 0x38;
    ram_int[0x53] = 0x58;
    ram_int[0x54] = 0x50;
    ram_int[0x55] = 0x6f;
    ram_int[0x56] = 0x68;
    ram_int[0x57] = 0x86;
    ram_int[0x58] = 0x81;
  }

  sub_83f4(r5, r7);

  if(ram_ext[0x0302] != 0xaa) {
    ram_ext[0x0371] = ram_ext[0x0302] + ram_int[0x4d] + ram_int[0x4c];
    sub_dedf();

    ram_ext[0x0371] = ram_ext[0x0302] + ram_int[0x4e] - ram_int[0x4c];
    sub_dedf();

    ram_ext[0x0371] = ram_ext[0x0302] + ram_int[0x4f] + ram_int[0x4c];
    sub_def2();

    ram_ext[0x0371] = ram_ext[0x0302] + ram_int[0x50] - ram_int[0x4c];
    sub_def2();

    ram_ext[0x0371] = ram_ext[0x0302] + ram_int[0x51] + ram_int[0x4c];
    sub_df05();

    ram_ext[0x0371] = ram_ext[0x0302] + ram_int[0x52] - ram_int[0x4c];
    sub_df05();

    ram_ext[0x0371] = ram_ext[0x0302] + ram_int[0x53] + ram_int[0x4c];
    sub_df18();

    ram_ext[0x0371] = ram_ext[0x0302] + ram_int[0x54] - ram_int[0x4c];
    sub_df18();

    ram_ext[0x0371] = ram_ext[0x0302] + ram_int[0x55] + ram_int[0x4c];
    sub_df2b();

    ram_ext[0x0371] = ram_ext[0x0302] + ram_int[0x56] - ram_int[0x4c];
    sub_df2b();

    ram_ext[0x0371] = ram_ext[0x0302] + ram_int[0x57] + ram_int[0x4c];
    sub_df3e();

    ram_ext[0x0371] = ram_ext[0x0302] + ram_int[0x58] - ram_int[0x4c];
    sub_df3e();
  }
}

// address df3e
void sub_df3e(void)
{
  if(ram_ext[0x0371] < 143 && ram_ext[0x0371] > 120) {sub_52c4();}
}

// address df2b
void sub_df2b(void)
{
  if(ram_ext[0x0371] < 119 && ram_ext[0x0371] > 96) {sub_52c4();}
}

// address df18
void sub_df18(void)
{
  if(ram_ext[0x0371] < 95 && ram_ext[0x0371] > 72) {sub_52c4();}
}

// address df05
void sub_df05(void)
{
  if(ram_ext[0x0371] < 71 && ram_ext[0x0371] > 48) {sub_52c4();}
}

// address def2
void sub_def2(void)
{
  if(ram_ext[0x0371] < 47 && ram_ext[0x0371] > 24) {sub_52c4();}
}

// address dedf
void sub_dedf(void)
{
  if(ram_ext[0x0371] > 1 && ram_ext[0x0371] < 23) {sub_52c4();}
}

// address b672
void sub_b672(void)
{
  if(!20.5) {return;}

  if(ram_int[0x64] & 1) { // indirect internal RAM access
    if(24.2) {
      asm("CLR 24.2");

      sub_d9ef(1, 0x00);
      sub_d9ef(2, 0x00);
      sub_d9ef(3, 0x00);

      ram_ext[0x01a9] = 0;
      ram_ext[0x01ac] = 0;
      ram_ext[0x01ae] = 0;
      ram_ext[0x01af] = 0;
      ram_ext[0x01b0] = 0;
      ram_ext[0x01b1] = 0;

      sub_b24d();
      erase_eeprom_sectors_012();

      ram_ext[0x0356] = 8;
      ram_ext[0x0392] = 8;
      ram_ext[0x0397] = 8;

      ram_int[0x09] = 4;

      ram_ext[0x03b6] = 0xe6;
      ram_ext[0x021d] = 0x3c;
      ram_ext[0x0370] = 4;
      ram_ext[0x0332] = 2;
      ram_ext[0x038d] = 0x1e;

      asm("CLR 25.3");
      ram_ext[0x03cc] = 1;

      sub_c9c8();

      asm("SETB 22.0");
      asm("SETB 25.1");
      asm("CLR 22.2");
      asm("CLR 23.3");
      ram_ext[0x03cb] = 1;
      asm("SETB 21.0");

      sub_c0a6(1);

      ram_int[0x09] = 4;
      
      sub_d506(ram_int[0x09]);
      delay(0x0212);

      sub_d506(ram_int[0x09]);
      delay(0x0212);

      program_eeprom_sector_6();
    }
  } else {
    asm("SETB 24.2");
  }

  if(!24.3) {
    asm("SETB 24.1");
  } else if(24.1) {
    asm("CLR 24.1");
    sub_b24d();
  }
}

// address 8c5e
void program_eeprom_sector_6(void)
{
  uint8_t r4 = 0;
  uint8_t r5 = 0;
  if(21.0) {
    r4 = 1;
  } else if(22.7) {
    r4 = 3;
  } else if(21.5) {
    r4 = 2;
  } else if(23.2) {
    r4 = 4;
  } else if(22.4) {
    r4 = 5;
  } else if(22.3) {
    r4 = 6;
  } else if(23.4) {
    r4 = 7;
  } else if(22.6) {
    r4 = 8;
  } else if(20.7) {
    r4 = 9;
  } else if(23.0) {
    r4 = 10;
  } else if(22.1) {
    r4 = 11;
  }

  if(2c.5) {r5 = 1;}

  // start programming at 0x0600
  ram_int[0x0e] = 6;
  ram_int[0x0f] = 0;
  
  asm("SETB 2c.1");
  ram_int[0x4c] = 6;
  erase_sector();

  asm("SETB 2c.1");
  ram_int[0x4c] = ram_int[0x0e];
  ram_int[0x4d] = ram_int[0x0f];
  ram_int[0x4c] = r4;
  program_byte();
  if(++ram_int[0x0f] == 0) {++ram_int[0x0e];}

  asm("SETB 2c.1");
  ram_int[0x4c] = ram_int[0x0e];
  ram_int[0x4d] = ram_int[0x0f];
  ram_int[0x4e] = ram_int[0x09];
  program_byte();
  if(++ram_int[0x0f] == 0) {++ram_int[0x0e];}

  asm("SETB 2c.1");
  ram_int[0x4c] = ram_int[0x0e];
  ram_int[0x4d] = ram_int[0x0f];
  ram_int[0x4e] = ram_ext[0x03b6];
  program_byte();
  if(++ram_int[0x0f] == 0) {++ram_int[0x0e];}

  asm("SETB 2c.1");
  ram_int[0x4c] = ram_int[0x0e];
  ram_int[0x4d] = ram_int[0x0f];
  ram_int[0x4e] = ram_ext[0x03cb];
  program_byte();
  if(++ram_int[0x0f] == 0) {++ram_int[0x0e];}

  asm("SETB 2c.1");
  ram_int[0x4c] = ram_int[0x0e];
  ram_int[0x4d] = ram_int[0x0f];
  ram_int[0x4e] = r5;
  program_byte();
  if(++ram_int[0x0f] == 0) {++ram_int[0x0e];}

  asm("SETB 2c.1");
  ram_int[0x4c] = ram_int[0x0e];
  ram_int[0x4d] = ram_int[0x0f];
  ram_int[0x4e] = ram_ext[0x0331];
  program_byte();
  if(++ram_int[0x0f] == 0) {++ram_int[0x0e];}

  asm("SETB 2c.1");
  ram_int[0x4c] = ram_int[0x0e];
  ram_int[0x4d] = ram_int[0x0f];
  ram_int[0x4e] = ram_ext[0x0356];
  program_byte();
  if(++ram_int[0x0f] == 0) {++ram_int[0x0e];}

  asm("SETB 2c.1");
  ram_int[0x4c] = ram_int[0x0e];
  ram_int[0x4d] = ram_int[0x0f];
  ram_int[0x4e] = ram_ext[0x0392];
  program_byte();
  if(++ram_int[0x0f] == 0) {++ram_int[0x0e];}

  asm("SETB 2c.1");
  ram_int[0x4c] = ram_int[0x0e];
  ram_int[0x4d] = ram_int[0x0f];
  ram_int[0x4e] = ram_ext[0x0397];
  program_byte();
  if(++ram_int[0x0f] == 0) {++ram_int[0x0e];}

  asm("SETB 2c.1");
  ram_int[0x4c] = ram_int[0x0e];
  ram_int[0x4d] = ram_int[0x0f];
  ram_int[0x4e] = ram_ext[0x03b4];
  program_byte();
  if(++ram_int[0x0f] == 0) {++ram_int[0x0e];}

  asm("SETB 2c.1");
  ram_int[0x4c] = ram_int[0x0e];
  ram_int[0x4d] = ram_int[0x0f];
  ram_int[0x4e] = ram_ext[0x03cc];
  program_byte();
  if(++ram_int[0x0f] == 0) {++ram_int[0x0e];}

  asm("SETB 2c.1");
  ram_int[0x4c] = ram_int[0x0e];
  ram_int[0x4d] = ram_int[0x0f];
  ram_int[0x4e] = ram_ext[0x021d];
  program_byte();
  if(++ram_int[0x0f] == 0) {++ram_int[0x0e];}

  asm("SETB 2c.1");
  ram_int[0x4c] = ram_int[0x0e];
  ram_int[0x4d] = ram_int[0x0f];
  ram_int[0x4e] = ram_ext[0x0370];
  program_byte();
  if(++ram_int[0x0f] == 0) {++ram_int[0x0e];}

  asm("SETB 2c.1");
  ram_int[0x4c] = ram_int[0x0e];
  ram_int[0x4d] = ram_int[0x0f];
  ram_int[0x4e] = ram_ext[0x0332];
  program_byte();
  if(++ram_int[0x0f] == 0) {++ram_int[0x0e];}

  asm("SETB 2c.1");
  ram_int[0x4c] = ram_int[0x0e];
  ram_int[0x4d] = ram_int[0x0f];
  ram_int[0x4e] = ram_ext[0x038d];
  program_byte();
  if(++ram_int[0x0f] == 0) {++ram_int[0x0e];}
}

// address ca4d
void program_byte(void)
{
  // disable interrupts, saving old flag in 2c.2
  asm("MOV C,EA");
  asm("MOV 2c.2,C");
  asm("CLR EA");

  if(2c.1) {
    ram_int[0xa7] |= 0x01; // set MOVC access to EEPROM
  } else {
    ram_int[0xa7] &= 0xfe; // set MOVC access to program memory
  }

  ram_int[0xf7] = ram_int[0x4c]; // set programming sector
  ram_int[0xfb] = ram_int[0x4d]; // set programming offset
  ram_int[0xfc] = ram_int[0x4e]; // set programming data
  ram_int[0xf2] = 0x6e; // select sector programming mode
  
  ram_int[0xf3] = 0x05; // write programming magic (1)
  ram_int[0xf4] = 0x0a; // write programming magic (2)
  ram_int[0xf5] = 0x09; // write programming magic (3)
  ram_int[0xf6] = 0x06; // write programming magic (4)

  // wait for programming to complete
  asm("NOP");
  asm("NOP");
  asm("NOP");
  asm("NOP");
  asm("NOP");
  asm("NOP");
  asm("NOP");
  asm("NOP");

  // clear all programming registers
  ram_int[0xf7] = 0x00;
  ram_int[0xf2] = 0x00;
  ram_int[0xf3] = 0x00;
  ram_int[0xf4] = 0x00;
  ram_int[0xf5] = 0x00;
  ram_int[0xf6] = 0x00;

  ram_int[0xa7] &= 0xfe; // set MOVC access to program memory

  // restore interrupt flag
  asm("MOV C, 2c.2");
  asm("MOV EA, C");
}

// address d43c
void erase_sector(void)
{
  // disable interrupts, saving old flag in 2c.2
  asm("MOV C,EA");
  asm("MOV 2c.2,C");
  asm("CLR EA");

  if(2c.1) {
    ram_int[0xa7] |= 0x01; // set MOVC access to EEPROM
  } else {
    ram_int[0xa7] &= 0xfe; // set MOVC access to program memory
  }

  ram_int[0xf7] = ram_int[0x4c]; // set programming sector
  ram_int[0xf2] = 0xe6; // select sector erase mode
  
  ram_int[0xf3] = 0x05; // write programming magic (1)
  ram_int[0xf4] = 0x0a; // write programming magic (2)
  ram_int[0xf5] = 0x09; // write programming magic (3)
  ram_int[0xf6] = 0x06; // write programming magic (4)

  // wait for sector erase to complete
  asm("NOP");
  asm("NOP");
  asm("NOP");
  asm("NOP");
  asm("NOP");
  asm("NOP");
  asm("NOP");
  asm("NOP");

  // clear all programming registers
  ram_int[0xf7] = 0x00;
  ram_int[0xf2] = 0x00;
  ram_int[0xf3] = 0x00;
  ram_int[0xf4] = 0x00;
  ram_int[0xf5] = 0x00;
  ram_int[0xf6] = 0x00;

  ram_int[0xa7] &= 0xfe; // set MOVC access to program memory

  // restore interrupt flag
  asm("MOV C, 2c.2");
  asm("MOV EA, C");
}

// address d437
void erase_eeprom_sector_1(void)
{
  asm("SETB 2c.1");
  ram_int[0x4c] = 0x01;
  erase_sector();
}

// address decc
void erase_eeprom_sectors_012(void)
{
  asm("SETB 2c.1");
  ram_int[0x4c] = 0x00;
  erase_sector();
  erase_eeprom_sector_1();
  asm("SETB 2c.1");
  ram_int[0x4c] = 0x02;
  erase_sector();
}

// address b24d
void sub_b24d(void)
{
  ram_int[0x0e] = 0x00;
  ram_int[0x0f] = 0x00;
  asm("SETB 2c.1");
  ram_int[0x4c] = 0x04;
  erase_sector();

  ram_int[0x0e] = 0x01;
  ram_int[0x0f] = 0x00;
  asm("SETB 2c.1");
  ram_int[0x4c] = 0x05;
  erase_sector();

  ram_int[0x0e] = 0x02;
  ram_int[0x0f] = 0x00;
  asm("SETB 2c.1");
  ram_int[0x4c] = 0x03;
  erase_sector();

  sub_d7f8();
  sub_d831();
  sub_d86a();
  sub_d8a3();
  sub_d8dc();

  ram_ext[0x0041] = 0x30;
  ram_ext[0x00c5] = 0x00;
  ram_ext[0x00c6] = 0x01;
  ram_ext[0x0135] = 0x00;
  ram_ext[0x0136] = 0x1f;
  ram_ext[0x0093] = 0x00;
  ram_ext[0x0094] = 0x20;
  
  ram_ext[0x0035] = 0x24;
  ram_ext[0x00ad] = 0x00;
  ram_ext[0x00ae] = 0x01;
  ram_ext[0x011d] = 0x00;
  ram_ext[0x011e] = 0x1f;
  ram_ext[0x0079] = 0x00;
  ram_ext[0x007a] = 0x20;

  ram_ext[0x0029] = 0x1e;
  ram_ext[0x0095] = 0x00;
  ram_ext[0x0096] = 0x01;
  ram_ext[0x0105] = 0x00;
  ram_ext[0x0106] = 0x1f;
  ram_ext[0x0077] = 0x00;
  ram_ext[0x0078] = 0x20;

  ram_ext[0x001d] = 0x18;
  ram_ext[0x007b] = 0x00;
  ram_ext[0x007c] = 0x01;
  ram_ext[0x00ed] = 0x00;
  ram_ext[0x00ee] = 0x1f;
  ram_ext[0x0075] = 0x00;
  ram_ext[0x0076] = 0x20;

  ram_ext[0x00e1] = 0x12;
  ram_ext[0x0005] = 0x00;
  ram_ext[0x0006] = 0x01;
  ram_ext[0x0059] = 0x00;
  ram_ext[0x005a] = 0x1f;
  ram_ext[0x0072] = 0x00;
  ram_ext[0x0073] = 0x20;
}

// address d8dc
void sub_d8dc(void)
{
  ram_ext[0x0093] = 0x00;
  ram_ext[0x0094] = 0x00;
  for(uint8_t i=0;i<12;++i) { // r7
    ram_ext[0x0041+i] = 0x00;
    ram_ext[0x00c5+(i*2)] = 0x00;
    ram_ext[0x00c6+(i*2)] = 0x00;
    ram_ext[0x0135+(i*2)] = 0x00;
    ram_ext[0x0136+(i*2)] = 0x00;
  }
}

// address d8a3
void sub_d8a3(void)
{
  ram_ext[0x0079] = 0x00;
  ram_ext[0x007a] = 0x00;
  for(uint8_t i=0;i<12;++i) { // r7
    ram_ext[0x0035+i] = 0x00;
    ram_ext[0x00ad+(i*2)] = 0x00;
    ram_ext[0x00ae+(i*2)] = 0x00;
    ram_ext[0x011d+(i*2)] = 0x00;
    ram_ext[0x011e+(i*2)] = 0x00;
  }
}

// address d86a
void sub_d86a(void)
{
  ram_ext[0x0077] = 0x00;
  ram_ext[0x0078] = 0x00;
  for(uint8_t i=0;i<12;++i) { // r7
    ram_ext[0x0029+i] = 0x00;
    ram_ext[0x0095+(i*2)] = 0x00;
    ram_ext[0x0096+(i*2)] = 0x00;
    ram_ext[0x0105+(i*2)] = 0x00;
    ram_ext[0x0106+(i*2)] = 0x00;
  }
}

// address d831
void sub_d831(void)
{
  ram_ext[0x0075] = 0x00;
  ram_ext[0x0076] = 0x00;
  for(uint8_t i=0;i<12;++i) { // r7
    ram_ext[0x001d] = 0x00;
    ram_ext[0x007b+(i*2)] = 0x00;
    ram_ext[0x007c+(i*2)] = 0x00;
    ram_ext[0x00ed+(i*2)] = 0x00;
    ram_ext[0x00ee+(i*2)] = 0x00;
  }
}

// address d7f8
void sub_d7f8(void)
{
  ram_ext[0x0072] = 0x00;
  ram_ext[0x0073] = 0x00;
  for(uint8_t i=0;i<12;++i) { // r7
    ram_ext[0x00e1+i] = 0x00;
    ram_ext[0x0005+(i*2)] = 0x00;
    ram_ext[0x0006+(i*2)] = 0x00;
    ram_ext[0x0059+(i*2)] = 0x00;
    ram_ext[0x005a+(i*2)] = 0x00;
  }
}

// address b73e
void sub_b73e(void)
{
  if(!20.5) {return;}

  if(ram_int[0x64] & 2) {
    if(28.4) {
      asm("CLR 28.4");
      sub_c9c8();
      ram_ext[0x03cd] = 3;
      asm("SETB 22.7");
      ram_ext[0x03cc] = 1;
      sub_a33f();
      program_eeprom_sector_6();
    }
  } else {
    asm("SETB 28.4");
  }

  if(ram_int[0x65] & 2) {
    if(28.5) {
      asm("CLR 28.5");
      sub_c9c8();
      ram_ext[0x03cd] = 3;
      asm("SETB 22.7");
      ram_ext[0x03cc] = 2;
      sub_a33f();
      program_eeprom_sector_6();
    }
  } else {
    asm("SETB 28.5");
  }

  if(ram_int[0x66] & 2) {
    if(28.6) {
      asm("CLR 28.6");
      sub_c9c8();
      ram_ext[0x03cd] = 3;
      asm("SETB 22.7");
      ram_ext[0x03cc] = 3;
      sub_a33f();
      program_eeprom_sector_6();
    }
  } else {
    asm("SETB 28.6");
  }

  if(ram_int[0x67] & 2) {
    if(28.7) {
      asm("CLR 28.7");
      sub_c9c8();
      ram_ext[0x03cd] = 3;
      asm("SETB 22.7");
      ram_ext[0x03cc] = 4;
      sub_a33f();
      program_eeprom_sector_6();
    }
  } else {
    asm("SETB 28.7");
  }

  if(ram_int[0x68] & 2) {
    if(29.0) {
      asm("CLR 29.0");
      sub_c9c8();
      ram_ext[0x03cd] = 3;
      asm("SETB 22.7");
      ram_ext[0x03cc] = 5;
      sub_a33f();
      program_eeprom_sector_6();
    }
  } else {
    asm("SETB 29.0");
  }
}

// address c4e8
void sub_c4e8(void)
{
  if(!20.5) {return;}

  if(22.7) {
    if(ram_int[0x73] & 2) {
      if(2c.3) {
        asm("CLR 2c.3");
        if(20.3) {
          asm("CLR 20.3");
          // dedicated LEDs
          asm("CLR P3.7");
          asm("CLR P3.6");
          asm("CLR P3.5");
          ram_ext[0x01dc] = 0;
          sub_b8bc();
        } else {
          asm("SETB 20.3");
          // dedicated LEDs
          asm("CLR P3.7");
          asm("CLR P3.6");
          asm("CLR P3.5");
          ram_ext[0x01dc] = 0;
          sub_d9ef(1, 0x00);
          sub_d9ef(2, 0x00);
          sub_d9ef(3, 0x00);
        }
      }
    } else {
      asm("SETB 2c.3");
    }
    return;
  }

  if(22.3 && ram_ext[0x0331] >= 8) {
    if(ram_int[0x73] & 2) {
      if(2c.3) {
        asm("CLR 2c.3");
        if(20.4) {
          asm("CLR 20.4");
          program_eeprom_sector_7();
        } else {
          asm("SETB 20.4");
        }
        // dedicated LEDs
        asm("CLR P3.7");
        asm("CLR P3.6");
        asm("CLR P3.5");
        ram_ext[0x01dc] = 0;
      }
    } else {
      asm("SETB 2c.3");
    }
    return;
  }
}

// address b97a
void program_eeprom_sector_7(void)
{
  // start programming at 0x0700
  ram_int[0x0e] = 7;
  ram_int[0x0f] = 0;
  
  asm("SETB 2c.1");
  ram_int[0x4c] = 7;
  erase_sector();

  asm("SETB 2c.1");
  ram_int[0x4c] = ram_int[0x0e];
  ram_int[0x4d] = ram_int[0x0f];
  ram_int[0x4e] = ram_ext[0x01a9];
  program_byte();
  if(++ram_int[0x0f] == 0) {++ram_int[0x0e];}

  asm("SETB 2c.1");
  ram_int[0x4c] = ram_int[0x0e];
  ram_int[0x4d] = ram_int[0x0f];
  ram_int[0x4e] = ram_ext[0x01ac];
  program_byte();
  if(++ram_int[0x0f] == 0) {++ram_int[0x0e];}

  asm("SETB 2c.1");
  ram_int[0x4c] = ram_int[0x0e];
  ram_int[0x4d] = ram_int[0x0f];
  ram_int[0x4e] = ram_ext[0x01ae];
  program_byte();
  if(++ram_int[0x0f] == 0) {++ram_int[0x0e];}

  asm("SETB 2c.1");
  ram_int[0x4c] = ram_int[0x0e];
  ram_int[0x4d] = ram_int[0x0f];
  ram_int[0x4e] = ram_ext[0x01af];
  program_byte();
  if(++ram_int[0x0f] == 0) {++ram_int[0x0e];}

  asm("SETB 2c.1");
  ram_int[0x4c] = ram_int[0x0e];
  ram_int[0x4d] = ram_int[0x0f];
  ram_int[0x4e] = ram_ext[0x01b0];
  program_byte();
  if(++ram_int[0x0f] == 0) {++ram_int[0x0e];}

  asm("SETB 2c.1");
  ram_int[0x4c] = ram_int[0x0e];
  ram_int[0x4d] = ram_int[0x0f];
  ram_int[0x4e] = ram_ext[0x01b1];
  program_byte();
  if(++ram_int[0x0f] == 0) {++ram_int[0x0e];}
}

// address b8bc
void sub_b8bc(void)
{
  uint8_t a = ram_ext[0x03cc];
  if(a == 1) {
    sub_baf0(0x0050);

    ram_int[0x0e] = 0x00;
    ram_int[0x0f] = 0x00;
    asm("SETB 2c.1");
    ram_int[0x4c] = 0x00;
    erase_sector();
    
    sub_c6cb();
    
    ram_int[0x0e] = 0x00;
    ram_int[0x0f] = 0x50;
    sub_c469();

    return;
  } else if(a == 2) {
    sub_baf0(0x0000);

    ram_int[0x0e] = 0x00;
    ram_int[0x0f] = 0x50;
    asm("SETB 2c.1");
    ram_int[0x4c] = 0x00;
    erase_sector();

    sub_c6cb();
    
    ram_int[0x0e] = 0x00;
    ram_int[0x0f] = 0x00;
    sub_c469();
  } else if(a == 3) {
    sub_baf0(0x0150);

    ram_int[0x0e] = 0x01;
    ram_int[0x0f] = 0x00;
    erase_eeprom_sector_1();

    sub_c6cb();

    ram_int[0x0e] = 0x01;
    ram_int[0x0f] = 0x50;
    sub_c469();
  } else if(a == 4) {
    sub_baf0(0x0100);

    ram_int[0x0e] = 0x01;
    ram_int[0x0f] = 0x50;
    erase_eeprom_sector_1();

    sub_c6cb();

    ram_int[0x0e] = 0x01;
    ram_int[0x0f] = 0x00;
    sub_c469();
  } else if(a == 5) {
    sub_baf0(0x0250);

    ram_int[0x0e] = 0x02;
    ram_int[0x0f] = 0x00;
    asm("SETB 2c.1");
    ram_int[0x4c] = 2;
    erase_sector();

    sub_c6cb();

    ram_int[0x0e] = 0x02;
    ram_int[0x0f] = 0x50;
    sub_c469();
  }
}

// address c469
void sub_c469(void)
{
  for(uint8_t i=0;i<18;++i) {
    asm("SETB 2c.1");
    ram_int[0x4c] = ram_int[0x0e];
    ram_int[0x4d] = ram_int[0x0f];
    ram_int[0x4e] = ram_ext[0x02eb+i];
    program_byte();
    if(++ram_int[0x0f] == 0) {++ram_int[0x0e];}
  }
  for(uint8_t i=0;i<18;++i) {
    asm("SETB 2c.1");
    ram_int[0x4c] = ram_int[0x0e];
    ram_int[0x4d] = ram_int[0x0f];
    ram_int[0x4e] = ram_ext[0x02d8+i];
    program_byte();
    if(++ram_int[0x0f] == 0) {++ram_int[0x0e];}
  }
  for(uint8_t i=0;i<18;++i) {
    asm("SETB 2c.1");
    ram_int[0x4c] = ram_int[0x0e];
    ram_int[0x4d] = ram_int[0x0f];
    ram_int[0x4e] = ram_ext[0x02c6+i];
    program_byte();
    if(++ram_int[0x0f] == 0) {++ram_int[0x0e];}
  }
}

// address c6cb
void sub_c6cb(void)
{
  for(uint8_t i=0;i<18;++i) {
    asm("SETB 2c.1");
    ram_int[0x4c] = ram_int[0x0e];
    ram_int[0x4d] = ram_int[0x0f];
    ram_int[0x4e] = ram_int[0xc3+i];
    program_byte();
    if(++ram_int[0x0f] == 0) {++ram_int[0x0e];}
  }
  for(uint8_t i=0;i<18;++i) {
    asm("SETB 2c.1");
    ram_int[0x4c] = ram_int[0x0e];
    ram_int[0x4d] = ram_int[0x0f];
    ram_int[0x4e] = ram_int[0xb1+i];
    program_byte();
    if(++ram_int[0x0f] == 0) {++ram_int[0x0e];}
  }
  for(uint8_t i=0;i<18;++i) {
    asm("SETB 2c.1");
    ram_int[0x4c] = ram_int[0x0e];
    ram_int[0x4d] = ram_int[0x0f];
    ram_int[0x4e] = ram_int[0x9f+i];
    program_byte();
    if(++ram_int[0x0f] == 0) {++ram_int[0x0e];}
  }
}

// address b7fd
void sub_b7fd(void)
{
  if(!20.5) {return;}

  if(ram_int[0x72] & 4) {
    if(29.1) {
      asm("CLR 29.1");
      if(22.4) {
        if(++ram_ext[0x0397] > 8) {ram_ext[0x0397] = 1;}
      } else if(20.7) {
        if(++ram_ext[0x0397] > 8) {ram_ext[0x0397] = 1;}
      } else if(21.0) {
        if(++ram_ext[0x03cb] > 7) {ram_ext[0x03cb] = 1;}
        sub_c0a6(ram_ext[0x03cb]);
      } else if(21.5) {
        if(++ram_ext[0x03b4] > 7) {ram_ext[0x03b4] = 1;}
        sub_c0a6(ram_ext[0x03b4]);
      } else if(22.3) {
        if(++ram_ext[0x0331] > 7) {ram_ext[0x0331] = 1;}
        clr_row_color_all();
        sub_bc55();
      } else if(23.4) {
        if(++ram_ext[0x0356] > 8) {ram_ext[0x0356] = 1;}
      } else if(22.6) {
        if(++ram_ext[0x0392] > 8) {ram_ext[0x0392] = 1;}
      } else if(23.0) {
        asm("CPL 2c.5");
      }
      program_eeprom_sector_6();
    }
  } else {
    asm("SETB 29.1");
  }
}

// address a09c
void sub_a09c(void)
{
  if(!20.5) {return;}

  if(ram_int[0x72] & 2) {
    if(29.2) {
      asm("CLR 29.2");
      ++ram_ext[0x03cd];
      if(ram_ext[0x03cd] == 1) {
        sub_c9c8();
        asm("SETB 21.0");
        sub_c0a6(ram_ext[0x03cb]);
      } else if(ram_ext[0x03cd] == 3) {
        sub_c9c8();
        asm("SETB 22.7");
        sub_a33f();
      } else if(ram_ext[0x03cd] == 2) {
        sub_c9c8();
        asm("SETB 21.5");
        sub_c0a6(ram_ext[0x03b4]);
        sub_de53();
      } else if(ram_ext[0x03cd] == 4) {
        sub_c9c8();
        asm("SETB 23.2");
        sub_de53();

        ram_int[0xdd] = 0;
        ram_int[0xde] = 0; // set PWM2 duty cycle to 0x0000
        ram_int[0xdb] = 0;
        ram_int[0xdc] = 0; // set PWM1 duty cycle to 0x0000
        ram_int[0xd9] = 0;
        ram_int[0xda] = 0; // set PWM0 duty cycle to 0x0000

        sub_d9ef(1, 0xff);
        sub_d9ef(2, 0xff);
        sub_d9ef(3, 0xff);
      } else if(ram_ext[0x03cd] == 5) {
        sub_c9c8();
        asm("SETB 22.4");
      } else if(ram_ext[0x03cd] == 6) {
        sub_c9c8();
        asm("SETB 22.3");
        clr_row_color_all();
        sub_bc55();
      } else if(ram_ext[0x03cd] == 7) {
        sub_c9c8();
        asm("SETB 23.4");
      } else if(ram_ext[0x03cd] == 8) {
        sub_c9c8();
        asm("SETB 22.6");
      } else if(ram_ext[0x03cd] == 9) {
        sub_c9c8();
        asm("SETB 20.7");
      } else if(ram_ext[0x03cd] >= 10) {
        ram_ext[0x03cd] = 0;
        sub_c9c8();
        asm("SETB 23.0");
        ram_ext[0x03c9] = 0;
        
        sub_c991(0x03b7, 0x00, 0xff, 0xf0, 0x00);
        sub_a836(1);
        
        sub_c991(0x03b7, 0x00, 0x01, 0xff, 0xe0);
        sub_a836(2);

        sub_c991(0x03b7, 0x00, 0xe0, 0x01, 0xff);
        sub_a836(3);
      }
      sub_d506(ram_int[0x09]);
      program_eeprom_sector_6();
    }
  } else {
    asm("SETB 29.2");
  }

  if(ram_int[0x72] & 1) {
    if(29.3) {
      asm("CLR 29.3");
      ram_ext[0x03cd] = 0;
      sub_c9c8();
      asm("SETB 22.1");
      clr_row_color_all();
      set_col_drive_all();
      program_eeprom_sector_6();
    }
  } else {
    asm("SETB 29.3");
  }
}

// address c991
void sub_c991(uint16_t dptr, uint8_t v0, uint8_t v1, uint8_t v2, uint8_t v3)
{
  ram_ext[dptr++] = v0;
  ram_ext[dptr++] = v1;
  ram_ext[dptr++] = v2;
  ram_ext[dptr++] = v3;
}

// address 7580
void sub_7580(void)
{
  if(!20.5) {return;}

  if(ram_int[0x74] & 32) {
    if(2b.6) {
      asm("CLR 2b.6");

      if(23.2) {
        uint8_t r7 = ram_ext[0x03b6];
        if(r7 >= 5) {
          ram_ext[0x03b6] = 4;
        } else if(r7 == 4) {
          ram_ext[0x03b6] = 3;
        } else if(r7 == 3) {
          ram_ext[0x03b6] = 2;
        } else if(r7 <= 2) {
          ram_ext[0x03b6] = 1;
        }
      } else if(21.5) {
        uint8_t r7 = ram_ext[0x03c4];
        if(r7 >= 5) {
          ram_ext[0x03c4] = 4;
        } else if(r7 == 4) {
          ram_ext[0x03c4] = 3;
        } else if(r7 == 3) {
          ram_ext[0x03c4] = 2;
        } else if(r7 <= 2) {
          ram_ext[0x03c4] = 1;
        }
      } else if(22.4) {
        uint8_t r7 = ram_ext[0x021d];
        if(r7 >= 0xa0) {
          ram_ext[0x021d] = 0x82;
        } else if(r7 == 0x82) {
          ram_ext[0x021d] = 0x5a;
        } else if(r7 == 0x5a) {
          ram_ext[0x021d] = 0x3c;
        } else if(r7 <= 0x3c) {
          ram_ext[0x021d] = 0x1e;
        }
      } else if(22.3) {
        uint8_t r7 = ram_ext[0x0332];
        if(r7 >= 5) {
          ram_ext[0x0332] = 4;
        } else if(r7 == 4) {
          ram_ext[0x0332] = 3;
        } else if(r7 == 3) {
          ram_ext[0x0332] = 2;
        } else if(r7 <= 2) {
          ram_ext[0x0332] = 1;
        }
      } else if(23.4) {
        uint8_t r7 = ram_ext[0x0370];
        if(r7 >= 8) {
          ram_ext[0x0370] = 6;
        } else if(r7 == 6) {
          ram_ext[0x0370] = 4;
        } else if(r7 == 4) {
          ram_ext[0x0370] = 2;
        } else if(r7 <= 2) {
          ram_ext[0x0370] = 1;
        }
      } else if(22.6) {
        uint8_t r7 = ram_ext[0x038d];
        if(r7 >= 8) {
          ram_ext[0x038d] = 6;
        } else if(r7 == 6) {
          ram_ext[0x038d] = 4;
        } else if(r7 == 4) {
          ram_ext[0x038d] = 2;
        } else if(r7 <= 2) {
          ram_ext[0x038d] = 1;
        }
      } else if(23.0 || 22.0) {
        if(ram_ext[0x03c6] != 0) {--ram_ext[0x03c6];}
      }
    }
  } else {
    asm("SETB 2b.6");
  }

  if(ram_int[0x72] & 32) {
    if(2c.0) {
      asm("CLR 2c.0");

      if(23.2) {
        uint8_t r7 = ram_ext[0x03b6];
        if(r7 <= 1) {
          ram_ext[0x03b6] = 2;
        } else if(r7 == 2) {
          ram_ext[0x03b6] = 3;
        } else if(r7 == 3) {
          ram_ext[0x03b6] = 4;
        } else if(r7 >= 4) {
          ram_ext[0x03b6] = 5;
        }
      } else if(21.5) {
        uint8_t r7 = ram_ext[0x03c4];
        if(r7 <= 1) {
          ram_ext[0x03c4] = 2;
        } else if(r7 == 2) {
          ram_ext[0x03c4] = 3;
        } else if(r7 == 3) {
          ram_ext[0x03c4] = 4;
        } else if(r7 >= 4) {
          ram_ext[0x03c4] = 5;
        }
      } else if(22.4) {
        uint8_t r7 = ram_ext[0x021d];
        if(r7 <= 0x1e) {
          ram_ext[0x021d] = 0x3c;
        } else if(r7 == 0x3c) {
          ram_ext[0x021d] = 0x5a;
        } else if(r7 == 0x5a) {
          ram_ext[0x021d] = 0x82;
        } else if(r7 >= 0x82) {
          ram_ext[0x021d] = 0xa0;
        }
      } else if(22.3) {
        uint8_t r7 = ram_ext[0x0332];
        if(r7 <= 1) {
          ram_ext[0x0332] = 2;
        } else if(r7 == 2) {
          ram_ext[0x0332] = 3;
        } else if(r7 == 3) {
          ram_ext[0x0332] = 4;
        } else if(r7 >= 4) {
          ram_ext[0x0332] = 5;
        }
      } else if(23.4) {
        uint8_t r7 = ram_ext[0x0370];
        if(r7 <= 1) {
          ram_ext[0x0370] = 2;
        } else if(r7 == 2) {
          ram_ext[0x0370] = 4;
        } else if(r7 == 4) {
          ram_ext[0x0370] = 6;
        } else if(r7 >= 6) {
          ram_ext[0x0370] = 8;
        }
      } else if(22.6) {
        uint8_t r7 = ram_ext[0x038d];
        if(r7 <= 1) {
          ram_ext[0x038d] = 2;
        } else if(r7 == 2) {
          ram_ext[0x038d] = 4;
        } else if(r7 == 4) {
          ram_ext[0x038d] = 6;
        } else if(r7 >= 6) {
          ram_ext[0x038d] = 8;
        }
      } else if(23.0 || 22.0) {
        if(++ram_ext[0x03c6] > 4) {ram_ext[0x03c6] = 4;}
      }
    }
  } else {
    asm("SETB 2c.0");
  }
}

// address d54b
void sub_d54b(void)
{
  if(!20.5) {return;}

  if(ram_int[0x73] & 32 || ram_int[0x73] & 16) {
    if(2b.7) {
      asm("CLR 2b.7");

      if(ram_int[0x73] & 32) {
        if(ram_int[0x09] != 0) {--ram_int[0x09];}
      }

      if(ram_int[0x73] & 16) {
        if(ram_int[0x09] < 4) {
          ++ram_int[0x09];
        } else {
          ram_int[0x09] = 4;
        }
      }

      if(ram_ext[0x03c5] != ram_int[0x09]) {
        ram_ext[0x03c5] = ram_int[0x09];
        sub_d506(ram_int[0x09]);
      }
    }
  } else {
    asm("SETB 2b.7");
  }
}

// address dea5
void sub_dea5(void)
{
  if(!20.5) {return;}

  if(ram_int[0x71] & 32) {
    if(25.4) {
      asm("CLR 25.4");
      
      asm("CPL 23.3");
    }
  } else {
    asm("SETB 25.4");
  }
}

// address 2fb7
void sub_2fb7(void)
{
  ram_int[0x51] = 0;
  ram_int[0x50] = 0;
  ram_int[0x4f] = 0;
  ram_int[0x4e] = 0;
  ram_int[0x4d] = 0;
  ram_int[0x4c] = 0;

  if(21.3 || 21.4 || 21.6 || 20.6 && 20.4) {
    if(20.5) {return;}

    for(uint8_t i=0;i<26;++i) { // held in 4b
      uint8_t r7 = ram_int[0x62+i];
      if(r7 & 1)  {ram_int[0x4c] = 1;}
      if(r7 & 2)  {ram_int[0x4d] = 1;}
      if(r7 & 4)  {ram_int[0x4e] = 1;}
      if(r7 & 8)  {ram_int[0x4f] = 1;}
      if(r7 & 16) {ram_int[0x50] = 1;}
      if(r7 & 32) {ram_int[0x51] = 1;}
    }
    clr_row_color_all();

    if(ram_int[0x4c] != 0) {
      if(++ram_ext[0x01a9] >= 8) {ram_ext[0x01a9] = 1;}
    }
    if(ram_int[0x4d] != 0) {
      if(++ram_ext[0x01ac] >= 8) {ram_ext[0x01ac] = 1;}
    }
    if(ram_int[0x4e] != 0) {
      if(++ram_ext[0x01ae] >= 8) {ram_ext[0x01ae] = 1;}
    }
    if(ram_int[0x4f] != 0) {
      if(++ram_ext[0x01af] >= 8) {ram_ext[0x01af] = 1;}
    }
    if(ram_int[0x50] != 0) {
      if(++ram_ext[0x01b0] >= 8) {ram_ext[0x01b0] = 1;}
    }
    if(ram_int[0x51] != 0) {
      if(++ram_ext[0x01b1] >= 8) {ram_ext[0x01b1] = 1;}
    }

    set_row_color_6(ram_ext[0x01a9]);
    set_row_color_5(ram_ext[0x01ac]);
    set_row_color_4(ram_ext[0x01ae]);
    set_row_color_3(ram_ext[0x01af]);
    set_row_color_2(ram_ext[0x01b0]);
    set_row_color_1(ram_ext[0x01b1]);
  }

  if(21.3 || 21.4 || 21.6 || 20.6 || 2b.3 && 20.3) {
    if(!20.5 && 2b.3) {
      asm("CLR 2b.3");
      if(++ram_ext[0x01ad] > 8) {ram_ext[0x01ad] = 1;}
      sub_b16f(ram_ext[0x01ad], 0x0004, 0x0010);
    }
    if(20.5) {return;}

    if(ram_int[0x62] & 1) {
      if(++ram_ext[0x01e0] > 8) {ram_ext[0x01e0] = 1;}
      sub_b16f(ram_ext[0x01e0], 0x0001, 0x0000);
    }
    if(ram_int[0x63] & 1) {
      if(++ram_ext[0x01e1] > 8) {ram_ext[0x01e1] = 1;}
      sub_b16f(ram_ext[0x01e1], 0x0002, 0x0000);
    }
    if(ram_int[0x64] & 1) {
      if(++ram_ext[0x01e2] > 8) {ram_ext[0x01e2] = 1;}
      sub_b16f(ram_ext[0x01e2], 0x0004, 0x0000);
    }
    if(ram_int[0x65] & 1) {
      if(++ram_ext[0x01e3] > 8) {ram_ext[0x01e3] = 1;}
      sub_b16f(ram_ext[0x01e3], 0x0008, 0x0000);
    }
    if(ram_int[0x66] & 1) {
      if(++ram_ext[0x01e4] > 8) {ram_ext[0x01e4] = 1;}
      sub_b16f(ram_ext[0x01e4], 0x0010, 0x0000);
    }
    if(ram_int[0x67] & 1) {
      if(++ram_ext[0x01e5] > 8) {ram_ext[0x01e5] = 1;}
      sub_b16f(ram_ext[0x01e5], 0x0020, 0x0000);
    }
    if(ram_int[0x68] & 1) {
      if(++ram_ext[0x01e6] > 8) {ram_ext[0x01e6] = 1;}
      sub_b16f(ram_ext[0x01e6], 0x0040, 0x0000);
    }
    if(ram_int[0x69] & 1) {
      if(++ram_ext[0x01e7] > 8) {ram_ext[0x01e7] = 1;}
      sub_b16f(ram_ext[0x01e7], 0x0080, 0x0000);
    }
    if(ram_int[0x6a] & 1) {
      if(++ram_ext[0x01e8] > 8) {ram_ext[0x01e8] = 1;}
      sub_b16f(ram_ext[0x01e8], 0x0001, 0x0001);
    }
    if(ram_int[0x6b] & 1) {
      if(++ram_ext[0x01e9] > 8) {ram_ext[0x01e9] = 1;}
      sub_b16f(ram_ext[0x01e9], 0x0002, 0x0001);
    }
    if(ram_int[0x6c] & 1) {
      if(++ram_ext[0x0152] > 8) {ram_ext[0x0152] = 1;}
      sub_b16f(ram_ext[0x0152], 0x0004, 0x0001);
    }
    if(ram_int[0x6d] & 1) {
      if(++ram_ext[0x0154] > 8) {ram_ext[0x0154] = 1;}
      sub_b16f(ram_ext[0x0154], 0x0008, 0x0001);
    }
    if(ram_int[0x6e] & 1) {
      if(++ram_ext[0x0157] > 8) {ram_ext[0x0157] = 1;}
      sub_b16f(ram_ext[0x0157], 0x0010, 0x0001);
    }
    if(ram_int[0x6f] & 1) {
      if(++ram_ext[0x015b] > 8) {ram_ext[0x015b] = 1;}
      sub_b16f(ram_ext[0x015b], 0x0020, 0x0001);
    }
    if(ram_int[0x70] & 1) {
      if(++ram_ext[0x0160] > 8) {ram_ext[0x0160] = 1;}
      sub_b16f(ram_ext[0x0160], 0x0040, 0x0001);
    }
    if(ram_int[0x71] & 1) {
      if(++ram_ext[0x0166] > 8) {ram_ext[0x0166] = 1;}
      sub_b16f(ram_ext[0x0166], 0x0080, 0x0001);
    }
    if(ram_int[0x72] & 1) {
      if(++ram_ext[0x016d] > 8) {ram_ext[0x016d] = 1;}
      sub_b16f(ram_ext[0x016d], 0x0001, 0x0002);
    }
    if(ram_int[0x73] & 1) {
      if(++ram_ext[0x0175] > 8) {ram_ext[0x0175] = 1;}
      sub_b16f(ram_ext[0x0175], 0x0002, 0x0002);
    }
    if(ram_int[0x74] & 1) {
      if(++ram_ext[0x017e] > 8) {ram_ext[0x017e] = 1;}
      sub_b16f(ram_ext[0x017e], 0x0004, 0x0002);
    }
    if(ram_int[0x75] & 1) {
      if(++ram_ext[0x0187] > 8) {ram_ext[0x0187] = 1;}
      sub_b16f(ram_ext[0x0187], 0x0008, 0x0002);
    }
    if(ram_int[0x76] & 1) {
      if(++ram_ext[0x0153] > 8) {ram_ext[0x0153] = 1;}
      sub_b16f(ram_ext[0x0153], 0x0010, 0x0002);
    }
    if(ram_int[0x77] & 1) {
      if(++ram_ext[0x0156] > 8) {ram_ext[0x0156] = 1;}
      sub_b16f(ram_ext[0x0156], 0x0020, 0x0002);
    }
    if(ram_int[0x78] & 1) {
      if(++ram_ext[0x015a] > 8) {ram_ext[0x015a] = 1;}
      sub_b16f(ram_ext[0x015a], 0x0040, 0x0002);
    }
    if(ram_int[0x79] & 1) {
      if(++ram_ext[0x015f] > 8) {ram_ext[0x015f] = 1;}
      sub_b16f(ram_ext[0x015f], 0x0080, 0x0002);
    }
    
    if(ram_int[0x62] & 2) {
      if(++ram_ext[0x0165] > 8) {ram_ext[0x0165] = 1;}
      sub_b16f(ram_ext[0x0165], 0x0001, 0x0003);
    }
    if(ram_int[0x63] & 2) {
      if(++ram_ext[0x016c] > 8) {ram_ext[0x016c] = 1;}
      sub_b16f(ram_ext[0x016c], 0x0002, 0x0003);
    }
    if(ram_int[0x64] & 2) {
      if(++ram_ext[0x0174] > 8) {ram_ext[0x0174] = 1;}
      sub_b16f(ram_ext[0x0174], 0x0004, 0x0003);
    }
    if(ram_int[0x65] & 2) {
      if(++ram_ext[0x017d] > 8) {ram_ext[0x017d] = 1;}
      sub_b16f(ram_ext[0x017d], 0x0008, 0x0003);
    }
    if(ram_int[0x66] & 2) {
      if(++ram_ext[0x0186] > 8) {ram_ext[0x0186] = 1;}
      sub_b16f(ram_ext[0x0186], 0x0010, 0x0003);
    }
    if(ram_int[0x67] & 2) {
      if(++ram_ext[0x018f] > 8) {ram_ext[0x018f] = 1;}
      sub_b16f(ram_ext[0x018f], 0x0020, 0x0003);
    }
    if(ram_int[0x68] & 2) {
      if(++ram_ext[0x0155] > 8) {ram_ext[0x0155] = 1;}
      sub_b16f(ram_ext[0x0155], 0x0040, 0x0003);
    }
    if(ram_int[0x69] & 2) {
      if(++ram_ext[0x0159] > 8) {ram_ext[0x0159] = 1;}
      sub_b16f(ram_ext[0x0159], 0x0080, 0x0003);
    }
    if(ram_int[0x6a] & 2) {
      if(++ram_ext[0x015e] > 8) {ram_ext[0x015e] = 1;}
      sub_b16f(ram_ext[0x015e], 0x0001, 0x0004);
    }
    if(ram_int[0x6b] & 2) {
      if(++ram_ext[0x0164] > 8) {ram_ext[0x0164] = 1;}
      sub_b16f(ram_ext[0x0164], 0x0002, 0x0004);
    }
    if(ram_int[0x6c] & 2) {
      if(++ram_ext[0x016b] > 8) {ram_ext[0x016b] = 1;}
      sub_b16f(ram_ext[0x016b], 0x0004, 0x0004);
    }
    if(ram_int[0x6d] & 2) {
      if(++ram_ext[0x0173] > 8) {ram_ext[0x0173] = 1;}
      sub_b16f(ram_ext[0x0173], 0x0008, 0x0004);
    }
    if(ram_int[0x6e] & 2) {
      if(++ram_ext[0x017c] > 8) {ram_ext[0x017c] = 1;}
      sub_b16f(ram_ext[0x017c], 0x0010, 0x0004);
    }
    if(ram_int[0x6f] & 2) {
      if(++ram_ext[0x0185] > 8) {ram_ext[0x0185] = 1;}
      sub_b16f(ram_ext[0x0185], 0x0020, 0x0004);
    }
    if(ram_int[0x70] & 2) {
      if(++ram_ext[0x018e] > 8) {ram_ext[0x018e] = 1;}
      sub_b16f(ram_ext[0x018e], 0x0040, 0x0004);
    }
    if(ram_int[0x71] & 2) {
      if(++ram_ext[0x0196] > 8) {ram_ext[0x0196] = 1;}
      sub_b16f(ram_ext[0x0196], 0x0080, 0x0004);
    }
    if(ram_int[0x72] & 2) {
      if(++ram_ext[0x0158] > 8) {ram_ext[0x0158] = 1;}
      sub_b16f(ram_ext[0x0158], 0x0001, 0x0005);
    }
    if(ram_int[0x73] & 2) {
      if(++ram_ext[0x015d] > 8) {ram_ext[0x015d] = 1;}
      sub_b16f(ram_ext[0x015d], 0x0002, 0x0005);
    }
    if(ram_int[0x74] & 2) {
      if(++ram_ext[0x0163] > 8) {ram_ext[0x0163] = 1;}
      sub_b16f(ram_ext[0x0163], 0x0004, 0x0005);
    }
    if(ram_int[0x75] & 2) {
      if(++ram_ext[0x016a] > 8) {ram_ext[0x016a] = 1;}
      sub_b16f(ram_ext[0x016a], 0x0008, 0x0005);
    }
    if(ram_int[0x76] & 2) {
      if(++ram_ext[0x0172] > 8) {ram_ext[0x0172] = 1;}
      sub_b16f(ram_ext[0x0172], 0x0010, 0x0005);
    }
    if(ram_int[0x77] & 2) {
      if(++ram_ext[0x017b] > 8) {ram_ext[0x017b] = 1;}
      sub_b16f(ram_ext[0x017b], 0x0020, 0x0005);
    }
    if(ram_int[0x78] & 2) {
      if(++ram_ext[0x0184] > 8) {ram_ext[0x0184] = 1;}
      sub_b16f(ram_ext[0x0184], 0x0040, 0x0005);
    }
    if(ram_int[0x79] & 2) {
      if(++ram_ext[0x018d] > 8) {ram_ext[0x018d] = 1;}
      sub_b16f(ram_ext[0x018d], 0x0080, 0x0005);
    }

    ram_int[0x4b] = 0x06;

    if(ram_int[0x62] & 4) {
      if(++ram_ext[0x0195] > 8) {ram_ext[0x0195] = 1;}
      sub_b16f(ram_ext[0x0195], 0x0001, 0x0006);
    }
    if(ram_int[0x63] & 4) {
      if(++ram_ext[0x019c] > 8) {ram_ext[0x019c] = 1;}
      sub_b16f(ram_ext[0x019c], 0x0002, 0x0006);
    }
    if(ram_int[0x64] & 4) {
      if(++ram_ext[0x015c] > 8) {ram_ext[0x015c] = 1;}
      sub_b16f(ram_ext[0x015c], 0x0004, 0x0006);
    }
    // TODO 3857
  }
}

// address b16f
void sub_b16f(uint8_t r7, uint16_t r2r3, uint16_t r4r5)
{
  if(r7 == 1) {
    // TODO
  }
}

// address a70a
void sub_a70a(void)
{
  ;
}

// address 6c17
void sub_6c17(void)
{
  ;
}

// address 9414
void sub_9414(void)
{
  ;
}

// address de0e
void sub_de0e(void)
{
  ;
}

// address 7a9c
void sub_7a9c(void)
{
  ;
}

// address 9dca
void sub_9dca(void)
{
  ;
}

// address b091
void sub_b091(void)
{
  ;
}

// address 8acb
void sub_8acb(void)
{
  ;
}

// address b4d2
void sub_b4d2(void)
{
  ;
}

// address d04d
void sub_d04d(void)
{
  ;
}

// address c25e
void sub_c25e(void)
{
  ;
}

// address 7833
void sub_7833(void)
{
  ;
}

// address dd70
void sub_dd70(void)
{
  if(20.5) {
    if((ram_int[0x74] & 1) == 0) { // indirect RAM access
      asm("SETB 2a.6");
    } else if(2a.6) {
      asm("CLR 2a.6");
      asm("CPL 2a.2");
    }
  }

  if(2a.2) {
    asm("SETB P3.1");
  } else {
    asm("CLR P3.1");
  }
}

// address de3d
void start_timer2(void)
{
  ram_int[0xc8] = 0x00; // stop Timer2 (1)
  ram_int[0xc9] = 0x00; // stop Timer2 (2); set prescale to 1/12

  ram_int[0xca] = 0x17;
  ram_int[0xcb] = 0xfc; // set Timer2 reload value to 0xfc17

  ram_int[0xcc] = 0x17;
  ram_int[0xcd] = 0xfc; // set Timer2 counter to 0xfc17

  asm("SETB TR2"); // start Timer2
  asm("SETB IEN0.ET2"); // enable Timer2 interrupt
}

// address af9c
void suspend(void)
{
  asm("CLR EA"); // disable interrupts

  stop_timer2();
  clr_row_color_all();
  clr_col_drive_all();

  asm("SETB P3.7");
  asm("SETB P3.6");
  asm("SETB P3.5");
  asm("SETB P3.4");
  asm("SETB P3.1");
  asm("CLR 21.1");
  ram_int[0x36] = 0;

  ram_int[0xc8] = 0x00; // stop Timer2

  ram_int[0x86] |= 0x40; // select SFR bank 1
  ram_int[0x88] = 0x10; // stop Timer3

  ram_int[0x86] &= 0x3f; // select SFR bank 0
  if(2b.0) {
    ram_int[0xba] = 0x38; // enable external INT43,44,45 (P2.1-3, pins 37-39)
    ram_int[0xa9] |= 0x0e; // enable external INT2,3,4x
  } else {
    ram_int[0xba] = 0x00; // disable all external INT4x
    ram_int[0xa9] &= 0xf1; // disable external INT2,3,4x
  }

  ram_int[0x88] = 0x00; // clear external INT0,1; set as low level trigger mode
  ram_int[0xe8] = 0x7c; // set external INT2,3 as trigger on both edge; INT4x as falling edge
  ram_int[0xd8] = 0x00; // clear external INT4x

  ram_int[0x86] |= 0x40; // select SFR bank 1
  ram_int[0x99] |= 0x01; // enter USB suspend state

  ram_int[0x86] &= 0x3f; // select SFR bank 0
  ram_int[0xb2] &= 0xfb; // use OSC1 as primary clock (run at 12MHz)
  ram_int[0xb3] &= 0xfe; // disconnect PLL from OSC2
  ram_int[0xb3] &= 0xfd; // turn off 48MHz PLL
  ram_int[0xb2] &= 0xf7; // turn off OSC2

  ram_int[0x86] |= 0x40; // select SFR bank 1
  ram_int[0x9a] |= 0x85; // enable USB plug/unplug, resume, and bus reset interrupts

  ram_int[0x86] &= 0x3f; // select SFR bank 0
  ram_int[0xa9] |= 0x10; // enable USB interrupt

  asm("SETB EA"); // enable interrupts
  ram_int[0x8e] = 0x55;  // POWER DOWN sequence (1)
  ram_int[0x87] |= 0x02; // POWER DOWN sequence (2)
  asm("NOP");            // POWER DOWN sequence (3)
  asm("NOP");            // POWER DOWN sequence (4)
  asm("NOP");            // POWER DOWN sequence (5)

  // The CPU will resume execution HERE once an interrupt occurs and the ISR has executed
  
  ram_int[0xb1] = 0x00; // clear the reset flag
  
  ram_int[0x0c] = 0x00;
  ram_int[0x0d] = 0xc8;
  
  asm("CLR EA"); // disable interrupts
  ram_int[0x88] = 0x00; // clear external INT0,1; set as low level trigger mode
  ram_int[0xe8] = 0x7c; // set external INT2,3 as trigger on both edge; INT4x as falling edge
  ram_int[0xd8] = 0x00; // clear external INT4x
  ram_int[0xba] = 0x00; // disable all external INT4x
  ram_int[0xa9] &= 0xf1; // disable external INT2,3,4
  
  ram_int[0xb2] = 0x08; // turn on OSC2, use OSC1 as primary with no prescale (run at 12MHz)
  ram_int[0xb3] = 0x02; // turn on 48MHz PLL
  delay(0x0d05);        // wait 5ms for PLL to lock
  ram_int[0xb3] = 0x03; // connect PLL to OSC2
  ram_int[0xb2] = 0x0c; // use OSC2 as primary (run at 24MHz)

  // USB reset
  if(21.1) {
    ram_int[0x80] &= 0xef; // P0.4 = 0
    ram_int[0xe1] |= 0x10; // set P0.4 as output
    ram_int[0xa0] |= 0x01; // P2.0 = 1
    ram_int[0xe3] |= 0x01; // set P2.0 as output
    
    delay(0x3414);         // delay for 10ms
    
    ram_int[0xe9] &= 0xef; // disable pullup on P0.4
    ram_int[0xe1] &= 0xef; // set P0.4 as input
    ram_int[0xeb] &= 0xfe; // disable pullup on P2.0
    ram_int[0xe3] &= 0xfe; // set P2.0 as input

    asm("CLR 21.1");
  }

  ram_int[0x86] |= 0x40; // select SFR bank 1
  ram_int[0xe8] &= 0xfd; // clear USB resume interrupt flag
  ram_int[0x99] &= 0xfe; // exit USB suspend state
  
  ram_int[0x86] &= 0x3f; // select SFR bank 0
  ram_int[0x36] = 0;

  ram_int[0x86] |= 0x40; // select SFR bank 1
  ram_int[0x9a] = 0x5f; // enable all USB interrupts except plug/unplug

  ram_int[0x86] &= 0x3f; // select SFR bank 0
  asm("SETB 20.0");
  ram_ext[0x0399] = 0;
  ram_ext[0x039a] = 0;

  gpio_init();
  clr_row_color_all();
  
  asm("CLR 23.1");
  start_timer3();
  
  asm("CLR 22.2");
  asm("CLR 23.3");
  if(22.3) {
    clr_row_color_all();
    sub_bc55();
  }

  asm("SETB EA"); // enable interrupts
}

// address bc55
void sub_bc55(void)
{
  if(ram_ext[0x0331] == 7) {
    read_eeprom_row_colors();
    set_row_color_6(ram_ext[0x01a9]);
    set_row_color_5(ram_ext[0x01ac]);
    set_row_color_4(ram_ext[0x01ae]);
    set_row_color_3(ram_ext[0x01af]);
    set_row_color_2(ram_ext[0x01b0]);
    set_row_color_1(ram_ext[0x01b1]);
  } else if(ram_ext[0x0331] == 1) {
    set_row_color_all(1);
  } else if(ram_ext[0x0331] == 2) {
    set_row_color_all(2);
  } else if(ram_ext[0x0331] == 3) {
    set_row_color_all(3);
  } else if(ram_ext[0x0331] == 4) {
    set_row_color_all(1);
    set_row_color_all(2);
  } else if(ram_ext[0x0331] == 5) {
    set_row_color_all(2);
    set_row_color_all(3);
  } else if(ram_ext[0x0331] == 6) {
    set_row_color_all(1);
    set_row_color_all(3);
  } else if(ram_ext[0x0331] >= 8) {
    ram_ext[0x0331] = 1;
    set_row_color_all(1);
  } else if(ram_ext[0x0331] == 0) {
    ram_ext[0x0331] = 1;
    set_row_color_all(1);
  }
}

// address b327
void set_row_color_all(uint8_t arg)
{
  ram_int[0x86] &= 0x3f;
  if(arg == 1) { // set all LED rows pin 3
    asm("SETB P3.0");
    asm("SETB P2.7");
    asm("SETB P2.4");
    ram_int[0x86] |= 0x40;
    asm("SETB P8.5");
    asm("SETB P5.5");
    asm("SETB P5.2");
    ram_int[0x86] &= 0x3f;
  } else if(arg == 2) { // set all LED rows pin 4
    asm("SETB P2.6");
    ram_int[0x86] |= 0x40;
    asm("SETB P8.6");
    asm("SETB P8.3");
    asm("SETB P5.7");
    asm("SETB P5.4");
    asm("SETB P5.1");
    ram_int[0x86] &= 0x3f;
  } else if(arg == 3) { // set all LED rows pin 2
    asm("SETB P2.5");
    ram_int[0x86] |= 0x40;
    asm("SETB P8.7");
    asm("SETB P8.4");
    asm("SETB P5.6");
    asm("SETB P5.3");
    asm("SETB P5.0");
    ram_int[0x86] &= 0x3f;    
  } else if(arg == 4) { // set all LED rows pin 2,4
    asm("SETB P2.6");
    asm("SETB P2.5");
    ram_int[0x86] |= 0x40;
    asm("SETB P8.6");
    asm("SETB P8.3");
    asm("SETB P5.7");
    asm("SETB P5.4");
    asm("SETB P5.1");
    asm("SETB P8.7");
    asm("SETB P8.4");
    asm("SETB P5.6");
    asm("SETB P5.3");
    asm("SETB P5.0");
    ram_int[0x86] &= 0x3f;    
  } else if(arg == 5) { // set all LED rows pin 2,3
    asm("SETB P3.0");
    asm("SETB P2.7");
    asm("SETB P2.4");
    asm("SETB P2.5");
    ram_int[0x86] |= 0x40;
    asm("SETB P8.5");
    asm("SETB P5.5");
    asm("SETB P5.2");
    asm("SETB P8.7");
    asm("SETB P8.4");
    asm("SETB P5.6");
    asm("SETB P5.3");
    asm("SETB P5.0");
    ram_int[0x86] &= 0x3f;
  } else if(arg == 6) { // set all LED rows pin 3,4
    asm("SETB P3.0");
    asm("SETB P2.7");
    asm("SETB P2.4");
    asm("SETB P2.6");
    ram_int[0x86] |= 0x40;
    asm("SETB P8.5");
    asm("SETB P5.5");
    asm("SETB P5.2");
    asm("SETB P8.6");
    asm("SETB P8.3");
    asm("SETB P5.7");
    asm("SETB P5.4");
    asm("SETB P5.1");
    ram_int[0x86] &= 0x3f;
  } else if(arg == 7) { // set all LED rows pin 2,3,4
    asm("SETB P3.0");
    asm("SETB P2.7");
    asm("SETB P2.4");
    asm("SETB P2.6");
    asm("SETB P2.5");
    ram_int[0x86] |= 0x40;
    asm("SETB P8.5");
    asm("SETB P5.5");
    asm("SETB P5.2");
    asm("SETB P8.6");
    asm("SETB P8.3");
    asm("SETB P5.7");
    asm("SETB P5.4");
    asm("SETB P5.1");
    asm("SETB P8.7");
    asm("SETB P8.4");
    asm("SETB P5.6");
    asm("SETB P5.3");
    asm("SETB P5.0");
    ram_int[0x86] &= 0x3f;    
  }
}

// address d984
void set_row_color_1(uint8_t arg)
{
  ram_int[0x86] &= 0x3f;
  if(arg == 1) {
    asm("SETB P2.4");
  } else if(arg == 2) {
    asm("SETB P2.6");
  } else if(arg == 3) {
    asm("SETB P2.5");
  } else if(arg == 4) {
    asm("SETB P2.6");
    asm("SETB P2.5");
  } else if(arg == 5) {
    asm("SETB P2.4");
    asm("SETB P2.5");
  } else if(arg == 6) {
    asm("SETB P2.4");
    asm("SETB P2.6");
  } else if(arg == 7) {
    asm("SETB P2.4");
    asm("SETB P2.5");
    asm("SETB P2.6");
  }  
}

// address d4c1
void set_row_color_2(uint8_t arg)
{
  ram_int[0x86] &= 0x3f;
  if(arg == 1) {
    asm("SETB P2.7");
  } else if(arg == 2) {
    ram_int[0x86] |= 0x40;
    asm("SETB P5.1");
    ram_int[0x86] &= 0x3f;
  } else if(arg == 3) {
    ram_int[0x86] |= 0x40;
    asm("SETB P5.0");
    ram_int[0x86] &= 0x3f;
  } else if(arg == 4) {
    ram_int[0x86] |= 0x40;
    asm("SETB P5.1");
    asm("SETB P5.0");
    ram_int[0x86] &= 0x3f;
  } else if(arg == 5) {
    asm("SETB P2.7");
    ram_int[0x86] |= 0x40;
    asm("SETB P5.0");
    ram_int[0x86] &= 0x3f;
  } else if(arg == 6) {
    asm("SETB P2.7");
    ram_int[0x86] |= 0x40;
    asm("SETB P5.1");
    ram_int[0x86] &= 0x3f;
  } else if(arg == 7) {
    asm("SETB P2.7");
    ram_int[0x86] |= 0x40;
    asm("SETB P5.0");
    asm("SETB P5.1");
    ram_int[0x86] &= 0x3f;
  }
}

// address d1e1
void set_row_color_3(uint8_t arg)
{
  ram_int[0x86] &= 0x3f;
  if(arg == 1) {
    ram_int[0x86] |= 0x40;
    asm("SETB P5.2");
    ram_int[0x86] &= 0x3f;
  } else if(arg == 2) {
    ram_int[0x86] |= 0x40;
    asm("SETB P5.4");
    ram_int[0x86] &= 0x3f;
  } else if(arg == 3) {
    ram_int[0x86] |= 0x40;
    asm("SETB P5.3");
    ram_int[0x86] &= 0x3f;
  } else if(arg == 4) {
    ram_int[0x86] |= 0x40;
    asm("SETB P5.4");
    asm("SETB P5.3");
    ram_int[0x86] &= 0x3f;
  } else if(arg == 5) {
    ram_int[0x86] |= 0x40;
    asm("SETB P5.3");
    asm("SETB P5.2");
    ram_int[0x86] &= 0x3f;
  } else if(arg == 6) {
    ram_int[0x86] |= 0x40;
    asm("SETB P5.4");
    asm("SETB P5.2");
    ram_int[0x86] &= 0x3f;
  } else if(arg == 7) {
    ram_int[0x86] |= 0x40;
    asm("SETB P5.2");
    asm("SETB P5.3");
    asm("SETB P5.4");
    ram_int[0x86] &= 0x3f;
  }
}

// address d192
void set_row_color_4(uint8_t arg)
{
  ram_int[0x86] &= 0x3f;
  if(arg == 1) {
    ram_int[0x86] |= 0x40;
    asm("SETB P5.5");
    ram_int[0x86] &= 0x3f;
  } else if(arg == 2) {
    ram_int[0x86] |= 0x40;
    asm("SETB P5.7");
    ram_int[0x86] &= 0x3f;
  } else if(arg == 3) {
    ram_int[0x86] |= 0x40;
    asm("SETB P5.6");
    ram_int[0x86] &= 0x3f;
  } else if(arg == 4) {
    ram_int[0x86] |= 0x40;
    asm("SETB P5.7");
    asm("SETB P5.6");
    ram_int[0x86] &= 0x3f;
  } else if(arg == 5) {
    ram_int[0x86] |= 0x40;
    asm("SETB P5.6");
    asm("SETB P5.5");
    ram_int[0x86] &= 0x3f;
  } else if(arg == 6) {
    ram_int[0x86] |= 0x40;
    asm("SETB P5.7");
    asm("SETB P5.5");
    ram_int[0x86] &= 0x3f;
  } else if(arg == 7) {
    ram_int[0x86] |= 0x40;
    asm("SETB P5.5");
    asm("SETB P5.6");
    asm("SETB P5.7");
    ram_int[0x86] &= 0x3f;
  }
}

// address d143
void set_row_color_5(uint8_t arg)
{
  ram_int[0x86] &= 0x3f;
  if(arg == 1) {
    ram_int[0x86] |= 0x40;
    asm("SETB P8.5");
    ram_int[0x86] &= 0x3f;
  } else if(arg == 2) {
    ram_int[0x86] |= 0x40;
    asm("SETB P8.3");
    ram_int[0x86] &= 0x3f;
  } else if(arg == 3) {
    ram_int[0x86] |= 0x40;
    asm("SETB P8.4");
    ram_int[0x86] &= 0x3f;
  } else if(arg == 4) {
    ram_int[0x86] |= 0x40;
    asm("SETB P8.3");
    asm("SETB P8.4");
    ram_int[0x86] &= 0x3f;
  } else if(arg == 5) {
    ram_int[0x86] |= 0x40;
    asm("SETB P8.4");
    asm("SETB P8.5");
    ram_int[0x86] &= 0x3f;
  } else if(arg == 6) {
    ram_int[0x86] |= 0x40;
    asm("SETB P8.3");
    asm("SETB P8.5");
    ram_int[0x86] &= 0x3f;
  } else if(arg == 7) {
    ram_int[0x86] |= 0x40;
    asm("SETB P8.5");
    asm("SETB P8.4");
    asm("SETB P8.3");
    ram_int[0x86] &= 0x3f;
  }
}

// address d47c
void set_row_color_6(uint8_t arg)
{
  ram_int[0x86] &= 0x3f;
  if(arg == 1) {
    asm("SETB P3.0");
  } else if(arg == 2) {
    ram_int[0x86] |= 0x40;
    asm("SETB P8.6");
    ram_int[0x86] &= 0x3f;
  } else if(arg == 3) {
    ram_int[0x86] |= 0x40;
    asm("SETB P8.7");
    ram_int[0x86] &= 0x3f;
  } else if(arg == 4) {
    ram_int[0x86] |= 0x40;
    asm("SETB P8.6");
    asm("SETB P8.7");
    ram_int[0x86] &= 0x3f;
  } else if(arg == 5) {
    asm("SETB P3.0");
    ram_int[0x86] |= 0x40;
    asm("SETB P8.7");
    ram_int[0x86] &= 0x3f;
  } else if(arg == 6) {
    asm("SETB P3.0");
    ram_int[0x86] |= 0x40;
    asm("SETB P8.6");
    ram_int[0x86] &= 0x3f;
  } else if(arg == 7) {
    asm("SETB P3.0");
    ram_int[0x86] |= 0x40;
    asm("SETB P8.7");
    asm("SETB P8.6");
    ram_int[0x86] &= 0x3f;
  }
}

// address c2e4
void read_eeprom_row_colors(void)
{
  ram_int[0xa7] |= 0x01; // set MOVC access to EEPROM

  ram_ext[0x01a9] = eeprom[0x0700];
  ram_ext[0x01ac] = eeprom[0x0701];
  ram_ext[0x01ae] = eeprom[0x0702];
  ram_ext[0x01af] = eeprom[0x0703];
  ram_ext[0x01b0] = eeprom[0x0704];
  ram_ext[0x01b1] = eeprom[0x0705];

  ram_int[0xa7] &= 0xfe; // set MOVC access to program memory

  if(ram_ext[0x01a9] != 0) {return;}
  if(ram_ext[0x01ac] != 0) {return;}
  if(ram_ext[0x01ae] != 0) {return;}
  if(ram_ext[0x01af] != 0) {return;}
  if(ram_ext[0x01b0] != 0) {return;}
  if(ram_ext[0x01b1] != 0) {return;}

  ram_ext[0x01a9] = 1;
  ram_ext[0x01ac] = 5;
  ram_ext[0x01ae] = 2;
  ram_ext[0x01af] = 6;
  ram_ext[0x01b0] = 3;
  ram_ext[0x01b1] = 7;
}

// address de26
void clr_col_drive_all(void)
{
  ram_int[0x86] &= 0x3f; // select SFR bank 0
  ram_int[0x90] = 0x00; // P1 = 0

  ram_int[0x86] |= 0x40; // select SFR bank 1
  ram_int[0x90] = 0x00; // P6 = 0
  ram_int[0xa0] = 0x00; // P7 = 0
  asm("CLR P8.0");
  asm("CLR P8.1");
  asm("CLR P8.2");

  ram_int[0x86] &= 0x3f; // select SFR bank 0
}

// address dfb4
void stop_timer2(void)
{
  ram_int[0xc8] = 0x00; // stop Timer2 (1)
  ram_int[0xc9] = 0x00; // stop Timer2 (2)
  asm("CLR IE.5"); // clear Timer2 overflow interrupt
}

// address dc4b
void start_usb(void)
{
  ram_int[0x86] |= 0x40; // select SFR bank 1
  ram_int[0x99] |= 0xc0; // enable the USB function
  ram_int[0xe8] = 0x00; // clear USB interrupt flags (1)
  ram_int[0xf8] = 0x00; // clear USB interrupt flags (2)
  ram_int[0x9a] = 0x5f; // enable all generic USB interrupts except plug/unplug
  ram_int[0x9b] = 0x11; // enable EP1 IN and EP1 OUT interrupts
  ram_int[0x9c] = 0x00; // set USB device address to 0

  ram_int[0x86] &= 0x3f; // select SFT bank 0
  ram_int[0xa9] |= 0x10; // enable USB interrupt

  ram_int[0x37] = 1;
  ram_int[0x38] = 1;
  ram_int[0x36] = 0;
}

// address ae8f
void read_eeprom_5(void)
{
  ram_int[0xa7] |= 0x01; // set MOVC access to EEPROM

  int addr = 0x0300; // held in pair 0e:0f
  for(int i=0;i<12;++i) {
    ram_ext[0x41+i] = eeprom[addr++];
  }
  for(int i=0;i<12;++i) {
    ram_ext[0x02fd] = eeprom[addr++];
    ram_ext[0x02ea] = eeprom[addr++];
    copy_ext_word();
    ram_ext[0xc5+(i*2)] = ram_ext[0x02ff];
    ram_ext[0xc6+(i*2)] = ram_ext[0x0300];
  }
  for(int i=0;i<12;++i) {
    ram_ext[0x02fd] = eeprom[addr++];
    ram_ext[0x02ea] = eeprom[addr++];
    copy_ext_word();
    ram_ext[0x135+(i*2)] = ram_ext[0x02ff];
    ram_ext[0x136+(i*2)] = ram_ext[0x0300];
  }
  ram_ext[0x02fd] = eeprom[addr++];
  ram_ext[0x02ea] = eeprom[addr++];
  copy_ext_word();
  ram_ext[0x0093] = ram_ext[0x02ff];
  ram_ext[0x0094] = ram_ext[0x0300];

  ram_int[0xa7] &= 0xfe; // set MOVC access to program memory
}

// address ad82
void read_eeprom_4(void)
{
  ram_int[0xa7] |= 0x01; // set MOVC access to EEPROM

  int addr = 0x0550; // held in pair 0e:0f
  for(int i=0;i<12;++i) {
    ram_ext[0x35+i] = eeprom[addr++];
  }
  for(int i=0;i<12;++i) {
    ram_ext[0x02fd] = eeprom[addr++];
    ram_ext[0x02ea] = eeprom[addr++];
    copy_ext_word();
    ram_ext[0xad+(i*2)] = ram_ext[0x02ff];
    ram_ext[0xae+(i*2)] = ram_ext[0x0300];
  }
  for(int i=0;i<12;++i) {
    ram_ext[0x02fd] = eeprom[addr++];
    ram_ext[0x02ea] = eeprom[addr++];
    copy_ext_word();
    ram_ext[0x11d+(i*2)] = ram_ext[0x02ff];
    ram_ext[0x11d+(i*2)] = ram_ext[0x0300];
  }
  ram_ext[0x02fd] = eeprom[addr++];
  ram_ext[0x02ea] = eeprom[addr++];
  copy_ext_word();
  ram_ext[0x0079] = ram_ext[0x02ff];
  ram_ext[0x007a] = ram_ext[0x0300];

  ram_int[0xa7] &= 0xfe; // set MOVC access to program memory
}

// address ac75
void read_eeprom_3(void)
{
  ram_int[0xa7] |= 0x01; // set MOVC access to EEPROM

  int addr = 0x0500; // held in pair 0e:0f
  for(int i=0;i<12;++i) {
    ram_ext[0x29+i] = eeprom[addr++];
  }
  for(int i=0;i<12;++i) {
    ram_ext[0x02fd] = eeprom[addr++];
    ram_ext[0x02ea] = eeprom[addr++];
    copy_ext_word();
    ram_ext[0x95+(i*2)] = ram_ext[0x02ff];
    ram_ext[0x96+(i*2)] = ram_ext[0x0300];
  }
  for(int i=0;i<12;++i) {
    ram_ext[0x02fd] = eeprom[addr++];
    ram_ext[0x02ea] = eeprom[addr++];
    copy_ext_word();
    ram_ext[0x105+(i*2)] = ram_ext[0x02ff];
    ram_ext[0x106+(i*2)] = ram_ext[0x0300];
  }
  ram_ext[0x02fd] = eeprom[addr++];
  ram_ext[0x02ea] = eeprom[addr++];
  copy_ext_word();
  ram_ext[0x0077] = ram_ext[0x02ff];
  ram_ext[0x0078] = ram_ext[0x0300];

  ram_int[0xa7] &= 0xfe; // set MOVC access to program memory
}

// address ab68
void read_eeprom_2(void)
{
  ram_int[0xa7] |= 0x01; // set MOVC access to EEPROM

  int addr = 0x0450; // held in pair 0e:0f
  for(int i=0;i<12;++i) {
    ram_ext[0x1d+i] = eeprom[addr++];
  }
  for(int i=0;i<12;++i) {
    ram_ext[0x02fd] = eeprom[addr++];
    ram_ext[0x02ea] = eeprom[addr++];
    copy_ext_word();
    ram_ext[0x7b+(i*2)] = ram_ext[0x02ff];
    ram_ext[0x7c+(i*2)] = ram_ext[0x0300];
  }
  for(int i=0;i<12;++i) {
    ram_ext[0x02fd] = eeprom[addr++];
    ram_ext[0x02ea] = eeprom[addr++];
    copy_ext_word();
    ram_ext[0xed+(i*2)] = ram_ext[0x02ff];
    ram_ext[0xee+(i*2)] = ram_ext[0x0300];
  }
  ram_ext[0x02fd] = eeprom[addr++];
  ram_ext[0x02ea] = eeprom[addr++];
  copy_ext_word();
  ram_ext[0x0075] = ram_ext[0x02ff];
  ram_ext[0x0076] = ram_ext[0x0300];

  ram_int[0xa7] &= 0xfe; // set MOVC access to program memory
}

// address aa5b
void read_eeprom_1(void)
{
  ram_int[0xa7] |= 0x01; // set MOVC access to EEPROM

  int addr = 0x0400; // held in pair 0e:0f
  for(int i=0;i<12;++i) {
    ram_ext[0x00e1+i] = eeprom[addr++];
  }
  for(int i=0;i<12;++i) {
    ram_ext[0x02fd] = eeprom[addr++];
    ram_ext[0x02ea] = eeprom[addr++];
    copy_ext_word();
    ram_ext[5+(i*2)] = ram_ext[0x02ff];
    ram_ext[6+(i*2)] = ram_ext[0x0300];
  }
  for(int i=0;i<12;++i) {
    ram_ext[0x02fd] = eeprom[addr++];
    ram_ext[0x02ea] = eeprom[addr++];
    copy_ext_word();
    ram_ext[0x59+(i*2)] = ram_ext[0x02ff];
    ram_ext[0x5a+(i*2)] = ram_ext[0x0300];
  }
  ram_ext[0x02fd] = eeprom[addr++];
  ram_ext[0x02ea] = eeprom[addr++];
  copy_ext_word();
  ram_ext[0x0072] = ram_ext[0x02ff];
  ram_ext[0x0073] = ram_ext[0x0300];

  ram_int[0xa7] &= 0xfe; // set MOVC access to program memory
}

// address dbb7
void copy_ext_word(void)
{
  ram_ext[0x02ff] = 0x00;
  ram_ext[0x0300] = 0x00;

  uint8_t tmp1 = ram_ext[0x02fd];
  ram_ext[0x02ff] = 0x00;
  ram_ext[0x0300] = tmp1;

  uint8_t tmp2 = ram_ext[0x02ea];
  ram_ext[0x02ff] = tmp2;
  ram_ext[0x0300] = tmp1;
}

// address db14
void start_pwm(void)
{
  ram_int[0xc7] = 0x81; // enable PWM2 (P4.6, pin 10)
  ram_int[0xd5] = 0xfa;
  ram_int[0xd6] = 0x00; // set PWM2 period to 0x00fa
  ram_int[0xdd] = 0xb4;
  ram_int[0xde] = 0x00; // set PWM2 duty cycle to 0x00b4

  ram_int[0xc6] = 0x81; // enable PWM1 (P4.5, pin 9)
  ram_int[0xd3] = 0xfa;
  ram_int[0xd4] = 0x00; // set PWM1 period to 0x00fa
  ram_int[0xdb] = 0xb4;
  ram_int[0xdc] = 0x00; // set PWM1 duty cycle to 0x00b4

  ram_int[0xc5] = 0x81; // enable PWM0 (P4.4, pin 8)
  ram_int[0xd1] = 0xfa;
  ram_int[0xd2] = 0x00; // set PWM0 period to 0x00fa
  ram_int[0xd9] = 0xb4;
  ram_int[0xda] = 0x00; // set PWM0 duty cycle to 0x00b4

  ram_int[0xa9] |= 0x20; // enable PWM interrupt
}

// address 0006
void start_timer3(void)
{
  ram_int[0x86] |= 0x40; // select SFR bank 1
  
  ram_int[0x88] = 0x10; // disable Timer3, set prescaler to 1/8
  ram_int[0x8c] = 0xf1;
  ram_int[0x8d] = 0xb8; // set Timer3 counter to 0xb8f1
  ram_int[0x88] = 0x14; // start Timer3

  ram_int[0x86] &= 0x3f; // select SFR bank 0
  asm("SETB IEN0.ET3"); // enable Timer3 overflow interrupt
}

// address de53
void sub_de53(void)
{
  ram_ext[0x03b3] = 0;
  ram_ext[0x03b2] = 0;
  ram_ext[0x03b0] = 0;
  ram_ext[0x03af] = 0;
  ram_ext[0x03b3] = 0;
}

// address c9c8
void sub_c9c8(void)
{
  asm("CLR 21.0");
  asm("CLR 22.7");
  asm("CLR 23.2");
  asm("CLR 21.5");
  asm("CLR 22.4");
  asm("CLR 22.3");
  asm("CLR 23.4");
  asm("CLR 22.6");
  asm("CLR 20.7");
  asm("CLR 20.4");
  asm("CLR 22.1");
  asm("CLR 20.3");

  ram_ext[0x0354] = 0;
  asm("CLR 2c.4");
  ram_ext[0x03c8] = 0;

  sub_d9ef(1, 0x00);
  sub_d9ef(2, 0x00);
  sub_d9ef(3, 0x00);

  ram_ext[0x039d] = 0;
  asm("CLR 22.6");
  asm("CLR 23.4");
  ram_ext[0x033e] = 0;
  ram_ext[0x033f] = 0x0c;
  ram_ext[0x0340] = 0x18;
  ram_ext[0x0341] = 0x24;
  ram_ext[0x0342] = 0x30;
  asm("CLR 23.0");
  
  set_col_drive_all();
  clr_row_color_all();
}

// address dddd
void set_col_drive_all(void)
{
  ram_int[0x86] &= 0x3f; // select SFR bank 0
  ram_int[0x90] = 0xff; // P1 = 0xff

  ram_int[0x86] |= 0x40; // select SFR bank 1
  ram_int[0x90] = 0xff; // P6 = 0xff
  ram_int[0xa0] = 0xff; // P7 = 0xff
  asm("SETB P8.0");
  asm("SETB P8.1");
  asm("SETB P8.2");

  ram_int[0x86] &= 0x3f; // select SFR bank 0
}

// address dab9
void clr_row_color_all(void)
{
  ram_int[0x86] &= 0x3f; // select SFR bank 0

  asm("CLR P2.4");
  asm("CLR P2.5");
  asm("CLR P2.6");
  asm("CLR P2.7");
  asm("CLR P3.0");

  ram_int[0x86] |= 0x40; // select SFR bank 1

  asm("CLR P8.6");
  asm("CLR P8.7");
  asm("CLR P8.5");
  asm("CLR P8.4");
  asm("CLR P8.3");
  asm("CLR P5.5");
  asm("CLR P5.6");
  asm("CLR P5.7");
  asm("CLR P5.2");
  asm("CLR P5.3");
  asm("CLR P5.4");
  asm("CLR P5.0");
  asm("CLR P5.1");

  ram_int[0x86] &= 0x3f; // select SFR bank 0
}

// address d9ef
void sub_d9ef(int r7, int r5)
{
  if(r7 == 1) {
    for(int i=0;i<18;++i) {
      ram_int[0xc3+i] = r5; // indirectly accessing high RAM, not SFRs
    }
  }
  if(r7 == 2) {
    for(int i=0;i<18;++i) {
      ram_int[0xb1+i] = r5; // indirectly accessing high RAM, not SFRs
    }
  }
  if(r7 == 3) {
    for(int i=0;i<18;++i) {
      ram_int[0x9f+i] = r5; // indirectly accessing high RAM, not SFRs
    }
  }
}

// address d696
void init(void)
{
  ram_int[0xa1] = 0x03; // start internal regulator at 3.3V
  ram_int[0xb2] = 0x08; // use OSC1 as primary (run at 12MHz)
  ram_int[0xb3] = 0x02; // start 48MHz PLL, disconnected
  delay(0x0513);        // wait 2ms for PLL to lock
  ram_int[0xb3] = 0x03; // connect PLL to OSC2
  ram_int[0xb2] = 0x0c; // use OSC2 as primary (run at 24MHz)
  
  ram_int[0xa2] = 0x00; // disable OP function
  ram_int[0x93] = 0x00; // disable ADC function (1)
  ram_int[0x92] = 0x00; // disable ADC function (2)
  ram_int[0x95] = 0x00; // disable ADC function (3)
  ram_int[0xab] = 0x00; // disable LCD function (1)
  ram_int[0xaa] = 0x00; // disable LCD function (2)
  ram_int[0x9e] = 0x00; // disable LCD function (3)
  ram_int[0x9f] = 0x00; // disable LCD function (4)
  ram_int[0xac] = 0x00; // disable LCD function (5)
  ram_int[0xad] = 0x00; // disable LCD function (6)
  ram_int[0xae] = 0x00; // disable LCD function (7)
  ram_int[0xa4] = 0x00; // disable SPI function (1)
  ram_int[0xa5] = 0x00; // disable SPI function (2)
  ram_int[0xa6] = 0x00; // disable SPI function (3)
  
  ram_int[0xb9] = 0x10;
  ram_int[0xb5] = 0x10; // set USB interrupt to have highest priority

  gpio_init();
}

// address dd8d
void delay(int time)
{
  ;
}

// address ce49
void gpio_init(void)
{
  ram_int[0x80] = 0xe3; // P0 = 0xe3
  ram_int[0xe9] = 0xf3; // enable pullups on P0.0,1,4,5,6,7
  ram_int[0xe1] = 0x01; // set P0.0 as output

  ram_int[0x90] = 0xff; // P1 = 0xff
  ram_int[0xea] = 0xff; // enable pullups on P1.0-7
  ram_int[0xe2] = 0xff; // set P1.0-7 as output

  ram_int[0xa0] = 0xfe; // P2 = 0xfe
  ram_int[0xeb] = 0xfe; // enable pullups on P2.1-7
  ram_int[0xe3] = 0xf0; // set P2.4-7 as output

  ram_int[0xb0] = 0xff; // P3 = 0xff
  ram_int[0xec] = 0xff; // enable pullups on P3.0-7
  ram_int[0xe4] = 0xff; // set P3.0-7 as output

  ram_int[0xc0] = 0xff; // P4 = 0xff
  ram_int[0xed] = 0xff; // enable pullups on P4.0-7
  ram_int[0xe5] = 0xff; // set P4.0-7 as output

  ram_int[0x86] |= 0x40; // select SFR bank 1

  ram_int[0x80] = 0xff; // P5 = 0xff
  ram_int[0xe9] = 0xff; // enable pullups on P5.0-7
  ram_int[0xe1] = 0xff; // set P5.0-7 as output

  ram_int[0x90] = 0xff; // P6 = 0xff
  ram_int[0xea] = 0xff; // enable pullups on P6.0-7
  ram_int[0xe2] = 0xff; // set P6.0-7 as output

  ram_int[0xa0] = 0xff; // P7 = 0xff
  ram_int[0xeb] = 0xff; // enable pullups on P7.0-7
  ram_int[0xe3] = 0xff; // set P7.0-7 as output

  ram_int[0xb0] = 0xff; // P8 = 0xff
  ram_int[0xec] = 0xff; // enable pullups on P8.0-7
  ram_int[0xe4] = 0xff; // set P8.0-7 as output

  ram_int[0x86] &= 0x3f; // select SFR bank 0
}
