#ifndef RFID_H_
#define RFID_H_

char *find(char *search_uid);
void rfid_read(const char *const *argv);
void rfid_add(const char *const *argv);
void rfid_remove(const char *const *argv);
void rfid_print(void);

#endif
