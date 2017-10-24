#ifndef LINK_H
#define LINK_H

#define LINK_FRAME_SIZE		(4096+512)

struct LINK_FRAME_HEAD
{
	uint32_t beginFlag;
	uint16_t seq;
	uint16_t size;
}__attribute__((packed));
extern uint8_t	*linkFrame;
extern LINK_FRAME_HEAD *linkFrameHead;

extern void (*link_before_tx)(void);
extern void (*link_after_tx)(void);
extern void (*link_rx_cmd)(uint8_t cmd,uint8_t *data,uint16_t len);
extern void (*link_after_rx)(void);
extern void link_cmd_add(uint8_t cmd,void *data,uint16_t len);

#endif
