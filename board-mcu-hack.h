
void translate_board_mcu(const char *board_in, char *board_out, char *mcu_out);

#ifdef __WIN32__
#define fread       broken  /* fread() unusable on windows */
int fread_(char *buffer, unsigned len, FILE *fp);
#endif /* __WIN32__ */
