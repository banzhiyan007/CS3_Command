
// 使用__attribute__((weak))定义入口函数表，这样可以不占内存
// 使用的时候直接定义一个强定义就可以了

#undef  HANDLER
#define HANDLER(code)	__attribute__((weak)) int cmd_handler_0x1f_##code(uint8_t *Cmd,int CmdLen,int *pCmdLen,void (*tx)(char *buf,int len));
#undef  pHANDLER
#define pHANDLER(code)	cmd_handler_0x1f_##code

HANDLER(0x00);HANDLER(0x01);HANDLER(0x02);HANDLER(0x03);HANDLER(0x04);HANDLER(0x05);HANDLER(0x06);HANDLER(0x07);
HANDLER(0x08);HANDLER(0x09);HANDLER(0x0A);HANDLER(0x0B);HANDLER(0x0C);HANDLER(0x0D);HANDLER(0x0E);HANDLER(0x0F);

HANDLER(0x10);HANDLER(0x11);HANDLER(0x12);HANDLER(0x13);HANDLER(0x14);HANDLER(0x15);HANDLER(0x16);HANDLER(0x17);
HANDLER(0x18);HANDLER(0x19);HANDLER(0x1A);HANDLER(0x1B);HANDLER(0x1C);HANDLER(0x1D);HANDLER(0x1E);HANDLER(0x1F);

HANDLER(0x20);HANDLER(0x21);HANDLER(0x22);HANDLER(0x23);HANDLER(0x24);HANDLER(0x25);HANDLER(0x26);HANDLER(0x27);
HANDLER(0x28);HANDLER(0x29);HANDLER(0x2A);HANDLER(0x2B);HANDLER(0x2C);HANDLER(0x2D);HANDLER(0x2E);HANDLER(0x2F);

HANDLER(0x30);HANDLER(0x31);HANDLER(0x32);HANDLER(0x33);HANDLER(0x34);HANDLER(0x35);HANDLER(0x36);HANDLER(0x37);
HANDLER(0x38);HANDLER(0x39);HANDLER(0x3A);HANDLER(0x3B);HANDLER(0x3C);HANDLER(0x3D);HANDLER(0x3E);HANDLER(0x3F);

HANDLER(0x40);HANDLER(0x41);HANDLER(0x42);HANDLER(0x43);HANDLER(0x44);HANDLER(0x45);HANDLER(0x46);HANDLER(0x47);
HANDLER(0x48);HANDLER(0x49);HANDLER(0x4A);HANDLER(0x4B);HANDLER(0x4C);HANDLER(0x4D);HANDLER(0x4E);HANDLER(0x4F);

HANDLER(0x50);HANDLER(0x51);HANDLER(0x52);HANDLER(0x53);HANDLER(0x54);HANDLER(0x55);HANDLER(0x56);HANDLER(0x57);
HANDLER(0x58);HANDLER(0x59);HANDLER(0x5A);HANDLER(0x5B);HANDLER(0x5C);HANDLER(0x5D);HANDLER(0x5E);HANDLER(0x5F);

HANDLER(0x60);HANDLER(0x61);HANDLER(0x62);HANDLER(0x63);HANDLER(0x64);HANDLER(0x65);HANDLER(0x66);HANDLER(0x67);
HANDLER(0x68);HANDLER(0x69);HANDLER(0x6A);HANDLER(0x6B);HANDLER(0x6C);HANDLER(0x6D);HANDLER(0x6E);HANDLER(0x6F);

HANDLER(0x70);HANDLER(0x71);HANDLER(0x72);HANDLER(0x73);HANDLER(0x74);HANDLER(0x75);HANDLER(0x76);HANDLER(0x77);
HANDLER(0x78);HANDLER(0x79);HANDLER(0x7A);HANDLER(0x7B);HANDLER(0x7C);HANDLER(0x7D);HANDLER(0x7E);HANDLER(0x7F);

HANDLER(0x80);HANDLER(0x81);HANDLER(0x82);HANDLER(0x83);HANDLER(0x84);HANDLER(0x85);HANDLER(0x86);HANDLER(0x87);
HANDLER(0x88);HANDLER(0x89);HANDLER(0x8A);HANDLER(0x8B);HANDLER(0x8C);HANDLER(0x8D);HANDLER(0x8E);HANDLER(0x8F);

HANDLER(0x90);HANDLER(0x91);HANDLER(0x92);HANDLER(0x93);HANDLER(0x94);HANDLER(0x95);HANDLER(0x96);HANDLER(0x97);
HANDLER(0x98);HANDLER(0x99);HANDLER(0x9A);HANDLER(0x9B);HANDLER(0x9C);HANDLER(0x9D);HANDLER(0x9E);HANDLER(0x9F);

HANDLER(0xA0);HANDLER(0xA1);HANDLER(0xA2);HANDLER(0xA3);HANDLER(0xA4);HANDLER(0xA5);HANDLER(0xA6);HANDLER(0xA7);
HANDLER(0xA8);HANDLER(0xA9);HANDLER(0xAA);HANDLER(0xAB);HANDLER(0xAC);HANDLER(0xAD);HANDLER(0xAE);HANDLER(0xAF);

