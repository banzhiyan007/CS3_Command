#ifndef LINK_REG_H
#define LINK_REG_H

extern int regRemoteCount;

uint32_t link_reg_count(void);
const char *link_reg_name(int index);
uint32_t link_reg_type(int index);
uint32_t link_reg_size(int index);
const char *link_reg_info(int index);
int link_reg_index(const char *name);
uint32_t link_reg_get_u32_by_index(int index);
char *link_reg_get_string_by_index(int index);
unsigned char *link_reg_get_bytes_by_index(int index);
unsigned char *link_reg_get_buffer_by_index(int index);
float link_reg_get_float_by_index(int index);
double link_reg_get_double_by_index(int index);
void link_reg_set_u32_by_index(int index,uint32_t value);
void link_reg_set_string_by_index(int index,char *pValue);
void link_reg_set_bytes_by_index(int index,uint8_t *pValue,int len);
void link_reg_set_float_by_index(int index,float value);
void link_reg_set_double_by_index(int index,double value);
void link_reg_exec_function_by_index(int index);
uint32_t link_reg_get_u32(const char *name,uint32_t default_value);
char *link_reg_get_string(const char *name,const char *default_value);
unsigned char *link_reg_get_bytes(const char *name,uint8_t *default_value);
unsigned char *link_reg_get_buffer(const char *name,uint8_t *default_value);
float link_reg_get_float(const char *name,float default_value);
double link_reg_get_double(const char *name,double default_value);
void link_reg_set_u32(const char *name,uint32_t value);
void link_reg_set_string(const char *name,char *pValue);
void link_reg_set_bytes(const char *name,uint8_t *pValue,int len);
void link_reg_set_float(const char *name,float value);
void link_reg_set_double(const char *name,double value);
void link_reg_exec_function(const char *name);

#endif

