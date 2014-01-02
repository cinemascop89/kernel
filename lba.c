#include <colors.h>
#include <system.h>
#include <lba.h>

#define IO_DELAY()        do{inportb(0x80); inportb(0x80); inportb(0x80); inportb(0x80);}while(0)

lba_drive_t *drives[4];

void primary_disk_handler(struct regs *r) {
  printf("primary disk handler: %b\n", inportb(LBA_COMMAND_P_PORT));
  outportb(PIC1_COMMAND, PIC_EOI);
  outportb(PIC2_COMMAND, PIC_EOI);
}

void secondary_disk_handler(struct regs *r) {
  printf("secondary disk handler called\n");
}

int wait_for_data(int offset) {
  while(!(inportb(LBA_COMMAND_PORT + offset) & LBA_STATUS_DRIVE_READY));
  //check LBAhi and LBAmid
  while(!(inportb(LBA_COMMAND_PORT + offset) & LBA_STATUS_DATA_READY));

}
void send_rw_command(int command, unsigned short count, lba_drive_t* drive, unsigned long address) {
  int i;
  unsigned short offset = drive->bus == PRIMARY ? 0 : LBA_S_OFFSET,
    slavebit = drive->type == MASTER ? 0 : 1,
    thing = drive->type == MASTER ? 0x40 : 0x50; //How's this called?

  //  outportb(LBA_3_BLOCK_ADDR_P_PORT, LBA_SELECT_MASTER_COMMAND);
  outportb(LBA_FEATURES_PORT + offset, 0x00);
  outportb(LBA_3_BLOCK_ADDR_PORT + offset, thing | (slavebit << 4));
  IO_DELAY();

  outportb(LBA_SECTOR_COUNT_PORT + offset, count >> 8);
  outportb(LBA_0_BLOCK_ADDR_PORT + offset, (unsigned char) (address >> 24) & 0xff);
  outportb(LBA_1_BLOCK_ADDR_PORT + offset, (unsigned char) (address >> 32) & 0xff);
  outportb(LBA_2_BLOCK_ADDR_PORT + offset, (unsigned char) (address >> 40) & 0xff);
  outportb(LBA_SECTOR_COUNT_PORT + offset, count & 0xff);
  outportb(LBA_0_BLOCK_ADDR_PORT + offset, (unsigned char) address & 0xff);
  outportb(LBA_1_BLOCK_ADDR_PORT + offset, (unsigned char) (address >> 8) & 0xff);
  outportb(LBA_2_BLOCK_ADDR_PORT + offset, (unsigned char) (address >> 16) & 0xff);

  outportb(LBA_COMMAND_PORT + offset, command);
  IO_DELAY();
}

int lba_read_sectors
(lba_drive_t* drive, unsigned long address, unsigned short count, unsigned char *buff) {
  send_rw_command(LBA_READ_COMMAND_EXT, count, drive, address);
  unsigned short offset = drive->bus == PRIMARY ? 0 : LBA_S_OFFSET;
  int i;
  unsigned char response;
  while( (response & LBA_STATUS_DRIVE_BUSY) && !(response & 1) )
    response = inportb(LBA_COMMAND_PORT + offset);
  for (i = 0; i < count; i++) {
    while(!(inportb(LBA_COMMAND_PORT + offset) & LBA_STATUS_DRIVE_READY));
    //wait_for_data(offset);
    do {
      response = inportb(LBA_COMMAND_PORT + offset);
      if ((response & LBA_STATUS_ERROR) || (response & LBA_STATUS_DRIVE_FAULT)) {
        printf("disk error: %b\n", response);
        return 0;
      }
    } while(!(response & LBA_STATUS_DATA_READY));

    inportsw(LBA_DATA_PORT+offset, (char*)(buff+i*512), 256);
    int i2;
    unsigned int temp;
  }
  return 1;
}

void lba_write_sectors
(lba_drive_t* drive, unsigned long address, unsigned short count, unsigned char *buff) {
  send_rw_command(LBA_WRITE_COMMAND_EXT, count, drive, address);
  unsigned short offset = drive->bus == PRIMARY ? 0 : LBA_S_OFFSET;
  int i;
  unsigned char response;
  for (i = 0; i < count; i++) {
    do {
      response = inportb(LBA_COMMAND_PORT + offset);
      if ((response & LBA_STATUS_ERROR) || (response & LBA_STATUS_DRIVE_FAULT)) {
        return;
      }

    } while(!(response & LBA_STATUS_DRIVE_READY));


    int idx;
    unsigned short temp;
    for (idx = 0; idx < 256; idx++) {
      temp = ((unsigned short)buff[i*256 + idx*2] << 8) + buff[i*256 + idx*2 + 1];
      outportw(LBA_DATA_PORT + offset, temp);
      printf("");
    }
    outportb(LBA_COMMAND_PORT + offset, LBA_CACHE_FLUSH_COMMAND);
    //    sleep(0.1);
    while(!(inportb(LBA_COMMAND_PORT + offset) & LBA_STATUS_DATA_READY));

  }
}