HANDLER(0xB0);HANDLER(0xB1);HANDLER(0xB2);HANDLER(0xB3);HANDLER(0xB4);HANDLER(0xB5);HANDLER(0xB6);HANDLER(0xB7);
HANDLER(0xB8);HANDLER(0xB9);HANDLER(0xBA);HANDLER(0xBB);HANDLER(0xBC);HANDLER(0xBD);HANDLER(0xBE);HANDLER(0xBF);

HANDLER(0xC0);HANDLER(0xC1);HANDLER(0xC2);HANDLER(0xC3);HANDLER(0xC4);HANDLER(0xC5);HANDLER(0xC6);HANDLER(0xC7);
HANDLER(0xC8);HANDLER(0xC9);HANDLER(0xCA);HANDLER(0xCB);HANDLER(0xCC);HANDLER(0xCD);HANDLER(0xCE);HANDLER(0xCF);

HANDLER(0xD0);HANDLER(0xD1);HANDLER(0xD2);HANDLER(0xD3);HANDLER(0xD4);HANDLER(0xD5);HANDLER(0xD6);HANDLER(0xD7);
HANDLER(0xD8);HANDLER(0xD9);HANDLER(0xDA);HANDLER(0xDB);HANDLER(0xDC);HANDLER(0xDD);HANDLER(0xDE);HANDLER(0xDF);

HANDLER(0xE0);HANDLER(0xE1);HANDLER(0xE2);HANDLER(0xE3);HANDLER(0xE4);HANDLER(0xE5);HANDLER(0xE6);HANDLER(0xE7);
HANDLER(0xE8);HANDLER(0xE9);HANDLER(0xEA);HANDLER(0xEB);HANDLER(0xEC);HANDLER(0xED);HANDLER(0xEE);HANDLER(0xEF);

HANDLER(0xF0);HANDLER(0xF1);HANDLER(0xF2);HANDLER(0xF3);HANDLER(0xF4);HANDLER(0xF5);HANDLER(0xF6);HANDLER(0xF7);
HANDLER(0xF8);HANDLER(0xF9);HANDLER(0xFA);HANDLER(0xFB);HANDLER(0xFC);HANDLER(0xFD);HANDLER(0xFE);HANDLER(0xFF);

