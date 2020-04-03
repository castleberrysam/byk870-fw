#include <stdio.h>

extern char *fw_data;
extern int fw_len;

// used to hold the RunOnlyOneUpdateTools mutex
extern HANDLE dword_458908;

// used to hold the first byte pair after the firmware header
extern int dword_45868c;

// used to hold the second byte pair after the firmware header
extern int dword_458688;

// args: ecx, arg_0
void load_resource(wchar_t **resource_ptr, char *fname)
{
  ;
}

// Unknown is a superclass of CWinApp
void Unknown::sub_405C10(void)
{
  INITCOMMONCONTROLSEX icce;
  icce.dwSize = 8;
  icce.dwICC = 0xff;
  InitCommonControlsEx(&icce);
  
  this->InitInstance();
  // intialize some error strings on the stack...
  
  OSVERSIONINFOW versionInfo;
  versionInfo.dwOSVersionInfoSize = 0x114;
  GetVersionExW(&versionInfo);

  dword_458908 = CreateMutexW(NULL, FALSE, "RunOnlyOneUpdateTools");
  if(dword_458908 == NULL) {
    return 1;
  }

  if(GetLastError() == ERROR_ALREADY_EXISTS) {
    MessageBoxW(NULL, "Tool has already run!", "Warning", MB_ICONWARNING);
    return 1;
  }

  char fname[260];
  struct AFX_MODULE_STATE *state = AfxGetModuleState();
  GetModuleFileNameW(state[8], fname, sizeof(fname));

  wchar_t *resource_ptr;
  load_resource(&resource_ptr, fname);

  // used after "if(file != NULL)", set inside only though
  int var_98b4;
  
  // esi
  FILE *file = __wfopen(resource_ptr, "rb");
  if(file != NULL) {
    int var_98cc;

  loop:
    var_98cc = 0;
    _fread(&var_98cc, 1, 1, file);
    if(var_98cc != 0x5a) {goto loop;}

    var_98cc = 0;
    _fread(&var_98cc, 1, 1, file);
    if(var_98cc != 0xa5) {goto loop;}

    var_98cc = 0;
    _fread(&var_98cc, 1, 4, file);
    if(((var_98cc >> 0) & 0xff) != 0x53) {goto loop;}
    if(((var_98cc >> 8) & 0xff) != 0x49) {goto loop;}
    if(((var_98cc >> 16) & 0xff) != 0x4e) {goto loop;}
    if(((var_98cc >> 24) & 0xff) != 0x4f) {goto loop;}

    short var_98c0;
    _fread(&var_98c0, 1, 2, file);
    dword_45868c = ((var_98c0 & 0xff) << 8) + (var_98c0 >> 8);

    short var_98c4;
    _fread(&var_98c4, 1, 2, file);
    dword_458688 = ((var_98c4 & 0xff) << 8) + (var_98c4 >> 8);

    short var_98c8;
    _fread(&var_98c8, 1, 2, file);

    fw_len = ((var_98c8 & 0xff) << 8) + (var_98c8 >> 8);

    // edi
    fw_data = malloc(fw_len);
    memset(fw_data, 0x00, fw_len);
    _fread(fw_data, 1, fw_len, file);

    _fread(&var_98b4, 1, 4, file);
    _fclose(file);
  }

  sub_402FF0(&var_98b4);

  this->SetRegistryKey("SH68F073");

  // some dialog stuff...
  return 0;
}

void sub_402FF0(int arg_0)
{
  int eax = fw_len;
  if(eax < 0) {eax += 3;}
  
  int edi = eax / 4;
  eax = edi * 4;
  
  // esi
  int[] buf = new int[edi];

  char *ptr = fw_data;
  for(int ecx=0;ecx<edi;++i) {
    buf[ecx] = (ptr[0] << 24) + (ptr[1] << 16) + (ptr[2] << 8) + ptr[3];
    ptr += 4;
  }

  int eax = fw_len;
  if(eax < 0) {eax += 7;}
  eax /= 8;
  
  char *ebx = buf;
  for(;eax>0;--eax) {
    sub_402F50(ebx, arg_0);
    ebx += 8;
  }

  char *ecx = fw_data;
  for(int eax=0;eax<edi;++eax) {
    ecx[0] = buf[(eax*4)+3];
    ecx[1] = buf[(eax*4)+2];
    ecx[2] = buf[(eax*4)+1];
    ecx[3] = buf[(eax*4)+0];
    ecx += 4;
  }
  
  delete[] buf;
}