void show_drive_info(lba_drive_t* d) {
  printf("%s %s using %s, %d available sectors (%d mbytes)\n",
         d->bus == PRIMARY ? "Primary" : "Secondary",
         d->type == MASTER ? "master" : "slave",
         d->supports_lba48 ? "LBA48" : "LBA28",
         d->addressable_sectors,
         d->addressable_sectors * 512 / 1024 / 1024);
}

void identify(unsigned short drive_t) {
  unsigned short offset;
  switch (drive_t) {
  case LBA_PRIMARY_MASTER:
    outportb(LBA_3_BLOCK_ADDR_P_PORT, LBA_SELECT_MASTER_COMMAND);
    offset = 0;
    break;
  case LBA_PRIMARY_SLAVE:
    outportb(LBA_3_BLOCK_ADDR_P_PORT, LBA_SELECT_SLAVE_COMMAND);
    offset = 0;
    break;
  case LBA_SECONDARY_MASTER:
    outportb(LBA_3_BLOCK_ADDR_S_PORT, LBA_SELECT_MASTER_COMMAND);
    offset = LBA_S_OFFSET;
    break;
  case LBA_SECONDARY_SLAVE:
    outportb(LBA_3_BLOCK_ADDR_S_PORT, LBA_SELECT_SLAVE_COMMAND);
    offset = LBA_S_OFFSET;
    break;
  }
  settextcolor(GREEN, BLACK);
  printf("IDENTIFY\n");

  outportb(LBA_SECTOR_COUNT_PORT + offset, 0x00);
  outportb(LBA_0_BLOCK_ADDR_PORT + offset, 0x00);
  outportb(LBA_1_BLOCK_ADDR_PORT + offset, 0x00);
  outportb(LBA_2_BLOCK_ADDR_PORT + offset, 0x00);
  outportb(LBA_COMMAND_PORT + offset, LBA_IDENTIFY_COMMAND);
  IO_DELAY();

  unsigned int result;
  result = inportb(LBA_COMMAND_PORT + offset);
  if (result) {
    wait_for_data(offset);
    unsigned int buff[256];
    int i;
    for(i=0;i<256;i++)
      buff[i] = inportw(LBA_DATA_P_PORT);

    lba_drive_t* drive = (lba_drive_t*)malloc(sizeof(lba_drive_t*));
    drive->type = drive_t == LBA_PRIMARY_MASTER || drive_t == LBA_SECONDARY_MASTER ? MASTER : SLAVE;
    drive->bus = drive_t == LBA_PRIMARY_MASTER || drive_t == LBA_PRIMARY_SLAVE ? PRIMARY : SECONDARY;

    drive->supports_lba48 = (buff[83] & (1<<10)) != 0;
    if (drive->supports_lba48)
      drive->addressable_sectors = (unsigned long)buff[100] +
        (buff[101] << 32) +
        (buff[102] << 64) +
        (buff[102] << 96) +
        (buff[103] << 128);
    else
      drive->addressable_sectors = buff[60] + (buff[61] << 32);
    drives[drive_t] = drive;
  } else {
    // Drive doesn't exists
  }
  settextcolor(WHITE, BLACK);
}

void init_drives() {
  unsigned char response;
  int i;

  for (i=0; i < 4; i++) drives[i] = NULL;

   /* irq_install_handler(14, primary_disk_handler); */
   /* irq_install_handler(15, secondary_disk_handler); */
  outportb(LBA_COMMAND_P_PORT, (1<<1));
  outportb(LBA_COMMAND_S_PORT, (1<<1));

  outportb(LBA_0_BLOCK_ADDR_P_PORT, LBA_MAGIC_VALUE);
  response = inportb(LBA_0_BLOCK_ADDR_P_PORT);
  if (response == LBA_MAGIC_VALUE) {
    outportb(LBA_3_BLOCK_ADDR_P_PORT, LBA_SELECT_MASTER_COMMAND);
    sleep(0.1);
    response = inportb(LBA_COMMAND_P_PORT);
    if (response & LBA_STATUS_DRIVE_READY) {
      identify(LBA_PRIMARY_MASTER);
    }
  } else {
    //Primary controller not present
  }

  outportb(LBA_0_BLOCK_ADDR_S_PORT, LBA_MAGIC_VALUE);
  response = inportb(LBA_0_BLOCK_ADDR_S_PORT);
  if (response == LBA_MAGIC_VALUE) {
    outportb(LBA_3_BLOCK_ADDR_P_PORT, LBA_SELECT_SLAVE_COMMAND);
    sleep(0.1);
    response = inportb(LBA_COMMAND_S_PORT);
    if (response & LBA_STATUS_DRIVE_READY) {
      identify(LBA_PRIMARY_SLAVE);
    }
  } else {
    // Secondary controller not present
  }

  for (i=0; i < 4; i++) {
    if (drives[i])
      show_drive_info(drives[i]);
  }
}

lba_drive_t* get_drive(unsigned short drive_n) {
  return drives[drive_n];
}