int (* const gCmdHandler0x1f_Array[256])(uint8_t *Cmd,int CmdLen,int *pCmdLen,void (*tx)(char *buf,int len))=
{
	pHANDLER(0x00),pHANDLER(0x01),pHANDLER(0x02),pHANDLER(0x03),pHANDLER(0x04),pHANDLER(0x05),pHANDLER(0x06),pHANDLER(0x07),
	pHANDLER(0x08),pHANDLER(0x09),pHANDLER(0x0A),pHANDLER(0x0B),pHANDLER(0x0C),pHANDLER(0x0D),pHANDLER(0x0E),pHANDLER(0x0F),

	pHANDLER(0x10),pHANDLER(0x11),pHANDLER(0x12),pHANDLER(0x13),pHANDLER(0x14),pHANDLER(0x15),pHANDLER(0x16),pHANDLER(0x17),
	pHANDLER(0x18),pHANDLER(0x19),pHANDLER(0x1A),pHANDLER(0x1B),pHANDLER(0x1C),pHANDLER(0x1D),pHANDLER(0x1E),pHANDLER(0x1F),

	pHANDLER(0x20),pHANDLER(0x21),pHANDLER(0x22),pHANDLER(0x23),pHANDLER(0x24),pHANDLER(0x25),pHANDLER(0x26),pHANDLER(0x27),
	pHANDLER(0x28),pHANDLER(0x29),pHANDLER(0x2A),pHANDLER(0x2B),pHANDLER(0x2C),pHANDLER(0x2D),pHANDLER(0x2E),pHANDLER(0x2F),

	pHANDLER(0x30),pHANDLER(0x31),pHANDLER(0x32),pHANDLER(0x33),pHANDLER(0x34),pHANDLER(0x35),pHANDLER(0x36),pHANDLER(0x37),
	pHANDLER(0x38),pHANDLER(0x39),pHANDLER(0x3A),pHANDLER(0x3B),pHANDLER(0x3C),pHANDLER(0x3D),pHANDLER(0x3E),pHANDLER(0x3F),

	pHANDLER(0x40),pHANDLER(0x41),pHANDLER(0x42),pHANDLER(0x43),pHANDLER(0x44),pHANDLER(0x45),pHANDLER(0x46),pHANDLER(0x47),
	pHANDLER(0x48),pHANDLER(0x49),pHANDLER(0x4A),pHANDLER(0x4B),pHANDLER(0x4C),pHANDLER(0x4D),pHANDLER(0x4E),pHANDLER(0x4F),

	pHANDLER(0x50),pHANDLER(0x51),pHANDLER(0x52),pHANDLER(0x53),pHANDLER(0x54),pHANDLER(0x55),pHANDLER(0x56),pHANDLER(0x57),
	pHANDLER(0x58),pHANDLER(0x59),pHANDLER(0x5A),pHANDLER(0x5B),pHANDLER(0x5C),pHANDLER(0x5D),pHANDLER(0x5E),pHANDLER(0x5F),

	pHANDLER(0x60),pHANDLER(0x61),pHANDLER(0x62),pHANDLER(0x63),pHANDLER(0x64),pHANDLER(0x65),pHANDLER(0x66),pHANDLER(0x67),
	pHANDLER(0x68),pHANDLER(0x69),pHANDLER(0x6A),pHANDLER(0x6B),pHANDLER(0x6C),pHANDLER(0x6D),pHANDLER(0x6E),pHANDLER(0x6F),

	pHANDLER(0x70),pHANDLER(0x71),pHANDLER(0x72),pHANDLER(0x73),pHANDLER(0x74),pHANDLER(0x75),pHANDLER(0x76),pHANDLER(0x77),
	pHANDLER(0x78),pHANDLER(0x79),pHANDLER(0x7A),pHANDLER(0x7B),pHANDLER(0x7C),pHANDLER(0x7D),pHANDLER(0x7E),pHANDLER(0x7F),

	pHANDLER(0x80),pHANDLER(0x81),pHANDLER(0x82),pHANDLER(0x83),pHANDLER(0x84),pHANDLER(0x85),pHANDLER(0x86),pHANDLER(0x87),
	pHANDLER(0x88),pHANDLER(0x89),pHANDLER(0x8A),pHANDLER(0x8B),pHANDLER(0x8C),pHANDLER(0x8D),pHANDLER(0x8E),pHANDLER(0x8F),

	pHANDLER(0x90),pHANDLER(0x91),pHANDLER(0x92),pHANDLER(0x93),pHANDLER(0x94),pHANDLER(0x95),pHANDLER(0x96),pHANDLER(0x97),
	pHANDLER(0x98),pHANDLER(0x99),pHANDLER(0x9A),pHANDLER(0x9B),pHANDLER(0x9C),pHANDLER(0x9D),pHANDLER(0x9E),pHANDLER(0x9F),

	pHANDLER(0xA0),pHANDLER(0xA1),pHANDLER(0xA2),pHANDLER(0xA3),pHANDLER(0xA4),pHANDLER(0xA5),pHANDLER(0xA6),pHANDLER(0xA7),
	pHANDLER(0xA8),pHANDLER(0xA9),pHANDLER(0xAA),pHANDLER(0xAB),pHANDLER(0xAC),pHANDLER(0xAD),pHANDLER(0xAE),pHANDLER(0xAF),

	pHANDLER(0xB0),pHANDLER(0xB1),pHANDLER(0xB2),pHANDLER(0xB3),pHANDLER(0xB4),pHANDLER(0xB5),pHANDLER(0xB6),pHANDLER(0xB7),
	pHANDLER(0xB8),pHANDLER(0xB9),pHANDLER(0xBA),pHANDLER(0xBB),pHANDLER(0xBC),pHANDLER(0xBD),pHANDLER(0xBE),pHANDLER(0xBF),

	pHANDLER(0xC0),pHANDLER(0xC1),pHANDLER(0xC2),pHANDLER(0xC3),pHANDLER(0xC4),pHANDLER(0xC5),pHANDLER(0xC6),pHANDLER(0xC7),
	pHANDLER(0xC8),pHANDLER(0xC9),pHANDLER(0xCA),pHANDLER(0xCB),pHANDLER(0xCC),pHANDLER(0xCD),pHANDLER(0xCE),pHANDLER(0xCF),

	pHANDLER(0xD0),pHANDLER(0xD1),pHANDLER(0xD2),pHANDLER(0xD3),pHANDLER(0xD4),pHANDLER(0xD5),pHANDLER(0xD6),pHANDLER(0xD7),
	pHANDLER(0xD8),pHANDLER(0xD9),pHANDLER(0xDA),pHANDLER(0xDB),pHANDLER(0xDC),pHANDLER(0xDD),pHANDLER(0xDE),pHANDLER(0xDF),

	pHANDLER(0xE0),pHANDLER(0xE1),pHANDLER(0xE2),pHANDLER(0xE3),pHANDLER(0xE4),pHANDLER(0xE5),pHANDLER(0xE6),pHANDLER(0xE7),
	pHANDLER(0xE8),pHANDLER(0xE9),pHANDLER(0xEA),pHANDLER(0xEB),pHANDLER(0xEC),pHANDLER(0xED),pHANDLER(0xEE),pHANDLER(0xEF),

	pHANDLER(0xF0),pHANDLER(0xF1),pHANDLER(0xF2),pHANDLER(0xF3),pHANDLER(0xF4),pHANDLER(0xF5),pHANDLER(0xF6),pHANDLER(0xF7),
	pHANDLER(0xF8),pHANDLER(0xF9),pHANDLER(0xFA),pHANDLER(0xFB),pHANDLER(0xFC),pHANDLER(0xFD),pHANDLER(0xFE),pHANDLER(0xFF),
};