// args: arg_0, eax
void sub_402F50(unsigned int *ebx, unsigned char *eax)
{
  unsigned char byte0 = eax[0];
  unsigned char byte1 = eax[1];
  unsigned char byte2 = eax[2];
  unsigned char byte3 = eax[3];

  unsigned int int0 = ebx[0];
  unsigned int int1 = ebx[1];

  unsigned int edx = 0xc6ed3720;
  for(int esi=32;esi>0;--esi) {
    unsigned int edi, ebx;
    
    edi = byte3 + (int0 >> 5);
    ebx = byte2 + (int0 << 4);
    edi ^= ebx;
    edi ^= edx + int0;
    int1 -= edi;
    
    edi = byte0 + (int1 << 4);
    ebx = byte1 + (int1 >> 5);
    edi ^= ebx;
    edi ^= edx + int1;
    int0 -= edi;
    
    edx += 0x61c89647;
  }
  
  ebx[0] = int0;
  ebx[1] = int1;
}

// Used for sending data
// args: arg_0, arg_4, arg_8, arg_C, ecx (this)
void HANDLE::fw_upload(arg_0, arg_4, arg_8, arg_C)
{
  // ebx
  char[] buf = new char[arg_8];
  buf[0] = 5;
  buf[1] = 0x57;
  buf[2] = 0;
  buf[3] = 0;
  buf[4] = arg_C & 0xff;
  buf[5] = arg_C >> 8;
  
  if(HidD_SetFeature(this, buf, sizeof(buf)) == 0) {
    delete[] buf;
    return 0;
  }

  // ebp
  char[] buf2 = new char[2048+2];
  memset(buf2, 0x00, sizeof(buf2));
  buf2[0] = 6;
  buf2[1] = 0x77;

  // reset progress bar
  SendMessageW(..., 0x402, 0, NULL);
  
  if(arg_C < 0) {arg_C += 2047;}
  int num_pages = arg_C / 2048;

  char big_buf[2048];

  for(int page=0;page<num_pages;++page) {
    memset(big_buf, 0x00, sizeof(big_buf));
    
    int offset = page * 2048;
    
    int size = 2048;
    if(offset + size > fw_len) {
      size = fw_len - offset;
    }
    
    memcpy(big_buf, fw_data+offset, size);
    if(page == 0) {
      ((int *) big_buf)[0] = 0;
    }
    
    memcpy(&buf2[2], big_buf, size);
    if(HidD_SetFeature(arg_4, buf2, sizeof(buf2)) == 0) {
      delete[] buf;
      delete[] buf2;
      return 0;
    }

    // update progress bar
    SendMessageW(..., 0x402, ..., NULL);
  }

  // restart upload at base addr
  HidD_SetFeature(this, buf, sizeof(buf));
  
  // resend first page of fw_data??
  int *str = (int *) fw_data;
  int *dst = (int *) (buf2 + 2);
  for(int i=0;i<512;++i) {
    dst[i] = ptr[i];
  }
  HidD_SetFeature(arg_4, buf2, sizeof(buf2));
  
  delete[] buf2;
  delete[] buf;
  return 1;
}

