#include <windowserver/fontchars.h>

FontChar fontchar_for_char(char chr) {
	switch (chr) {
	case ' ': return (FontChar) { .width=FONTCHAR_A_W, .height=0, .raw=0 };
	case 'E':
	case 'e':
		return (FontChar) { .width=FONTCHAR_E_W, .height=FONTCHAR_E_H, .raw=FONTCHAR_E_RAW };
	case 'R':
	case 'r':
		return (FontChar) { .width=FONTCHAR_R_W, .height=FONTCHAR_R_H, .raw=FONTCHAR_R_RAW };
	case 'S':
	case 's':
		return (FontChar) { .width=FONTCHAR_S_W, .height=FONTCHAR_S_H, .raw=FONTCHAR_S_RAW };
	case 'D':
	case 'd':
		return (FontChar) { .width=FONTCHAR_D_W, .height=FONTCHAR_D_H, .raw=FONTCHAR_D_RAW };
	case 'F':
	case 'f':
		return (FontChar) { .width=FONTCHAR_F_W, .height=FONTCHAR_F_H, .raw=FONTCHAR_F_RAW };
	case 'Q':
	case 'q':
		return (FontChar) { .width=FONTCHAR_Q_W, .height=FONTCHAR_Q_H, .raw=FONTCHAR_Q_RAW };
	case 'P':
	case 'p':
		return (FontChar) { .width=FONTCHAR_P_W, .height=FONTCHAR_P_H, .raw=FONTCHAR_P_RAW };
	case 'G':
	case 'g':
		return (FontChar) { .width=FONTCHAR_G_W, .height=FONTCHAR_G_H, .raw=FONTCHAR_G_RAW };
	case 'T':
	case 't':
		return (FontChar) { .width=FONTCHAR_T_W, .height=FONTCHAR_T_H, .raw=FONTCHAR_T_RAW };
	case 'C':
	case 'c':
		return (FontChar) { .width=FONTCHAR_C_W, .height=FONTCHAR_C_H, .raw=FONTCHAR_C_RAW };
	case 'B':
	case 'b':
		return (FontChar) { .width=FONTCHAR_B_W, .height=FONTCHAR_B_H, .raw=FONTCHAR_B_RAW };
	case 'U':
	case 'u':
		return (FontChar) { .width=FONTCHAR_U_W, .height=FONTCHAR_U_H, .raw=FONTCHAR_U_RAW };
	case 'W':
	case 'w':
		return (FontChar) { .width=FONTCHAR_W_W, .height=FONTCHAR_W_H, .raw=FONTCHAR_W_RAW };
	case 'A':
	case 'a':
		return (FontChar) { .width=FONTCHAR_A_W, .height=FONTCHAR_A_H, .raw=FONTCHAR_A_RAW };
	case 'V':
	case 'v':
		return (FontChar) { .width=FONTCHAR_V_W, .height=FONTCHAR_V_H, .raw=FONTCHAR_V_RAW };
	case 'L':
	case 'l':
		return (FontChar) { .width=FONTCHAR_L_W, .height=FONTCHAR_L_H, .raw=FONTCHAR_L_RAW };
	case 'M':
	case 'm':
		return (FontChar) { .width=FONTCHAR_M_W, .height=FONTCHAR_M_H, .raw=FONTCHAR_M_RAW };
	case 'Z':
	case 'z':
		return (FontChar) { .width=FONTCHAR_Z_W, .height=FONTCHAR_Z_H, .raw=FONTCHAR_Z_RAW };
	case 'X':
	case 'x':
		return (FontChar) { .width=FONTCHAR_X_W, .height=FONTCHAR_X_H, .raw=FONTCHAR_X_RAW };
	case 'O':
	case 'o':
		return (FontChar) { .width=FONTCHAR_O_W, .height=FONTCHAR_O_H, .raw=FONTCHAR_O_RAW };
	case 'N':
	case 'n':
		return (FontChar) { .width=FONTCHAR_N_W, .height=FONTCHAR_N_H, .raw=FONTCHAR_N_RAW };
	case 'Y':
	case 'y':
		return (FontChar) { .width=FONTCHAR_Y_W, .height=FONTCHAR_Y_H, .raw=FONTCHAR_Y_RAW };
	case 'J':
	case 'j':
		return (FontChar) { .width=FONTCHAR_J_W, .height=FONTCHAR_J_H, .raw=FONTCHAR_J_RAW };
	case 'K':
	case 'k':
		return (FontChar) { .width=FONTCHAR_K_W, .height=FONTCHAR_K_H, .raw=FONTCHAR_K_RAW };
	case 'I':
	case 'i':
		return (FontChar) { .width=FONTCHAR_I_W, .height=FONTCHAR_I_H, .raw=FONTCHAR_I_RAW };
	case 'H':
	case 'h':
		return (FontChar) { .width=FONTCHAR_H_W, .height=FONTCHAR_H_H, .raw=FONTCHAR_H_RAW };
	default: return (FontChar) { .width=0, .height=0, .raw=0 };
	}
}