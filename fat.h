int fat_getpartition(void);
unsigned int fat_getcluster(char *fn);
char *fat_readfile(unsigned int cluster);
void *fat_writefile(unsigned int cluster, unsigned char *buffer);
int fat_resize(char *fn, unsigned int size);