// Used for reading back data
// args: arg_0, arg_4, arg_8, ecx, edx
void HANDLE::fw_verify(arg_0, arg_4, arg_8)
{
  int edi = edx;
  
  // ebp
  char[] buf = new char[ecx];
  buf[0] = 5;
  buf[1] = 0x52;
  buf[2] = 0;
  buf[3] = 0;
  buf[4] = arg_8 & 0xff;
  buf[5] = arg_8 >> 8;

  if(HidD_SetFeature(edi, buf, sizeof(buf)) == 0) {
    delete[] buf;
    return 0;
  }

  // edi
  char[] buf2 = new char[2048+2];
  memset(buf2, 0x00, 2048+2);
  buf2[0] = 6;
  buf2[1] = 0x72;
  
  // reset progress bar
  SendMessageW(..., 0x402, 0, NULL);

  if(arg_8 < 0) {arg_8 += 2047;}
  int num_pages = arg_8 / 2048;

  char big_buf[2048];

  int offset = 0;
  for(int page=0;page<num_pages;++page) {
    if(HidD_GetFeature(arg_4, buf2, sizeof(buf2)) == 0) {
      delete[] buf;
      delete[] buf2;
      return 0;
    }
    
    // update progress bar
    SendMessageW(..., 0x402, ..., NULL);
    
    memset(big_buf, 0x00, sizeof(big_buf));
    
    int size = 2048;
    if(offset + 2048 > fw_len) {
      size = fw_len - offset;
    }
    memcpy(big_buf, fw_data+offset, size);

    int index = 0;
    while(size >= 4) {
      int val0 = ((int *) big_buf)[index/4];
      int val1 = ((int *) (buf2+2))[index/4];
      if(val0 != val1) {goto finer;}
      index += 4;
      size -= 4;
    }
    
    if(size == 0) {
      offset += 2048;
      continue;
    }

  finer:
    char val0 = big_buf[index];
    char val1 = buf2[2+index];
    if(val0 != val1) {
      delete[] buf;
      delete[] buf2;
      return 0;
    }

    --size;
    ++index;
    if(size == 0) {
      offset += 2048;
      continue;
    }

    val0 = big_buf[index];
    val1 = buf2[2+index];
    if(val0 != val1) {
      delete[] buf;
      delete[] buf2;
      return 0;
    }

    --size;
    ++index;
    if(size == 0) {
      offset += 2048;
      continue;
    }

    val0 = big_buf[index];
    val1 = buf2[2+index];
    if(val0 != val1) {
      delete[] buf;
      delete[] buf2;
      return 0;
    }

    --size;
    ++index;
    if(size == 0) {
      offset += 2048;
      continue;
    }

    val0 = big_buf[index];
    val1 = buf2[2+index];
    if(val0 != val1) {
      delete[] buf;
      delete[] buf2;
      return 0;
    }
  }

  delete[] buf;
  delete[] buf2;
  Sleep(100);
  return 1;
}

// Used for burning in sent data
// args: arg_0, eax
void fw_burn(arg_0)
{
  int edi = eax;
  
  // esi
  char[] buf = new char[6];
  buf[0] = 5;
  buf[1] = 0x52;
  buf[2] = 0x80;
  buf[3] = 0xff;
  buf[4] = 8;
  buf[5] = 0;

  HidD_SetFeature(edi, buf, sizeof(buf));

  buf[0] = 5;
  buf[1] = 0x72;
  buf[2] = 0;
  buf[3] = 0;
  buf[4] = 0;
  buf[5] = 0;
  
  HidD_GetFeature(edi, buf, sizeof(buf));

  // eax, arg_0
  int val0 = (buf[4] << 8) | buf[5];
  // ebx
  int val1 = (buf[2] << 8) | buf[3];
  
  HidD_GetFeature(edi, buf, sizeof(buf));

  buf[0] = 5;
  buf[1] = 0x65;
  buf[2] = 0xff;
  buf[3] = 0;
  buf[4] = 0;
  buf[5] = 0;
  
  HidD_SetFeature(edi, buf, sizeof(buf));

  Sleep(200);

  buf[0] = 5;
  buf[1] = 0x57;
  buf[2] = 0x80;
  buf[3] = 0xff;
  buf[4] = 8;
  buf[5] = 0;
  
  HidD_SetFeature(edi, buf, sizeof(buf));

  buf[0] = 5;
  buf[1] = 0x77;
  buf[2] = val1 >> 8;
  buf[3] = val1 & 0xff;
  buf[4] = val0 >> 8;
  buf[5] = val0 & 0xff;
  
  HidD_SetFeature(edi, buf, sizeof(buf));

  Sleep(100);

  buf[0] = 5;
  buf[1] = 0x77;
  buf[2] = arg_0[0x42c];
  buf[3] = 0;
  buf[4] = arg_0[0x424] >> 8;
  buf[5] = arg_0[0x424] & 0xff;
  
  int ret = HidD_SetFeature(edi, buf, sizeof(buf));

  delete[] buf;
  return (ret != 0) ? 1 : 0;
}

void Unknown::sub_4014E0(void)
{
  printf("Open device...\n");

  LPCWSTR fname0 = "???";
  LPCWSTR fname1 = "???";

  // edi = file0, ebx = file1
  HANDLE file0 = CreateFileW(fname0, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE,
                             NULL, OPEN_EXISTING, FILE_FLAG_OVERLAPPED, NULL);
  HANDLE file1 = CreateFileW(fname1, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE,
                             NULL, OPEN_EXISTING, FILE_FLAG_OVERLAPPED, NULL);

  if(file0 == -1) {
    printf("Open device fail\n");
    return 2;
  }

  printf("Open is Ok\n");

  HIDD_ATTRIBUTES attrs;
  attrs.Size = 12;
  HidD_GetAttributes(file0, &attrs);

  PHIDP_PREPARSED_DATA ppData;
  HidD_GetPreparsedData(file0, ppData);

  HIDP_CAPS caps;
  HidP_GetCaps(ppData, &caps);

  HidD_FreePreparsedData(ppData);

  // ebp
  char[] buf = new char[caps.FeatureReportByteLength];
  memset(buf, 0x45, sizeof(buf));

  buf[0] = 5;
  if(HidD_SetFeature(file1, buf, sizeof(buf)) == 0) {
    printf("Erase fail\n");
    return 2;
  }
  
  printf("Erasing...\n");
  Sleep(2000);

  int eax = fw_len;
  int ecx = eax & 0x800007ff;
  int var_78 = eax;
  if(var_78 < 0) {
    ecx = ((ecx - 1) & 0xfffff800) + 1;
    if(ecx != 0) {goto label;}
  } else if(var_78 != 0) {
  label:
    if(eax < 0) {eax += 2047;}
    eax = ((eax / 2048) + 1) * 2048;
    var_78 = eax;
  }
  
  int counter = 0;
  while(true) {
    // args: arg_0 = this, arg_4 = file0, arg_8 = sizeof(buf), arg_C = var_78, ecx = file1
    if(file1.fw_upload(this, file0, sizeof(buf), var_78) != 0) {
      if(counter < 5) {
        break;
      } else {
      error:
        CloseHandle(file0);
        CloseHandle(file1);
        delete[] buf;

        printf("Send data fail\n");
        return 2;
      }
    } else {
      if(counter < 5) {
        ++counter;
      } else {
        goto error;
      }
    }
  }

  counter = 0;
  while(true) {
    // args: arg_0 = this, arg_4 = file0, arg_8 = var_78, ecx = sizeof(buf), edx = file1
    if(file1.fw_verify(this, file0, var_78) != 0) {
      if(counter < 10) {
        break;
      } else {
      error:
        CloseHandle(file0);
        CloseHandle(file1);
        delete[] buf;
        printf("Read back fail\n");
        return 2;
      }
    } else {
      if(counter < 10) {
        ++counter;
      } else {
        goto error;
      }
    }
  }

  // args: arg_0 = this, eax = file1
  if(fw_burn(this) == 0) {
    CloseHandle(file0);
    CloseHandle(file1);
    delete[] buf;
    printf("Burn fail\n");
    return 2;
  }

  memset(buf, 0x55, sizeof(buf));
  buf[0] = 5;
  if(HidD_SetFeature(file1, buf, sizeof(buf)) == 0) {
    printf("Burn fail\n");
    CloseHandle(file0);
    CloseHandle(file1);
    delete[] buf;
    return 2;
  }

  CloseHandle(file0);
  CloseHandle(file1);
  delete[] buf;
  return 0;
}
